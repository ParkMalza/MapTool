struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

cbuffer CB_Material
{
    MaterialDesc Material;

};

struct LightDesc  //perFrame
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;

    float3 Position; //���� ��ġ
};

cbuffer CB_Light
{
    LightDesc GlobalLight; //Amient Light
    
};

//void ComputeLight(inout float4 color, float3 normal, float3 wPosition/*��*/)
//{
//    float4 ambient = 0;
//    float4 diffuse = 0;
//    float4 specular = 0;

//    ambient = GlobalLight.Ambient * Material.Ambient; // �������� * �ڱ� ���� ��

//    float3 direction = -GlobalLight.Direction;  // �� ����
//    float NdotL = dot(direction, normalize(normal)); 

//    [flatten]
//    if(NdotL > 0.0f)
//    {
//        diffuse = Material.Diffuse * NdotL; //DC * TC �� �ܺο��� �޾ƿ´�. �� ���� diffuseLight(DL) ������

//        wPosition = ViewPosition() - wPosition;

//        float3 R = normalize(reflect(-direction, normal)); //
//        float3 RdotE = saturate(dot(R, normalize(wPosition)));

//        //������ �������� ����. DIFFUSE�� ���İ��� �ǹ̰� ����. 
//        //�̰� ������ ġ�� SPECULAR�� ���� ���� �ǹ̰� ����.
//        float phong = pow(RdotE, Material.Specular.a); //SHINESS ���� �߰� ���ϰ� ���� ������ ó��
//        specular = phong * Material.Specular * GlobalLight.Specular;
//    }
//    color = ambient +diffuse + specular;
//}
void ComputeLight(inout float4 color, float3 normal, float3 wPosition)
{
    float4 ambient = 0;
    float4 diffuse = 0;
    float4 specular = 0;

    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));
    
    
    ambient = GlobalLight.Ambient * Material.Ambient;

    [flatten]
    if (NdotL > 0.0f)
    {
        diffuse = NdotL * Material.Diffuse;

        [flatten]
        if (any(Material.Specular.rgb) && any(Material.Specular.a))
        {
            wPosition = ViewPosition() - wPosition;

            float3 R = normalize(reflect(-direction, normal));
            float RdotE = saturate(dot(R, normalize(wPosition)));
            
            float shininess = pow(RdotE, Material.Specular.a);
            specular = shininess * Material.Specular * GlobalLight.Specular;
        }
    }

    color = ambient + diffuse + specular;
}

void TerrainComputeLight(inout float4 color, float4 diffuse2, float3 normal)
{
    float4 diffuse = 0;

    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));

    [flatten]
    if (NdotL > 0.0f)
        diffuse = diffuse2 * NdotL;  //�ͷ����� ���� ���غ���..

    color = diffuse;
}

void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float4 map = NormalMap.Sample(samp, uv);

    [flatten]
    if(any(map) == false)
        return;
    //ź��Ʈ ����
    float3 N = normalize(normal); //z�� ����
    float3 T = normalize(tangent - dot(tangent, N) * N); //x�� ����
    float3 B = cross(N, T); //y�� ����
    float3x3 TBN = float3x3(T, B, N);

    //�̹����� ���� ���� �븻
    float3 coord = map.rgb * 2.0f - 1.0f;  //��ġ������ �븻�� , �ȼ������� ������

    //ź��Ʈ �������� ��ȯ
    coord = mul(coord, TBN);

    Material.Diffuse *= saturate(dot(coord, -GlobalLight.Direction));
}

void NormalMapping(float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(uv, normal, tangent, LinearSampler);
}

/////////////////////////////////////////////////////////////////////////
void InstanceNormalMapping(float3 uvw, float3 normal, float3 tangent, SamplerState samp)
{
    float4 map = NormalMaps.Sample(samp, uvw);

    [flatten]
    if (any(map) == false)   //�븻���� ������ ����
        return;

    //ź��Ʈ ����
    float3 N = normalize(normal); //z
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T); //y
    float3x3 TBN = float3x3(T, B, N);

    //�̹����� ���� ���� ���
    float3 coord = map.rgb * 2.0f - 1.0f; 
    
    //ź��Ʈ �������� ��ȯ
    coord = mul(coord, TBN);

    Material.Diffuse *= saturate(dot(coord, -GlobalLight.Direction));
}

