#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    float2 MapSize;
    float2 Scale;
    float Vignette;
    float test;
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

//망원경 같은 효과
float4 PS_Vignette(VertexOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);

    float radius = length((input.Uv - 0.5f) * 2 / Scale); //uv를 -1 ~ 1까지로
    float value = pow(abs(test + radius), Vignette);

    color.rgb = color.rgb * saturate(1 - value); //0부터 1까지니까 뒤집은거다

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
        SetPixelShader(CompileShader(ps_5_0, PS_Vignette()));
    }
}