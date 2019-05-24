#pragma once
#include "InstanceMesh.h"

class InstanceCube : public InstanceMesh
{
public:
	InstanceCube(Shader* shader);
	~InstanceCube();

protected:
	void Create() override;

private:
};