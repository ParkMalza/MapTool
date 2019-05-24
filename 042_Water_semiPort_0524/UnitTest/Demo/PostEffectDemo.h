#pragma once
#include "Systems/IExecute.h"

class PostEffectDemo : public IExecute
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
	RenderTarget* renderTarget;  //오리지날
	RenderTarget* targets[10];

	DepthStencil* depthStencil;
	Viewport* viewport;

	Render2D* postEffect;

	Render2D* render2D;
	Render2D* finalRender2D;
	ID3D11ShaderResourceView* finalSrv;

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

	D3DXCOLOR specular;

#ifdef  ALLOW_MULTI_VIEWPORT
	RenderTarget* minimapTarget;
	DepthStencil* minimapDepth;
	Viewport* minimapViewPort;
	Render2D* minimap2D;
#endif //  ALLOW_MULTI_VIEWPORT

};