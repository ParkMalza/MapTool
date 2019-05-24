#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    float2 Offset;
    float2 Amount;
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
//�ﷷ�Ÿ�
float4 PS_Wiggle(VertexOutput input) : SV_TARGET0
{
    float2 uv = input.Uv;
    //�Ϲ������� X�� sin���� �δ°�찡 ����. 0���� ����..
    uv.x += sin(Time + uv.x * Offset.x) * Amount.x;
    uv.y += cos(Time + uv.y * Offset.y) * Amount.y;

    return DiffuseMap.Sample(LinearSampler, uv);
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
        SetPixelShader(CompileShader(ps_5_0, PS_Wiggle()));
    }
}