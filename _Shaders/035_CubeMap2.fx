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
    //P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender) //¸Þ½¬
    //P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender) //Å¥ºê ¾È¹Þ´Â ¸ðµ¨
    //P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender) //ÇÏ´Ã

    ////Render
    //P_VP(P3, VS_Main, PS)  //¸Þ½¬
    //P_VP(P4, VS_Model, PS) //Å¥ºê ¾È¹Þ´Â ¸ðµ¨
    //P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)  //ÇÏ´Ã

    ////CubeMap Render
    //P_VP(P6, VS_Main, PS_Cube)
    //P_VP(P7, VS_Model, PS_Cube) //Å¥ºê¿ë ¸ðµ¨ (ÇÁ¸®·»´õ¾øÀ½)

[maxvertexcount(18)]
void GS_PreRender(triangle MainOutput_GS input[3], inout TriangleStream<GeometryOutput> stream)
{
    int vertex = 0;
    GeometryOutput output;
    //[unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i; //¸î¹ø¿¡ ±×·ÁÁúÁö

        //[unroll(3)]
        for (vertex = 0; vertex < 3; vertex ++)
        {
            output.Position = mul(input[vertex].Position, Views[i]); //¿ì¸®°¡ ¹Þ¾Æ¿Â ºä·Î ºä º¯È¯
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

// ¹àÀº ºû¸¸ ³²±â´Â °úÁ¤µéÀ» ÅëÇØ ¼±¸íÇÑ ±×·¡ÇÈ È¿°ú¸¦ ÁØ´Ù!
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
    P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender) //¸Þ½¬
    P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender) //Å¥ºê ¾È¹Þ´Â ¸ðµ¨
    P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender) //ÇÏ´Ã

    //Render
    P_VP(P3, VS_Main, PS)  //¸Þ½¬
    P_VP(P4, VS_Model, PS) //Å¥ºê ¾È¹Þ´Â ¸ðµ¨
    P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)  //ÇÏ´Ã

    //CubeMap Render
    P_VP(P6, VS_Main, PS_Cube)
    P_VP(P7, VS_Model, PS_Cube) //Å¥ºê¿ë ¸ðµ¨ (ÇÁ¸®·»´õ¾øÀ½)

    //Box
    
}