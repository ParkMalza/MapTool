#include "framework.h"
#include "ModelMeshPart.h"
#include "ModelMesh.h"

ModelMeshPart::ModelMeshPart()
	: material(NULL), perFrame(NULL), pass(0)
	,frameBuffer(NULL)
	//transform(NULL),
{
	boneBuffer = new ConstantBuffer(&boneDesc, sizeof(BoneDesc));
}

ModelMeshPart::~ModelMeshPart()
{
	SafeDelete(boneBuffer);
	//SafeDelete(transform);
	SafeDelete(perFrame);
}

void ModelMeshPart::SetShader(Shader * shader)
{
	this->shader = shader;


	//SafeDelete(transform);
	//transform = new Transform(shader);

	SafeDelete(perFrame);
	perFrame = new PerFrame(shader);

	material->SetShader(shader);

	sBoneBuffer = shader->AsConstantBuffer("CB_Bone");

	sTransfromsSRV = shader->AsSRV("BoneTransforms");
	sFrameBuffer = shader->AsConstantBuffer("CB_AnimationFrame");
}

//void ModelMeshPart::Transforms(D3DXMATRIX * transforms)
//{
//	memcpy(boneDesc.Transforms, transforms, sizeof(D3DXMATRIX) * MAX_MODEL_TRANSFORMS);
//}
//
//void ModelMeshPart::SetTransform(Transform * transform)
//{
//	this->transform->Set(transform);
//}

void ModelMeshPart::TransformsSRV(ID3D11ShaderResourceView * srv)
{
	transformsSRV = srv;
}

void ModelMeshPart::Update()
{
	boneDesc.Index = parent->BoneIndex();

	perFrame->Update();
	//transform->Update();
}

void ModelMeshPart::Render(UINT count)
{
	boneBuffer->Apply();
	sBoneBuffer->SetConstantBuffer(boneBuffer->Buffer());

	if (frameBuffer != NULL)
	{
		frameBuffer->Apply();
		sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
	}

	sTransfromsSRV->SetResource(transformsSRV);

	perFrame->Render();
	//transform->Render();
	material->Render();
	//shader->DrawIndexed(0, pass, indexCount, startIndex, 0);
	shader->DrawIndexedInstanced(0, pass, indexCount, count, startIndex);
}

void ModelMeshPart::Binding(Model * model)
{
	Material* srcMaterial = model->MaterialByName(materialName);

	material = new Material();
	material->Ambient(srcMaterial->Ambient());
	material->Diffuse(srcMaterial->Diffuse());
	material->Specular(srcMaterial->Specular());

	if (srcMaterial->DiffuseMap() != NULL)
		material->DiffuseMap(srcMaterial->DiffuseMap()->GetFile());

	if (srcMaterial->SpecularMap() != NULL)
		material->SpecularMap(srcMaterial->SpecularMap()->GetFile());

	if (srcMaterial->NormalMap() != NULL)
		material->NormalMap(srcMaterial->NormalMap()->GetFile());
}
