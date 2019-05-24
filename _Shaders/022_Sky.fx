#include "000_Header.fx"

struct DataDesc
{
    float4 Center;
    float4 Apex;

    float Height;
};
DataDesc Data;

//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 Normal : Normal0;
    float2 Uv : Uv0;
    
    float Height : Height0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;

    output.Height = input.Position.y;

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
//-----------------------------------------------------------------------------

RasterizerState RS
{
    FrontCounterClockWise = true;
};

RasterizerState RS2
{
    FrontCounterClockWise = true;

    FillMode = Wireframe;
};

DepthStencilState DS
{
    DepthEnable = false;
};

technique11 T0
{
    pass P0
    {
        SetRasterizerState(RS);
        SetDepthStencilState(DS, 0);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS2);
        SetDepthStencilState(DS, 0);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}