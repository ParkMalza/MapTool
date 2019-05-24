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
   // float2 Uv : Uv0;
    float2 Scale : Scale0;
    uint VertexID : SV_VertexID;
};

struct VertexOutput
{
    float4 Position : POSITION0;
    //float2 Uv : Uv0;
    float2 Scale : Scale0;
    uint VertexID : VertexID;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = input.Position;
    //output.Uv = input.Uv;
    output.Scale = input.Scale;
    output.VertexID = input.VertexID;
    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
   // float2 Scale : Scale1;
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
    GeometryOutput output;

    float3 velocity = Velocity;
    velocity.xz /= input[0].Scale.y * 0.1f; //ºø¹æ¿ïÀÌ Å¬¼ö·Ï »¡¸® ³«ÇÏ

    float3 displace = Time * velocity; //¶³¾îÁö´Â ¾ç Á¶Àý
    float2 size = input[0].Scale * 0.5f;

    input[0].Position.xyz = Origin + (Extent + (input[0].Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f); //
    
        
    float3 up = normalize(-velocity);
    float3 forward = input[0].Position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));

    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1.0f); //ÁÂÇÏ´Ü
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1.0f); //ÁÂ»ó´Ü
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1.0f); //¿ìÇÏ´Ü
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1.0f); //¿ì»ó´Ü


    [roll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = WorldPosition(position[i]);

        position[i].xyz += (Uvs[i].x - 0.5f) * right * input[0].Scale.x;
        position[i].xyz += (1.5f - Uvs[i].y * 1.5f) * up * input[0].Scale.y;
        position[i].w = 1.0f;

        output.Position = ViewProjection(output.Position);

        output.Uv = Uvs[i];

        float alpha = cos(Time + (input[0].Position.x + input[0].Position.z));
        alpha = saturate(1.5f + alpha / DrawDistance * 2);
     
        output.Alpha = 0.2f * saturate(1 - output.Position.z / DrawDistance) * alpha;
        output.VertexID = input[0].VertexID;

        stream.Append(output);
    }
        
}
float4 PS(GeometryOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(Sampler, input.Uv);
    color.rgb += Color.rgb * (1 + input.Alpha) * 2.0f;
    color.a = color.a * (input.Alpha * 1.5f);

    return float4(color.rgb, color.a);
  //  return color;
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