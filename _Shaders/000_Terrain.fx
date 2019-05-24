float LayerAlpha;
Texture2D BaseMap;
SamplerState BaseSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

Texture2D LayerMap;
SamplerState LayerSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

Texture2D AlphaMap;
SamplerState AlphaSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

Texture2DArray LayerMaps;

float4 GetTerrainColor(float2 uv)
{
    float4 base = BaseMap.Sample(BaseSampler, uv);
    float4 layer = LayerMap.Sample(LayerSampler, uv);
    float4 alpha = AlphaMap.Sample(AlphaSampler, uv);
    
    return lerp(base, layer, (1 - alpha.g) * 0.5f);
}

float4 GetTerrainColors(float2 uv, float4 terrainColor)
{
    float4 base = BaseMap.Sample(BaseSampler, uv);
    float4 layer = LayerMap.Sample(LayerSampler, uv);
    float4 layer1 = LayerMaps.Sample(LayerSampler, float3(uv, 0));
    float4 layer2 = LayerMaps.Sample(LayerSampler, float3(uv, 1));
    float4 layer3 = LayerMaps.Sample(LayerSampler, float3(uv, 2));
    float4 layer4 = LayerMaps.Sample(LayerSampler, float3(uv, 3));
    float4 alpha = AlphaMap.Sample(AlphaSampler, uv);


    float4 r = terrainColor.r;
    float4 g = terrainColor.g;
    float4 b = terrainColor.b;
    float4 a = terrainColor.a;

    float4 lerp0 = lerp(base, layer, (1 - alpha.g) * 0.5f);
    float4 lerp1 = lerp(lerp0, layer1, r * LayerAlpha);
    float4 lerp2 = lerp(lerp1, layer2, g * LayerAlpha);
    float4 lerp3 = lerp(lerp2, layer3, b * LayerAlpha);
    float4 lerp4 = lerp(lerp3, layer4, a * LayerAlpha);
    return lerp4;
}

//-----------------------------------------------------------------------------
// Brush
//-----------------------------------------------------------------------------
cbuffer CB_Brush
{
    float4 BrushColor;
    float3 BrushLocation; //16����Ʈ�� ����� �Ѵ�. ���� ���̴����� �׷��ʿ�� ����, ���콺��ġ
    int BrushType;
    int BrushRange;
};

float3 GetBrushColor(float3 position)
{
    [flatten]
    if (BrushType == 0)
        return float3(0, 0, 0);
    
    if (BrushType == 1)  //�簢�� ����
    { //BrushLocation.x - BrushRange : ���콺 ��ġ���� ������ŭ ��
        if ((position.x >= (BrushLocation.x - BrushRange)) &&
            (position.x <= (BrushLocation.x + BrushRange)) &&
            (position.z >= (BrushLocation.z - BrushRange)) &&
            (position.z <= (BrushLocation.z + BrushRange)))
        {
            return BrushColor.rgb; //BrushColor�� float4�̹Ƿ�..
        }
    }

    if (BrushType == 2)  //����� ����
    {
        float dx = position.x - BrushLocation.x;
        float dz = position.z - BrushLocation.z;

        float dist = sqrt(dx * dx + dz * dz);
        //float dist = distance(dx, dz);

        if (dist <= BrushRange)
            return BrushColor.rgb;
    }

    return float3(0, 0, 0);
}


//-----------------------------------------------------------------------------
// GridLine
//-----------------------------------------------------------------------------

//    C��� 
//struct GridLineDesc
//{
//    D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

//    int Visible = 1;
//    float Thickness = 0.1f; //�׷����� ���� �β�
//    float size = 25.5f; //sizeũ�� ��ŭ�� ��ĭ���� �����.

//    float Padding;
//} gridLineDesc;

cbuffer CB_GridLine
{
    float4 GridLineColor;
    
    int VisibleGridLine; //���������̳�
    float GridLineThickness; //�׷����� ���� �β�
    float GridLineSize; //sizeũ�� ��ŭ�� ��ĭ���� �����.
};

//float3 GetGridLineColor(float3 position)
//{
//    [flatten]
//    if (VisibleGridLine == 0)
//        return float3(0, 0, 0);

//    float2 grid = position.xz / GridLineSize;
//    //grid = frac(grid);
//    grid = abs(frac(grid - 0.5f) - 0.5f);

//    float thick = GridLineThickness / GridLineSize;

//    [flatten]
//    if (grid.x < thick || grid.y < thick)
//        return GridLineColor.rgb;

//    return float3(0, 0, 0);
//}

float3 GetGridLineColor(float3 position)
{
    [flatten]
    if (VisibleGridLine == 0)
        return float3(0, 0, 0);

    float2 grid = position.xz / GridLineSize;
    //grid = frac(grid);
    float2 range = abs(frac(grid - 0.5f) - 0.5f);


    float2 speed = fwidth(grid); //��̺� �˾ƺ���
    float2 pixel = range / speed;;

    //return float3(pixel, 0);

    float weight = saturate(min(pixel.x, pixel.y) - GridLineThickness); //0 ���� 1������ �����ϴ� �Ϲ�

    return lerp(GridLineColor.rgb, float3(0, 0, 0), weight); //(1-t) * A + t*B // t�� 0~1����
}

struct VertexTerrain
{
    float4 Position : SV_Position0;
    float4 wvpPosition : Position1;
    float3 oPosition : Position2;
    float3 wPosition : Position3;
    float4 sPosition : Position4;

    float4 Color : Color0;
    float3 Normal : Normal0;
    float2 Uv : Uv0;
    float3 Tangent : Tanget0;
};