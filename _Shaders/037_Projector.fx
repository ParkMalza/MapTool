#include "000_Header.fx"
#include "000_Light.fx"
#include "000_Model.fx"

cbuffer CB_Projector
{
    Matrix ProjectorView;
    Matrix ProjectorProjection;

    float4 ProjectorColor;
};
Texture2D ProjectorMap;

void ProjectorPosition(inout float4 wvp, float4 position)
{
    wvp = WorldPosition(position);
    wvp = mul(wvp, ProjectorView);
    wvp = mul(wvp, ProjectorProjection);
}
MainOutput VS_Projector_Mesh(VertexMesh input)
{
    MainOutput output = VS_Main(input);
    ProjectorPosition(output.wvpPosition, input.Position);

    return output;
}

MainOutput VS_Projector_Model(VertexModel input)
{
    MainOutput output = VS_Model(input);
    ProjectorPosition(output.wvpPosition, input.Position);

    return output;
}

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
    //NDC공간에 떨어지게 하기 위해  -1 ~ 1
    float2 uv = 0;
    uv.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    uv.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

    [flatten] //화면 안에 들어오는지 체크 0~1을 벗어나면 false
    if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
    {
        float4 map = ProjectorMap.Sample(LinearSampler, uv);
        map *= ProjectorColor;
        color = lerp(color, map, map.a);
    }

    return color;
}


//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------


technique11 T0
{
    //Render
    P_VP(P0, VS_Projector_Mesh, PS)
    P_VP(P1, VS_Projector_Model, PS)

}