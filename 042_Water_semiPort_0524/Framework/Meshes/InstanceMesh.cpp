#include "Framework.h"
#include "InstanceMesh.h"
#include "Environment/Terrain.h"
#include "Utilities/BinaryFile.h"

InstanceMesh::InstanceMesh(Shader * shader)
	: Renderer(shader), color(1, 1, 1, 1),drawCount(0), saveIndex(0), pass(0), type(0)
{
	scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	rotation = D3DXVECTOR3(0, 0, 0);
	
	sColor = shader->AsVector("Colors");
	sIndex = shader->AsScalar("TextureType");  //텍스쳐 뭐 찍는지 shader에 값 넣어줄것
	//ImGui::IsAnyItemHovered();
	for (UINT i = 0; i < MAX_INSTANCE; i++)
	{
		D3DXMatrixIdentity(&worlds[i]);
		transforms[i] = new Transform();
		colors[i] = D3DXCOLOR(1, 1, 1, 1);
	}
	instanceBuffer = new VertexBuffer(worlds, MAX_INSTANCE, sizeof(D3DXMATRIX), 1, true);
}

InstanceMesh::~InstanceMesh()
{
	SafeDelete(instanceBuffer);
	SafeDeleteArray(indices);
	SafeDeleteArray(vertices);
	for (UINT i = 0; i < MAX_INSTANCE; i++)
		SafeDelete(transforms[i]);
}

void InstanceMesh::Update()
{
	Renderer::Update();
	for (UINT i = 0; i < MAX_INSTANCE; i++)
		memcpy(&worlds[i], &transforms[i]->World(), sizeof(D3DXMATRIX));

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * MAX_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);

}

void InstanceMesh::Render()
{
	Renderer::Render();

	if (vertexBuffer == NULL && indexBuffer == NULL)
	{
		Create();

		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(MeshVertex));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}
	instanceBuffer->Render();
	sColor->SetFloatVectorArray((float*)colors, 0, MAX_INSTANCE);
	sIndex->SetIntArray(textures, 0, MAX_INSTANCE); //쉐이더에 값 넣어주기

	shader->DrawIndexedInstanced(0, pass, indexCount, drawCount);
}

void InstanceMesh::MakeInstance(UINT meshNum, Terrain * terrain)
{
	for (UINT i = 0; i < meshNum; i++)
	{
		UINT index = Push();
		Transform* transform = GetTransform(index);

		position = Math::RandomVec3(0.5f, 254.5f);
		position.y = terrain->GetHeight(position) + (scale.y * 0.5f);

		transform->Position(position);
		transform->Scale(scale);
	}
}

void InstanceMesh::MakeInstance(D3DXVECTOR3 & picked, Terrain * terrain, UINT textureNum)
{
	UINT index = PushTexture(textureNum); //0, textures[0]
	Transform* transform = GetTransform(index);

	texes.push_back(textureNum);
	transes.push_back(transform);

	position = picked;
	position.y = terrain->GetHeight(position) + (scale.y * 5.0f);

	transform->Position(position);
	transform->Scale(scale);
	transform->Rotation(rotation);

	//save용
	transform->Position(&position);
	transform->Scale(&scale);
	transform->Rotation(&rotation);

	//save
	save.push_back(position);
	save.push_back(scale);
	save.push_back(rotation);
	saveColor.push_back(color);

	saveIndex++;
}

void InstanceMesh::MakeInstance(D3DXVECTOR3 & picked, UINT textureNum)
{
	UINT index = PushTexture(textureNum); //0, textures[0]
	Transform* transform = GetTransform(index);

	position = picked;
	position.y = 15.0f;

	transform->Position(position);
	transform->Scale(scale);
	transform->Rotation(rotation);
}

void InstanceMesh::Color(D3DXCOLOR & val)
{
	color = val;
}

void InstanceMesh::Color(float r, float g, float b)
{
	Color(D3DXCOLOR(r, g, b, 1));
}

void InstanceMesh::Color(D3DXCOLOR * val)
{
	*val = color;
}

void InstanceMesh::Colors(D3DXCOLOR & val, int selectNum)
{
	colors[selectNum] = val;
}

void InstanceMesh::Colors(D3DXCOLOR * val, int selectNum)
{
	*val = colors[selectNum];
}

UINT InstanceMesh::Push()
{
	drawCount++;
	return drawCount - 1;
}

UINT InstanceMesh::PushTexture(UINT textureNum)
{
	//if (saveDeleteIndexes.size() >0)
	//{
	//	int temp = saveDeleteIndexes.back();
	//	saveDeleteIndexes.pop_back();
	//	textures[temp] = textureNum;
	//	return temp;
	//}
	//else
	//{
		drawCount++;
		textures[drawCount - 1] = textureNum; //textures[0]
		return drawCount - 1;  //0
	//}

}

Transform * InstanceMesh::GetTransform(UINT index)
{
	return transforms[index];
}

UINT InstanceMesh::GetTexture(UINT index)
{
	return textures[index];
}

UINT InstanceMesh::GetTextureNum(UINT selectNum)
{
	int number = textures[selectNum];
	return number;
}

void InstanceMesh::ChangeTexture(UINT selectNum, UINT textureNum)
{
	textures[selectNum] = textureNum;
}

void InstanceMesh::SaveDeleteIndex(UINT val)
{
	saveDeleteIndexes.push_back(val);
}

void InstanceMesh::DeleteIndex(UINT index)
{
	for (UINT i = index; i < drawCount; i++)
	{
		textures[i] = textures[i + 1];
		transforms[i] = transforms[i + 1];
	}
	drawCount--;
}

void InstanceMesh::DeleteCollboxIndex(UINT index)
{
	for (UINT i = index; i < drawCount; i++)
		transforms[i] = transforms[i + 1];
	drawCount--;
}

