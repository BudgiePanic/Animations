#version 330 core
// VERTEX SHADER FOR SKINNING VERTICES TO BONES WITHIN THE GPU
// uniforms
uniform mat4 model_transform; // model space to world space
uniform mat4 view_transform; // world space to view space
uniform mat4 projection; // convert from world3D to screen space (NDC)
uniform mat4 posedBones[120]; // The choice of 120 bones is arbitrary, as long as the model has less than 120 bones we're fine. Matrix to go from local bone space to model space.
uniform mat4 inverseBindPose[120]; // matrix to go from model space to bone local space when bone is in T pose configuration.
// input attributes, per vertex, local model space
in vec3 position;
in vec3 normal;
in vec2 textureCoordinate;
in vec4 boneWeights; // 'invalid' bones will have zero weight, eliminating their influence in the skinning calculation
in ivec4 boneIndices;
// output attributes, passing data down the pipeline to help the fragment shader perform lighting calculations
out vec3 world_normal; // The vertex normal in world space
out vec3 fragment_world_position; // The position of the fragment in world space
out vec2 uv;

void main() {
    // create skinning matrix (model -> T pose bone local -> posed bone model)
    mat4 skinner = (posedBones[boneIndices.x] * inverseBindPose[boneIndices.x]) * boneWeights.x;
    skinner += (posedBones[boneIndices.y] * inverseBindPose[boneIndices.y]) * boneWeights.y;
    skinner += (posedBones[boneIndices.z] * inverseBindPose[boneIndices.z]) * boneWeights.z;
    skinner += (posedBones[boneIndices.w] * inverseBindPose[boneIndices.w]) * boneWeights.w;
    // Convert the vertex position into NDC, right to left application 
    gl_Position = projection * view_transform * model_transform * skinner * vec4(position, 1.0f);
    fragment_world_position = vec3(model_transform * skinner * vec4(position, 1.0f)); 
    world_normal = vec3(model_transform * skinner * vec4(normal, 0.0f));
    uv = textureCoordinate;
}
