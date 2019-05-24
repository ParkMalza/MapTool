#pragma once
#include "InstanceMesh.h"

class InstanceGrid : public InstanceMesh
{
public:
	InstanceGrid(Shader* shader, float offsetU = 1.0f, float offsetV = 1.0f);
	~InstanceGrid();

protected:
	void Create() override;

private:
	float offsetU;
	float offsetV;

	Texture* texture;
};