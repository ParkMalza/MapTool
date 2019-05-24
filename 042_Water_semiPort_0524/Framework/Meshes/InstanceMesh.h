#pragma once
#define MAX_INSTANCE 200

class InstanceMesh : public Renderer
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	InstanceMesh(Shader* shader);
	virtual ~InstanceMesh();

	void Update() override;
	void Render() override;

	void Color(D3DXCOLOR& val);
	void Color(float r, float g, float b);
	void Color(D3DXCOLOR* val);

	void Colors(D3DXCOLOR& val, int selectNum);
	void Colors(float r, float g, float b, int selectNum);
	void Colors(D3DXCOLOR* val, int selectNum);

	void Position(D3DXVECTOR3* position) { *position = this->position; }
	void Position(D3DXVECTOR3& position) { position = this->position; }
	void Scale(D3DXVECTOR3* scale) { *scale = this->scale; }
	void Scale(D3DXVECTOR3& scale) { scale = this->scale; }
	void Rotation(D3DXVECTOR3* rotation) { *rotation = this->rotation; }
	void Rotation(D3DXVECTOR3& rotation) { rotation = this->rotation; }

	void MakeInstance(UINT meshNum, class Terrain* terrain);
	void MakeInstance(D3DXVECTOR3& picked, class Terrain* terrain, UINT textureNum);
	void MakeInstance(D3DXVECTOR3& picked, UINT textureNum);
	UINT Push();
	UINT PushTexture(UINT textureNum);
	void ChangeTexture(UINT selectNum, UINT textureNum);
	void SaveDeleteIndex(UINT val);
	void DeleteIndex(UINT index);
	void DeleteCollboxIndex(UINT index);

	Transform* GetTransform(UINT index);
	UINT GetTexture(UINT index);
	UINT GetTextureNum(UINT selectNum);
	UINT GetDrawCount() { return drawCount; }

	void Type(UINT i) { type = i; }
	void InstancePass(UINT val) { pass = val; }

	virtual bool Selected(D3DXVECTOR3 start, D3DXVECTOR3 direction, UINT index);
	
	//언두 리두
public:
	void SetOldMesh();
	void MakeOldMesh();
	void MakeNewMesh();
	//void MeshtoNew();
	void Undo();
	void Redo();
protected:
	virtual void Create() = 0;

protected:
	MeshVertex* vertices;
	UINT* indices;
	UINT type;

	D3DXCOLOR color;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	int textures[MAX_INSTANCE];
	D3DXCOLOR colors[MAX_INSTANCE];
private:
	ID3DX11EffectVectorVariable* sColor;
	VertexBuffer* instanceBuffer;
	ID3DX11EffectScalarVariable * sIndex;
private:
	UINT drawCount;
	UINT pass;
	UINT saveIndex;

	Transform* transforms[MAX_INSTANCE];
	D3DXMATRIX worlds[MAX_INSTANCE];
private:
	struct MeshSave
	{
		Transform transform[MAX_INSTANCE];
		int textures[MAX_INSTANCE];
	};

	MeshSave oldMesh;
	MeshSave newMesh;

	vector<Transform*> transes;
	vector<int> texes;

	vector<Transform*> oldTrans;
	vector<int> oldTex;
	vector<Transform*> newTrans;
	vector<int> newTex;

	vector<MeshSave> vUndoStack;
	vector<MeshSave> vRedoStack;
	vector<vector<Transform*>> undoStack;
	vector<vector<int>> texUndoStack;
	vector<vector<Transform*>> redoStack;
	vector<vector<int>> texRedoStack;
	vector<MeshSave>::iterator it;
private:
	vector<D3DXVECTOR3> save;
	vector<int> saveTextureIndex;
	vector<D3DXCOLOR> saveColor;

	vector<int> saveDeleteIndexes;
};