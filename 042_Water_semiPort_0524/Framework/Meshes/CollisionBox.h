#pragma once
#include "Mesh.h"

class CollisionBox : public Mesh
{
public:
	VertexCollisionBox vertexCB;
	MeshVertex* GetVertices() { return vertices; }
public:
	CollisionBox(Shader* shader, float highestNormal);
	CollisionBox(Shader* shader, float x, float y, float z);
	CollisionBox(Shader* shader, D3DXVECTOR3 highPos, D3DXVECTOR3 lowPos);
	~CollisionBox();

protected:
	void Create() override;
};