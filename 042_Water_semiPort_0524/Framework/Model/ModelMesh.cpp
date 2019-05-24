#include "Framework.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"

ModelBone::ModelBone()
{

}

ModelBone::~ModelBone()
{

}

///////////////////////////////////////////////////////////////////////////////

ModelMesh::ModelMesh()
{

}

ModelMesh::~ModelMesh()
{
	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	for (ModelMeshPart* part : meshParts)
		SafeDelete(part);
}

void ModelMesh::Pass(UINT pass)
{
	for (ModelMeshPart* part : meshParts)
		part->Pass(pass);
}

void ModelMesh::SetShader(Shader * shader)
{
	for (ModelMeshPart* part : meshParts)
		part->SetShader(shader);
}

void ModelMesh::Update()
{
	for (ModelMeshPart* part : meshParts)
		part->Update();
}

void ModelMesh::Render(UINT count)
{
	vertexBuffer->Render();
	indexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (ModelMeshPart* part : meshParts)
		part->Render(count);
}

void ModelMesh::TransformsSRV(ID3D11ShaderResourceView * srv)
{
	for(ModelMeshPart* part : meshParts)
		part->transformsSRV = srv;
}

void ModelMesh::FrameBuffer(ConstantBuffer * buffer)
{
	for (ModelMeshPart* part : meshParts)
		part->FrameBuffer(buffer);
}

//void ModelMesh::Transforms(D3DXMATRIX * transforms)
//{
//	for (ModelMeshPart* part : meshParts)
//		part->Transforms(transforms);
//}
//
//void ModelMesh::SetTransform(Transform * transform)
//{
//	for (ModelMeshPart* part : meshParts)
//		part->SetTransform(transform);
//}

void ModelMesh::Binding(Model * model)
{
	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(Model::ModelVertex));
	indexBuffer = new IndexBuffer(indices, indexCount);

	for (ModelMeshPart* part : meshParts)
		part->Binding(model);
}
