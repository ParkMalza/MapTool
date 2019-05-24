#include "000_Header.fx"
#define MAX_BLOOM_COUNT 63

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};
struct BloomDesc
{
    float2 Offset;
    float Weight;

    float Padding;
};

cbuffer CB_Values
{
    float2 MapSize;
    float Threshold;
    float Intensity;

    uint BlurCount;
    float3 CB_Values_Padding;

    BloomDesc Valuse[MAX_BLOOM_COUNT];
};

Texture2D LuminosityMap;
//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, View2D);
    output.Position = mul(output.Position, Projection2D);
    
    output.Uv = input.Uv;

    return output;
}
// 밝은 빛만 남기는 과정들을 통해 선명한 그래픽 효과를 준다!
float4 PS_Luminosity(VertexOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    //Threshold보다 큰 값을 남긴다. 즉 밝은 빛만 남긴다.
    return saturate((color - Threshold) / (1 - Threshold));
}

float4 PS_Blur(VertexOutput input) : SV_TARGET0
{
    float4 color = 0;
    float2 uv = 0;
    uint count = BlurCount * 2 - 1;
    [roll(MAX_BLOOM_COUNT)]
    for (int i = 0; i < count; i++)
    {
        uv = input.Uv + Valuse[i].Offset; // offset uv에..
        color += DiffuseMap.Sample(LinearSampler, uv) * Valuse[i].Weight;
    }

    return color;
}

float4 PS_Composite(VertexOutput input) : SV_TARGET0
{
    float4 l = LuminosityMap.Sample(LinearSampler, input.Uv) * Intensity;
    float4 b = DiffuseMap.Sample(LinearSampler, input.Uv);

    b *= (1 - saturate(l));

    return b + l;
}
//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
DepthStencilState Depth
{
    DepthEnable = false;
};

technique11 T0
{
    pass P0
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Luminosity()));
    }
    pass P1
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Blur()));
    }
    pass P2
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Composite()));
    }
    
}