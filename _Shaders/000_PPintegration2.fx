#include "000_Header.fx"
#include "000_Terrain.fx"
#include "000_Light.fx"
#include "000_Model.fx"

matrix Views[6];
TextureCube CubeMap;

static const float2 Uvs[4] =
{
    float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
};

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
    float4 Color : Color0;

    uint TargetIndex : SV_RenderTargetArrayIndex0;
};

[maxvertexcount(4)]
void GS_Billboard(point billboardOutput input[1], inout TriangleStream<Billboard_GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 forward = ViewPosition() - input[0].Position.xyz;

    [flatten]
    if (FixedY == 1)
        forward.y = 0.0f;

    forward = normalize(forward);
    float3 right = cross(forward, up);

    float2 size = input[0].Scale * 0.5f;


    float3 position[4];
    position[0] = float3(input[0].Position.xyz - size.x * right - size.y * up);
    position[1] = float3(input[0].Position.xyz - size.x * right + size.y * up);
    position[2] = float3(input[0].Position.xyz + size.x * right - size.y * up);
    position[3] = float3(input[0].Position.xyz + size.x * right + size.y * up);

    
    Billboard_GeometryOutput output;
    float windSpeed = 1.5f;
    output.TreeType = input[0].TreeType;
    [roll(4)]
    for (int i = 0; i < 4; i++)
    {
        //흠.. (1 - Uvs[i].y) 는 바닥을 고정시키기 위함인것 같음
        float wind = (1 - Uvs[i].y) * sin(Time * windSpeed + position[i].x + position[i].y);
        position[i].x += wind;
       // position[i].x += sin(Time * position[i].x * 0.2f) * 0.08f;
        output.Position = float4(position[i], 1);
        output.Position = ViewProjection(output.Position);
        output.oPosition = input[0].oPosition;
        output.Uv = Uvs[i];
        output.InstID = input[0].InstID;
        
        output.wvpPosition = output.Position;
        output.sPosition = input[0].sPosition;
        stream.Append(output);
    }
}

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
            output.Color = input[vertex].Color;
            stream.Append(output);
        }
        stream.RestartStrip();
    }

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


///////////////////////////////////////////////////////////////////////////////
float4 PS_Model_CubeRener(GeometryOutput input) : SV_TARGET0
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
///////////////////////////////////////////////////////////////////////////////
float4 PS_SKY(GeometryOutput input) : SV_TARGET0
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition.xyz);
}

float4 PS_SKY_CubeMap(SkyVertexOutput input) : SV_TARGET0
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition.xyz);
}
///////////////////////////////////////////////////////////////////////////////

float4 PS_Terrain_CubeRender(GeometryOutput input) : SV_TARGET0
{
    float4 diffuse2 = GetTerrainColors(input.Uv, input.Color);
    float3 grid = GetGridLineColor(input.oPosition);

    float4 color = 0;
    TerrainComputeLight(color, diffuse2, input.Normal);
    TerrainPointLight(color, input.oPosition);
    TerrainSpotLight(color, input.oPosition);
 
    return float4(color.rgb, 1) + float4(grid, 1); //그림자 그려버리기
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
///////////////////////////////////////////////////////////////////////////////

float4 PS_Instance_Mesh_CubRender(GeometryOutput input) : SV_TARGET0
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

///////////////////////////////////////////////////////////////////////////////

float4 PS_Billboard_CubeRender(Billboard_GeometryOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, BillboardsTypes[input.InstID]);

    float4 diffuse = BillboardTextures.Sample(Sampler, uvw);

    if (diffuse.a < 0.3)
        discard;

    TerrainPointLight(diffuse, input.oPosition);
    TerrainSpotLight(diffuse, input.oPosition);

    return diffuse;
}

///////////////////////////////////////////////////////////////////////////////

float4 PS(MainOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    Texture(Material.Specular, SpecularMap, input.Uv);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    float2 uv = 0;
    uv.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    uv.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

    input.sPosition.xyz /= input.sPosition.w;

 
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
       input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
        return float4(color.rgb, 1);

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

    return float4(color.rgb * factor, 1);
}

///////////////////////////////////////////////////////////////////////////////

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

    float2 uv = 0;
    uv.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    uv.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

    input.sPosition.xyz /= input.sPosition.w;

 
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
       input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
        return float4(color.rgb, 1);

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

    return color * factor;
}

float4 PS_CollBox(MainOutput input) : SV_TARGET0
{
    return float4(0, 1, 0, 1);
}


float4 PS_Billboard(Billboard_GeometryOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, BillboardsTypes[input.InstID]);

    float4 diffuse = BillboardTextures.Sample(Sampler, uvw);

    if (diffuse.a < 0.3)
        discard;

    TerrainPointLight(diffuse, input.oPosition);
    TerrainSpotLight(diffuse, input.oPosition);

    float2 uv = 0;
    uv.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    uv.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

    input.sPosition.xyz /= input.sPosition.w;

 
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
       input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f)
        return diffuse;

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

    return diffuse * factor;
}

//-----------------------------------------------------------------------------

RasterizerState RS
{
    CullMode = Front; //Back 뒷면처리 None 암것도 안함
};

RasterizerState RS_ClockWise
{
    FrontCounterClockWise = true;
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
    P_VGP(P0, VS_Model_GS, GS_PreRender, PS_Model_CubeRener) //큐브 안받는 모델
    P_VGP(P1, VS_Animation_GS, GS_PreRender, PS_Model_CubeRener) //큐브 안받는 모델 애니
    P_VP(P2, VS_Model, PS_Cube) //큐브 받는 모델
    P_RS_DSS_VGP(P3, RS_ClockWise, DS, VS_Main_GS, GS_PreRender, PS_SKY) //하늘 프리랜더
    P_RS_DSS_VP(P4, RS_ClockWise, DS, VS_SKY, PS_SKY_CubeMap) //하늘 랜더
    P_VGP(P5, VS_Main_GS, GS_PreRender, PS_Terrain_CubeRender) //터레인 큐브
    P_RS_VP(P6, RS, VS_Terrain_Depth, PS_Depth) //터레인 깊이
    P_VP(P7, VS_Terrain, PS_Terrain) //터레인 렌더링
    P_VGP(P8, VS_InstanceMesh_GS, GS_PreRender, PS_Instance_Mesh_CubRender) //메쉬 큐브
    P_RS_VP(P9, RS, VS_Terrain_Depth, PS_Depth) //더미

    P_RS_VP(P10, RS, VS_Model_Depth, PS_Depth) //모델 그림자
    P_RS_VP(P11, RS, VS_Animation_Depth, PS_Depth) //모델 애니 그림자
    P_RS_VP(P12, RS, VS_Instance_Mesh_Depth, PS_Depth)  //메쉬 그림자

    P_VGP(P13, VS_Billboard, GS_Billboard, PS_Billboard) //빌보드
    P_VP(P14, VS_Model, PS) //모델 렌더링
    P_VP(P15, VS_Animation, PS) //모델 애니 렌더링
    P_VP(P16, VS_InstanceMesh, PS_Instance_Mesh) //메쉬 렌더링
    P_RS_VP(P17, RS2, VS_InstanceMesh, PS_CollBox) //메쉬 콜박스

    P_RS_VP(P18, RS, VS_Terrain_Depth, PS_Depth)  //더미
    P_VP(P19, VS_Terrain, PS_Terrain) //더미
}