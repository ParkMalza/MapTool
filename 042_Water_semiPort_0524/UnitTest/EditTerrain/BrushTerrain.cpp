#include "stdafx.h"
#include "BrushTerrain.h"
#include "Environment/Terrain.h"
#include "Viewer/Projector.h"
#include "EnumGroup.h"

BrushTerrain::BrushTerrain(class Projector* projector, Terrain* terrain)
	:projector(projector), terrain(terrain)
{
	type = 0;
	range = 0;
	lockHeight = 0;
	speed = 0;

	selectHeight = -1;
	color = D3DXCOLOR(1, 1, 1, 1);

	comboName = "UpHeight";
	names.push_back(comboName);
	comboName = "DownHeight";
	names.push_back(comboName);
	comboName = "FixHeight";
	names.push_back(comboName);
	comboName = "Smoothing";
	names.push_back(comboName);

}

BrushTerrain::~BrushTerrain()
{
}

void BrushTerrain::SetImgui(D3DXVECTOR3& picked)
{
	static const char* current_edit = NULL;
	//Height
	if (ImGui::BeginTabItem("MapHeightEditor"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "EditType");
		if (ImGui::BeginCombo("##combo", current_edit))
		{
			for (UINT n = 0; n < names.size(); n++)
			{
				bool is_selected = (current_edit == names[n].c_str());
				if (ImGui::Selectable(names[n].c_str(), is_selected))
				{
					current_edit = names[n].c_str();
					selectHeight = n;
				}
			}
			ImGui::EndCombo();
		}
		SettingMapHeight(picked);  //높낮이 조절 함수
		ImGui::EndTabItem();
	}
}


void BrushTerrain::SettingMapHeight(D3DXVECTOR3& picked)
{
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Type0: None  Type1: Square  Type2: Circle");
	ImGui::SliderInt("", (int*)&type, 0, 2);

	ImGui::ColorEdit3("BrushColor", (float*)&color);

	ImGui::SliderInt("Range", (int*)&range, 1, 20);

	ImGui::SliderFloat("LockHeight", &lockHeight, -50, 50);

	ImGui::SliderFloat("DrawSpeed", &speed, 0, 50);

	terrain->BrushLocation(picked);
	terrain->BrushColor(color);
	terrain->BrushRange(range);
	terrain->BrushType(type);

	projector->Update(picked, type, (UINT&)range, color);

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Up(0))
		{
			terrain->CreateNormalData();
			terrain->Mapping();
		}
		if (Mouse::Get()->Down(0))
		{
			if (type == 1 || type == 2)
			{
				terrain->SetOldTerrain();
				COMMANDMANAGER->SetUndoCommand(STACK_BRUSH,this);
			}
		}
		if (Mouse::Get()->Press(0) && picked.x >-1 && picked.z > -1 && picked.x <255 && picked.z < 255)
		{
			if (type == 1 || type == 2)
			{
				switch (selectHeight)
				{
					case RAISE:
						terrain->RaiseHeight(picked, type, range, speed);
						break;
					case DOWN:
						terrain->FallDownHeight(picked, type, range, speed);
						break;
					case FIX:
						terrain->LockHeight(picked, type, range, lockHeight, speed);
						break;
					case SMOOTHING:
						terrain->Smoothing(picked, type, range, speed);
						break;
				}
			}
		}
	}
}


void BrushTerrain::Undo()
{
	terrain->UndoTerrain();
}

void BrushTerrain::Redo()
{
	terrain->RedoTerrain();
}

void BrushTerrain::Excute()
{
}
