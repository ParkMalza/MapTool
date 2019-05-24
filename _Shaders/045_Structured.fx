struct Group
{
    uint Index;
    float3 Id;
    float Data;
    float2 Data2;
};

StructuredBuffer<Group> Input;

RWStructuredBuffer<Group> Output; //���Ͽ� ����

struct CS_Input
{
    uint3 GroupID : SV_GroupID;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex; //�������� ��ȣ
};

[numthreads(10, 8, 3)]
void CS(CS_Input input)
{
    uint index = (input.GroupID.x * 240) + input.GroupIndex;
    Output[index].Index = index;
    Output[index].Id.x = input.DispatchThreadID.x; //�Ǵ� float3
    Output[index].Id.y = input.DispatchThreadID.y; //�Ǵ� float3
    Output[index].Id.z = input.DispatchThreadID.z; //�Ǵ� float3
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
//������� �˰���
//��������
//������ȹ �˰���
//������������
//Ž��