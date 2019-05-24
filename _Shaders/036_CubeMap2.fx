#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

matrix Views[6];
TextureCube CubeMap;

float4 Color[2000];
Texture2DArray Map;

struct GeometryOutput
{
    float4 Position : SV_Position0;
    float3 oPosition : Position2;
    float3 wPosition : Position3;
    float4 sPosition : Position4;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    uint TargetIndex : SV_RenderTargetArrayIndex0;
};

struct GeoMeshOutput
{
    float4 Position : SV_Position0;
    float3 oPosition : Position2;
    float3 wPosition : Position3;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    uint InstID : ID0;

    uint TargetIndex : SV_RenderTargetArrayIndex0;
};

[maxvertexcount(18)]
void GS_PreRender(triangle MainOutput_GS input[3], inout TriangleStream<GeometryOutput> stream)
{
    int vertex = 0;
    GeometryOutput output;
    //[unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i; //몇번에 그려질지
        //output.sPosition = input[0].sPosition;
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

//[maxvertexcount(18)]
//void GS_Mesh_PreRender(triangle InstanceMeshOutput_GS input[3], inout TriangleStream<GeoMeshOutput> stream)
//{
//    int vertex = 0;
//    GeoMeshOutput output;
//    //[unroll(6)]
//    for (int i = 0; i < 6; i++)
//    {
//        output.TargetIndex = i; //몇번에 그려질지
//        output.InstID = input[0].InstID;
//        //[unroll(3)]
//        for (vertex = 0; vertex < 3; vertex++)
//        {
//            output.Position = mul(input[vertex].Position, Views[i]); //우리가 받아온 뷰로 뷰 변환
//            output.Position = mul(output.Position, Projection);

//            output.wPosition = input[vertex].wPosition;
//            output.oPosition = input[vertex].oPosition;
//            output.Normal = input[vertex].Normal;
//            output.Tangent = input[vertex].Tangent;
//            output.Uv = input[vertex].Uv;

//            stream.Append(output);
//        }
//        stream.RestartStrip();
//    }

//}

float4 PS_MeshPreRender(GeoMeshOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, TextureType[input.InstID]);
    Textures(Material.Diffuse, Map, uvw);
    InstanceNormalMapping(uvw, input.Normal, input.Tangent);//
    Textures(Material.Specular, SpecularMaps, uvw);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);
    return Color[input.InstID] * color;
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

float4 PS_PreRenderDepth(GeometryOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);
    
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    float depth = input.sPosition.z / input.sPosition.w;

    return color + depth;
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

    input.sPosition.xyz /= input.sPosition.w;

    [flatten]
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
       input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
        return color;

    input.sPosition.x = input.sPosition.x * 0.5f + 0.5f;
    input.sPosition.y = -input.sPosition.y * 0.5f + 0.5f;
    input.sPosition.z -= ShadowBias;

    float depth = 0;
    float factor = 0;

    [branch] //if else시 사용
    if (ShadowIndex == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, input.sPosition.xy).r; //뒷면 처리
        factor = (float) input.sPosition.z <= depth; //앞면 처리
    }
    else if (ShadowIndex == 1) //PCF
    {
        depth = input.sPosition.z;
        factor = ShadowMap.SampleCmpLevelZero(ShadowSampler, input.sPosition.xy, depth).r;
    }

    factor = saturate(factor + depth);

    return float4(color.rgb * factor, 1);
}

float4 PS_Mesh(MainOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, TextureType[input.InstID]);
    Textures(Material.Diffuse, Map, uvw);
    InstanceNormalMapping(uvw, input.Normal, input.Tangent); //
    Textures(Material.Specular, SpecularMaps, uvw);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);
    return Color[input.InstID] * color;
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

RasterizerState RS2
{
    CullMode = None;
};


DepthStencilState DS
{
    DepthEnable = false;
};

technique11 T0
{
    //0 이랑 3 메쉬
    //2랑 5 스카이
    //1이랑 4 모델
    //PreRender
    P_VGP(P0, VS_Mesh_GS, GS_Mesh_PreRender, PS_MeshPreRender)
    //P_VGP(P0, VS_Main_GS, GS_PreRender, PS_PreRender)
    P_VGP(P1, VS_Model_GS, GS_PreRender, PS_PreRender)
    //P_RS_VGP(P1, RS2, VS_Model_GS_Depth, GS_PreRender, PS_PreRenderDepth)
    P_RS_DSS_VGP(P2, RS, DS, VS_Main_GS, GS_PreRender, PS_Sky_PreRender)

    //Render
    //P_VP(P3, VS_InstanceMesh_Main, PS_Mesh)
    P_VP(P3, VS_Main, PS)
    P_VP(P4, VS_Model, PS)
    P_RS_DSS_VP(P5, RS, DS, VS_Main, PS_Sky)

    //CubeMap Render
    P_VP(P6, VS_Main, PS_Cube)
    P_VP(P7, VS_Model, PS_Cube)
}