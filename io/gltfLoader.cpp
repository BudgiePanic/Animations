#include "gltfLoader.h"
#include <iostream>
#include <string>
#include "../transforms/srt.h"
#include "../animation/Pose.h"
#include "../animation/Track.h"
#include "../animation/Interpolate.h"
#include "../animation/Frame.h"
#include "../FloatHelp.h"

namespace io {

namespace helpers {
    /* Assuming the CGLTF file only contains 1 node for an animated character */

    transforms::srt GetLocalTransformFromNode(cgltf_node& node) {
        transforms::srt result;
        if (node.has_matrix) {
            mat4f matrix(&node.matrix[0]);
            result = transforms::toSRT(matrix);
        }
        if (node.has_rotation) {
            result.rotation = rotation::quaternion(
                node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]
            );
        }
        if (node.has_scale) {
            result.scale = f3(node.scale[0], node.scale[1], node.scale[2]);
        }
        if (node.has_translation) {
            result.position = f3(node.translation[0], node.translation[1], node.translation[2]);
        }
        return result;
    }

    /// <summary>
    /// All node in the scenes are stored in an array. A node contains: a mesh, animation data, camera, light, etc.
    /// </summary>
    /// <param name="node"></param>
    /// <param name="nodeArray"></param>
    /// <param name="numbNodes"></param>
    /// <returns>If no node index can be found, -1 is returned</returns>
    int GetNodeIndex(cgltf_node* node, cgltf_node* nodeArray, unsigned int numbNodes) {
        if (node == NULL) { return -1; }
        for (unsigned int i = 0; i < numbNodes; i++) {
            if (node == &nodeArray[i]) {
                return (int)i;
            }
        }
        return -1;
    }

    /// <summary>
    /// Uses an accessor to access values from a buffer. Writes the buffer values into a vector.
    /// </summary>
    /// <param name="values"></param>
    /// <param name="numbFloats">The number of component floats stored in the cgltf node</param>
    /// <param name="accessor"></param>
    void ExtractValuesFromNodes(std::vector<float>& values, unsigned int numbFloats, const cgltf_accessor& accessor) {
        values.resize(accessor.count * numbFloats);
        for (unsigned int i = 0; i < (unsigned int) accessor.count; i++) {
            cgltf_accessor_read_float(&accessor, i, &values[i * numbFloats], numbFloats);
        }
    }

