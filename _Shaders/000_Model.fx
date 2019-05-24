//-----------------------------------------------------------------------------
// Bones
//-----------------------------------------------------------------------------
#define MAX_MODEL_INSTANCE 100
#define MAX_MODEL_TRANSFORMS 128
Texture2DArray BoneTransforms;

struct VertexModel
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
    float3 Tangent : Tangent0;
    float4 BlendIndices : BlendIndices0;
    float4 BlendWeights : BlendWeights0;

    matrix Transform : Inst0;  //월드
    uint InstID : SV_InstanceID0;
};
cbuffer CB_Bone
{
    //Deprecated
   //matrix BoneTransforms[MAX_MODEL_TRANSFORMS];

    uint BoneIndex;  //열
};

///////////////////////////////////////////////////////////////////////////////

void SetModelWorld(inout matrix world, VertexModel input)  //레퍼런스 = inout , 리턴받을때는 out
{
    float4 m1 = BoneTransforms.Load(uint4(BoneIndex * 4 + 0, input.InstID, 0, 0)); //맨 뒤, MIN_MAG번호
    float4 m2 = BoneTransforms.Load(uint4(BoneIndex * 4 + 1, input.InstID, 0, 0));
    float4 m3 = BoneTransforms.Load(uint4(BoneIndex * 4 + 2, input.InstID, 0, 0));
    float4 m4 = BoneTransforms.Load(uint4(BoneIndex * 4 + 3, input.InstID, 0, 0));

    matrix transform = matrix(m1, m2, m3, m4);
    world = mul(transform, input.Transform);
}

DepthOutput VS_Model_Depth(VertexModel input)
{
    DepthOutput output;

    SetModelWorld(World, input);

    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, ShadowView);
    output.Position = mul(output.Position, ShadowProjection);

    output.sPosition = output.Position;

    return output;
}
MainOutput_GS VS_Model_GS(VertexModel input)
{
    MainOutput_GS output;

    SetModelWorld(World, input);
  
    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;
    output.InstID = 0;
    output.Color = 0;
    output.TreeType = 0;
    output.Scale = 0;
    return output;
}
MainOutput VS_Model(VertexModel input)
{
    MainOutput output;

    SetModelWorld(World, input);
  
    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;
    output.InstID = input.InstID;
    output.Color = 0;

    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    return output;
}



///////////////////////////////////////////////////////////////////////////////


WaterOutput VS_Model_Water(VertexModel input)
{
    WaterOutput output;

    SetModelWorld(World, input);

    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;


    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    output.Clip = dot(WorldPosition(input.Position), WaterClipPlane);

    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct Keyframe
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float RunningTime;

    float3 Padding;
};

struct TweenFrame
{
    float TakeTweenTime;
    float TweenTime;
    float2 Padding;

    Keyframe Curr;
    Keyframe Next;
};

cbuffer CB_AnimationFrame
{
    TweenFrame Tweenframes[MAX_MODEL_INSTANCE];
};

void SetAnimationWorld(inout matrix world, VertexModel input)
{
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;
    matrix anim = 0;
    matrix nextAnim = 0;

    float boneIndices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float boneWeights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };


    uint clip[2];
    uint currFrame[2];
    uint nextFrame[2];
    //현재동작
    clip[0] = Tweenframes[input.InstID].Curr.Clip;
    currFrame[0] = Tweenframes[input.InstID].Curr.CurrFrame;
    nextFrame[0] = Tweenframes[input.InstID].Curr.NextFrame;
    //다음동작
    clip[1] = Tweenframes[input.InstID].Next.Clip;
    currFrame[1] = Tweenframes[input.InstID].Next.CurrFrame;
    nextFrame[1] = Tweenframes[input.InstID].Next.NextFrame;  

    //[unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);

        anim = lerp(curr, next, (matrix) Tweenframes[input.InstID].Curr.Time);


        [flatten]
        if (clip[1] >= 0) //다음동작이 있는거임
        {
            c0 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = BoneTransforms.Load(uint4(boneIndices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);

            nextAnim = lerp(curr, next, (matrix) Tweenframes[input.InstID].Next.Time); 

            anim = lerp(anim, nextAnim, Tweenframes[input.InstID].TweenTime); //TweenTime 애니 넘어가는 시간
        }

        transform += mul(boneWeights[i], anim); //해당정점의 공간?
    }

    world = mul(transform, input.Transform); //최종 월드
}

DepthOutput VS_Animation_Depth(VertexModel input)
{
    DepthOutput output;

    SetAnimationWorld(World, input);

    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, ShadowView);
    output.Position = mul(output.Position, ShadowProjection);

    output.sPosition = output.Position;

    return output;
}

MainOutput VS_Animation(VertexModel input)
{
    MainOutput output;

    SetAnimationWorld(World, input);
  
    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;
    output.InstID = input.InstID;
    output.Color = 0;

    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    return output;
}

MainOutput_GS VS_Animation_GS(VertexModel input)
{
    MainOutput_GS output;

    SetAnimationWorld(World, input);
  
    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;
    output.InstID = 0;
    output.Color = 0;
    output.TreeType = 0;
    output.Scale = 0;

    return output;
}

WaterOutput VS_Animation_Water(VertexModel input)
{
    WaterOutput output;

    SetAnimationWorld(World, input);

    output.oPosition = input.Position.xyz;
    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;

    output.Position = ViewProjection(output.Position);
    output.wvpPosition = output.Position;

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);
    output.Uv = input.Uv;


    output.sPosition = WorldPosition(input.Position);
    output.sPosition = mul(output.sPosition, ShadowView);
    output.sPosition = mul(output.sPosition, ShadowProjection);

    output.Clip = dot(WorldPosition(input.Position), WaterClipPlane);

    return output;
}