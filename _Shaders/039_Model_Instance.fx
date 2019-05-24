#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(MainOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);

    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    return color;
}

//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
RasterizerState Wire
{
    FillMode = WireFrame;
};

technique11 T0
{
    P_VP(P0, VS_Model, PS)
    P_VP(P1, VS_Animation, PS)
}