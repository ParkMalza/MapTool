#pragma once
#include "Systems/IExecute.h"

class EnvDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override;

private:
	class Sky* sky;
	bool bSnow;
	class Rain* rain;
	class Snow* snow;

	Shader* terrainShader;
	class Terrain* terrain;

	Shader* billShader;
	class BillBoard* billBoard;
	
private:
	wstring base;
	vector<wstring> textureNames;

	//Shader* cubeShader;
	//D3DXVECTOR3 position;
};