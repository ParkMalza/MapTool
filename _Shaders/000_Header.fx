cbuffer CB_PerFrame
{
    matrix View;
    matrix ViewInverse;
    matrix Projection;
    matrix VP;
    
    matrix OrthoView;
    matrix FrontOrthoViewProjection;
    matrix UpOrthoViewProjection;
    matrix RightOrthoViewProjection;

    float Time;
};

cbuffer CB_World
{
    matrix World;
};

cbuffer CB_Projector
{
    Matrix ProjectorView;
    Matrix ProjectorProjection;

    float4 ProjectorColor;
};
Texture2D ProjectorMap;
Texture2DArray ProjectorMaps;

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;

Texture2D ShadowMap;
TextureCube SkyCubeMap;

Texture2DArray DiffuseMaps;
Texture2DArray SpecularMaps;
Texture2DArray NormalMaps;

Texture2DArray BillboardTextures; //텍스쳐의 크기가 모두 동일해야 한다.

uint FixedY = 0; //x축을 고정하겠다
int TextureType[200];
float4 Colors[200];
int BillboardsTypes[3500]; //빌보드 인스턴스
Texture2DArray Maps;

SamplerState Sampler;

SamplerState LinearSampler  //offset 이용해서 사진크기 줄여줌
{
    Filter = MIN_MAG_MIP_LINEAR;
    
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

SamplerComparisonState ShadowSampler;

///////////////////////////////////////////////////////////////////////////////

struct Vertex
{
    float4 Position : POSITION0;
};

struct VertexNormal
{
    float4 Position : POSITION0;
    float3 Normal : COLOR0;
};

struct VertexColor
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

struct VertexColorNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTexture
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
};

struct VertexTextureColor
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float4 Color : COLOR0;
};

struct VertexTextureColorNormal
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTextureNormal
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexTextureNormalType
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    uint Type : Type0;
    float3 Normal : NORMAL0;
};

struct VertexColorTextureNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexColorTextureNormalTangent
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

struct VertexTextureNormalBlend
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;
};

struct VertexTextureNormalTangent
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    matrix Transform : InstTransform0;
    uint InstID : SV_InstanceID0;
};

struct VertexTextureNormalTangentBlend
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;
};

///////////////////////////////////////////////////////////////////////////////

float4 WorldPosition(float4 position)
{
    return mul(position, World);
}

float4 ViewProjection(float4 position)
{
    return mul(position, VP);
}

float4 OrthoViewProjectionFront(float4 position)
{
    return mul(position, FrontOrthoViewProjection);
}

float4 OrthoViewProjectionUp(float4 position)
{
    return mul(position, UpOrthoViewProjection);
}

float4 OrthoViewProjectionRight(float4 position)
{
    return mul(position, RightOrthoViewProjection);
}

float3 WorldNormal(float3 normal)
{
    return mul(normal, (float3x3) World);
}

float3 WorldTangent(float3 tangent)
{
    return mul(tangent, (float3x3) World);
}

float3 ViewPosition()
{
    return ViewInverse._41_42_43;
}

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 temp = t.Sample(samp, uv);

    [flatten] //하나라도 0보다 크면 true
    if (any(temp) == true)
        color = color * temp;
}

void TextureArray(inout float4 color, float4 inst)
{
    [flatten] //하나라도 0보다 크면 true
    if (any(inst) == true)
        color = color * inst;
}

//기본 샘플러로 동작
void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

void Textures(inout float4 color, Texture2DArray t, float3 uvw, SamplerState samp)
{
    float4 temp = t.Sample(samp, uvw);

    [flatten]
    if (any(temp) == true)
        color = color * temp;
}

void Textures(inout float4 color, Texture2DArray t, float3 uvw)
{
    Textures(color, t, uvw, LinearSampler);
}

void ProjectorPosition(inout float4 wvp, float4 position)
{
    wvp = WorldPosition(position);
    wvp = mul(wvp, ProjectorView);
    wvp = mul(wvp, ProjectorProjection);
}
///////////////////////////////////////////////////////////////////////////////

struct DataDesc
{
    float4 Center;
    float4 Apex;

