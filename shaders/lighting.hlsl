cbuffer GlobalUBO : register(b0, space0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 invView;
    float4x4 invProj;
    float3 sunLightDirection;
    float sunLightIntensity;
    float3 cameraPosition; 
};

Texture2D gbufAlbedoMetallic : register(t0, space1); 
Texture2D gbufNormalRoughness: register(t1, space1); 
Texture2D gbufEmissiveAO    : register(t2, space1); 
Texture2D gbufDepth         : register(t3, space1); 

SamplerState defaultSampler : register(s0, space1);

struct VSOutput
{
    float4 posH : SV_POSITION;
    float2 outUV : TEXCOORD0;
};

struct PSInput
{
    float4 posH : SV_POSITION; 
    float2 inUV : TEXCOORD0; 
};

struct PSOutput
{
    float4 outFragColor : SV_TARGET0; 
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    
    if (vertexID == 0) {
        output.outUV = float2(0.0f, 0.0f);
        output.posH = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    } else if (vertexID == 1) {
        output.outUV = float2(2.0f, 0.0f);
        output.posH = float4(3.0f, -1.0f, 0.0f, 1.0f);
    } else { // vertexID == 2
        output.outUV = float2(0.0f, 2.0f);
        output.posH = float4(-1.0f, 3.0f, 0.0f, 1.0f);
    }

    return output;
}


#define PI 3.14159265359f

float DistributionGGX(float3 N, float3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / (denom + 0.0001f);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0) {
    return F0 + (1.0f - F0) * pow(saturate(1.0f - cosTheta), 5.0f);
}

float3 getWorldPosFromDepth(float2 uv, float depth) {
    float z = depth * 2.0f - 1.0f;
    float4 clipSpacePosition = float4(uv * 2.0f - 1.0f, z, 1.0f);
    
    float4 viewSpacePosition = mul(invProj, clipSpacePosition);
    viewSpacePosition /= viewSpacePosition.w;
    float4 worldSpacePosition = mul(invView, viewSpacePosition);
    
    return worldSpacePosition.xyz;
}

PSOutput PSMain(PSInput input)
{
    float2 uv = input.inUV;
    
    float depth = gbufDepth.Sample(defaultSampler, uv).r; 
    
    if (depth >= 1.0f) {
        discard;
    }

    float3 worldPos = getWorldPosFromDepth(uv, depth);

    float4 albedoData = gbufAlbedoMetallic.Sample(defaultSampler, uv);
    float3 albedo   = albedoData.rgb;
    float metallic  = albedoData.a;

    float4 normalData = gbufNormalRoughness.Sample(defaultSampler, uv);
    float3 N          = normalize(normalData.rgb);
    float roughness = normalData.a;
    
    float4 emissiveAOData = gbufEmissiveAO.Sample(defaultSampler, uv);
    float3 emissive       = emissiveAOData.rgb;
    float ao              = emissiveAOData.a;

    float3 L = normalize(sunLightDirection); 
    float3 V = normalize(cameraPosition - worldPos); 
    float3 H = normalize(L + V);

    float3 F0 = float3(0.04f, 0.04f, 0.04f); 
    F0 = lerp(F0, albedo, metallic);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    float3 F  = FresnelSchlick(max(dot(H, V), 0.0f), F0);

    float3 kD = float3(1.0f, 1.0f, 1.0f) - F; 
    kD *= 1.0f - metallic;
    float3 diffuse = kD * albedo / PI;

    float3 numerator    = NDF * G * F;
    float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
    float3 specular     = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0f);
    float3 directLighting = (diffuse + specular) * sunLightIntensity * NdotL;
    
    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;
    float3 finalColor = ambient + directLighting + emissive;

    finalColor = finalColor / (finalColor + float3(1.0f, 1.0f, 1.0f));
    finalColor = pow(finalColor, float3(1.0f/2.2f, 1.0f/2.2f, 1.0f/2.2f));

    PSOutput output;
    output.outFragColor = float4(finalColor, 1.0f);
    return output;
}