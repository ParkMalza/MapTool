#include "000_Header.fx"
#include "000_Light.fx"
uint FixedY = 0; //x축을 고정하겠다

//-----------------------------------------------------------------------------
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
};

billboardOutput VS(billboardInput input)
{
    billboardOutput output;

    output.Position = mul(input.Position, input.Transform);
    output.oPosition = output.Position;
    output.Scale = input.Scale;
    output.InstID = input.InstID;
    output.TreeType = input.TreeType;
    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position0; //픽셸쉐이더로 들어가기 전에 하나는 무조건 sv붙어야함
    float2 Uv : Uv0;
    float3 oPosition : Position1;
    uint TreeType : TreeType0;
    uint InstID : ID0;
};
//static const를 붙이면 상수가 된다.
static const float2 Uvs[4] =
{
    float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
};

[maxvertexcount(4)]
void GS(point billboardOutput input[1], inout TriangleStream<GeometryOutput> stream)
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

    
    GeometryOutput output;
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
        stream.Append(output);
    }
}

Texture2DArray BillboardTextures; //텍스쳐의 크기가 모두 동일해야 한다.

float4 PS(GeometryOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, BillboardsTypes[input.InstID]);

    float4 diffuse = BillboardTextures.Sample(Sampler, uvw);

    if (diffuse.a < 0.3)
        discard;

    TerrainPointLight(diffuse, input.oPosition);
    TerrainSpotLight(diffuse, input.oPosition);

    return diffuse;
}
//-----------------------------------------------------------------------------

RasterizerState RS
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}