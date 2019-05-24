#pragma once
#include "stdafx.h"

struct AsMaterial
{
	string Name;

	D3DXCOLOR Ambient;
	D3DXCOLOR Diffuse;
	D3DXCOLOR Specular;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;
};

struct AsBone
{
	int Index;
	string Name;

	int Parent;
	D3DXMATRIX Transform;
};

struct AsMeshPart
{
	string Name;
	string MaterialName;

	UINT StartVertex;  //몇번 정점에서 그리기 시작할지
	UINT VertexCount;

	UINT StartIndex;  //몇번 인데스에서 그리기 시작할지
	UINT IndexCount;
};

struct AsMesh
{
	string Name;
	int BoneIndex;   //본 번호

	vector<Model::ModelVertex> Vertices;
	vector<UINT> Indices;

	vector<AsMeshPart *> MeshParts;
};

struct AsKeyframeData
{
	float Time;
	//본에 대한 정보
	D3DXVECTOR3 Scale;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
};

struct AsKeyframe
{
	string BoneName;
	vector<AsKeyframeData> Transforms;
};

struct AsClipNode
{
	vector<AsKeyframeData> Keyframe;
	aiString Name;
};

struct AsClip
{
	string Name;

	UINT FrameCount; //전체 프레임 수 (카추진 60)
	float FrameRate; //몇 프레임으로 다운받는지..(카추진 30)
	float Duration;  //전체 애니메이션의 시간

	vector<AsKeyframe *> Keyframes;
};

struct AsBlendWeight  //우리 메쉬 정점에다가 옮기기 위한 놈인건가..
{
	D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
	D3DXVECTOR4 Weights = D3DXVECTOR4(0, 0, 0, 0);

	void Set(UINT index, UINT boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
			case 0: Indices.x = i; Weights.x = w; break;
			case 1: Indices.y = i; Weights.y = w; break;
			case 2: Indices.z = i; Weights.z = w; break;
			case 3: Indices.w = i; Weights.w = w; break;
		}
	}
};

struct AsBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddWeights(UINT boneIndex, float boneWeights)
	{
		if (boneWeights <= 0.0f) return;

		bool bAdd = false;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (boneWeights > it->second)
			{
				BoneWeights.insert(it, Pair(boneIndex, boneWeights));
				bAdd = true;

				break;
			}

			it++;
		} // while(it)

		if (bAdd == false) //흠.. 뭐하는거지..
			BoneWeights.push_back(Pair(boneIndex, boneWeights));
	}

	void GetBlendWeights(AsBlendWeight& blendWeights)
	{
		for (UINT i = 0; i < BoneWeights.size(); i++)
		{
			if (i >= 4) return; //4개밖에 못쓰게 막아놓은거?

			blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
		}
	}

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				i++; it++;
			}
			else
				it = BoneWeights.erase(it);
		}

		float scale = 1.0f / totalWeight;

		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= scale;
			it++;
		}
	}
};