void InstanceNormalMapping(float3 uvw, float3 normal, float3 tangent)
{
    InstanceNormalMapping(uvw, normal, tangent, LinearSampler);

}

////////////////////////////////////////////////////////////////////////////

#define MAX_POINT_LIGHT 32  //�ִ� 32��. �� �̻��� �����ս� ������ �ִ�.
struct PointLightDesc
{
    float4 color; // �����
    float3 Position; // ���� ��ġ
    float Range; //����
    float intensity; //����
    float3 Padding;  //�����Ⱚ
};

cbuffer CB_PointLight
{
    uint PointLightCount;
    float3 CB_PointLight_Padding;

    PointLightDesc PointLights[MAX_POINT_LIGHT]; //�迭�̱⶧���� �е��� �ʿ���
    
};

void ComputePointLights(inout float4 color, float3 wPosition)
{
   // [unroll(MAX_POINT_LIGHT)]
    for (uint i = 0; i < PointLightCount; i++)
    {
        float dist = distance(PointLights[i].Position, wPosition);

        [flatten]
        if(dist > 0.0f)
        {  //att = ����
            float att = saturate((PointLights[i].Range - dist) / PointLights[i].Range);  //0~1 ������ ���谪
            att = pow(att, PointLights[i].intensity);  //���� ���� ���谪

            color += PointLights[i].color * att * Material.Specular; //�ڱ� �� * ���谪 * Specular(���� ���� �ȵ��� ���ؾ�)
        }
    }

}

void TerrainPointLight(inout float4 color, float3 wPosition)
{
   // [unroll(MAX_POINT_LIGHT)]
    for (uint i = 0; i < PointLightCount; i++)
    {
        float dist = distance(PointLights[i].Position, wPosition) - 5;

        [flatten]
        if (dist > 0.0f)
        { //att = ����
            float att = saturate((PointLights[i].Range - dist) / PointLights[i].Range); //0~1 ������ ���谪
            att = pow(att, PointLights[i].intensity); //���� ���� ���谪

            color += PointLights[i].color * att; //�ڱ� �� * ���谪 * Specular(���� ���� �ȵ��� ���ؾ�)
        }
    }

}

////////////////////////////////////////////////////////////////////////////

#define MAX_SPOT_LIGHT 32
struct SpotLightDesc
{
    float4 color; // �����
    float3 Position; // ���� ��ġ
    float Range; //����
    float3 Direction;
    float Angle;
    float intensity; //����
    float3 Padding; //�����Ⱚ
};

cbuffer CB_SpotLight
{
    uint SpotLightCount;
    float3 CB_SpotLightt_Padding;

    SpotLightDesc SpotLights[MAX_SPOT_LIGHT]; //�迭�̱⶧���� �е��� �ʿ���
    
};

void ComputeSpotLights(inout float4 color, float3 wPosition)
{
 //   [unroll(MAX_POINT_LIGHT)]
    for (uint i = 0; i < SpotLightCount; i++)
    {
        float3 dist = SpotLights[i].Position - wPosition;

        [flatten]
        if (length(dist) < SpotLights[i].Range)
        {
            float3 direction = normalize(SpotLights[i].Position - wPosition);  //������ �Ʒ��� ����
            float angle = dot(-SpotLights[i].Direction, direction);

           [flatten]
            if (angle > 0.0f)  //angle �� 0���� Ŭ ���� ����ϸ��.
            {
                float intensity = max(dot(-dist, SpotLights[i].Direction), 0);
                float att = pow(intensity, SpotLights[i].Angle);

                color += SpotLights[i].color * att * Material.Specular;
            }
        }
    }
}

void TerrainSpotLight(inout float4 color, float3 wPosition)
{
  //  [unroll(MAX_POINT_LIGHT)]
    for (uint i = 0; i < SpotLightCount; i++)
    {
        float3 dist = SpotLights[i].Position - wPosition - 5;

        [flatten]
        if (length(dist) < SpotLights[i].Range)
        {
            float3 direction = normalize(SpotLights[i].Position - wPosition); //������ �Ʒ��� ����
            float angle = dot(-SpotLights[i].Direction, direction);

           [flatten]
            if (angle > 0.0f)  //angle �� 0���� Ŭ ���� ����ϸ��.
            {
                float intensity = max(dot(-dist, SpotLights[i].Direction), 0);
                float att = pow(intensity, SpotLights[i].Angle);

                color += SpotLights[i].color * att;
            }
        }
    }
}