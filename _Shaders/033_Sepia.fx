#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    matrix Sepia;
    float Strength;
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

//spia 색상 검색..
float4 PS_Full(VertexOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);

    return mul(color, Sepia);  //매트릭스가 앞에 잇으면 열우선, 뒤에 있으면 행우선
}

float4 PS_Patial(VertexOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);

    return lerp(color, mul(color, Sepia), Strength); 

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
        SetPixelShader(CompileShader(ps_5_0, PS_Full()));
    }

    pass P1
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Patial()));
    }
}