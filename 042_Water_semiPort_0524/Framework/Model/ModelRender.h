#pragma once
class ModelRender
{
public:
	ModelRender(Shader* shader, Model* model);
	~ModelRender();

	void Update();
	void Render();

	void Pass(UINT val);

	void MakeInstance(class Terrain* terrain, D3DXVECTOR3& picked, D3DXVECTOR3& scale = D3DXVECTOR3(1,1,1));
	void MakeInstace(UINT meshNum, class Terrain* terrain);
	void SaveDeleteIndex(UINT val);
	UINT GetDrawCount() { return drawCount; }

	//Deprecated
	//Transform* GetTransform() { return transform; }
	UINT AddTransform();
	Transform* GetTransform(UINT index);
	Shader* boxShader;
	class CollisionBox* box;
	void UpdateTransform();
private:

	void UpdateBoneTransforms();
private:
	Model* model; //외부에서 모델 받아와서 렌더
	UINT drawCount;
	Transform* transforms[MAX_MODEL_INSTANCE];
	D3DXMATRIX worlds[MAX_MODEL_INSTANCE];

	D3DXMATRIX boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS];
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;

	VertexBuffer* instanceBuffer;

	vector<int> saveDeleteIndexes;
	
//private:
//	struct InstanceDesc
//	{
//		UINT index;
//		Transform* Transform;
//	};
//	vector<InstanceDesc> updateTransforms;
};