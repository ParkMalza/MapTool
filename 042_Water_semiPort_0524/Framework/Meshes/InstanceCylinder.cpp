#include "Framework.h"
#include "InstanceCylinder.h"

InstanceCylinder::InstanceCylinder(Shader * shader, float radius, float height, UINT sliceCount, UINT stackCount)
	: InstanceMesh(shader)
	, topRadius(radius), bottomRadius(radius), height(height), sliceCount(sliceCount), stackCount(stackCount)
{

}

InstanceCylinder::~InstanceCylinder()
{

}

void InstanceCylinder::Create()
{
	vector<MeshVertex> vertices;  //버텍스 벡터

	float stackHeight = height / (float)stackCount;  //하나의 스택의 높이, 0.1
	float radiusStep = (topRadius - bottomRadius) / (float)stackCount;  //반지름 같을시 0으로 처리, 0


	UINT ringCount = stackCount + 1; //   11
	for (UINT i = 0; i < ringCount; i++)  //  0 ~ 10  11번돈다.
	{
		float y = -0.5f * height + i * stackHeight; //  -0.5, 
		float r = bottomRadius + i * radiusStep;  //  0.5, 
		float theta = 2.0f * Math::PI / (float)sliceCount;  //  6.28 / 10 = 0.628..;

		for (UINT k = 0; k <= sliceCount; k++) // 0 ~ 10  11번 돈다.
		{
			float c = cosf(k * theta);  //cos 0 = 1
			float s = sinf(k * theta);  //sin 0 = 0


			MeshVertex vertex;
			vertex.Position = D3DXVECTOR3(r * c, y, r * s); //(0,5, -0.5, 0)


			D3DXVECTOR3 tangent = D3DXVECTOR3(-s, 0.0f, c); // 0, 0, 1

			float dr = bottomRadius - topRadius;
			D3DXVECTOR3 biTangent = D3DXVECTOR3(dr * c, -height, dr * s); //(0, -1, 0)

			D3DXVec3Cross(&vertex.Normal, &tangent, &biTangent);
			D3DXVec3Normalize(&vertex.Normal, &vertex.Normal);
			vertex.Uv = D3DXVECTOR2((float)k / (float)sliceCount, 1.0f - (float)i / (float)stackCount);

			vertices.push_back(vertex);
		}
	}
	//면 만들기

	vector<UINT> indices;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT y = 0; y < stackCount; y++)
	{
		for (UINT x = 0; x < sliceCount; x++)
		{
			indices.push_back(y * ringVertexCount + x);
			indices.push_back((y + 1) * ringVertexCount + x);
			indices.push_back((y + 1) * ringVertexCount + (x + 1));

			indices.push_back(y * ringVertexCount + x);
			indices.push_back((y + 1) * ringVertexCount + x + 1);
			indices.push_back(y * ringVertexCount + x + 1);
		}
	}

	BuildTopCap(vertices, indices);
	BuildBottomCap(vertices, indices);
	//위뚜껑 아래뚜껑

	this->vertices = new MeshVertex[vertices.size()];
	vertexCount = vertices.size();
	copy(vertices.begin(), vertices.end(), stdext::checked_array_iterator<MeshVertex *>(this->vertices, vertexCount));

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT *>(this->indices, indexCount));
}

void InstanceCylinder::BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	float y = 0.5f * height;
	float theta = 2.0f * Math::PI / (float)sliceCount;

	for (UINT i = 0; i <= sliceCount; i++)
	{
		float x = topRadius * cosf(i * theta);
		float z = topRadius * sinf(i * theta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		vertices.push_back(MeshVertex(x, y, z, u, v, 0, 1, 0, 1, 0, 0));
	}
	vertices.push_back(MeshVertex(0, y, 0, 0.5f, 0.5f, 0, 1, 0, 1, 0, 0));


	UINT baseIndex = vertices.size() - sliceCount - 2;
	UINT centerIndex = vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}

void InstanceCylinder::BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	float y = -0.5f * height;
	float theta = 2.0f * Math::PI / (float)sliceCount;

	for (UINT i = 0; i <= sliceCount; i++)
	{
		float x = bottomRadius * cosf(i * theta);
		float z = bottomRadius * sinf(i * theta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		vertices.push_back(MeshVertex(x, y, z, u, v, 0, -1, 0, 1, 0, 0));
	}
	vertices.push_back(MeshVertex(0, y, 0, 0.5f, 0.5f, 0, -1, 0, 1, 0, 0));


	UINT baseIndex = vertices.size() - sliceCount - 2;
	UINT centerIndex = vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}
}