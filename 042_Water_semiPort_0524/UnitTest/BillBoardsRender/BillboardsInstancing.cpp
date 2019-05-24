#include "stdafx.h"
#include "BillboardsInstancing.h"
#include "Environment/BillboardInstance.h"
#include "Environment/Terrain.h"
#include "Viewer/Projector.h"
#include "Utilities/BinaryFile.h"

BillboardsInstancing::BillboardsInstancing(Projector * projector, Shader* shader, Terrain * terrain)
	:projector(projector), terrain(terrain), shader(shader)
{
	selectGrass = -1;
	wstring path = L"../../_Textures/Grass/";
	wstring filter = L"*.png";
	Path::GetFiles(&getFolderImage, path, filter, false);
	for (UINT i = 0; i < getFolderImage.size(); i++)
	{
		wstring temp = Path::GetFileName(getFolderImage[i]);
		temp = L"Grass/" + temp;
		grassNames.push_back(temp);
	}
	//shader = new Shader(L"024_BillBoard_Geometry_Instance.fx");
	billboard = new BillboardInstance(shader, grassNames, terrain);

	imageX = imageY = 50;
	brushType = 0;
	brushRange = 1;
	grassNum = 3;

	brushColor = D3DXCOLOR(1, 1, 1, 1);
}

BillboardsInstancing::~BillboardsInstancing()
{
}

void BillboardsInstancing::Update()
{
	billboard->Update();
}

void BillboardsInstancing::Render()
{
	billboard->Render();
}

void BillboardsInstancing::ShadowRender()
{
	billboard->ShadowRender();
}

