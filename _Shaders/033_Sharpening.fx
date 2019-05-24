#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    float2 MapSize;
    float Sharpening;
};

//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : Uv0;
    float2 Uv1 : Uv1;
    float2 Uv2 : Uv2;
    float2 Uv3 : Uv3;
    float2 Uv4 : Uv4;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, View2D);
    output.Position = mul(output.Position, Projection2D);
    float2 offset = 1 / MapSize;
    output.Uv = input.Uv;
    output.Uv1 = input.Uv + float2(0, -offset.y);
    output.Uv2 = input.Uv + float2(-offset.x, 0);
    output.Uv3 = input.Uv + float2(+offset.x, 0);
    output.Uv4 = input.Uv + float2(0, +offset.y);

    return output;
}

Texture2DArray Map;
//피카소 그림 느낌난다..
float4 PS_Sharpening(VertexOutput input) : SV_TARGET0
{
    float4 center = DiffuseMap.Sample(LinearSampler, input.Uv);
    float4 top = DiffuseMap.Sample(LinearSampler, input.Uv1);
    float4 left = DiffuseMap.Sample(LinearSampler, input.Uv2);
    float4 right = DiffuseMap.Sample(LinearSampler, input.Uv3);
    float4 bottom = DiffuseMap.Sample(LinearSampler, input.Uv4);

   // float4 color = (center + top + left + right + bottom) / 5.0f;
    float4 color = 4 * center - top - left - right - bottom;

   // return float4(color.rgb, 1);
    return center + Sharpening * color;
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
        SetPixelShader(CompileShader(ps_5_0, PS_Sharpening()));
    }
}