#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 u_DiffuseColor;
    float u_AmbientFactor;
    vec3 pad;
    vec4 u_SunParameters;
} pushConstants;


layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);

    vec3 lightDir = normalize(pushConstants.u_SunParameters.xyz);
    vec3 lightColor = vec3(1.0) * pushConstants.u_SunParameters.w;

    vec3 viewDir = normalize(-fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pushConstants.u_DiffuseColor.xyz * diff * lightColor;

    vec3 ambient = pushConstants.u_AmbientFactor * pushConstants.u_DiffuseColor.xyz;

    vec3 color = ambient + diffuse;
    outColor = vec4(color, 1.0);
}