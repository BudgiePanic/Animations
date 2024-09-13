#pragma once

#include "../cgltf.h"

namespace io {

/// <summary>
/// Loads the data stored in a GLTF file into datastructures in memory that can be accessed.
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
cgltf_data* LoadGLTFFile(const char* path);

/// <summary>
/// Releases GLTF data
/// </summary>
/// <param name="dataHandle"></param>
void FreeGLTFData(cgltf_data* dataHandle);

}
