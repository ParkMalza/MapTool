#include "000_Header.fx"
#include "000_Model.fx"

//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 Normal : Normal0;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;

    SetModelWorld(World);

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);

    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);

    float3 normal = normalize(input.Normal);
    float3 light = -LightDirection;
    float NdotL = dot(normal, light);

    return (diffuse * NdotL);
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
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}