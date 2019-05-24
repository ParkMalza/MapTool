#include "000_Header.fx"

struct VertexOutput
{
    float4 Position : SV_Position0;
    float3 Normal : Normal0;
};

VertexOutput VS(VertexNormal input)
{
    VertexOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);
    /* 3 x 3으로 넘어오기 때문에 */
    //월드 공간변환을 해야한다. 노말은 보여질건 아니기 때문에 뷰, 프로젝션은 필요없다.

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    float4 diffuse = float4(1, 1, 1, 1);  // 그려질 애 밑바탕 색깔인듯?

    //return diffuse;

    float3 normal = normalize(input.Normal); //방향은 노멀라이즈 해주는게 좋다.
    float3 light = -LightDirection;  //라이트 방향 뒤집는다. 빛이 물체에서 나오는거 처럼 하기위해서
    float NdotL = dot(normal, light);  //노말 벡터와 라이트 벡터 내적, 순서 관계없음 . 음영에 대한 감쇠값

    return diffuse * NdotL;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
struct VertexInput2
{
    float4 Position : Position0;
    float3 Normal : Normal0;
};

struct VertexOutput2
{
    float4 Position : SV_Position0;
    float4 Color : Color0;
    float3 Normal : Normal0;
};

VertexOutput2 VS2(VertexInput2 input)
{
    VertexOutput2 output;

    output.Color = float4(1, 1, 1, 1);
    if (input.Position.y > 5)
        output.Color = float4(1, 0, 0, 1);

    if (input.Position.y > 10)
        output.Color = float4(0, 1, 0, 1);

    if (input.Position.y > 20)
        output.Color = float4(0, 0, 1, 1);

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);

    return output;
}

float4 PS2(VertexOutput2 input) : SV_TARGET0
{
    float4 diffuse = input.Color;

    //return diffuse;

    float3 normal = normalize(input.Normal);
    float3 light = -LightDirection;
    float NdotL = dot(normal, light);

    return diffuse * NdotL;
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
        SetVertexShader(CompileShader(vs_5_0, VS2()));
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }
}