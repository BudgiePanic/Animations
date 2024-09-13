#include "gltfLoader.h"
#include <iostream>

namespace io {

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

}
