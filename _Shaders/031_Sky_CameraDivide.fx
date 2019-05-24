#include "000_Header.fx"

TextureCube CubeMap;
uint Dir;

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

    switch (Dir)
    {
        case 0:
            output.Position = ViewProjection(output.Position);
            break;
        case 1:
            output.Position = OrthoViewProjectionFront(output.Position);
            break;
        case 2:
            output.Position = OrthoViewProjectionUp(output.Position);
            break;
        case 3:
            output.Position = OrthoViewProjectionRight(output.Position);
            break;
    }
    
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
    return CubeMap.Sample(LinearSampler, input.oPosition.xyz);
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
    pass P0
    {
        SetRasterizerState(RS);
        SetDepthStencilState(DS, 0);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);
        SetDepthStencilState(DS, 0);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_CubeMap()));
    }
}