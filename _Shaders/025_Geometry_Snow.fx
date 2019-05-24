#include "000_Header.fx"

cbuffer CB_Snow
{
    float4 Color;
    
    float3 Velocity;
    float DrawDistance;

    float3 Origin;
    float Turbulence;

    float3 Extent;
};

//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------
struct VertexInput
{
    float4 Position : POSITION0;
    float2 Random : Random0;
    float Scale : Scale0;

    uint VertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : POSITION0;
    float2 Random : Random0;
    float Scale : Scale0;

    uint VertexID : VertexID;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = input.Position; //넘겨쥼
    output.Scale = input.Scale;
    output.Random = input.Random;
    output.VertexID = input.VertexID;

    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
    float Alpha : Alpha0;

    uint VertexID : VertexID;
};

static const float2 Uvs[4] =
{
    float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 displace = Time * Velocity;
    float2 size = input[0].Scale * 0.5f;

    input[0].Position.x += cos(Time - input[0].Random.x) * Turbulence;
    input[0].Position.z += cos(Time - input[0].Random.y) * Turbulence;
    input[0].Position.xyz = Origin + (Extent + (input[0].Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f);
    //범위 밖으로 나가면 그려지지 않음..
    
    //빌보드 생성
    float3 up = normalize(-Velocity);
    float3 forward = input[0].Position.xyz - ViewPosition(); //카메라 향하게
    float3 right = normalize(cross(up, forward));

    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1.0f); //좌하단
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1.0f); //좌상단
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1.0f); //우하단
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1.0f); //우상단

    GeometryOutput output;

    [roll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = WorldPosition(position[i]);

        position[i].xyz += (Uvs[i].x - 0.5f) * right * input[0].Scale;
        position[i].xyz += (1.5f - Uvs[i].y * 1.5f) * up * input[0].Scale;
        position[i].w = 1.0f;

        output.Position = ViewProjection(output.Position);
        output.Uv = Uvs[i];

        float alpha = cos(Time + (input[0].Position.x + input[0].Position.z)); //투명도 변화
        alpha = saturate(1.5f + alpha / DrawDistance * 2); //1.5f 와 2는 임의로 정해놓은 상수

        output.Alpha = 0.5f * saturate(1 - output.Position.z / DrawDistance) * alpha;
        output.VertexID = input[0].VertexID;

        stream.Append(output);
    }
}

float4 PS(GeometryOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(Sampler, input.Uv);
    color.rgb += Color.rgb * (1 + input.Alpha) * 2.0f; //색상 비례해서 알파값 찾기
    color.a = color.a * (input.Alpha * 1.5f);

    return float4(color.rgb, color.a);
}

BlendState AlphaBlend
{
    BlendEnable[0] = true;
    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 0x0F;
};

//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
RasterizerState Wire
{
    FillMode = WireFrame;
};

technique11 T0
{
    pass P0
    {
        SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFF);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(Wire);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}