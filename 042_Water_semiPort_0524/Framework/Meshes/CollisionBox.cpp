#include "Framework.h"
#include "CollisionBox.h"

CollisionBox::CollisionBox(Shader * shader, float highestNormal)
	:Mesh(shader)
{
	vertexCB.minusX = -highestNormal;
	vertexCB.minusY = -highestNormal;
	vertexCB.minusZ = -highestNormal;

	vertexCB.plusX = highestNormal;
	vertexCB.plusY = highestNormal;
	vertexCB.plusZ = highestNormal;
}

CollisionBox::CollisionBox(Shader * shader, float x, float y, float z)
	:Mesh(shader)
{
	vertexCB.minusX = -x;
	vertexCB.minusY = -y;
	vertexCB.minusZ = -z;

	vertexCB.plusX = x;
	vertexCB.plusY = y;
	vertexCB.plusZ = z;
}

CollisionBox::CollisionBox(Shader * shader, D3DXVECTOR3 highPos, D3DXVECTOR3 lowPos)
	:Mesh(shader)
{
	vertexCB.minusX = lowPos.x;
	vertexCB.minusY = lowPos.y;
	vertexCB.minusZ = lowPos.z;

	vertexCB.plusX = highPos.x;
	vertexCB.plusY = highPos.y;
	vertexCB.plusZ = highPos.z;
}

CollisionBox::~CollisionBox()
{
}

void CollisionBox::Create()
{
	vector<MeshVertex> v;
	//큐브에 있는거 그대로 옮겼음
	//Front
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.minusY, vertexCB.minusZ, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.plusY, vertexCB.minusZ, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.plusY, vertexCB.minusZ, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.minusY, vertexCB.minusZ, 1, 1, 0, 0, -1, 1, 0, 0));

	//Back
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.minusY, vertexCB.plusZ, 1, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.minusY, vertexCB.plusZ, 0, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.plusY, vertexCB.plusZ, 0, 0, 0, 0, 1, -1, 0, 0));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.plusY, vertexCB.plusZ, 1, 0, 0, 0, 1, -1, 0, 0));

	//Top
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.plusY, vertexCB.minusZ, 0, 1, 0, 1, 0, 1, 0, 0));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.plusY, vertexCB.plusZ, 0, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.plusY, vertexCB.plusZ, 1, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.plusY, vertexCB.minusZ, 1, 1, 0, 1, 0, 1, 0, 0));

	//Bottom
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.minusY, vertexCB.minusZ, 1, 1, 0, -1, 0, -1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.minusY, vertexCB.minusZ, 0, 1, 0, -1, 0, -1, 0, 0));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.minusY, vertexCB.plusZ, 0, 0, 0, -1, 0, -1, 0, 0));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.minusY, vertexCB.plusZ, 1, 0, 0, -1, 0, -1, 0, 0));

	//Left
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.minusY, vertexCB.plusZ, 0, 1, -1, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.plusY, vertexCB.plusZ, 0, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.plusY, vertexCB.minusZ, 1, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(vertexCB.minusX, vertexCB.minusY, vertexCB.minusZ, 1, 1, -1, 0, 0, 0, 0, -1));

	//Right
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.minusY, vertexCB.minusZ, 0, 1, 1, 0, 0, 0, 0, 1));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.plusY, vertexCB.minusZ, 0, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.plusY, vertexCB.plusZ, 1, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(MeshVertex(vertexCB.plusX, vertexCB.minusY, vertexCB.plusZ, 1, 1, 1, 0, 0, 0, 0, 1));

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex *>(vertices, vertexCount)
	);

	indexCount = 36;
	this->indices = new UINT[indexCount]
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
}
