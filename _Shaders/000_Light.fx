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

    float3 Position; //빛의 위치
};

cbuffer CB_Light
{
    LightDesc GlobalLight; //Amient Light
    
};

//void ComputeLight(inout float4 color, float3 normal, float3 wPosition/*눈*/)
//{
//    float4 ambient = 0;
//    float4 diffuse = 0;
//    float4 specular = 0;

//    ambient = GlobalLight.Ambient * Material.Ambient; // 전역조명 * 자기 재질 색

//    float3 direction = -GlobalLight.Direction;  // 빛 방향
//    float NdotL = dot(direction, normalize(normal)); 

//    [flatten]
//    if(NdotL > 0.0f)
//    {
//        diffuse = Material.Diffuse * NdotL; //DC * TC 는 외부에서 받아온다. 그 값에 diffuseLight(DL) 곱해줌

//        wPosition = ViewPosition() - wPosition;

//        float3 R = normalize(reflect(-direction, normal)); //
//        float3 RdotE = saturate(dot(R, normalize(wPosition)));

//        //빛에는 반투명이 없다. DIFFUSE의 알파값은 의미가 없다. 
//        //이건 버린다 치고 SPECULAR의 알파 또한 의미가 없다.
//        float phong = pow(RdotE, Material.Specular.a); //SHINESS 변수 추가 안하고 남는 변수로 처리
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
        diffuse = diffuse2 * NdotL;  //터레인의 색에 곱해보면..

    color = diffuse;
}

void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState samp)
{
    float4 map = NormalMap.Sample(samp, uv);

    [flatten]
    if(any(map) == false)
        return;
    //탄젠트 공간
    float3 N = normalize(normal); //z에 매핑
    float3 T = normalize(tangent - dot(tangent, N) * N); //x에 매핑
    float3 B = cross(N, T); //y에 매핑
    float3x3 TBN = float3x3(T, B, N);

    //이미지로 부터 계산된 노말
    float3 coord = map.rgb * 2.0f - 1.0f;  //위치에서의 노말값 , 픽셀에서의 음영값

    //탄젠트 공간으로 변환
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
    if (any(map) == false)   //노말맵이 없으면 리턴
        return;

    //탄젠트 공간
    float3 N = normalize(normal); //z
    float3 T = normalize(tangent - dot(tangent, N) * N);
    float3 B = cross(N, T); //y
    float3x3 TBN = float3x3(T, B, N);

    //이미지로 부터 계산된 노멀
    float3 coord = map.rgb * 2.0f - 1.0f; 
    
    //탄젠트 공간으로 변환
    coord = mul(coord, TBN);

    Material.Diffuse *= saturate(dot(coord, -GlobalLight.Direction));
}

void InstanceNormalMapping(float3 uvw, float3 normal, float3 tangent)
{
    InstanceNormalMapping(uvw, normal, tangent, LinearSampler);

}

////////////////////////////////////////////////////////////////////////////

#define MAX_POINT_LIGHT 32  //최대 32개. 그 이상은 퍼포먼스 문제가 있다.
struct PointLightDesc
{
    float4 color; // 조명색
    float3 Position; // 조명 위치
    float Range; //범쉬
    float intensity; //강도
    float3 Padding;  //쓰레기값
};

cbuffer CB_PointLight
{
    uint PointLightCount;
    float3 CB_PointLight_Padding;

    PointLightDesc PointLights[MAX_POINT_LIGHT]; //배열이기때문에 패딩값 필요함
    
};

void ComputePointLights(inout float4 color, float3 wPosition)
{
   // [unroll(MAX_POINT_LIGHT)]
    for (uint i = 0; i < PointLightCount; i++)
    {
        float dist = distance(PointLights[i].Position, wPosition);

        [flatten]
        if(dist > 0.0f)
        {  //att = 감쇠
            float att = saturate((PointLights[i].Range - dist) / PointLights[i].Range);  //0~1 까지의 감쇠값
            att = pow(att, PointLights[i].intensity);  //빛에 대한 감쇠값

            color += PointLights[i].color * att * Material.Specular; //자기 색 * 감쇠값 * Specular(빛이 들어갈지 안들어갈지 정해야)
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
        { //att = 감쇠
            float att = saturate((PointLights[i].Range - dist) / PointLights[i].Range); //0~1 까지의 감쇠값
            att = pow(att, PointLights[i].intensity); //빛에 대한 감쇠값

            color += PointLights[i].color * att; //자기 색 * 감쇠값 * Specular(빛이 들어갈지 안들어갈지 정해야)
        }
    }

}

////////////////////////////////////////////////////////////////////////////

#define MAX_SPOT_LIGHT 32
struct SpotLightDesc
{
    float4 color; // 조명색
    float3 Position; // 조명 이치
    float Range; //범위
    float3 Direction;
    float Angle;
    float intensity; //강도
    float3 Padding; //쓰레기값
};

cbuffer CB_SpotLight
{
    uint SpotLightCount;
    float3 CB_SpotLightt_Padding;

    SpotLightDesc SpotLights[MAX_SPOT_LIGHT]; //배열이기때문에 패딩값 필요함
    
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
            float3 direction = normalize(SpotLights[i].Position - wPosition);  //위에서 아래로 방향
            float angle = dot(-SpotLights[i].Direction, direction);

           [flatten]
            if (angle > 0.0f)  //angle 이 0보다 클 때만 계산하면됨.
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
            float3 direction = normalize(SpotLights[i].Position - wPosition); //위에서 아래로 방향
            float angle = dot(-SpotLights[i].Direction, direction);

           [flatten]
            if (angle > 0.0f)  //angle 이 0보다 클 때만 계산하면됨.
            {
                float intensity = max(dot(-dist, SpotLights[i].Direction), 0);
                float att = pow(intensity, SpotLights[i].Angle);

                color += SpotLights[i].color * att;
            }
        }
    }
}