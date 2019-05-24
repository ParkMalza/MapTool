#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Values
{
    float Noise;
    int Seed;
    
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
//��������
float4 PS_Shake(VertexOutput input) : SV_TARGET0
{
    float2 uv = input.Uv;
    float noise = Seed * Time;

    float2 distort = 0;
    //fmod : ���� �������� ���簪
    //noise�� Noise�� ���� �������� ���簪�� ���ڴ�
    distort.x = fmod(noise, Noise);
    distort.y = fmod(noise, Noise + test); //������ �ٲ� �����غ�

    return DiffuseMap.Sample(LinearSampler, uv + distort);
}
//��������
float4 PS_Noise(VertexOutput input) : SV_TARGET0
{
    float2 uv = input.Uv;
    float noise = Seed * Time * sin(uv.x + uv.y); //0~1������ ��������...

    float2 distort = 0;
    distort.x = fmod(noise, Noise);
    distort.y = fmod(noise, Noise + test); //������ �ٲ� �����غ�

    return DiffuseMap.Sample(LinearSampler, uv + distort);
}
//���� Ƽ�� ����Ƽ��ó�� ������ ���� ������
float4 PS_Wave(VertexOutput input) : SV_TARGET0
{
    float2 uv = input.Uv;
    float noise = Seed * Time * sin(uv.x + uv.y + Time);

    float2 distort = 0;
    distort.x = fmod(noise, Noise);
    distort.y = fmod(noise, Noise + test); //������ �ٲ� �����غ�

    return DiffuseMap.Sample(LinearSampler, uv + distort);
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
        SetPixelShader(CompileShader(ps_5_0, PS_Shake()));
    }

    pass P1
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Noise()));
    }

    pass P2
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Wave()));
    }
}