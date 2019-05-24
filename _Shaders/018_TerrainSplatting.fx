#include "000_Header.fx"
#include "000_Terrain.fx"
#include "000_Light.fx"

cbuffer CB_Projector
{
    Matrix ProjectorView;
    Matrix ProjectorProjection;

    float4 ProjectorColor;
};
Texture2D ProjectorMap;
Texture2DArray ProjectorMaps;

void ProjectorPosition(inout float4 wvp, float4 position)
{
    wvp = WorldPosition(position);
    wvp = mul(wvp, ProjectorView);
    wvp = mul(wvp, ProjectorProjection);
}
//-----------------------------------------------------------------------------
struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 oPosition : Position1;
    float3 wPosition : Position2;
    float4 wvpPosition : Position3;
    float4 sPosition : Position4;
    float3 Normal : Normal0;
    float4 Color : Color0;
    float3 Tangent : Tangent0;
    float2 Uv : Uv0;
};

VertexOutput VS(VertexColorTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);
    output.Color = input.Color;

    output.oPosition = input.Position.xyz;
    output.wPosition = output.Position.xyz;
    output.wvpPosition = output.Position;
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;
    ProjectorPosition(output.wvpPosition, input.Position);

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    //float4 diffuse = GetTerrainColor(input.Uv);
    

    //float3 normal = normalize(input.Normal);
    //float3 light = -GlobalLight.Direction;
    //float NdotL = dot(normal, light);
    //NormalMapping(input.Uv, input.Normal, input.Tangent);
    float4 diffuse2 = GetTerrainColors(input.Uv, input.Color);
    float3 grid = GetGridLineColor(input.oPosition);
    //float3 brush = GetBrushColor(input.oPosition);

    float4 color = 0;
    TerrainComputeLight(color, diffuse2, input.Normal);
    TerrainPointLight(color, input.oPosition);
    TerrainSpotLight(color, input.oPosition);

    float2 uv = 0;
    uv.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    uv.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

      [flatten] //화면 안에 들어오는지 체크 0~1을 벗어나면 false
    if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
    {
        float3 uvw = float3(uv, BrushType - 1);
        float4 map = ProjectorMaps.Sample(LinearSampler, uvw);
        map *= ProjectorColor;
        color = lerp(color, map, map.a);
    }

    return color + float4(grid, 1) /*+ float4(brush, 1)*/;
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