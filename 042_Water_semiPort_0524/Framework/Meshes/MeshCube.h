#pragma once
#include "Mesh.h"

class MeshCube : public Mesh
{
public:
	MeshCube(Shader* shader);
	~MeshCube();

protected:
	void Create() override;

private:
	
};