    float Height;
};
DataDesc Data;


struct VertexMesh
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    matrix Transform : InstTransform0;
    uint InstID : SV_InstanceID0;
};

struct MeshVertexInput
{
    float4 Position : POSITION0;
    //float3 wPosition : POSITION1;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    matrix Transform : InstTransform0;
    uint InstID : SV_InstanceID0;
};

struct SkyVertexOutput
{
    float4 Position : SV_Position0;
    float4 oPosition : Position1;
    float3 Normal : Normal0;
    float2 Uv : Uv0;
    
    float Height : Height0;
};

struct MainOutput
{
    float4 Position : SV_Position0;
    float4 wvpPosition : Position1;
    float3 oPosition : Position2;
    float3 wPosition : Position3;
    float4 sPosition : Position4;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    uint InstID : ID0;
    float4 Color : Color0;
};

cbuffer CB_Shadow
{
    matrix ShadowView;
    matrix ShadowProjection;

    float2 ShadowMapSize;
    float ShadowBias;

    uint ShadowIndex;
};

struct DepthOutput
{
    float4 Position : SV_Position0;
    float4 sPosition : Position1;
};

///////////////////////////////////////////////////////////////////////////////

struct billboardInput
{
    float4 Position : Position0;
    float2 Scale : Scale0;
    uint TreeType : TreeType0;
    matrix Transform : InstTransform0;
    uint InstID : SV_InstanceID0;
};

struct billboardOutput
{
    float4 Position : Position0;
    float3 oPosition : Position1;
    float2 Scale : Scale0;
    uint TreeType : TreeType0;
    uint InstID : SV_InstanceID0;

    //float4 wvpPosition : Position2;
    float3 wPosition : Position3;
    float4 sPosition : Position4;
    //float2 Uv : Uv0;
};

billboardOutput VS_Billboard(billboardInput input)
{
    billboardOutput output;

    output.Position = mul(input.Position, input.Transform);
    output.oPosition = output.Position.xyz;
    output.Scale = input.Scale;
    output.InstID = input.InstID;
    output.TreeType = input.TreeType;

    //output.wPosition = output.Position.xyz;

    output.sPosition = mul(input.Position, input.Transform);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);
    return output;
}

struct Billboard_GeometryOutput
{
    float4 Position : SV_Position0; //픽셸쉐이더로 들어가기 전에 하나는 무조건 sv붙어야함
    float2 Uv : Uv0;
    float3 oPosition : Position1;
    uint TreeType : TreeType0;
    uint InstID : ID0;

    float4 wvpPosition : Position2;
    //float3 wPosition : Position3;
    float4 sPosition : Position4;

};
///////////////////////////////////////////////////////////////////////////////

DepthOutput VS_Main_Depth(VertexMesh input)
{
    DepthOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, ShadowView);
    output.Position = mul(output.Position, ShadowProjection);

    output.sPosition = output.Position;

    return output;

}

DepthOutput VS_Terrain_Depth(VertexMesh input)
{
    DepthOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, ShadowView);
    output.Position = mul(output.Position, ShadowProjection);

    output.sPosition = output.Position;

    return output;
}

//billboardOutput VS_Instance_Billboard_Depth(VertexMesh input)
//{
//    billboardOutput output;

//    output.Position = mul(input.Position, input.Transform);

//    output.Position = mul(output.Position, ShadowView);
//    output.Position = mul(output.Position, ShadowProjection);

//    output.sPosition = output.Position;

//    return output;
//}

DepthOutput VS_Instance_Mesh_Depth(VertexMesh input)
{
    DepthOutput output;

    output.Position = mul(input.Position, input.Transform);
    
    output.Position = mul(output.Position, ShadowView);
    output.Position = mul(output.Position, ShadowProjection);

    output.sPosition = output.Position;

    return output;
}

float4 PS_Depth(DepthOutput input) : SV_Target0
{
    float depth = input.sPosition.z / input.sPosition.w;
    
    return float4(0, 0, depth, 1);
}

