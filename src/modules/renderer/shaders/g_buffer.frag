#version 450

#define PI 3.14159265359

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 outAlbedo;    // RT0: Albedo (RGB), Metallic (A)
layout(location = 1) out vec4 outNormal;    // RT1: Normal (RGB), Roughness (A)
layout(location = 2) out vec4 outEmissive;  // RT2: Emissive (RGB), AO (A)

layout(set = 1, binding = 0) uniform MaterialUniformBuffer {
    vec4 albedoColor;
    vec4 pbrParams; // x: metallic, y: roughness, z: ambientOcclusion, w: (padding)
    ivec4 textureFlags; // x: hasAlbedoMap, y: hasNormalMap, z: hasMetallicRoughessMap, w: hasAoMap
} materialData;

void main() {
    vec3 albedo = materialData.albedoColor.rgb;
    float metallic = materialData.pbrParams.x;
    float roughness = materialData.pbrParams.y;
    float ao = materialData.pbrParams.z;
    vec3 emissiveColor = vec3(0.0);

    outAlbedo = vec4(albedo, metallic);
    outNormal = vec4(normalize(inNormal), roughness);
    outEmissive = vec4(emissiveColor, ao);
}