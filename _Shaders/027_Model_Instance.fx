#include "000_Header.fx"
#include "000_Model.fx"
#include "000_Light.fx"
//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : Position1;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;

    uint InstID : ID0;
};

VertexOutput VS(VertexModel input)
{
    VertexOutput output;
    SetModelWorld(World, input);

    output.Position = WorldPosition(input.Position); //
    output.wPosition = output.Position;

    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.InstID = input.InstID;
    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Specular, SpecularMap, input.Uv);

    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition); //
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);

    return color;
    //float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);
    //float NdotL = saturate(dot(normalize(input.Normal), -GlobalLight.Direction));

    //return diffuse * NdotL;
   // //return float4(input.Normal * 0.5f + 0.5f, 1);
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
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(Wire);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}