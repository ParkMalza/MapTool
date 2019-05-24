#include "000_Header.fx"

cbuffer CB_Rain
{
    float4 Color1;
    
    float3 Velocity;
    float DrawDistance1;

    float3 Origin1;
    float CB_Rain_Padding;

    float3 Extent1;
};

//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------
struct VertexInput
{
    float4 Position : POSITION1;
    float2 Uv : Uv0;
    float2 Scale : Scale0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : Uv0;
    float Alpha : Alpha0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float3 velocity = Velocity;
    velocity.xz /= input.Scale.y * 0.1f; //빗방울이 크면클수록 빨리 떨어져라
    
    float3 displace = Time * velocity; //시간에 따라떨어지는 양 조절
    input.Position.xyz = Origin1 + (Extent1 + (input.Position.xyz + displace) % Extent1) % Extent1 - (Extent1 * 0.5f); //
    
    float4 position = WorldPosition(input.Position);
    
    float3 up = normalize(-velocity);
    float3 forward = position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));
    position.xyz += (input.Uv.x - 0.5f) * right * input.Scale.x;
    position.xyz += (1.5f - input.Uv.y * 1.5f) * up * input.Scale.y;
    position.w = 1.0f;

    output.Position = ViewProjection(position);
    output.Uv = input.Uv;

    float alpha = cos(Time + (input.Position.x + input.Position.z));
    alpha = saturate(1.5f + alpha / DrawDistance1 * 2);

    output.Alpha = 0.2f * saturate(1 - output.Position.z / DrawDistance1) * alpha;

    return output;
}



float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 color = DiffuseMap.Sample(Sampler, input.Uv);
    color.rgb += Color1.rgb * (1 + input.Alpha) * 2.0f;
    color.a = color.a * (input.Alpha * 1.5f);

    return float4(color.rgb, color.a);
    return color;
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
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(Wire);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}