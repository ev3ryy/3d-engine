#version 450

#define PI 3.14159265359

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform GlobalUBO {
    mat4 view;
    mat4 proj;
    vec3 sunLightDirection;
    float sunLightIntensity;
    vec3 cameraPosition;
} globalUBO;


layout(set = 1, binding = 0) uniform sampler2D gbufAlbedoMetallic; // .rgb = Albedo, .a = Metallic
layout(set = 1, binding = 1) uniform sampler2D gbufNormalRoughness; // .rgb = World Normal, .a = Roughness
layout(set = 1, binding = 2) uniform sampler2D gbufEmissiveAO;      // .rgb = Emissive, .a = AO
layout(set = 1, binding = 3) uniform sampler2D gbufDepth;           // .r = Depth

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / (denom + 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 getWorldPosFromDepth(vec2 uv, float depth) {
    mat4 invProj = inverse(globalUBO.proj);
    mat4 invView = inverse(globalUBO.view);
    
    float z = depth * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invProj * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    vec4 worldSpacePosition = invView * viewSpacePosition;
    
    return worldSpacePosition.xyz;
}

void main() {
    float depth = texture(gbufDepth, inUV).r;
    
    if (depth >= 1.0) {
        discard;
    }

    vec3 worldPos = getWorldPosFromDepth(inUV, depth);

    vec4 albedoData = texture(gbufAlbedoMetallic, inUV);
    vec3 albedo     = albedoData.rgb;
    float metallic  = albedoData.a;

    vec4 normalData = texture(gbufNormalRoughness, inUV);
    vec3 N          = normalize(normalData.rgb);
    float roughness = normalData.a;
    
    vec4 emissiveAOData = texture(gbufEmissiveAO, inUV);
    vec3 emissive       = emissiveAOData.rgb;
    float ao            = emissiveAOData.a;

    vec3 L = normalize(globalUBO.sunLightDirection);
    vec3 V = normalize(globalUBO.cameraPosition - worldPos);
    vec3 H = normalize(L + V);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;
    vec3 diffuse = kD * albedo / PI;

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0);
    vec3 directLighting = (diffuse + specular) * globalUBO.sunLightIntensity * NdotL;
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 finalColor = ambient + directLighting + emissive;
    
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2)); 

    outFragColor = vec4(finalColor, 1.0);
}