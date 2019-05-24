#pragma once
#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	MeshQuad(Shader* shader);
	~MeshQuad();

protected:
	void Create() override;

private:
	
};