#include "Framework.h"
#include "ModelRender.h"
#include "ModelMesh.h"
#include "Environment\Terrain.h"

ModelRender::ModelRender(Shader * shader, Model * model)
	: model(model), drawCount(0)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->SetShader(shader);

	instanceBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(D3DXMATRIX), 1, true);

	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;  
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
		Check(hr);
	}

	//Create SRV
	{
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = desc.Format;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);
		Check(hr);
	}

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		transforms[i] = new Transform();

	for (ModelMesh* mesh : model->Meshes())
		mesh->TransformsSRV(srv);

	UpdateBoneTransforms();

	//boxShader = new Shader(L"010_CollisionBox.fx");
	//box = new CollisionBox(boxShader, model->GetHighVal() * 0.015f, model->GetLowVal() * 0.015f);
}

ModelRender::~ModelRender()
{
	//SafeDelete(transform);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		SafeDelete(transforms[i]);

	SafeRelease(texture);
	SafeRelease(srv);
}

void ModelRender::Update()
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelRender::Render()
{
	instanceBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (ModelMesh* mesh : model->Meshes())
	{
		//Depreated
		//mesh->SetTransform(transform);
		mesh->Render(drawCount);
	}
}

void ModelRender::Pass(UINT val)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(val);
}

void ModelRender::MakeInstance(Terrain * terrain, D3DXVECTOR3 & picked, D3DXVECTOR3& size)
{
	UINT index = AddTransform();
	Transform* transform = GetTransform(index);

	D3DXVECTOR3 position(0, 0, 0);
	position.x = picked.x;
	position.z = picked.z;
	position.y = terrain->GetPickedHeight(position);
	D3DXVECTOR3 rotation(0, 0, 0);

	transform->Position(position);
	transform->Rotation(rotation);
	transform->Scale(size);

	UpdateTransform();
}

void ModelRender::MakeInstace(UINT meshNum, Terrain * terrain)
{
	for (UINT i = 0; i < meshNum; i++)
	{
		UINT index = AddTransform();
		Transform* transform = GetTransform(index);
		D3DXVECTOR3 scale(1, 1, 1);
		D3DXVECTOR3 position(0, 0, 0);
		position.x = Math::Random(1.0f, 250.0f);
		position.z = Math::Random(1.0f, 250.0f);
		position.y = terrain->GetPickedHeight(position);
		D3DXVECTOR3 rotation(0, 0, 0);

		transform->Position(position);
		transform->Rotation(rotation);
		transform->Scale(scale);
	}
	UpdateTransform();
}

void ModelRender::SaveDeleteIndex(UINT val)
{
	//saveDeleteIndexes.push_back(val);
	for (UINT i = val; i < drawCount; i++)
		transforms[i] = transforms[i + 1];
	drawCount--;
}

UINT ModelRender::AddTransform()
{
	//if (saveDeleteIndexes.size() > 0)
	//{
	//	int temp = saveDeleteIndexes.back();
	//	saveDeleteIndexes.pop_back();
	//	return temp;
	//}
	//else
	//{
	//	drawCount++;
	//	return drawCount - 1;
	//}
	drawCount++;
	return drawCount - 1;
}

Transform* ModelRender::GetTransform(UINT index)
{
	return transforms[index];
}

void ModelRender::UpdateTransform()
{
	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		worlds[i] = transforms[i]->World();


	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);
}

void ModelRender::UpdateBoneTransforms()
{
	D3DXMATRIX bones[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			D3DXMATRIX parentTransform;
			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
				D3DXMatrixIdentity(&parentTransform);
			else
				parentTransform = bones[parentIndex];


			D3DXMATRIX matrix = bone->Transform();

			bones[b] = parentTransform;
			boneTransforms[i][b] = matrix * bones[b];
		}
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		{
			void* p = ((D3DXMATRIX *)subResource.pData + (i * MAX_MODEL_TRANSFORMS));

			memcpy(p, boneTransforms[i], sizeof(D3DXMATRIX) * MAX_MODEL_TRANSFORMS);
		}
	}
	D3D::GetDC()->Unmap(texture, 0);
}
