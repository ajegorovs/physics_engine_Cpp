#version 450

struct point3D
{
    vec4 color;
    vec3 position;
    vec3 velocity;
    vec3 acceleration;
    float mass;
    float damping;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec3 velocity;
layout(location = 3) in vec3 acceleration;
layout(location = 4) in float mass;
layout(location = 5) in float damping;
layout(location = 6) in uint group_id;

layout(location = 0) out vec4 fragColor;

void main() {

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
	gl_PointSize = 3.0;

}