#include "Framework.h"
#include "BillboardInstance.h"
#include "Environment/Terrain.h"
#include "Utilities/BinaryFile.h"

BillboardInstance::BillboardInstance(Shader * shader, vector<wstring>& textureNames, Terrain * terrain)
	:Renderer(shader), fixedY(true), terrain(terrain), drawCount(0), pass(13), shadowPass(9)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	textures = new TextureArray(textureNames);
	shader->AsSRV("BillboardTextures")->SetResource(textures->SRV());
	sType = shader->AsScalar("BillboardsTypes");
	for (wstring name : textureNames)
	{
		wstring temp = Path::GetFileName(name);
		temp = L"Grass/" + temp;
		vTextures.push_back(new Texture(temp));
	}

	for (UINT i = 0; i < MAX_BLIIBOARD_INSTANCE; i++)
	{
		D3DXMatrixIdentity(&worlds[i]);
		transforms[i] = new Transform();
		transforms[i]->Position(0, 0, 0);
		types[i] = -1;
	}
	scale = Vector3(5.0f, 5.0f, 1.0f);

	vertexBuffer = new VertexBuffer(&vertexScale, 1, sizeof(VertexScale), 0);
	instanceBuffer = new VertexBuffer(worlds, MAX_BLIIBOARD_INSTANCE, sizeof(Matrix), 1, true);

}

BillboardInstance::~BillboardInstance()
{
	SafeRelease(sType);
}

void BillboardInstance::Update()
{
	Renderer::Update();
	UpdateTransforms();
	CheckHeight();
}

void BillboardInstance::Render()
{
	Renderer::Render();
	instanceBuffer->Render();
	sType->SetIntArray(types, 0, MAX_BLIIBOARD_INSTANCE);
	shader->DrawIndexedInstanced(0, pass, 1, drawCount);
}

void BillboardInstance::ShadowRender()
{
	Renderer::Render();
	instanceBuffer->Render();
	sType->SetIntArray(types, 0, MAX_BLIIBOARD_INSTANCE);
	shader->DrawIndexedInstanced(0, shadowPass, 1, drawCount);
}

void BillboardInstance::CheckHeight()
{
	for (UINT i = 0; i < drawCount; i++)
	{
		Vector3 temp;
		//Vector3 tempS;
		transforms[i]->Position(&temp);
		//transforms[i]->Scale(&tempS);
		float y = terrain->GetPickedHeight(temp);
		temp.y = y + 2.5f;
		transforms[i]->Position(temp);
	}
}

void BillboardInstance::Add(Vector3 & position, int& type)
{
	if (drawCount < MAX_BLIIBOARD_INSTANCE)
	{
		transforms[drawCount]->Position(position);
		types[drawCount] = type;
		drawCount++;
	}
	else
		return;
}

UINT BillboardInstance::PushTexture(UINT textureNum)
{
	return 0;
}

Transform * BillboardInstance::GetTransform(UINT index)
{
	return transforms[index];
}

void BillboardInstance::InstancePass(UINT& val)
{
	pass = val;
}

void BillboardInstance::UpdateTransforms()
{
	for (UINT i = 0; i < MAX_BLIIBOARD_INSTANCE; i++)
		memcpy(&worlds[i], &transforms[i]->World(), sizeof(D3DXMATRIX));

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * MAX_BLIIBOARD_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);
}
