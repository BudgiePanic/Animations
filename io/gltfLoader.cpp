#include "gltfLoader.h"
#include <iostream>
#include "../transforms/srt.h"
#include "../animation/Pose.h"
#include "../animation/Track.h"
#include "../animation/Interpolate.h"

namespace io {

namespace helpers {
    /* Assuming the CGLTF file only contains 1 node for an animated character */

    transforms::srt GetLocalTransformFromNode(cgltf_node& node) {
        transforms::srt result;
        if (node.has_matrix) {
        }
        if (node.has_rotation) {
        }
        if (node.has_scale) {
        }
        if (node.has_translation) {
        }
        return result;
    }

    int GetNodeIndex(cgltf_node* node, cgltf_node* nodeArray, int numbNodes) {
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
        ExtractValuesFromNodes(frameTimes, 1, *sampler.input);
        std::vector<float> values;
        ExtractValuesFromNodes(values, NodeSize, *sampler.output);
        unsigned int frameCount = sampler.input->count;
        unsigned int frameComponentCount = values.size() / frameTimes.size();
        result.Resize(frameComponentCount);
        for () {
            // TODO populate the result track with frame data
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

std::vector<std::string> LoadBoneNames(cgltf_data* data)
{
    return std::vector<std::string>();
}

}
