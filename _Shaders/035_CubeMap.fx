#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

TextureCube CubeMap;

// 밝은 빛만 남기는 과정들을 통해 선명한 그래픽 효과를 준다!
float4 PS(MainOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    Texture(Material.Specular, SpecularMap, input.Uv);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    return color;
}

float4 PS_Cube(MainOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);

    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    float3 eye = normalize(input.wPosition - ViewPosition());
    float3 r = reflect(eye, normalize(input.Normal));
    color *= (0.85f + CubeMap.Sample(LinearSampler, r) * 0.75f);
    return color;
}


//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------


technique11 T0
{
    P_VP(P0, VS_Main, PS)
    P_VP(P1, VS_Model, PS)

    P_VP(P2, VS_Main, PS_Cube)
    P_VP(P3, VS_Model, PS_Cube)
}