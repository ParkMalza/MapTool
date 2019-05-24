#pragma once
#define MAX_BLIIBOARD_INSTANCE 3500

class BillboardInstance : public Renderer
{
public:
	BillboardInstance(Shader* shader, vector<wstring>& textureNames, class Terrain* terrain);
	virtual ~BillboardInstance();

	void Update() override;
	void Render() override;
	void ShadowRender();

	void CheckHeight();
	void Add(Vector3& position, int& type);
	UINT PushTexture(UINT textureNum);
	UINT& DrawCount() { return drawCount; }
	void DrawCount(int drawCount) { this->drawCount = drawCount; }
	int& Type(UINT index) { return types[index]; }

	Transform* GetTransform(UINT index);
	void GetTexture(Texture** texture, UINT index) { *texture = vTextures[index]; }

	void InstancePass(UINT& val);

	void UpdateTransforms();

private:
	struct VertexScale
	{
		Vector3 Position = Vector3(0, 0, 0);
		Vector2 scale = Vector2(5, 5);
		UINT type = 3; //테스트용
	} vertexScale;

private:
	bool fixedY;
	UINT drawCount;
	UINT pass;
	UINT shadowPass;
	Vector3 scale;
	TextureArray* textures;
	vector<Texture*> vTextures;

private:
	class Terrain* terrain;

	VertexBuffer * instanceBuffer;
	Transform* transforms[MAX_BLIIBOARD_INSTANCE];
	D3DXMATRIX worlds[MAX_BLIIBOARD_INSTANCE];
	int types[MAX_BLIIBOARD_INSTANCE];
private:
	ID3DX11EffectScalarVariable * sType;
};