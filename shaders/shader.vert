#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(std140, binding= 2) readonly buffer storageBuffer{
mat4 model[];} ObjectData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float hasTexture;
layout(location = 4) in float objIDin;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragHasTexture;
layout(location = 3) out float objID;

void main() {
    int objIDIndex = int(objIDin);
    gl_Position = ubo.proj * ubo.view * ObjectData.model[objIDIndex] * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    objID = objIDin;
}