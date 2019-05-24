#include "000_Header.fx"

//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}


//uint Address = 0;
//uint Filter = 0;

float2 Uv;
Texture2D Map1;
Texture2D Map2;

SamplerState Sampler;

//SamplerState SamplerAddress
//{
//    AddressU = WRAP;
//    AddressV = WRAP;
//};

//SamplerState SamplerAddress2
//{
//    AddressU = MIRROR;
//    AddressV = MIRROR;
//};

//SamplerState SamplerAddress3
//{
//    AddressU = CLAMP;
//    AddressV = CLAMP;
//};

//SamplerState SamplerAddress4
//{
//    AddressU = BORDER;
//    AddressV = BORDER;

//    BorderColor = float4(1, 0, 0, 1);
//};

//SamplerState SamplerFilter
//{
//    Filter = MIN_MAG_MIP_POINT;
//};

//SamplerState SamplerFilter2
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//};

//float4 PS(VertexOutput input) : SV_TARGET0
//{
//    float4 color = 0;

//    [branch]
//    switch (Filter)
//    {
//        case 0:
//            color = Map.Sample(SamplerFilter, input.Uv);
//            break;

//        case 1:
//            color = Map.Sample(SamplerFilter2, input.Uv);
//            break;
//    }

//    return color;
//}

//float4 PS(VertexOutput input) : SV_TARGET0
//{
//    float4 color = 0;

//    [branch]
//    switch (Address)
//    {
//        case 0:
//            color = Map.Sample(SamplerAddress, input.Uv);
//            break;

//        case 1:
//            color = Map.Sample(SamplerAddress2, input.Uv);
//            break;

//        case 2:
//            color = Map.Sample(SamplerAddress3, input.Uv);
//            break;

//        case 3:
//            color = Map.Sample(SamplerAddress4, input.Uv);
//            break;
//    }

//    return color;
//}

float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 color = 0;

    [branch]
    if(input.Uv.x > Uv.x)
        color = Map1.Sample(Sampler, input.Uv);
    else
        color = Map2.Sample(Sampler, input.Uv);

    return color;
}
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}