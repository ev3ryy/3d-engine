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

[[vk::push_constant]]
struct PushConstantData 
{
    float4x4 model;
} pushData;

struct VSInput
{
    float3 inPosition : POSITION;
    float3 inNormal   : NORMAL;
    float2 inTexCoord : TEXCOORD0;
    float3 inTangent  : TANGENT;
    float3 inBitangent : BITANGENT;
};

struct VSOutput
{
    float4 posH       : SV_POSITION;
    float3 fragWorldPos : TEXCOORD0; // World Position for PS
    float3 fragNormal   : TEXCOORD1; // World Normal for PS
    float2 fragTexCoord : TEXCOORD2; // UV for PS (texture sampling)
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    
    float4 worldPos = mul(pushData.model, float4(input.inPosition, 1.0f));
    output.fragWorldPos = worldPos.xyz;
    
    float3 normal = mul(input.inNormal, (float3x3)pushData.model);

    output.fragNormal = normalize(normal);
    
    output.fragTexCoord = input.inTexCoord;
    
    float4 viewPos = mul(view, worldPos);
    output.posH = mul(proj, viewPos);
    
    return output;
}

struct PSInput
{
    float4 posH       : SV_POSITION; 
    float3 fragWorldPos : TEXCOORD0; 
    float3 fragNormal   : TEXCOORD1; 
    float2 fragTexCoord : TEXCOORD2;
};

struct PSOutput
{
    float4 gbufAlbedoMetallic  : SV_TARGET0; // .rgb = Albedo, .a = Metallic
    float4 gbufNormalRoughness : SV_TARGET1; // .rgb = World Normal, .a = Roughness
    float4 gbufEmissiveAO      : SV_TARGET2; // .rgb = Emissive, .a = AO
    //float4 gbufDepth           : SV_TARGET3; // .r = Linear Depth
};

Texture2D albedoTexture     : register(t1, space1);
Texture2D normalTexture     : register(t2, space1);
Texture2D metallicRoughnessTexture : register(t3, space1);
Texture2D aoTexture        : register(t4, space1);

SamplerState defaultSampler : register(s1, space1);

cbuffer MaterialUBO : register(b0, space1)
{
    float materialRoughness;
    float materialMetallic;
};

PSOutput PSMain(PSInput input)
{
    PSOutput output;
    
    // 1. Albedo & Metallic
    float4 albedo = albedoTexture.Sample(defaultSampler, input.fragTexCoord);
    output.gbufAlbedoMetallic.rgb = albedo.rgb;
    output.gbufAlbedoMetallic.a   = materialMetallic;
    
    // 2. Normal & Roughness
    output.gbufNormalRoughness.rgb = normalize(input.fragNormal); 
    output.gbufNormalRoughness.a   = materialRoughness;
    
    // 3. Emissive & AO
    output.gbufEmissiveAO.rgb = float3(0.0f, 0.0f, 0.0f); 
    output.gbufEmissiveAO.a   = 1.0f;

    //output.gbufDepth = float4(input.posH.z / input.posH.w, 0.0f, 0.0f, 1.0f);

    return output;
}