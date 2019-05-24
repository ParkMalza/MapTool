#pragma once
#include "Systems/IExecute.h"

class Test : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}

private:
	Shader* integrationShader;
	//RenderTarget
	class RTVMinimap* renderTarget;
	//Weather
	class Sky* sky;
	class GeometryRain* geoRain;
	class GeometrySnow* geoSnow;
	//Terrain
	class Terrain* terrain;
	class BrushTerrain* bTerrain;
	class PaintTerrain* pTerrain;
	//InstanceMesh
	vector<wstring> meshTextures;
	class MeshRender* meshRender;
	//Billboard
	class BillboardsInstancing* billboardsInstancing;
	//model
	class ModelManager* modelManager;
	//Lights
	class Lights* lights;
	//projector
	class Projector* projector;
	//Shadow
	Shadow* shadow;
private:
	//각종 설정
	void WeatherImgui();
	void SetImgui();
	//Save
	void Save(wstring savePath);
	void Load(wstring savePath);
	void RenderObject();
	void PreRenderObject();
	void SetPass();

private:
	//레이어 사진 넘겨주려고 이름 모아놓기
	vector<wstring> textureNames;
	//메쉬 사진 넘겨주려고 이름 모아놓기
	vector<wstring> meshTexturesNames;
	
private:
	wstring startTerrain;
	wstring paint;
private:
	D3DXVECTOR3 picked;
	bool bSnow;
	bool bRain;

private:
	TextureCube* textureCube;
	ID3DX11EffectShaderResourceVariable* sCubeSrv;
	bool isCube;
};