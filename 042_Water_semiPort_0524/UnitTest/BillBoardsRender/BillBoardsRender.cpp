#include "stdafx.h"
#include "BillBoardsRender.h"
#include "Environment/BillBoard.h"
#include "Environment/Terrain.h"
#include "Viewer/Projector.h"

BillBoardsRender::BillBoardsRender(Projector* projector, Terrain* terrain)
	:projector(projector)
{
	this->terrain = terrain;
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
	shader = new Shader(L"024_BillBoard_Geometry.fx");
	billBoard = new BillBoard(shader, grassNames, terrain);
	
	imageX = imageY = 50;
	brushType = 0;
	brushRange = 1;
	grassNum = 3;

	brushColor = D3DXCOLOR(1, 1, 1, 1);
}

BillBoardsRender::~BillBoardsRender()
{
	SafeDelete(shader);
	SafeDelete(billBoard);
}

void BillBoardsRender::Update()
{
	billBoard->Update();
}

void BillBoardsRender::Render()
{
	billBoard->Render();
}

void BillBoardsRender::SettingImgui(Terrain* terrain, D3DXVECTOR3& picked)
{
	if (ImGui::BeginTabItem("GrassEditor"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Grass");
		if (ImGui::Button("AddGrass"))  //베이스 변경
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&BillBoardsRender::AddGrass, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Grass/", func, desc.Handle);
			
		}
		billBoard->Texture0(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 0; ImGui::SameLine();
		billBoard->Texture1(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 1; ImGui::SameLine();
		billBoard->Texture2(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 2; ImGui::SameLine();
		billBoard->Texture3(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 3; ImGui::SameLine();
		billBoard->Texture4(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)) == true)
			selectGrass = 4;
		SettingBillboard(terrain, picked);
		ImGui::EndTabItem();
	}
}

void BillBoardsRender::SettingBillboard(Terrain* terrain, D3DXVECTOR3& picked)
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

	projector->Update(picked, (int&)brushType,(UINT&)brushRange, brushColor);

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
			billBoard->SetOldBillBoard(); //현재 빌보드 저장
			COMMANDMANAGER->SetUndoCommand(STACK_BILLBOARD, this);

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

void BillBoardsRender::MakeRandomBillboard(D3D11_BOX & box, Terrain* terrain, D3DXVECTOR3& picked)
{
	if (brushType == 0)
	{
		{
			D3DXVECTOR2 scale;
			scale.x = 5.0f; //Math::Random(5.0f, 15.0f);
			scale.y = 4.0f; //Math::Random(5.0f, 15.0f);

			D3DXVECTOR3 position;
			position.x = picked.x;
			position.z = picked.z;
			position.y = picked.y + (scale.y* 0.5f);

			billBoard->Add(position, scale, grassType);

			billBoard->CreateBuffer();
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

			billBoard->Add(position, scale, grassType);
		}

		billBoard->CreateBuffer();
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

				billBoard->Add(position, scale, grassType);
			}
		}
		billBoard->CreateBuffer();
	}
}

void BillBoardsRender::AddGrass(wstring file)
{

}

void BillBoardsRender::Save(wstring name)
{
	billBoard->Save(name);
}

void BillBoardsRender::Load(wstring name)
{
	billBoard->Load(name);
}

void BillBoardsRender::Undo()  // c + z
{
	billBoard->UndoBillBoard();
}

void BillBoardsRender::Redo() // c + y
{
	billBoard->RedoBillBoard();
}

void BillBoardsRender::Excute()
{
}
