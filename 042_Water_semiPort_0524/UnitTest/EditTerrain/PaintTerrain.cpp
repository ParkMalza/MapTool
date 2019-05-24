#include "stdafx.h"
#include "PaintTerrain.h"
#include "Environment/Terrain.h"
#include "Viewer/Projector.h"
#include "EnumGroup.h"

PaintTerrain::PaintTerrain(Projector* projector, Terrain* terrain)
	:projector(projector), terrain(terrain)
{
	imageX = 55;
	imageY = 52;
	brushType = LAYER1;
	paint = L"Terrain/Dirt.png";
	baseTexture = paint;
	layerTexture = paint;
	layer1 = paint;
	layer2 = paint;
	layer3 = paint;
	layer4 = paint;
	power = 0.3f;

	type = 0;
	color = D3DXCOLOR(1, 1, 1, 1);
	range = 0;
	alpha = 1;

	isGridView = false;
	gridLineThick = 0;
	gridLineSize = 255;
}

PaintTerrain::~PaintTerrain()
{
}

void PaintTerrain::SetImgui(D3DXVECTOR3& picked)
{
	if (ImGui::BeginTabItem("PaintEditor"))
	{
		if (ImGui::Button("ChangeBase"))  //베이스 변경
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&PaintTerrain::OpenBaseTexture, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Terrain/", func, desc.Handle);
			terrain->BaseMap(baseTexture);
		}
		string baseName = String::ToString(Path::GetFileName(terrain->GetBaseTextureName()));
		ImGui::SameLine(ImGui::CalcItemWidth() - baseName.size());
		ImGui::TextColored(ImVec4(1.0, 1.0, 0, 1.0f), baseName.c_str());
		if (ImGui::Button("ChangeLayer"))  //전체 레이어 변경
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&PaintTerrain::OpenLayerTexture, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Terrain/", func, desc.Handle);
			terrain->LayerMap(layerTexture);
		}
		string layerName = String::ToString(Path::GetFileName(terrain->GetLayerTextureName()));
		ImGui::SameLine(ImGui::CalcItemWidth() - layerName.size());
		ImGui::TextColored(ImVec4(1.0, 1.0, 0, 1.0f), layerName.c_str());

		if (ImGui::Button(" Layer1 "))
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&PaintTerrain::OpenLayer1, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Terrain/", func, desc.Handle);
			brushType = LAYER1;
			terrain->LayerMaps(layer1, brushType);  // 레이어스 다시 설정
		} ImGui::SameLine();
		if (ImGui::Button(" Layer2 "))
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&PaintTerrain::OpenLayer2, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Terrain/", func, desc.Handle);
			brushType = LAYER2;
			terrain->LayerMaps(layer2, brushType); // 레이어스 다시 설정
		} ImGui::SameLine();
		if (ImGui::Button(" Layer3 "))
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&PaintTerrain::OpenLayer3, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Terrain/", func, desc.Handle);
			brushType = LAYER3;
			terrain->LayerMaps(layer3, brushType); // 레이어스 다시 설정
		} ImGui::SameLine();
		if (ImGui::Button(" Layer4 "))
		{
			D3DDesc desc = D3D::GetDesc();
			function<void(wstring)> func = bind(&PaintTerrain::OpenLayer4, this, placeholders::_1);
			Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures/Terrain/", func, desc.Handle);
			brushType = LAYER4;
			terrain->LayerMaps(layer4, brushType); // 레이어스 다시 설정
		}ImGui::SameLine();
		if (ImGui::Button(" Origin "))
		{
			brushType = ORIGIN;
			terrain->LayerMaps(baseTexture, brushType);  //알파값 다시 내꺼로 바꿈
		}
		terrain->LayerMap1(&tex);  //임구이에 띄울 사진 받아오기
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)))
			brushType = LAYER1; ImGui::SameLine();
		terrain->LayerMap2(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)))
			brushType = LAYER2; ImGui::SameLine();
		terrain->LayerMap3(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)))
			brushType = LAYER3;  ImGui::SameLine();
		terrain->LayerMap4(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)))
			brushType = LAYER4;  ImGui::SameLine();
		terrain->GetBaeMap(&tex);
		if (ImGui::ImageButton(tex->SRV(), ImVec2(imageX, imageY)))
		{
			brushType = ORIGIN;
			terrain->LayerMaps(baseTexture, brushType);
		}
		Paint(picked);  // 페인트 동작

		ImGui::EndTabItem();
	}
}

void PaintTerrain::OpenBaseTexture(wstring file)
{
	baseTexture = Path::GetFileName(file);
	baseTexture = L"Terrain/" + baseTexture;
}

void PaintTerrain::OpenLayerTexture(wstring file)
{
	layerTexture = Path::GetFileName(file);
	layerTexture = L"Terrain/" + layerTexture;
}

void PaintTerrain::OpenLayer1(wstring file)
{
	layer1 = Path::GetFileName(file);
	layer1 = L"Terrain/" + layer1;
}

void PaintTerrain::OpenLayer2(wstring file)
{
	layer2 = Path::GetFileName(file);
	layer2 = L"Terrain/" + layer2;
}

void PaintTerrain::OpenLayer3(wstring file)
{
	layer3 = Path::GetFileName(file);
	layer3 = L"Terrain/" + layer3;
}

void PaintTerrain::OpenLayer4(wstring file)
{
	layer4 = Path::GetFileName(file);
	layer4 = L"Terrain/" + layer4;
}

void PaintTerrain::Paint(D3DXVECTOR3& picked)
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Type0: None  Type1: Square  Type2: Circle");
	ImGui::SliderInt("", (int*)&type, 0, 2);

	ImGui::ColorEdit3("BrushColor", (float*)&color);

	ImGui::SliderInt("Range", (int*)&range, 1, 20);

	ImGui::SliderFloat("Alpha", (float*)&alpha, 0, 1);

	ImGui::Checkbox("GridLineView", &isGridView);
	ImGui::PushButtonRepeat(true);
	ImGui::InputFloat("GridLineThick", &gridLineThick, 0.1f);
	gridLineThick = (gridLineThick < 0) ? 0 : gridLineThick;
	ImGui::InputFloat("GridLineSize", &gridLineSize, 1.0f);
	gridLineSize = (gridLineSize < 0) ? 0 : gridLineSize;
	ImGui::PopButtonRepeat();

	terrain->BrushLocation(picked);
	terrain->BrushColor(color);
	terrain->BrushRange(range);
	terrain->BrushType(type);
	terrain->GridLineVisible(isGridView);
	terrain->GridLineThick(gridLineThick);
	terrain->GridLineSize(gridLineSize);

	projector->Update(picked, type, (UINT&)range, color);

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			terrain->SetOldTerrain();
			COMMANDMANAGER->SetUndoCommand(STACK_PAINT,this);
		}
		if (Mouse::Get()->Press(0))
		{
			terrain->BrushLayer(picked, type, range, 0, brushType);
			terrain->SettingAlpha(alpha);
		}
	}
}

void PaintTerrain::Undo()
{
	terrain->UndoTerrain();
}

void PaintTerrain::Redo()
{
	terrain->RedoTerrain();
}

void PaintTerrain::Excute()
{
}
