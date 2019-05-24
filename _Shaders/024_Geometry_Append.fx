#include "000_Header.fx"

//-----------------------------------------------------------------------------
struct VertexInput
{
    float4 Position : Position0;
};

struct VertexOutput
{
    float4 Position : Position0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position0; //픽셸쉐이더로 들어가기 전에 하나는 무조건 sv붙어야함
};

void SubDivide(VertexOutput vertices[3], out VertexOutput outVertices[6])
{
    VertexOutput p[3];
    p[0].Position = (vertices[0].Position + vertices[1].Position) * 0.5f; //왼쪽 변 중간
    p[1].Position = (vertices[1].Position + vertices[2].Position) * 0.5f; //오른쪽 변 중간
    p[2].Position = (vertices[2].Position + vertices[0].Position) * 0.5f; //밑 변 중간


    outVertices[0] = vertices[0];
    outVertices[1] = p[0];
    outVertices[2] = p[2];
    outVertices[3] = p[1];
    outVertices[4] = vertices[2];
    outVertices[5] = vertices[1];

}

[maxvertexcount(8)]
void GS(triangle VertexOutput input[3], inout TriangleStream<GeometryOutput> stream)
{
    VertexOutput p[6];
    SubDivide(input, p);

    float4 position = 0;
    GeometryOutput output[6];

    [roll(6)]
    for (int i = 0; i < 6; i++)
    {
        position = WorldPosition(p[i].Position);
        position = ViewProjection(position);

        output[i].Position = position;
    }
    [roll(5)]
    for (int k = 0; k < 5; k ++)
        stream.Append(output[k]);

    stream.RestartStrip();

    stream.Append(output[1]);
    stream.Append(output[5]);
    stream.Append(output[3]);

}

Texture2DArray Map; //텍스쳐의 크기가 모두 동일해야 한다.

float4 PS(GeometryOutput input) : SV_TARGET0
{
    return float4(1, 0, 0, 1);
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
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetRasterizerState(RS);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}