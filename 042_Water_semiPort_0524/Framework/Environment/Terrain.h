#pragma once

class Terrain : public Renderer
{
public:
	typedef VertexColorTextureNormalTangent TerrainVertex;

public:
	Terrain(Shader* shader, wstring heightMap, wstring alphaMap, vector<wstring>& textureNames);
	~Terrain();

	void Update() override;
	void Render() override;
	void PreRender();
	void CubeRender();

	void BrushType(UINT val) { brushDesc.Type = val; }
	void BrushLocation(D3DXVECTOR3& vec) { brushDesc.Location = vec; }
	void BrushRange(UINT val) { brushDesc.Range = val; }
	void BrushColor(D3DXCOLOR& color) { brushDesc.Color = color; }
	void BrushRange(UINT* val) { *val = brushDesc.Range; }
	void BrushColor(D3DXCOLOR* color) { *color = brushDesc.Color; }

	void GridLineVisible(UINT val) { gridLineDesc.Visible = val; }
	void GridLineThick(float val) { gridLineDesc.Thickness = val; }
	void GridLineSize(float val) { gridLineDesc.Size= val; }

	const wstring GetBaseTextureName() { return baseTextureName; }
	const wstring GetLayerTextureName() { return layerTextureName; }
	const wstring GetAlphaTextureName() { return alphaTextureName; }
public:
	float GetHeight(D3DXVECTOR3& position);
	float GetPickedHeight(D3DXVECTOR3& position);
	D3DXVECTOR3 GetPickedHeight();

	void RaiseHeight(D3DXVECTOR3& position, UINT type, UINT range, float speed);
	void FallDownHeight(D3DXVECTOR3& position, UINT type, UINT range, float speed);
	void LockHeight(D3DXVECTOR3& position, UINT type, UINT range, float _height, float speed);
	void Smoothing(D3DXVECTOR3& position, UINT type, UINT range, float speed);
	//사진 교체
	void BaseMap(wstring file);  
	void LayerMap(wstring file);
	void LayerMaps(wstring file, int i);
	//사진 받아오려고
	void GetBaeMap(Texture** texture);
	void LayerMap1(Texture** texture);
	void LayerMap2(Texture** texture);
	void LayerMap3(Texture** texture);
	void LayerMap4(Texture** texture);

	void BrushLayer(D3DXVECTOR3& position, UINT type, UINT range, float power, UINT layerNum);
	void SettingAlpha(float alpha);

	void Save(wstring fileName);
	void Load(wstring fileName);

	void Mapping();
	void CreateNormalData();

public:
	void SetOldTerrain();  //클릭시 실행.. redo를 날릴지 결정
	void MakeOldTerrain();
	void MakeNewTerrain();

	void UndoTerrain(); // c + z
	void RedoTerrain(); // c + y

private:
	vector<TerrainVertex*> undoStack;
	vector<TerrainVertex*> redoStack;
	vector<TerrainVertex*>::iterator it;

private:
	void CreateVertexData();
	void CreateIndexData();

private:
	struct BrushDesc
	{
		//16바이트씩 맞춰야 한다. 
		//물론 쉐이더에선 그럴필요는 없음
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);
		D3DXVECTOR3 Location = D3DXVECTOR3(-1, -1, -1);
		int Type = 0;
		int Range = 1;

		float Padding[3];//16바이트 맞추기 위한 쓰레기값  //여기서 12바이트 추가해줘따
	} brushDesc;

	struct GridLineDesc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		int Visible = 1; //보여질 것이냐
		float Thickness = 0.1f; //그려지는 선의 두께
		float Size = 255; //size 크기가 그리드 한칸의 변 길이

		float Padding;
	} gridLineDesc;

	float alpha;

	bool isBase;

private:
	Texture* heightMap;
	Texture* layer[4];

	UINT width, height;
	TerrainVertex* vertices;
	TerrainVertex* oldTerrainVertices;
	TerrainVertex* newTerrainVertices;

	UINT* indices;

	wstring baseTextureName;
	wstring layerTextureName;
	wstring alphaTextureName;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* gridLineBuffer;
	ID3DX11EffectConstantBuffer* sGridLineBuffer;

	Texture* baseMap;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* layerMap;
	ID3DX11EffectShaderResourceVariable* sLayerMap;

	Texture* alphaMap;
	ID3DX11EffectShaderResourceVariable* sAlphaMap;

	TextureArray* layerMaps;
	ID3DX11EffectShaderResourceVariable* sLayerMaps;

	ID3DX11EffectScalarVariable * sIndex;

	vector<wstring> layersNames;

private:
	UINT cubeMapPass;
	UINT shadowPass;
	UINT renderPass;
};