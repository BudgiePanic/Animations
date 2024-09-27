#include "gltfLoader.h"
#include <iostream>
#include <string>
#include "../transforms/srt.h"
#include "../animation/Pose.h"
#include "../animation/Track.h"
#include "../animation/Interpolate.h"
#include "../animation/Frame.h"

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
                node.rotation[0], node.rotation[1], node.rotation[3], node.rotation[4]
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
    /// 
    /// </summary>
    /// <param name="node"></param>
    /// <param name="nodeArray"></param>
    /// <param name="numbNodes"></param>
    /// <returns>If no node index can be found, -1 is returned</returns>
    int GetNodeIndex(cgltf_node* node, cgltf_node* nodeArray, int numbNodes) {
        if (node == NULL) { return -1; }
        for (int i = 0; i < numbNodes; i++) {
            if (node == &nodeArray[i]) {
                return (int)i;
            }
        }
        return -1;
    }

    anim::Pose MakeRestPose(cgltf_data* data) {
        unsigned int numbNodes = data->nodes_count;
        anim::Pose result;
        result.Resize(numbNodes);
        for (int i = 0; i < numbNodes; i++) {
            cgltf_node* node = &(data->nodes[i]);
            transforms::srt transform = GetLocalTransformFromNode(data->nodes[i]);
            result.SetLocalTransform(i, transform);
            int parentBone = GetNodeIndex(node->parent, data->nodes, numbNodes);
            result.SetParentIndex(i, parentBone);
        }
        return result;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="values"></param>
    /// <param name="numbFloats">The number of component floats stored in the cgltf node</param>
    /// <param name="accessor"></param>
    void ExtractValuesFromNodes(std::vector<float>& values, unsigned int numbFloats, const cgltf_accessor& accessor) {
        values.resize(accessor.count * numbFloats);
        for (int i = 0; i < accessor.count; i++) {
            cgltf_accessor_read_float(&accessor, i, &values[i * numbFloats], numbFloats);
        }
    }

    template<typename T, int NodeSize>
    void ExtractTrack(anim::Track<T, NodeSize>& result, const cgltf_animation_channel& channel) {
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
        unsigned int frameComponentCount = values.size() / frameTimes.size();
        result.Resize(frameComponentCount);
        for (unsigned int i = 0; i < NodeSize; i++) {
            // this function feels dangerous, curious to see if it works in the debugger
            int index = i * frameComponentCount;
            anim::Frame<NodeSize>& frame = result[i];
            int offset = 0;
            frame.timestamp = frameTimes[i];
            for (int component = 0; component < NodeSize; component++) {
                frame.in[component] = isCubicInterpolation ?
                    values[index + offset] : 0.0f;
                offset++;
            }
            for (int component = 0; component < NodeSize; component++) {
                frame.value[component] = values[index + offset];
                offset++;
            }
            for (int component = 0; component < NodeSize; component++) {
                frame.out[component] = isCubicInterpolation ? 
                   values[index + offset] : 0.0f;
                index++;
            }
        }
    }

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
        unsigned int nodeSize = data->animations[i].channels_count;
        for (int j = 0; j < nodeSize; j++) {
            cgltf_animation_channel& channel = data->animations[i].channels[j];
            cgltf_node* node = channel.target_node; 
            int nodeID = helpers::GetNodeIndex(node, data->nodes, nodeCount);
            if (channel.target_path == cgltf_animation_path_type_translation) {
                anim::TrackVector translation = result[i][nodeID].GetTranslationTrack();
                helpers::ExtractTrack<f3, 3>(translation, channel);
            } else if (channel.target_path == cgltf_animation_path_type_scale) {
                anim::TrackVector& scale = result[i][nodeID].GetScaleTrack();
                helpers::ExtractTrack<f3, 3>(scale, channel);
            } else if (channel.target_path == cgltf_animation_path_type_rotation) {
                anim::TrackQuaternion& rotation = result[i][nodeID].GetQuaternionTrack();
                helpers::ExtractTrack<rotation::quaternion, 4>(rotation, channel);
            }
        }
        result[i].CalculateClipDuration();
    }
    return result;
}

}
