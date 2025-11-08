[[vk::push_constant]]
struct WireframePushConstant 
{
    float4x4 mvp;
} wireframePushData;

struct VSInput
{
    float3 inPosition : POSITION;
    float3 inColor    : COLOR0;
};

struct VSOutput
{
    float4 posH    : SV_POSITION;
    float3 fragColor : COLOR0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    
    output.posH = mul(wireframePushData.mvp, float4(input.inPosition, 1.0f));
    
    output.fragColor = input.inColor;
    
    return output;
}

struct PSInput
{
    float4 posH    : SV_POSITION; 
    float3 fragColor : COLOR0;
};

struct PSOutput
{
    float4 outColor : SV_TARGET0;
};

PSOutput PSMain(PSInput input)
{
    PSOutput output;
    
    output.outColor = float4(input.fragColor, 1.0f); 
    
    return output;
}