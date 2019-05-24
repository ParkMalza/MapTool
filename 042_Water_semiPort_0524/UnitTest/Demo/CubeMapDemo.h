#pragma once
#include "Systems/IExecute.h"

class CubeMapDemo : public IExecute
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
	void RenderObject();

private:
	class Sky* sky;

	Shader* shader;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshSphere* sphere[10];
	MeshCylinder* cylinder[10];
	MeshCube* cube;
	MeshGrid* grid;

	Model* model[2];
	ModelRender* modelRender[2];


	TextureCube* textureCube;
	TextureCube* textureCube2;
	//ID3D11ShaderResourceView* cubeSrv;
	ID3DX11EffectShaderResourceVariable* sCubeSrv;
};