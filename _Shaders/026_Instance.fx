#include "000_Header.fx"
#include "000_Light.fx"
//-----------------------------------------------------------------------------
// Pass0
//-----------------------------------------------------------------------------

//int TextureType[2000];
//float4 Color[2000];

struct VertexInput
{
    float4 Position : POSITION0;
    //float3 wPosition : POSITION1;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    matrix Transform : InstTransform0;
    uint InstID : SV_InstanceID0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float3 wPosition : POSITION1;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    uint InstID : ID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    output.Position = mul(input.Position, input.Transform);  //
 
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.InstID = input.InstID;
    output.Uv = input.Uv;

    return output;
}

//Texture2DArray Maps;

float4 PS(VertexOutput input) : SV_TARGET0
{
    float3 uvw = float3(input.Uv, TextureType[input.InstID]);

    Textures(Material.Diffuse, SpecularMaps, uvw);
    InstanceNormalMapping(uvw, input.Normal, input.Tangent);//
    Textures(Material.Specular, SpecularMaps, uvw);
    float4 color = 0;
    ComputeLight(color, input.Normal, input.wPosition);
    ComputePointLights(color, input.wPosition);
    ComputeSpotLights(color, input.wPosition);
    return Colors[input.InstID] * color;
}

float4 PS2(VertexOutput input) : SV_TARGET0
{
    //float3 uvw = float3(input.Uv, input.InstID % 5);
    //float4 diffuse = Map.Sample(Sampler, uvw);
    //if(diffuse.a <0.3)
    //    discard;
    //return color[input.InstID % 5];
    //return diffuse;

    return float4(0, 1, 0, 1);
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
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }
}