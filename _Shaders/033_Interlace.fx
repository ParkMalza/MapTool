#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    float2 MapSize;
    float interlace;
    uint Line;

    float3 LuminanceWeights;
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

//검은 줄 그어짐
float4 PS_Saturation(VertexOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    //floor : 무조건 내림, ceil : 올림, round : 반올림
    [flatten]
    if(floor(input.Uv.y * MapSize.y/*실제 픽셀*/) % Line) 
    {//홀수만 실행
        float gray = dot(color.rgb, LuminanceWeights);
        gray = min(0.999f, gray);

        color.rgb = lerp(color.rgb, color.rgb * gray, interlace);
    }

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
        SetPixelShader(CompileShader(ps_5_0, PS_Saturation()));
    }
}