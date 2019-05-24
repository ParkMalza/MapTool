#include "000_Header.fx"
#include "000_Terrain.fx"
#include "000_Light.fx"
#include "000_Model.fx"

matrix Views[6];
TextureCube CubeMap;
float4 collBoxColor[80];

cbuffer CB_Projector
{
    Matrix ProjectorView;
    Matrix ProjectorProjection;

    float4 ProjectorColor;
};
Texture2D ProjectorMap;
Texture2DArray ProjectorMaps;

struct GeometryOutput
{
    float4 Position : SV_Position0;
    float3 oPosition : Position2;
    float3 wPosition : Position3;
   // float4 sPosition : Position4;

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

        //[unroll(3)]
        for (vertex = 0; vertex < 3; vertex++)
        {
            output.Position = mul(input[vertex].Position, Views[i]); //우리가 받아온 뷰로 뷰 변환
            output.Position = mul(output.Position, Projection);

            output.wPosition = input[vertex].wPosition;
            output.oPosition = input[vertex].oPosition;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.Uv = input[vertex].Uv;
            output.InstID = input[vertex].InstID;
            stream.Append(output);
        }
        stream.RestartStrip();
    }

}


void ProjectorPosition(inout float4 wvp, float4 position)
{
    wvp = WorldPosition(position);
    wvp = mul(wvp, ProjectorView);
    wvp = mul(wvp, ProjectorProjection);
}
//-----------------------------------------------------------------------------

MainOutput VS_Terrain(VertexColorTextureNormalTangent input)
{
    MainOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);
    output.Color = input.Color;

    output.oPosition = input.Position.xyz;
    output.wPosition = output.Position.xyz;
    output.wvpPosition = output.Position;
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;

    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);   
    output.sPosition = mul(output.sPosition, ShadowProjection);

    ProjectorPosition(output.wvpPosition, input.Position);

    //
    output.InstID = 0;

    return output;
}

MainOutput VS_InstanceMesh(MeshVertexInput input)
{
    MainOutput output;

    output.Position = mul(input.Position, input.Transform);
 
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.InstID = input.InstID;
    output.Uv = input.Uv;
    //
    output.Color = 0;
    output.oPosition = input.Position.xyz;
    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);
    output.wvpPosition = output.Position;

    return output;
}

float4 PS_PreRender(GeometryOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    Texture(Material.Specular, SpecularMap, input.Uv);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    return  color;
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

    float2 size = 1.0f / ShadowMapSize;
    float2 offsets[] =
    {
        float2(+size.x, -size.y), float2(0.0f, -size.y), float2(-size.x, -size.y),
            float2(+size.x, 0.0f), float2(0.0f, 0.0f), float2(-size.x, 0.0f),
            float2(+size.x, +size.y), float2(0.0f, +size.y), float2(-size.x, +size.y),
    };

    float sum = 0.0f;
    float2 uv = 0.0f;
        
    depth = input.sPosition.z;

        //[unroll(9)]
    for (int i = 0; i < 9; i++)
    {
        uv = input.sPosition.xy + offsets[i];
        sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
    }

    factor = sum / 9.0f;


    factor = saturate(factor + depth);

    return float4(color.rgb * factor, 1);
}



float4 PS_Instance_Mesh(MainOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, TextureType[input.InstID]);
    Textures(Material.Diffuse, Maps, uvw);
    InstanceNormalMapping(uvw, input.Normal, input.Tangent); //
    Textures(Material.Specular, SpecularMaps, uvw);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);
    
    return Colors[input.InstID] * color;
}

float4 PS_CollBox(MainOutput input) : SV_TARGET0
{
    return collBoxColor[input.InstID];
    //return float4(0, 1, 1, 1);
}

