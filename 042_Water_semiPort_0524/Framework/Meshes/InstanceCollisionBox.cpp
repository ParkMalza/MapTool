#include "Framework.h"
#include "InstanceCollisionBox.h"



InstanceCollisionBox::InstanceCollisionBox(Shader * shader, D3DXVECTOR3 highPos, D3DXVECTOR3 lowPos)
	:InstanceMesh(shader)
{
	this->high = highPos;
	this->low = lowPos;
}

InstanceCollisionBox::InstanceCollisionBox(Shader * shader, float width, float height, float depth)
	:InstanceMesh(shader)
{
	high.x = width;
	high.y = height;
	high.z = depth;

	low.x = -width;
	low.y = -height;
	low.z = -depth;
}

InstanceCollisionBox::~InstanceCollisionBox()
{
}

bool InstanceCollisionBox::Selected(D3DXVECTOR3 start, D3DXVECTOR3 direction, UINT index)
{
	D3DXVECTOR3 position(0, 0, 0);
	D3DXVECTOR3 scale(0, 0, 0);

	Transform* transform = GetTransform(index);
	transform->Position(&position);
	transform->Scale(&scale);

	D3DXVECTOR3* temp = new D3DXVECTOR3[vertexCount];

	for (UINT i = 0; i < vertexCount; i++)
	{
		temp[i].x = vertices[i].Position.x * scale.x + position.x;
		temp[i].y = vertices[i].Position.y * scale.y + position.y;
		temp[i].z = vertices[i].Position.z * scale.z + position.z;

	}
	float u, v, distance;

	// front 
	if (D3DXIntersectTri(&temp[0], &temp[1], &temp[2], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}
	if (D3DXIntersectTri(&temp[0], &temp[2], &temp[3], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}


	//
	if (D3DXIntersectTri(&temp[4], &temp[5], &temp[6], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}
	if (D3DXIntersectTri(&temp[4], &temp[6], &temp[7], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}

	//
	if (D3DXIntersectTri(&temp[8], &temp[9], &temp[10], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}
	if (D3DXIntersectTri(&temp[8], &temp[10], &temp[11], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}

	//
	if (D3DXIntersectTri(&temp[12], &temp[13], &temp[14], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}
	if (D3DXIntersectTri(&temp[12], &temp[14], &temp[15], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}

	//
	if (D3DXIntersectTri(&temp[16], &temp[17], &temp[18], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}
	if (D3DXIntersectTri(&temp[16], &temp[18], &temp[19], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}


	//
	if (D3DXIntersectTri(&temp[20], &temp[21], &temp[22], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}
	if (D3DXIntersectTri(&temp[20], &temp[22], &temp[23], &start, &direction, &u, &v, &distance) == TRUE)
	{
		return true;
	}


	return false;
}

void InstanceCollisionBox::Create()
{
	vector<MeshVertex> v;
	//큐브에 있는거 그대로 옮겼음
	//Front
	v.push_back(MeshVertex(low.x, low.y, low.z, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(low.x, high.y, low.z, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(high.x, high.y, low.z, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(high.x, low.y, low.z, 1, 1, 0, 0, -1, 1, 0, 0));

	//Back
	v.push_back(MeshVertex(low.x, low.y, high.z, 1, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(MeshVertex(high.x, low.y, high.z, 0, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(MeshVertex(high.x, high.y, high.z, 0, 0, 0, 0, 1, -1, 0, 0));
	v.push_back(MeshVertex(low.x, high.y, high.z, 1, 0, 0, 0, 1, -1, 0, 0));

	//Top
	v.push_back(MeshVertex(low.x, high.y, low.z, 0, 1, 0, 1, 0, 1, 0, 0));
	v.push_back(MeshVertex(low.x, high.y, high.z, 0, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(MeshVertex(high.x, high.y, high.z, 1, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(MeshVertex(high.x, high.y, low.z, 1, 1, 0, 1, 0, 1, 0, 0));

	//Bottom
	v.push_back(MeshVertex(low.x, low.y, low.z, 1, 1, 0, -1, 0, 0 - 1, 0, 0));
	v.push_back(MeshVertex(high.x, low.y, low.z, 0, 1, 0, -1, 0, 0 - 1, 0, 0));
	v.push_back(MeshVertex(high.x, low.y, high.z, 0, 0, 0, -1, 0, 0 - 1, 0, 0));
	v.push_back(MeshVertex(low.x, low.y, high.z, 1, 0, 0, -1, 0, 0 - 1, 0, 0));

	//Left
	v.push_back(MeshVertex(low.x, low.y, high.z, 0, 1, -1, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(low.x, high.y, high.z, 0, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(low.x, high.y, low.z, 1, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(MeshVertex(low.x, low.y, low.z, 1, 1, -1, 0, 0, 0, 0, -1));

	//Right
	v.push_back(MeshVertex(high.x, low.y, low.z, 0, 1, 1, 0, 0, 0, 0, 1));
	v.push_back(MeshVertex(high.x, high.y, low.z, 0, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(MeshVertex(high.x, high.y, high.z, 1, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(MeshVertex(high.x, low.y, high.z, 1, 1, 1, 0, 0, 0, 0, 1));

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
