#include "000_Header.fx"

cbuffer CB_Render2D
{
    matrix View2D;
    matrix Projection2D;
};

cbuffer CB_Default
{
    float4 Tone;
    float4 Gamma;
    uint nBit;
};

//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, View2D);
    output.Position = mul(output.Position, Projection2D);
    
    output.Uv = input.Uv;

    return output;
}

Texture2DArray Map;

float4 PS(VertexOutput input) : SV_TARGET0
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}


float4 PS_Inverse(VertexOutput input) : SV_TARGET0
{
    return 1.0f - DiffuseMap.Sample(LinearSampler, input.Uv); //1¿¡¼­ »©ÁÜÀ¸·Î½á ¹ÝÀü½ÃÅ²´Ù.
}

//float4 PS_GrayScale(VertexOutput input) : SV_TARGET0
//{
//    float4 pixel = DiffuseMap.Sample(LinearSampler, input.Uv);
//    float color = (pixel.r + pixel.g + pixel.b) / 3.0f;              //¾êº¸´Ù´Â
//    return float4(color, color, color, 1);
//}
//Èæ¹é
float4 PS_GrayScale2(VertexOutput input) : SV_TARGET0            //¾ê·Î ¾´´Ù!!
{
    float3 tone = float3(0.299f, 0.587f, 0.114f);

    float4 pixel = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    float color = dot(pixel.rgb, tone);
    //dot == °öÇÏ°í ´õÇÑ°Í°ú °°´Ù

    return float4(color, color, color, 1);
}
//Åæ»ö±ò
float4 PS_Tone(VertexOutput input) : SV_TARGET0
{
   
    float4 pixel = DiffuseMap.Sample(LinearSampler, input.Uv);
    pixel.r *= Tone.r;
    pixel.g *= Tone.g;
    pixel.b *= Tone.b;

    return float4(pixel.rgb, 1);
}
//ºñÆ® °¨¸¶
float4 PS_Gamma(VertexOutput input) : SV_TARGET0
{
   
    float4 pixel = DiffuseMap.Sample(LinearSampler, input.Uv);
    pixel.r = pow(pixel.r, 1.0f / Gamma.r);
    pixel.g = pow(pixel.g, 1.0f / Gamma.g);
    pixel.b = pow(pixel.b, 1.0f / Gamma.b);

    return float4(pixel.rgb, 1);
}

float4 PS_BitPlannerSlicing(VertexOutput input) : SV_TARGET0
{
    float4 pixel = DiffuseMap.Sample(LinearSampler, input.Uv);

    uint r = uint(pixel.r * 255);
    uint g = uint(pixel.g * 255);
    uint b = uint(pixel.b * 255);

    r >>= (8 - nBit);
    g >>= (8 - nBit);
    b >>= (8 - nBit);

    r <<= (8 - nBit);
    g <<= (8 - nBit);
    b <<= (8 - nBit);

    return float4(r / 255.f, g/255.f, b/255.f, 1.0f);
}


//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
DepthStencilState Depth
{
    DepthEnable = false;
};

technique11 T0
{
    pass P0
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
    pass P1
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Inverse()));
    }
    pass P2
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_GrayScale2()));
    }
    pass P3
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Tone()));
    }
    pass P4
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Gamma()));
    }
    pass P5
    {
        SetDepthStencilState(Depth, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_BitPlannerSlicing()));
    }
}