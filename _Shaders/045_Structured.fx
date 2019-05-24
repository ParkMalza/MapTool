struct Group
{
    uint Index;
    float3 Id;
    float Data;
    float2 Data2;
};

StructuredBuffer<Group> Input;

RWStructuredBuffer<Group> Output; //리턴용 변수

struct CS_Input
{
    uint3 GroupID : SV_GroupID;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex; //쓰레드의 번호
};

[numthreads(10, 8, 3)]
void CS(CS_Input input)
{
    uint index = (input.GroupID.x * 240) + input.GroupIndex;
    Output[index].Index = index;
    Output[index].Id.x = input.DispatchThreadID.x; //또는 float3
    Output[index].Id.y = input.DispatchThreadID.y; //또는 float3
    Output[index].Id.z = input.DispatchThreadID.z; //또는 float3
    Output[index].Data = Input[index].Data;
    Output[index].Data2 = Input[index].Data * 10.0f;

}

technique T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}
//바이토닉 알고리즘
//분할정복
//동적계획 알고리즘
//분할정복결합
//탐욕