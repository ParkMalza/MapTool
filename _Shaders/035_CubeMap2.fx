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
    //P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender) //메쉬
    //P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender) //큐브 안받는 모델
    //P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender) //하늘

    ////Render
    //P_VP(P3, VS_Main, PS)  //메쉬
    //P_VP(P4, VS_Model, PS) //큐브 안받는 모델
    //P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)  //하늘

    ////CubeMap Render
    //P_VP(P6, VS_Main, PS_Cube)
    //P_VP(P7, VS_Model, PS_Cube) //큐브용 모델 (프리렌더없음)

[maxvertexcount(18)]
void GS_PreRender(triangle MainOutput_GS input[3], inout TriangleStream<GeometryOutput> stream)
{
    int vertex = 0;
    GeometryOutput output;
    //[unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i; //몇번에 그려질지

        //[unroll(3)]
        for (vertex = 0; vertex < 3; vertex ++)
        {
            output.Position = mul(input[vertex].Position, Views[i]); //우리가 받아온 뷰로 뷰 변환
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

// 밝은 빛만 남기는 과정들을 통해 선명한 그래픽 효과를 준다!
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
    P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender) //메쉬
    P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender) //큐브 안받는 모델
    P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender) //하늘

    //Render
    P_VP(P3, VS_Main, PS)  //메쉬
    P_VP(P4, VS_Model, PS) //큐브 안받는 모델
    P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)  //하늘

    //CubeMap Render
    P_VP(P6, VS_Main, PS_Cube)
    P_VP(P7, VS_Model, PS_Cube) //큐브용 모델 (프리렌더없음)

    //Box
    
}