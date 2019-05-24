#pragma once

class ModelMeshPart
{
public:
	friend class Model;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

public:
	void SetShader(Shader* shader);
	void Pass(UINT val) { pass = val; }

private:
	//Deprecated
	//void Transforms(D3DXMATRIX* transforms);
	//void SetTransform(Transform* transform);

	void TransformsSRV(ID3D11ShaderResourceView* srv);
	void FrameBuffer(ConstantBuffer* buffer) { frameBuffer = buffer; }

	void Update();
	void Render(UINT count);

	void Binding(Model* model);

private:
	struct BoneDesc
	{
		//Deprecated
		//D3DXMATRIX Transforms[MAX_MODEL_TRANSFORMS];

		UINT Index;
		float Padding[3];
	} boneDesc;

private:
	wstring name;

	Shader* shader;
	UINT pass;

	PerFrame* perFrame;
	//Deprecated
	//Transform* transform;

	class ModelMesh* parent;

	Material* material;
	wstring materialName;

	UINT startVertex;
	UINT vertexCount;

	UINT startIndex;
	UINT indexCount;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;
	ID3D11ShaderResourceView* transformsSRV;
	ID3DX11EffectShaderResourceVariable* sTransfromsSRV;

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;
};