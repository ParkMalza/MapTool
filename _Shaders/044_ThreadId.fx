ByteAddressBuffer Input;
RWByteAddressBuffer Output;

struct Group
{
    uint3 GroupID;
    uint3 GroupThreadID;
    uint3 DispatchThreadID;
    uint GroupIndex;

    float Random;
    float RandomOutput;
};

struct CS_Input
{
    uint3 GroupID : SV_GroupID;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex;
};

[numthreads(10, 8, 3)]
void CS(CS_Input input)
{
    uint index = (input.GroupID.x * 240) + input.GroupIndex;
    uint fetchAddress = index * 12 * 4;

    Group group;
    group.GroupID = asuint(input.GroupID);
    group.GroupThreadID = asuint(input.GroupThreadID);
    group.DispatchThreadID = asuint(input.DispatchThreadID);
    group.GroupIndex = asuint(input.GroupIndex);
    
    group.Random = asfloat(Input.Load(fetchAddress + 40));
    group.RandomOutput = group.Random * 10.0f;


    Output.Store3(fetchAddress + 0, asuint(group.GroupID));
    Output.Store3(fetchAddress + 12, asuint(group.GroupThreadID));
    Output.Store3(fetchAddress + 24, asuint(group.DispatchThreadID));
    Output.Store(fetchAddress + 36, asuint(group.GroupIndex));
    Output.Store(fetchAddress + 40, asuint(group.Random));
    Output.Store(fetchAddress + 44, asuint(group.RandomOutput));

}

technique11 T0
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