bool InstanceMesh::Selected(D3DXVECTOR3 start, D3DXVECTOR3 direction, UINT index)
{
	return false;
}


void InstanceMesh::SetOldMesh()
{
	D3DXVECTOR3 tempPos;
	D3DXVECTOR3 tempRot;
	D3DXVECTOR3 tempScale;

	if (drawCount == 0)
	{
		oldMesh.transform[0].Position(tempPos);
		oldMesh.transform[0].Rotation(tempRot);
		oldMesh.transform[0].Scale(tempScale);	

		copy(textures, textures + MAX_INSTANCE, oldMesh.textures);

		vUndoStack.push_back(oldMesh);
		return;
	}

	Vector3 trash;

	oldMesh.transform[0].Position(trash);
	oldMesh.transform[0].Rotation(trash);
	oldMesh.transform[0].Scale(trash);

	copy(textures , textures + MAX_INSTANCE, oldMesh.textures + 1);

	for (UINT i = 0; i < drawCount; i++)
	{
		{
			Transform* trans = GetTransform(i);
			trans->Position(&tempPos);
			trans->Rotation(&tempRot);
			trans->Scale(&tempScale);

			oldMesh.transform[i].Position(tempPos);
			oldMesh.transform[i].Rotation(tempRot);
			oldMesh.transform[i].Scale(tempScale);

			copy(textures, textures + MAX_INSTANCE, oldMesh.textures + 1);
		}
	}


	vUndoStack.push_back(oldMesh);

	if (vRedoStack.size() > 0)
		vRedoStack.clear();
}

void InstanceMesh::MakeOldMesh()
{
	D3DXVECTOR3 tempPos;
	D3DXVECTOR3 tempRot;
	D3DXVECTOR3 tempScale;
	//현재의 상태 저장해 준다. undo에 저장될 것이다.
	//for (UINT i = 0; i < drawCount; i++)
	//{
	//	Transform* trans = GetTransform(i);
	//	trans->Position(&tempPos);
	//	trans->Rotation(&tempRot);
	//	trans->Scale(&tempScale);

	//	oldMesh.transform[i].Position(tempPos);
	//	oldMesh.transform[i].Rotation(tempRot);
	//	oldMesh.transform[i].Scale(tempScale);
	//	copy(textures, textures + MAX_INSTANCE, oldMesh.textures);
	//}
	oldTrans.clear();

	for (Transform* trans : transes)
	{
		Vector3 pos;
		Vector3 scale;
		Vector3 rot;
		trans->Position(&pos);
		trans->Scale(&scale);
		trans->Rotation(&rot);

		Transform* temp;
		temp->Position(pos);
		temp->Scale(scale);
		temp->Rotation(rot);

		oldTrans.push_back(temp);
	}
}

void InstanceMesh::MakeNewMesh()
{
	//D3DXVECTOR3 tempPos;
	//D3DXVECTOR3 tempRot;
	//D3DXVECTOR3 tempScale;

	////현재의 상태를 저장한다. redo에 저장될것이다.
	//for (UINT i = 0; i < drawCount; i++)
	//{
	//	Transform* trans = GetTransform(i);

	//	trans->Position(&tempPos);
	//	trans->Rotation(&tempRot);
	//	trans->Scale(&tempScale);

	//	newMesh.transform[i].Position(tempPos);
	//	newMesh.transform[i].Rotation(tempRot);
	//	newMesh.transform[i].Scale(tempScale);

	//	copy(textures, textures + MAX_INSTANCE, newMesh.textures);
	//}

	newTrans.clear();

	for (Transform* trans : transes)
	{
		Vector3 pos;
		Vector3 scale;
		Vector3 rot;
		trans->Position(&pos);
		trans->Scale(&scale);
		trans->Rotation(&rot);

		Transform* temp;
		temp->Position(pos);
		temp->Scale(scale);
		temp->Rotation(rot);

		newTrans.push_back(temp);
	}
}

void InstanceMesh::Undo()
{
	if (vUndoStack.empty())
		return;

	MakeNewMesh();
	//vRedoStack.push_back(newMesh);
	redoStack.push_back(newTrans);

	it = vUndoStack.end() - 1;

	D3DXVECTOR3 tempPos;
	D3DXVECTOR3 tempRot;
	D3DXVECTOR3 tempScale;

	for (UINT i = 0; i < vUndoStack.size(); i++)
	{
		Transform* trans = GetTransform(i);

		it->transform[i].Position(&tempPos);
		it->transform[i].Scale(&tempScale);
		it->transform[i].Rotation(&tempRot);
		int a = i * 30;
		trans->Position(tempPos);
		trans->Scale(tempScale);
		trans->Rotation(tempRot);
		
		copy(it->textures, it->textures + MAX_INSTANCE, textures);
	}
	
	vUndoStack.erase(it);
}

void InstanceMesh::Redo()
{
	if (vRedoStack.empty())
		return;

	MakeOldMesh();
	vUndoStack.push_back(oldMesh);

	it = vRedoStack.end() - 1;

	D3DXVECTOR3 tempPos;
	D3DXVECTOR3 tempRot;
	D3DXVECTOR3 tempScale;

	for (UINT i = 0; i < drawCount; i++)
	{
		Transform* trans = GetTransform(i);

		it->transform[i].Position(&tempPos);
		it->transform[i].Rotation(&tempRot);
		it->transform[i].Scale(&tempScale);

		trans->Position(tempPos);
		trans->Rotation(tempRot);
		trans->Scale(tempScale);

		copy(it->textures, it->textures + MAX_INSTANCE, textures);
	}
	vRedoStack.erase(it);
}