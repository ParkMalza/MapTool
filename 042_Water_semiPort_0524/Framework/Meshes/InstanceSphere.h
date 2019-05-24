#pragma once
#include "InstanceMesh.h"

class InstanceSphere : public InstanceMesh
{
public:
	InstanceSphere(Shader* shader, float radius, UINT stackCount = 20, UINT sliceCount = 20);
	~InstanceSphere();

protected:
	void Create() override;

private:
	float radius;

	UINT stackCount;
	UINT sliceCount;  	//slicecount = 정점을 얼마나 세분화 할것인가

	Texture* texture;
};