#include "000_Header.fx"
#include "000_Terrain.fx"

struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 oPosition : Position1;  
    float3 Normal : Normal0;
};

VertexOutput VS(VertexNormal input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);

    output.oPosition = input.Position.xyz;

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 diffuse = float4(0.5f, 0.5f, 0.5f, 1);

    //return diffuse;

    float3 normal = normalize(input.Normal);
    float3 light = -LightDirection;
    float NdotL = dot(normal, light);

    float3 color = GetGridLineColor(input.oPosition);  //필셀에서 그려줌으로써 부드럽게

 
    return (diffuse * NdotL) + float4(color, 1);
    //return float4(color, 1);
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