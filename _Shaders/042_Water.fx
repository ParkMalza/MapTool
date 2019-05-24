#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

Texture2D WaveMap;
Texture2D ReflectionMap;
Texture2D RefractionMap;

cbuffer CB_WaterRender
{
    matrix ReflectionMatrix;
    float4 RefractionColor;

    float2 NormalMapTile;

    float WaveTranslation;
    float WaveScale;
    float WaterShininess;
};

struct VertexOutput
{
    float4 Position : SV_Position0;
    float4 wPosition : Position1;
    float4 ReflectionPosition : Position2;
    float4 RefractionPosition : Position3;

    float2 Uv : Uv0;
    float2 Uv1 : Uv1;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position;

    output.Position = ViewProjection(output.Position);
    output.Uv = input.Uv;

    matrix reflection = mul(World, mul(ReflectionMatrix, Projection));
    output.ReflectionPosition = mul(input.Position, reflection);

    matrix refraction = mul(World, mul(View, Projection));
    output.RefractionPosition = mul(input.Position, refraction);

    output.Uv = input.Uv / NormalMapTile.x;
    output.Uv1 = input.Uv / NormalMapTile.y;

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(VertexOutput input) : SV_TARGET0
{
    input.Uv.y += WaveTranslation;
    input.Uv1.y += WaveTranslation;

    float4 normalMap = WaveMap.Sample(LinearSampler, input.Uv) * 2.0f - 1.0f;
    float4 normalMap2 = WaveMap.Sample(LinearSampler, input.Uv1) * 2.0f - 1.0f;

    float3 normal = normalize(normalMap.rgb + normalMap2.rgb);

    float2 reflection;
    reflection.x = input.ReflectionPosition.x / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection.y = -input.ReflectionPosition.y / input.ReflectionPosition.w * 0.5f + 0.5f;

    float2 refraction;
    refraction.x = input.RefractionPosition.x / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction.y = -input.RefractionPosition.y / input.RefractionPosition.w * 0.5f + 0.5f;

    reflection = reflection + (normal.xy * WaveScale);
    refraction = refraction + (normal.xy * WaveScale);

    float4 reflectColor = ReflectionMap.Sample(LinearSampler, reflection);
    float4 refractColor = RefractionMap.Sample(LinearSampler, refraction);
    refractColor = saturate(refractColor + RefractionColor);

    float3 viewDirection = normalize(ViewPosition() - input.wPosition.xyz);
    float3 heightView = viewDirection.yyy;


    float r = (1.2f - 1.0f) / (1.2f + 1.0f);
    float fresnel = max(0.0f, min(1.0f, r + (1.0f - r) * pow(1.0f - dot(normal, heightView), 2)));
    float3 color = lerp(reflectColor, refractColor, fresnel);

    float3 R = -reflect(normalize(-GlobalLight.Direction), normal);
    float specular = dot(normalize(R), viewDirection);
    
    [flatten]
    if (specular > 0.0f)
    {
        specular = pow(specular, WaterShininess);
        color = saturate(color + specular);
    }

    return float4(color, 1.0f);
    //return refractColor;
    //return reflectColor;
}

float4 PS_PreRender(WaterOutput input) : SV_TARGET0
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

float4 PS_Render(MainOutput input) : SV_TARGET0
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
RasterizerState Cull
{
    CullMode = Back;
};

BlendState AlphaBlend
{
    BlendEnable[0] = true;
    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 0x0F;
};

technique11 T0
{
    //Refraction
    P_VP(P0, VS_Main_Water, PS_PreRender)
    P_VP(P1, VS_Model_Water, PS_PreRender)
    P_VP(P2, VS_Animation_Water, PS_PreRender)

    //Reflection
    P_RS_VP(P3, Cull, VS_Main_Water, PS_PreRender)
    P_RS_VP(P4, Cull, VS_Model_Water, PS_PreRender)
    P_RS_VP(P5, Cull, VS_Animation_Water, PS_PreRender)


    P_VP(P6, VS_Main, PS_Render)
    P_VP(P7, VS_Model, PS_Render)
    P_VP(P8, VS_Animation, PS_Render)

    P_BS_VP(P9, AlphaBlend, VS, PS)
}