//float4 PS_Billboard_Depth(Billboard_GeometryOutput input) : SV_Target0
//{
//    float depth = input.sPosition.z / input.sPosition.w;
    
//    return float4(0, 0, depth, 1);
//}

///////////////////////////////////////////////////////////////////////////////
SkyVertexOutput VS_SKY(VertexTextureNormal input)
{
    SkyVertexOutput output;

    output.Height = input.Position.y;
    output.oPosition = input.Position;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);

    output.Uv = input.Uv;

    return output;
}
/////////////////////////////////////////////////////////////////////////////////

MainOutput VS_Main(VertexMesh input)
{
    MainOutput output;

    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;
    output.InstID = input.InstID;
    output.Color = 0;

    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);
    return output;
}

MainOutput VS_InstanceMesh(VertexMesh input)
{
    MainOutput output;

    output.Position = mul(input.Position, input.Transform);
 
    output.oPosition = input.Position.xyz;
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;
    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.InstID = input.InstID;
    output.Uv = input.Uv;
    //
    output.Color = 0;

    output.sPosition = mul(input.Position, input.Transform);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    return output;
}
///////////////////////////////////////////////////////////////////////////////
struct MainOutput_GS
{
    float4 Position : Position0;
    float3 oPosition : Position2;
    float3 wPosition : Position3;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    uint InstID : ID0;
    float4 Color : Color0;

    float2 Scale : Scale0;
    uint TreeType : TreeTyoe0;
};

MainOutput_GS VS_Main_GS(VertexMesh input)
{
    MainOutput_GS output;

    //월드 변환만 한다.
    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;

    output.InstID = 0;
    output.Color = 0;
    output.Scale = 0;
    output.TreeType = 0;

    return output;
}

MainOutput_GS VS_Billboard_GS(billboardInput input)
{
    MainOutput_GS output;

    output.Position = mul(input.Position, input.Transform);
    output.oPosition = output.Position.xyz;
    output.Scale = input.Scale;
    output.InstID = input.InstID;
    output.TreeType = input.TreeType;

    output.Color = 0;
    output.Normal = 0;
    output.Tangent = 0;
    output.Uv = 0;
    output.wPosition = output.Position.xyz;
    return output;
}

MainOutput_GS VS_InstanceMesh_GS(MeshVertexInput input)
{
    MainOutput_GS output;

    output.Position = mul(input.Position, input.Transform);
 
    output.wPosition = output.Position.xyz;
    
    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.InstID = input.InstID;
    output.Uv = input.Uv;
    output.Color = 0;
    output.oPosition = input.Position.xyz;

    output.Scale = 0;
    output.TreeType = 0;
    
    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct WaterOutput
{
    float4 Position : SV_Position0;
    float4 wvpPosition : Position1;
    float3 oPosition : Position2;
    float3 wPosition : Position3;
    float4 sPosition : Position4;

    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    float Clip : SV_ClipDistance0; //PS에서 0 밑의의 값을 버린다.
    //SV컬링 VS에서 버리겠다.
};

cbuffer CB_Water
{
    float4 WaterClipPlane;
};

WaterOutput VS_Main_Water(VertexMesh input)
{
    WaterOutput output;

    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;


    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    output.Clip = dot(WorldPosition(input.Position), WaterClipPlane);

    return output;
}

///////////////////////////////////////////////////////////////////////////////

#define P_V(name, vs) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(NULL); \
}

#define P_VP(name, vs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VP(name, rs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_DSS_VP(name, dss, vs, ps) \
pass name \
{ \
    SetDepthStencilState(dss, 0); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_BS_VP(name, bs, vs, ps) \
pass name \
{ \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VP(name, rs, dss, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 0); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_BS_VP(name, rs, bs, vs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetBlendState(bs, float4(0, 0, 0, 0), 0xFF); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_VGP(name, vs, gs, ps) \
pass name \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_VGP(name, rs, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}

#define P_RS_DSS_VGP(name, rs, dss, vs, gs, ps) \
pass name \
{ \
    SetRasterizerState(rs); \
    SetDepthStencilState(dss, 0); \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetGeometryShader(CompileShader(gs_5_0, gs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
}