#pragma once
#include "Systems/IExecute.h"

class LoadModel : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	void ReadTower();
	void ReadTank();
	void ReadCar();
	void ReadKachujin();

private:
	Shader* shader;

	Model* tower;
	ModelRender* towerRender;

	Model* tank;
	ModelRender* tankRender;

	Model* car;
	ModelRender* carRender;

	Model* kachujin;
	ModelAnimator* kachujinRender;
};