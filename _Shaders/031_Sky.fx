#include "000_Header.fx"
#include "000_Light.fx"

//struct DataDesc
//{
//    float4 Center;
//    float4 Apex;

//    float Height;
//};
//DataDesc Data;

//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : SV_Position0;
    float4 oPosition : Position1;
    float3 Normal : Normal0;
    float2 Uv : Uv0;
    
    float Height : Height0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;

    output.Height = input.Position.y;
    output.oPosition = input.Position;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);

    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    return lerp(Data.Center, Data.Apex, input.Height * Data.Height);
}

float4 PS_CubeMap(VertexOutput input) : SV_TARGET0
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition.xyz);
}
//-----------------------------------------------------------------------------

RasterizerState RS
{
    FrontCounterClockWise = true;
};


DepthStencilState DS
{
    DepthEnable = false;
};

technique11 T0
{
    P_RS_DSS_VP(P0, RS, DS, VS, PS)
    P_RS_DSS_VP(P1, RS, DS, VS, PS_CubeMap)
}