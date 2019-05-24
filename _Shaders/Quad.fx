matrix World;
matrix View;
matrix Projection;

struct VertexInput
{
    float4 Position : Position0;
};

struct VertexOutput
{
    float4 Position : SV_Position0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET0
{
    return float4(1, 0, 0, 1);
}

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