float4 PS_Terrain(MainOutput input) : SV_TARGET0
{
    float4 diffuse2 = GetTerrainColors(input.Uv, input.Color);
    float3 grid = GetGridLineColor(input.oPosition);

    float4 color = 0;
    TerrainComputeLight(color, diffuse2, input.Normal);
    TerrainPointLight(color, input.oPosition);
    TerrainSpotLight(color, input.oPosition);

    float2 uv = 0;
    uv.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    uv.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

      [flatten] //화면 안에 들어오는지 체크 0~1을 벗어나면 false
    if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
    {
        float3 uvw = float3(uv, BrushType - 1);
        float4 map = ProjectorMaps.Sample(LinearSampler, uvw);
        map *= ProjectorColor;
        color = lerp(color, map, map.a);
    }

    input.sPosition.xyz /= input.sPosition.w;

 
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
       input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
        return color + float4(grid, 1);

    input.sPosition.x = input.sPosition.x * 0.5f + 0.5f;
    input.sPosition.y = -input.sPosition.y * 0.5f + 0.5f;
    input.sPosition.z -= ShadowBias;

    float depth = 0;
    float factor = 0;

    float2 size = 1.0f / ShadowMapSize;
    float2 offsets[] =
    {
        float2(+size.x, -size.y), float2(0.0f, -size.y), float2(-size.x, -size.y),
            float2(+size.x, 0.0f), float2(0.0f, 0.0f), float2(-size.x, 0.0f),
            float2(+size.x, +size.y), float2(0.0f, +size.y), float2(-size.x, +size.y),
    };

    float sum = 0.0f;
    uv = 0.0f;
        
    depth = input.sPosition.z;

     //[unroll(9)]
    for (int i = 0; i < 9; i++)
    {
        uv = input.sPosition.xy + offsets[i];
        sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
    }

    factor = sum / 9.0f;


    factor = saturate(factor + depth);

    //return color + float4(grid, 1); //기존 렌더링
    return float4(color.rgb * factor, 1) + float4(grid, 1); //그림자 그려버리기

}


//-----------------------------------------------------------------------------

RasterizerState RS
{
    CullMode = Front; //Back 뒷면처리 None 암것도 안함
};

RasterizerState RS2
{
    FillMode = Wireframe;
};

DepthStencilState DS
{
    DepthEnable = false;
};

technique11 T0
{
    //P_RS_VP(P0, RS, VS_Main_Depth, PS_Depth) //사용 x
    P_VGP(P0, VS_Model_GS, GS_PreRender, PS_PreRender) //큐브 안받는 모델
    P_VGP(P1, VS_Animation_GS, GS_PreRender, PS_PreRender) //큐브 안받는 모델 애니
    P_VP(P2, VS_Model, PS_Cube) //큐브 받는 모델
 P_VGP(P3, VS_Model_GS, GS_PreRender, PS_PreRender) //더미
 P_VGP(P4, VS_Model_GS, GS_PreRender, PS_PreRender) //더미
 P_VGP(P5, VS_Model_GS, GS_PreRender, PS_PreRender) //더미
 P_VGP(P6, VS_Model_GS, GS_PreRender, PS_PreRender) //더미
 P_VGP(P7, VS_Model_GS, GS_PreRender, PS_PreRender) //더미
 P_VGP(P8, VS_Model_GS, GS_PreRender, PS_PreRender) //더미
 P_VGP(P9, VS_Model_GS, GS_PreRender, PS_PreRender) //더미

    P_RS_VP(P10, RS, VS_Model_Depth, PS_Depth) //모델 그림자
    P_RS_VP(P11, RS, VS_Animation_Depth, PS_Depth) //모델 애니 그림자
    P_RS_VP(P12, RS, VS_Instance_Mesh_Depth, PS_Depth)  //메쉬 그림자

    P_VP(P13, VS_Main, PS) //더미
    P_VP(P14, VS_Model, PS) //모델 렌더링
    P_VP(P15, VS_Animation, PS) //모델 애니 렌더링
    P_VP(P16, VS_InstanceMesh, PS_Instance_Mesh) //메쉬 렌더링
    P_RS_DSS_VP(P17,RS2, DS, VS_InstanceMesh, PS_CollBox) //메쉬 콜박스

    P_RS_VP(P18, RS, VS_Terrain_Depth, PS_Depth)
    P_VP(P19, VS_Terrain, PS_Terrain) //터레인
}