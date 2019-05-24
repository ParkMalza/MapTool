#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

matrix Views[6];
TextureCube CubeMap;

struct GeometryOutput
{
    float4 Position : SV_Position0;
    float3 oPosition : Position2;
    float3 wPosition : Position3;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    uint TargetIndex : SV_RenderTargetArrayIndex;
};

    ////PreRender
    //P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender) //�޽�
    //P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender) //ť�� �ȹ޴� ��
    //P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender) //�ϴ�

    ////Render
    //P_VP(P3, VS_Main, PS)  //�޽�
    //P_VP(P4, VS_Model, PS) //ť�� �ȹ޴� ��
    //P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)  //�ϴ�

    ////CubeMap Render
    //P_VP(P6, VS_Main, PS_Cube)
    //P_VP(P7, VS_Model, PS_Cube) //ť��� �� (������������)

[maxvertexcount(18)]
void GS_PreRender(triangle MainOutput_GS input[3], inout TriangleStream<GeometryOutput> stream)
{
    int vertex = 0;
    GeometryOutput output;
    //[unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i; //����� �׷�����

        //[unroll(3)]
        for (vertex = 0; vertex < 3; vertex ++)
        {
            output.Position = mul(input[vertex].Position, Views[i]); //�츮�� �޾ƿ� ��� �� ��ȯ
            output.Position = mul(output.Position, Projection);

            output.wPosition = input[vertex].wPosition;
            output.oPosition = input[vertex].oPosition;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.Uv = input[vertex].Uv;

            stream.Append(output);
        }
        stream.RestartStrip();
    }

}

float4 PS_PreRender(GeometryOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);
    
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    return color;
}

float4 PS_Sky_PreRender(GeometryOutput input) : SV_TARGET0
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}

// ���� ���� ����� �������� ���� ������ �׷��� ȿ���� �ش�!
float4 PS(MainOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);

    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    return color;
}

float4 PS_Cube(MainOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);

    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    float3 eye = normalize(input.wPosition - ViewPosition());
    float3 r = reflect(eye, normalize(input.Normal));
    color *= (0.85f + CubeMap.Sample(LinearSampler, r) * 0.75f);
    return color;
}

float4 PS_Sky(MainOutput input) : SV_TARGET0
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}

//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------

RasterizerState RS
{
    FrontCounterClockWise = true;
};


DepthStencilState DS
{
    DepthEnable = false;
};

technique11 T0
{
    //PreRender
    P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender) //�޽�
    P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender) //ť�� �ȹ޴� ��
    P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender) //�ϴ�

    //Render
    P_VP(P3, VS_Main, PS)  //�޽�
    P_VP(P4, VS_Model, PS) //ť�� �ȹ޴� ��
    P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)  //�ϴ�

    //CubeMap Render
    P_VP(P6, VS_Main, PS_Cube)
    P_VP(P7, VS_Model, PS_Cube) //ť��� �� (������������)

    //Box
    
}