#version 330 core
// BASIC LIGHTING MODEL SHADER | DIFFUSE TEXTURE + LIGHT | NO AMBIENT OR SPECULAR
// input attributes recieved from the vertex shader
in vec3 world_normal;
in vec3 fragment_world_position;
in vec2 uv;

uniform vec3 sky_light_direction;
uniform sampler2D tex0;

out vec4 FragColor;

void main() {
    vec4 diffuse = texture(tex0, uv);
    vec3 normal = normalize(world_normal);
    vec3 light = normalize(sky_light_direction);
    float intensity = clamp(dot(normal, light), 0, 1);
    // Light color is implicit (1,1,1) white
    FragColor = diffuse * intensity;
}
