#include "stdafx.h"
#include "EnvDemo.h"
#include "Environment/Sky.h"
#include "Environment/Terrain.h"
#include "Environment/BillBoard.h"
#include "Environment/Rain.h"
#include "Environment/Snow.h"
void EnvDemo::Initialize()
{
	bSnow = false;
	Context::Get()->GetCamera()->RotationDegree(12, 0);
	Context::Get()->GetCamera()->Position(102, 23, -2);
	//Sky
	{
		sky = new Sky();
	}

	//Weather
	{
		//rain = new Rain(D3DXVECTOR3(300, 100, 400), 1000);
		//snow = new Snow(D3DXVECTOR3(300, 100, 400), 1000);
	}

	//Terrain
	{
		base = L"Terrain/Dirt.png";
		for (int i = 0; i < 5; i++)
			textureNames.push_back(base);
		terrainShader = new Shader(L"018_TerrainSplatting.fx");
		terrain = new Terrain(terrainShader, L"HeightMaps/Gray256.png", base, textureNames);
		terrain->BaseMap(base);
		terrain->LayerMap(base);
	}

	//Billboard
	{
		billShader = new Shader(L"024_BillBoard_Geometry.fx");
		vector<wstring> textureNames;
		textureNames.push_back(L"Terrain/Tree.png");
		textureNames.push_back(L"Terrain/Tree2.png");
		textureNames.push_back(L"Terrain/Tree3.png");
		textureNames.push_back(L"Terrain/Tree4.png");
		textureNames.push_back(L"Terrain/Tree5.png");
		billBoard = new BillBoard(billShader, textureNames);
		for (UINT i = 0; i < 4096; i++)
		{
			D3DXVECTOR2 scale;
			scale.x = Math::Random(5.0f, 15.0f);
			scale.y = Math::Random(5.0f, 15.0f);

			D3DXVECTOR3 position;
			position.x = Math::Random(0.0f, 255.0f);
			position.z = Math::Random(0.0f, 255.0f);
			position.y = terrain->GetPickedHeight(position) + (scale.y + 0.5f);

			billBoard->Add(position, scale);
		}
		billBoard->CreateBuffer();
	}
}

void EnvDemo::Ready()
{

}

void EnvDemo::Destroy()
{

}

void EnvDemo::Update()
{
	D3DXVECTOR3 picked = terrain->GetPickedHeight();
	ImGui::LabelText("Picking", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);

	sky->Update();
	terrain->Update();
	billBoard->Update();
	//rain->Update();
	//ImGui::Checkbox("Snow", &bSnow);
	//(bSnow == false) ? rain->Update() : snow->Update();
}

void EnvDemo::PreRender()
{

}

void EnvDemo::Render()
{
	sky->Render();
	terrain->Render();
	//rain->Render();
	//billBoard->Render();
	//(bSnow == false) ? rain->Render() : snow->Render();
}

void EnvDemo::PostRender()
{

}

void EnvDemo::ResizeScreen()
{

}