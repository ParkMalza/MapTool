#pragma once
#include "Systems/IExecute.h"

class WaterDemo : public IExecute
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
	class Sky* sky;
	class Water* water;

	Shader* shader;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshSphere* sphere[10];
	MeshCylinder* cylinder[10];
	MeshCube* cube;
	MeshGrid* grid;

	UINT meshPass = 0;
	UINT modelPass = 1;
	UINT animPass = 2;

	Model* model[2];
	ModelRender* modelRender;
	ModelAnimator* modelAnimator;
};