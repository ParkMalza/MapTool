#pragma once
#include "Systems/IExecute.h"

class ShadowDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override {}
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}


private:
	class Sky* sky;

	Shader* shader;
	Shadow* shadow;
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

	Render2D* render2D;
};