    /// <summary>
    /// Gets the channel's sampler and extracts the raw animation input and output data. These raw values are converted into 
    /// frames which are then stored in the track. Generates frames for one aspect (i.e. rotation, scale, position, for one bone) 
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <typeparam name="NodeSize"></typeparam>
    /// <param name="result"></param>
    /// <param name="channel"></param>
    template<typename T, int NodeSize>
    void ExtractTrack(anim::Track<T, NodeSize>& result, const cgltf_animation_channel& channel) {
        /* https://github.com/KhronosGroup/glTF-Tutorials/blob/main/gltfTutorial/gltfTutorial_007_Animations.md */
        cgltf_animation_sampler& sampler = *channel.sampler;
        bool isCubicInterpolation = false;
        anim::Interpolate interp = anim::Interpolate::Constant;
        if (sampler.interpolation == cgltf_interpolation_type_linear) { interp = anim::Interpolate::Linear; }
        if (sampler.interpolation == cgltf_interpolation_type_cubic_spline) {
            interp = anim::Interpolate::Cubic;
            isCubicInterpolation = true;
        }
        result.SetInterpolationMethod(interp);
        std::vector<float> frameTimes;
        constexpr unsigned int frameTimeTrackSize = 1;
        ExtractValuesFromNodes(frameTimes, frameTimeTrackSize, *sampler.input);
        std::vector<float> values;
        ExtractValuesFromNodes(values, NodeSize, *sampler.output);
        unsigned int frameCount = sampler.input->count;
        result.Resize(frameCount);
        unsigned int frameComponentCount = values.size() / frameTimes.size();
        for (unsigned int i = 0; i < frameCount; i++) {
            // this function feels dangerous, curious to see if it works in the debugger
            int index = i * frameComponentCount;
            anim::Frame<NodeSize>& frame = result[i];
            int offset = 0;
            frame.timestamp = frameTimes[i];
            for (int component = 0; component < NodeSize; component++) {
                frame.in[component] = isCubicInterpolation ?
                    values[index + offset] : 0.0f;
                if (isCubicInterpolation) { offset++; }
            }
            for (int component = 0; component < NodeSize; component++) {
                frame.value[component] = values[index + offset];
                offset++;
            }
            for (int component = 0; component < NodeSize; component++) {
                frame.out[component] = isCubicInterpolation ? 
                   values[index + offset] : 0.0f;
                if (isCubicInterpolation) { offset++; }
            }
        }
    }
    /// <summary>
    /// Writes mesh attribute data from the gltf file into the mesh object (i.e. positions, normals, bone indexes)
    /// </summary>
    /// <param name="mesh"></param>
    /// <param name="attribute"></param>
    /// <param name="skin"></param>
    /// <param name="nodeArray"></param>
    /// <param name="numbNodes"></param>
    void MeshFromAttribute(render::Mesh& mesh,
        cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodeArray, unsigned int numbNodes) {
        cgltf_accessor& accessor = *attribute.data;
        unsigned int numbComponents = 0;
        if (accessor.type == cgltf_type_vec2) {
            numbComponents = 2;
        } else if (accessor.type == cgltf_type_vec3) {
            numbComponents = 3;
        } else if (accessor.type == cgltf_type_vec4) {
            numbComponents = 4;
        }
        std::vector<float> values;
        ExtractValuesFromNodes(values, numbComponents, accessor);
        auto& verts = mesh.GetPositions();
        auto& norms = mesh.GetNormals();
        auto& texs = mesh.GetTextureCoords();
        auto& bones = mesh.GetBoneIndices();
        auto& boneWeights = mesh.GetBoneWeights();
        unsigned int numbAccessors = accessor.count;
        cgltf_attribute_type type = attribute.type;
        for (unsigned int i = 0; i < numbAccessors; i++) {
            int index = i * numbComponents;
            switch (type) {
            case cgltf_attribute_type_position:
                verts.push_back(f3(values[index], values[index + 1], values[index + 2]));
                break;
            case cgltf_attribute_type_texcoord:
                texs.push_back(f2(values[index], values[index + 1]));
                break;
            case cgltf_attribute_type_normal: {
                f3 normal = f3(values[index], values[index + 1], values[index + 2]);
                if (lengthSquared(normal) < epsilon) { // bad normal data
                    normal = f3(0,1,0);
                }
                norms.push_back(normal);
            }
                break;
            case cgltf_attribute_type_weights:
                boneWeights.push_back(f4(values[index], values[index + 1], values[index + 2], values[index + 3]));
                break;
            case cgltf_attribute_type_joints:
                int a = (int) (values[index] + 0.5f); 
                int b = (int) (values[index + 1] + 0.5f);
                int c = (int) (values[index + 2] + 0.5f);
                int d = (int) (values[index + 3] + 0.5f);
                // joint indices are relative to the joint array 
                // need to convert to be joints in the armature hierarchy
                i4 joints(a,b,c,d);
                auto node = skin->joints[joints.x];
                joints.x = GetNodeIndex(node, nodeArray, numbNodes);
                node = skin->joints[joints.y];
                joints.y = GetNodeIndex(node, nodeArray, numbNodes);
                node = skin->joints[joints.z];
                joints.z = GetNodeIndex(node, nodeArray, numbNodes);
                node = skin->joints[joints.w];
                joints.w = GetNodeIndex(node, nodeArray, numbNodes);
                // check for invalid values
                // technically we could modify our skinning algorithm to ignore negative joints but whatever, the CPU is fast
                joints.x = (joints.x < 0) ? 0 : joints.x;
                joints.y = (joints.y < 0) ? 0 : joints.y;
                joints.z = (joints.z < 0) ? 0 : joints.z;
                joints.w = (joints.w < 0) ? 0 : joints.w;
                bones.push_back(joints);
                break;
            }
        }
    }

}

