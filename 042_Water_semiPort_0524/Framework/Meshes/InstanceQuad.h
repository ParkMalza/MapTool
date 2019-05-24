#pragma once
#include "InstanceMesh.h"

class InstanceQuad : public InstanceMesh
{
public:
	InstanceQuad(Shader* shader);
	~InstanceQuad();

protected:
	void Create() override;

};