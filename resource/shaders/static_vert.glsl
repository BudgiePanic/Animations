#version 330 core
// VERTEX SHADER FOR VIEWING STATIC GEOMETRY OR MESHES THAT ARE SKINNED ON THE GPU
// uniforms
uniform mat4 model_transform; // model space to world space
uniform mat4 view_transform; // world space to view space
uniform mat4 projection; // convert from world3D to screen space (NDC)
// input attributes, per vertex, local model space
in vec3 position;
in vec3 normal;
in vec2 textureCoordinate;
// output attributes, passing data down the pipeline to help the fragment shader perform lighting calculations
out vec3 world_normal; // The vertex normal in world space
out vec3 fragment_world_position; // The position of the fragment in world space
out vec2 uv;

void main() {
    // Convert the vertex position into NDC
    gl_Position = projection * view_transform * model_transform * vec4(position, 1.0f);
    fragment_world_position = vec3(model_transform * vec4(position, 1.0f)); 
    world_normal = vec3(model_transform * vec4(normal, 0.0f));
    uv = textureCoordinate;
}