anim::Pose MakeRestPose(cgltf_data* data) {
    unsigned int numbNodes = (unsigned int) data->nodes_count;
    anim::Pose result(numbNodes);
    for (unsigned int i = 0; i < numbNodes; i++) {
        cgltf_node* node = &(data->nodes[i]);
        transforms::srt transform = helpers::GetLocalTransformFromNode(data->nodes[i]);
        result.SetLocalTransform(i, transform);
        int parentBone = helpers::GetNodeIndex(node->parent, data->nodes, numbNodes);
        result.SetParentIndex(i, parentBone);
    }
    return result;
}

anim::Pose MakeBindPose(cgltf_data* data) {
    anim::Pose rest = MakeRestPose(data);
    unsigned int numbBones = rest.Size();
    std::vector<transforms::srt> worldSpaceBind(numbBones);
    for (unsigned int i = 0; i < numbBones; i++) {
        // default values in case a bone is missing in the gltf file
        worldSpaceBind[i] = rest.GetWorldTransform(i); 
    }
    unsigned int numbSkins = data->skins_count;
    for (unsigned int i = 0; i < numbSkins; i++) {
        cgltf_skin* skin = &(data->skins[i]);
        // stores the inverse bind bose matrices for each bone in a contigous vector
        std::vector<float> values; 
        helpers::ExtractValuesFromNodes(values, (4*4), *skin->inverse_bind_matrices);
        unsigned int skinNumbBones = skin->joints_count;
        for (unsigned int j = 0; j < skinNumbBones; j++) {
            float* inverseBindBone = &(values[j * (4*4)]);
            mat4f inverseBindMatrix = mat4f(inverseBindBone);
            // finds the bind pose matrix by inverting the inverse bind pose
            mat4f bindMatrix = inverse(inverseBindMatrix);
            transforms::srt bindBone = transforms::toSRT(bindMatrix);
            cgltf_node* boneNode = skin->joints[j];
            int boneIndex = helpers::GetNodeIndex(boneNode, data->nodes, numbBones);
            // assert(boneIndex > 0);
            worldSpaceBind[boneIndex] = bindBone;
        }
    }
    // convert the world space bind pose bones to be relative to their parents
    anim::Pose bindPose = rest;
    for (unsigned int i = 0; i < numbBones; i++) {
        transforms::srt bone = worldSpaceBind[i];
        int parentIndex = bindPose.ParentIndexOf(i);
        if (parentIndex >= 0) {
            transforms::srt parent = worldSpaceBind[parentIndex];
            bone = transforms::combine(transforms::inverse(parent), bone);
        }
        bindPose.SetLocalTransform(i, bone);
    }
    return bindPose;
}

anim::Armature MakeArmature(cgltf_data* data) {
    return anim::Armature(
        MakeRestPose(data),
        MakeBindPose(data),
        LoadBoneNames(data)
    );
}

