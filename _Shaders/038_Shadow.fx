#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

//SamplerState Point
//{
//    Filter = MIN_MAG_MIP_POINT;
//};

//SamplerState Ani
//{
//    Filter = ANISOTROPIC; //비등방성 필터링 
//    MaxAnisotropy = 16;
//};

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

    input.sPosition.xyz /= input.sPosition.w;

    //input.sPosition.x = input.sPosition.x * 0.5f + 0.5f;
    //input.sPosition.y = -input.sPosition.y * 0.5f + 0.5f;
    //return ShadowMap.Sample(LinearSampler, input.sPosition.xy);


    [flatten]
    if (input.sPosition.x < -1.0f || input.sPosition.x > 1.0f ||
       input.sPosition.y < -1.0f || input.sPosition.y > 1.0f ||
        input.sPosition.z < 0.0f || input.sPosition.z > 1.0f )
        return color;

    input.sPosition.x = input.sPosition.x * 0.5f + 0.5f;
    input.sPosition.y = -input.sPosition.y * 0.5f + 0.5f;
    input.sPosition.z -= ShadowBias;

    float depth = 0;
    float factor = 0;

    //[branch] //if else시 사용
    //if(ShadowIndex == 0)
    //{
    //    depth = ShadowMap.Sample(LinearSampler, input.sPosition.xy).r; //뒷면 처리
    //    factor = (float) input.sPosition.z <= depth; //앞면 처리
    //}
    //else if(ShadowIndex == 1) //PCF
    //{
    //    depth = input.sPosition.z;
    //    factor = ShadowMap.SampleCmpLevelZero(ShadowSampler, input.sPosition.xy, depth).r;
    //}
    //else if (ShadowIndex == 2)
    //{
        float2 size = 1.0f / ShadowMapSize;
        float2 offsets[] =
        {
            float2(+size.x, -size.y), float2(0.0f, -size.y), float2(-size.x, -size.y),
            float2(+size.x, 0.0f), float2(0.0f, 0.0f), float2(-size.x, 0.0f),
            float2(+size.x, +size.y), float2(0.0f, +size.y), float2(-size.x, +size.y),
        };

        float sum = 0.0f;
        float2 uv = 0.0f;
        
        depth = input.sPosition.z;

        //[unroll(9)]
        for (int i = 0; i < 9; i++)
        {
            uv = input.sPosition.xy + offsets[i];
            sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
        }

        factor = sum / 9.0f;
  //  }

    factor = saturate(factor + depth);

    return float4(color.rgb * factor, 1);

    //float4 color2 = float4(depth, depth, depth, 1);

    //[flatten]
    //if (ShadowIndex == 1)
    //    return float4(factor, factor, factor, 1);

    //return color2;
}


//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------

RasterizerState RS
{
    //CullMode 는 Vertex Shader 에서 파기 시킨다.
    CullMode = Front; //앞을 지울것이다.

};

technique11 T0
{
    //Render
    P_RS_VP(P0, RS, VS_Main_Depth, PS_Depth)
    P_RS_VP(P1, RS, VS_Model_Depth, PS_Depth)

    P_VP(P2, VS_Main, PS)
    P_VP(P3, VS_Model, PS)
    //P_RS_VP(P2, RS, VS_Main, PS)
    //P_RS_VP(P3, RS, VS_Model, PS)
}