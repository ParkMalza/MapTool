#include "000_Header.fx"
cbuffer CB_Geo_Rain
{
    float4 Color;
    
    float3 Velocity;
    float DrawDistance;

    float3 Origin;
    float CB_Rain_Padding;

    float3 Extent;
};
//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------
struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : Uv0;
    float2 Scale : Scale0;
    uint VertexId : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : POSITION0;
    float2 Uv : Uv0;
    float2 Scale : Scale0;
    uint VertexId : SV_VertexID;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = input.Position;
    output.Uv = input.Uv;
    output.Scale = input.Scale;

    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
    float2 Scale : Scale0;
    float Alpha : Alpha0;
    uint VertexId : SV_VertexID;
};

static const float2 Uvs[4] =
{
    float2(0, 1), float2(0, 0), float2(1,1), float2(1, 0)
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    GeometryOutput output;
    
    float3 velocity = Velocity;
    velocity.xz /= input[0].Scale.y * 0.1f; //빗방울이 크면클수록 빨리 떨어져라

    float3 displace = Time * velocity; //시간에 따라떨어지는 양 조절
    input[0].Position.xyz = Origin + (Extent + (input[0].Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f); //
    

    [roll(4)]
    for (int i = 0; i < 4; i++)
    {
        float4 position = WorldPosition(input[0].Position);
    
        float3 up = normalize(-velocity);
        float3 forward = position.xyz - ViewPosition();
        float3 right = normalize(cross(up, forward));

        position.xyz += (input[i].Uv.x - 0.5f) * right * input[0].Scale.x;
        position.xyz += (1.5f - input[i].Uv.y * 1.5f) * up * input[0].Scale.y;
        position.w = 1.0f;

        output.Position = ViewProjection(position);
        //output.Uv = input[i].Uv;
        output.Uv = Uvs[i];

        float alpha = cos(Time + (input[0].Position.x + input[0].Position.z));
        alpha = saturate(1.5f + alpha / DrawDistance * 2);

        output.Alpha = 0.2f * saturate(1 - output.Position.z / DrawDistance) * alpha;

        stream.Append(output);
    }
        
}
Texture2DArray Map;

float4 PS(GeometryOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, input.VertexId);
   // float4 color = DiffuseMap.Sample(Sampler, input.Uv);
    float4 color = Map.Sample(Sampler, uvw);

    color.rgb += Color.rgb * (1 + input.Alpha) * 2.0f;
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