std::vector<render::Mesh> LoadMeshes(cgltf_data* data) {
    // check all nodes, skip nodes that do not possess a mesh and skin
    std::vector<render::Mesh> result;
    cgltf_node* nodes = data->nodes;
    unsigned int numbNodes = data->nodes_count;
    for (unsigned int i = 0; i < numbNodes; i++) {
        cgltf_node* node = &nodes[i];
        if (node->mesh == 0 || node->skin == 0) { continue; }
        cgltf_size primCount = node->mesh->primitives_count; // basically an unsigned int
        for (unsigned int j = 0; j < primCount; j++) {
            result.push_back(render::Mesh());
            render::Mesh& mesh = result.back();
            cgltf_primitive* primative = &node->mesh->primitives[j];
            unsigned int attributeCount = primative->attributes_count;
            for (unsigned int k = 0; k < attributeCount; k++) {
                // extracting verts, norms, textCoords, and more attributes for the mesh
                cgltf_attribute* attribute = &primative->attributes[k];
                helpers::MeshFromAttribute(mesh, *attribute, node->skin, nodes, numbNodes);
            }
            if (primative->indices != 0) {
                unsigned int indicesCount = primative->indices->count;
                auto& indices = mesh.GetVertexIndices();
                indices.resize(indicesCount);
                for (unsigned int k = 0; k < indicesCount; k++) {
                    indices[k] = cgltf_accessor_read_index(primative->indices, k);
                }
            }
            mesh.SyncOpenGL();
        }
    }
    return result;
}

cgltf_data* LoadGLTFFile(const char* path) {
    cgltf_options options{};
    cgltf_data* data = nullptr;
    cgltf_result result = cgltf_parse_file(&options, path, &data);
    if (result != cgltf_result_success) {
        std::cout << "Couldn't load file at [" << path << "]\n";
        return nullptr;
    }
    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success) {
        cgltf_free(data);
        std::cout << "Error getting data from buffers in [" << path << "]\n";
        return nullptr;
    }
    result = cgltf_validate(data);
    if (result != cgltf_result_success) {
        cgltf_free(data);
        std::cout << "Error while validating data in [" << path << "]\n";
        return nullptr;
    }
    return data;
}

void FreeGLTFData(cgltf_data* dataHandle) {
    if (dataHandle == NULL) { return; }
    cgltf_free(dataHandle);
}

std::vector<std::string> LoadBoneNames(cgltf_data* data) {
    unsigned int numbBones = (unsigned int)data->nodes_count;
    std::vector<std::string> result(numbBones, "Unset");
    for (unsigned int i = 0; i < numbBones; i++) {
        cgltf_node* node = &(data->nodes[i]);
        if (node->name == NULL) {
            result[i] = "No Name";
        } else {
            std::string boneName = node->name;
            result[i] = boneName;
        }
    }
    return result;
}

std::vector<anim::Clip> LoadClips(cgltf_data* data) {
    unsigned int clipCount = data->animations_count;
    unsigned int nodeCount = data->nodes_count;
    std::vector<anim::Clip> result;
    result.resize(clipCount);
    for (unsigned int i = 0; i < clipCount; i++) {
        std::string clipName = data->animations[i].name;
        result[i].SetClipName(clipName);
        // Each animated bone typically has 3 animation channels, one for position, one for rotation, one for scale. Not all bones are animated.
        unsigned int nodeSize = (unsigned int) data->animations[i].channels_count;  
        for (unsigned int j = 0; j < nodeSize; j++) {
            cgltf_animation_channel& channel = data->animations[i].channels[j];
            cgltf_node* node = channel.target_node; 
            int nodeID = helpers::GetNodeIndex(node, data->nodes, nodeCount);
            if (channel.target_path == cgltf_animation_path_type_translation) {
                anim::TrackVector& translation = result[i][nodeID].GetTranslationTrack();
                helpers::ExtractTrack<f3, 3>(translation, channel);
            } else if (channel.target_path == cgltf_animation_path_type_scale) {
                anim::TrackVector& scale = result[i][nodeID].GetScaleTrack();
                helpers::ExtractTrack<f3, 3>(scale, channel);
            } else if (channel.target_path == cgltf_animation_path_type_rotation) {
                anim::TrackQuaternion& rotation = result[i][nodeID].GetQuaternionTrack(); // [clip index][bone index]
                helpers::ExtractTrack<rotation::quaternion, 4>(rotation, channel);
            }
        }
        result[i].CalculateClipDuration();
    }
    return result;
}

}
