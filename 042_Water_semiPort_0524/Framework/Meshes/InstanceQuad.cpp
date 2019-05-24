#include "Framework.h"
#include "InstanceQuad.h"

InstanceQuad::InstanceQuad(Shader * shader)
	:InstanceMesh(shader)
{
}

InstanceQuad::~InstanceQuad()
{
}

void InstanceQuad::Create()
{
	float w = MESH_DEFAUL_LENGTH;
	float h = MESH_DEFAUL_LENGTH;

	vector<MeshVertex> v;
	v.push_back(MeshVertex(-w, -h, 0, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(-w, +h, 0, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, -h, 0, 1, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(MeshVertex(+w, +h, 0, 1, 0, 0, 0, -1, 1, 0, 0));

	vertices = new MeshVertex[v.size()];  //버텍스 4개필요, 인덱스가 6개 필요.
	vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<MeshVertex *>(vertices, vertexCount) //벡터를 배열로 복사 해 주는 방법
	);


	indexCount = 6;
	indices = new UINT[indexCount]
	{
		0, 1, 2, 2, 1, 3
	};
}