void BillboardsInstancing::SettingImgui(Terrain * terrain, Vector3 & picked)
{
	if (ImGui::BeginTabItem("GrassEditor"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Grass");
		//if (ImGui::Button("AddGrass"))  //베이스 변경
		//{
		//	D3DDesc desc = D3D::GetDesc();
		//	function<void(wstring)> func = bind(&BillBoardsRender::AddGrass, this, placeholders::_1);
		//	Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Grass/", func, desc.Handle);

		//}
		billboard->GetTexture(&texture, 0);
		if (ImGui::ImageButton(texture->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 0; ImGui::SameLine();
		billboard->GetTexture(&texture, 1);
		if (ImGui::ImageButton(texture->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 1; ImGui::SameLine();
		billboard->GetTexture(&texture, 2);
		if (ImGui::ImageButton(texture->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 2; ImGui::SameLine();
		billboard->GetTexture(&texture, 3);
		if (ImGui::ImageButton(texture->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 3; ImGui::SameLine();
		billboard->GetTexture(&texture, 4);
		if (ImGui::ImageButton(texture->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 4;
		SettingBillboard(terrain, picked);
		ImGui::EndTabItem();
	}
}

void BillboardsInstancing::SettingBillboard(Terrain * terrain, Vector3 & picked)
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Type0: None  Type1: Square  Type2: Circle");

	ImGui::SliderInt("Type", (int *)&brushType, 0, 2);

	ImGui::ColorEdit3("BrushColor", (float *)&brushColor);

	ImGui::SliderInt("Range", (int *)&brushRange, 1, 20);

	ImGui::SliderInt("GrassNum", (int*)&grassNum, 1, 10);

	terrain->BrushLocation(picked);
	terrain->BrushColor(brushColor);
	terrain->BrushRange(brushRange);
	terrain->BrushType(brushType);

	projector->Update(picked, (int&)brushType, (UINT&)brushRange, brushColor);

	D3D11_BOX box;
	box.left = (UINT)picked.x - brushRange;
	box.right = (UINT)picked.x + brushRange;
	box.top = (UINT)picked.z + brushRange;
	box.bottom = (UINT)picked.z - brushRange;

	if (box.left < 0) box.left = 0;
	if (box.right >= 255) box.right = 255;
	if (box.top >= 255) box.top = 255;
	if (box.bottom < 0) box.bottom = 0;

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Press(0) && picked.x >-1 && picked.z > -1 && picked.x <255 && picked.z < 255)
		{
			//billBoard->SetOldBillBoard(); //현재 빌보드 저장
			//COMMANDMANAGER->SetUndoCommand(STACK_BILLBOARD, this);

			switch (selectGrass)
			{
				case 0:
					grassType = 0;
					MakeRandomBillboard(box, terrain, picked);
					break;
				case 1:
					grassType = 1;
					MakeRandomBillboard(box, terrain, picked);
					break;
				case 2:
					grassType = 2;
					MakeRandomBillboard(box, terrain, picked);
					break;
				case 3:
					grassType = 3;
					MakeRandomBillboard(box, terrain, picked);
					break;
				case 4:
					grassType = 4;
					MakeRandomBillboard(box, terrain, picked);
					break;
				default:
					break;
			}
		}
	}

}

void BillboardsInstancing::MakeRandomBillboard(D3D11_BOX & box, Terrain * terrain, D3DXVECTOR3 & picked)
{
	if (brushType == 0)
	{
		{
			D3DXVECTOR2 scale;
			scale.x = 5.0f; //Math::Random(5.0f, 15.0f);
			scale.y = 5.0f; //Math::Random(5.0f, 15.0f);

			D3DXVECTOR3 position;
			position.x = picked.x;
			position.z = picked.z;
			position.y = picked.y + (scale.y* 0.5f);

			billboard->Add(position, (int&)grassType);
		}
	}
	if (brushType == 1)
	{
		for (UINT i = 0; i < grassNum; i++)
		{
			D3DXVECTOR2 scale;
			scale.x = 5.0f; //Math::Random(5.0f, 15.0f);
			scale.y = 5.0f; //Math::Random(5.0f, 15.0f);

			D3DXVECTOR3 position;
			position.x = Math::Random((float)box.left, (float)box.right);
			position.z = Math::Random((float)box.bottom, (float)box.top);
			position.y = terrain->GetPickedHeight(position) + (scale.y* 0.5f);

			billboard->Add(position, (int&)grassType);
		}
	}
	if (brushType == 2)
	{
		for (UINT i = 0; i < grassNum; i++)
		{

			D3DXVECTOR3 position;
			position.x = Math::Random((float)(picked.x - brushRange), (float)(picked.x + brushRange)); //나무의 x값 랜덤으로 뽑아보기
			position.z = Math::Random((float)(picked.z - brushRange), (float)(picked.z + brushRange)); ////나무의 z값 랜덤으로 뽑아보기
			float dX = position.x - picked.x;
			float dZ = position.z - picked.z;

			float dist = sqrt(dX * dX + dZ * dZ);  //랜덤으로 뽑힌 나무의 위치와 마우스 위치사이 거리
			if (brushRange >= dist)   //그 거리가 반지름보다 작거나 같으면 아래 처리
			{

				D3DXVECTOR2 scale;
				scale.x = 5.0f; //Math::Random(5.0f, 15.0f);
				scale.y = 5.0f; //Math::Random(5.0f, 15.0f);

				position.y = terrain->GetPickedHeight(position) + (scale.y* 0.5f);

				billboard->Add(position, (int&)grassType);
			}
		}
	}
}

void BillboardsInstancing::Save(wstring name)
{
	wstring savePath = L"../BinaryFolder/" + name;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	Transform* saveTransform;
	Vector3 savePos;
	Vector3 saveScale;
	//Vector3 saveRotation;
	int saveType = -1;
	int saveDrawcount = billboard->DrawCount();
	w->Int(saveDrawcount); // 첫번째 세이브

	for (int i = 0; i < saveDrawcount; i++)
	{
		saveTransform = billboard->GetTransform(i);
		saveTransform->Position(&savePos);
		saveTransform->Scale(&saveScale);
		saveType = billboard->Type(i);
		//
		w->Vector3(savePos); // 두번째 세이브
		w->Int(saveType); // 세번째 세이브
	}

	w->Close();
}

void BillboardsInstancing::Load(wstring name)
{
	wstring loadFile = L"../BinaryFolder/" + name;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);
	int drawCount = r->Int();

	Vector3 pos;
	int types;
	for (int i = 0; i < drawCount; i++)
	{
		pos = r->Vector3();
		types = r->Int();

		billboard->Add(pos, types);
	}
	r->Close();
}



void BillboardsInstancing::Undo()
{
}

void BillboardsInstancing::Redo()
{
}

void BillboardsInstancing::Excute()
{
}