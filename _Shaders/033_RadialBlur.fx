#include "000_Header.fx"
#define MAX_RADIAL_BLUR_COUNT 32

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    float2 MapSize;
    uint BlurCount;
    float Radius;
    float Amount;
    float addError;
};

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

//속도감 있는 연출 가능
float4 PS_RadialBlur(VertexOutput input) : SV_TARGET0
{
    float2 radius = input.Uv - float2(0.5f, 0.5f);
    float r = length(radius) + addError;
    radius /= r;

    r = 2 * r / Radius;
    r = saturate(r);

    float2 delta = radius * r * r * Amount / BlurCount;
    delta = -delta;

    float4 color = 0;
    
    [roll(MAX_RADIAL_BLUR_COUNT)]
    for (int i = 0; i < BlurCount; i++)
    {
        color += DiffuseMap.Sample(LinearSampler, input.Uv);
        input.Uv += delta;
    }
    color /= BlurCount;

    return float4(color.rgb, 1);
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
        SetPixelShader(CompileShader(ps_5_0, PS_RadialBlur()));
    }
}