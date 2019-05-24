#include "stdafx.h"
#include "ModelManager.h"
#include "Environment/Terrain.h"
#include "Assimp/Loader.h"
#include "Models/Tank.h"
#include "Models/Tower.h"
#include "Models/Car.h"
#include "Models/Kachujin.h"
#include "EnumGroup.h"

ModelManager::ModelManager(Shader* shader, Terrain* terrain)
	:shader(shader)
{
	this->terrain = terrain;
	selectModel = -1;  //어떤 모델 선택했는지 선택 여부
	//타워 익스포트
	ExportTower();
	towerRender = NULL;
	ReadTower(shader);
	//탱크 익스포트
	ExportTank();
	tankRender = NULL;
	ReadTank(shader);
	//자동차 익스포트
	ExportCar();
	carRender = NULL;
	ReadCar(shader);
	//카추진 익스포트
	ExportKachujin();
	kachujinRender = NULL;
	ReadKachujin(shader);


	//실제 모델
	instanceTank = new Tank(shader, tank, tankRender, terrain);
	instanceTower = new Tower(shader, tower, towerRender, terrain);
	instanceCar = new Car(shader, car, carRender, terrain);
	instanceKachujin = new Kachujin(shader, kachujin, kachujinRender, terrain);

	SettingComboNames();

	carCount = 0;
}

ModelManager::~ModelManager()
{
	SafeDelete(instanceCar);
	SafeDelete(instanceTower);
	SafeDelete(instanceTank);
	SafeDelete(carRender);
	SafeDelete(car);
	SafeDelete(tankRender);
	SafeDelete(tank);
	SafeDelete(towerRender);
	SafeDelete(tower);
}
void ModelManager::ExportTower()
{
	////Tower
	//Loader* loader = new Loader();
	//loader->ReadFile(L"Tower/Tower.fbx");
	//loader->ExportMaterial(L"Tower/Tower");
	//loader->ExportMesh(L"Tower/Tower");
}

void ModelManager::ExportTank()
{
	////Tank
	//Loader* loader = new Loader();
	//loader->ReadFile(L"Tank/Tank.fbx");
	//loader->ExportMaterial(L"Tank/Tank", false);
	//loader->ExportMesh(L"Tank/Tank");
}

void ModelManager::ExportCar()
{
	////Car
	//Loader* loader = new Loader();
	//loader->ReadFile(L"Car/Car.obj");
	//loader->ExportMaterial(L"Car/Car", false);
	//loader->ExportMesh(L"Car/Car");
	//SafeDelete(loader);
}

void ModelManager::ExportKachujin()
{
	//Kachujin
	Loader* loader = new Loader();
	loader->ReadFile(L"Kachujin/Mesh.fbx");
	loader->ExportMaterial(L"Kachujin/Mesh", false);
	loader->ExportMesh(L"Kachujin/Mesh");
	SafeDelete(loader);

	loader = new Loader();
	loader->ReadFile(L"Kachujin/Idle.fbx"); //without skin 했으므로 애니 정보만 있다~
	loader->ExportAnimClip(0, L"Kachujin/Idle");
	SafeDelete(loader);

	loader = new Loader();
	loader->ReadFile(L"Kachujin/Walking.fbx"); //without skin 했으므로 애니 정보만 있다~
	loader->ExportAnimClip(0, L"Kachujin/Walking");
	SafeDelete(loader);

	loader = new Loader();
	loader->ReadFile(L"Kachujin/Running.fbx"); //without skin 했으므로 애니 정보만 있다~
	loader->ExportAnimClip(0, L"Kachujin/Running");
	SafeDelete(loader);

	loader = new Loader();
	loader->ReadFile(L"Kachujin/Taunt.fbx"); //without skin 했으므로 애니 정보만 있다~
	loader->ExportAnimClip(0, L"Kachujin/Taunt");
	SafeDelete(loader);

	loader = new Loader();
	loader->ReadFile(L"Kachujin/Threatening.fbx"); //without skin 했으므로 애니 정보만 있다~
	loader->ExportAnimClip(0, L"Kachujin/Threatening");
	SafeDelete(loader);
}

void ModelManager::ReadTower(Shader* shader)
{
	tower = new Model();
	tower->ReadMaterial(L"Tower/Tower");
	tower->ReadMesh(L"Tower/Tower");

	towerRender = new ModelRender(shader, tower);
}

void ModelManager::ReadTank(Shader* shader)
{
	tank = new Model();
	tank->ReadMaterial(L"Tank/Tank");
	tank->ReadMesh(L"Tank/Tank");

	tankRender = new ModelRender(shader, tank);
}

void ModelManager::ReadCar(Shader * shader)
{
	car = new Model();
	car->ReadMaterial(L"Eclipse/Eclipse");
	car->ReadMesh(L"Eclipse/Eclipse");

	carRender = new ModelRender(shader, car);
}

void ModelManager::ReadKachujin(Shader * shader)
{
	kachujin = new Model();
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadMesh(L"Kachujin/Mesh");
	//애니 순서
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walking");
	kachujin->ReadClip(L"Kachujin/Running");
	kachujin->ReadClip(L"Kachujin/Taunt");
	kachujin->ReadClip(L"Kachujin/Threatening");

	kachujinRender = new ModelAnimator(shader, kachujin);
	kachujinRender->Pass(4);
}

void ModelManager::Update(Terrain* terrain)
{
	instanceTank->Update();
	instanceTower->Update(terrain);
	instanceCar->Update(terrain);
	instanceKachujin->Update(terrain);
}

void ModelManager::CubeRender()
{
	if (instanceCar->GetDrawCount() > 0)
	{
		carCount = instanceCar->GetDrawCount();
	}
	instanceTank->CubeRender();
	instanceTower->CubRender();
	instanceKachujin->CubeRender();
}

void ModelManager::PreRender()
{
	instanceTank->PreRender();
	instanceTower->PreRender();
	instanceCar->PreRender();
	instanceKachujin->PreRender();
}

void ModelManager::Render()
{
	instanceTank->Render();
	instanceTower->Render();
	instanceCar->Render();
	instanceKachujin->Render();
}

void ModelManager::SetImgui(D3DXVECTOR3 & picked)
{
	static const char* current_model = NULL;
	if (ImGui::BeginTabItem("ModelEditor"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Model");
		if (ImGui::BeginCombo("##combo", current_model))
		{
			for (UINT n = 0; n < names.size(); n++)
			{
				bool is_selected = (current_model == names[n].c_str());
				if (ImGui::Selectable(names[n].c_str(), is_selected))
				{
					current_model = names[n].c_str();
					selectModel = n;
				}
			}
			ImGui::EndCombo();
		}
		SettingModel(picked);
		ImGui::EndTabItem();
	}
}

void ModelManager::tankSave(wstring fileName)
{
	instanceTank->Save(fileName);
}

void ModelManager::towerSave(wstring fileName)
{
	instanceTower->Save(fileName);
}

void ModelManager::carSave(wstring fileName)
{
	instanceCar->Save(fileName);
}

void ModelManager::tankLoad(wstring fileName)
{
	instanceTank->Load(fileName);
}

void ModelManager::towerLoad(wstring fileName)
{
	instanceTower->Load(fileName);
}

void ModelManager::carLoad(wstring fileName)
{
	instanceCar->Load(fileName);
}

void ModelManager::SettingModel(D3DXVECTOR3 & picked)
{
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0) && picked.x >-1 && picked.z > -1 && picked.x <255 && picked.z < 255)
		{
			switch (selectModel)
			{
				case TANK:
					instanceTank->InstanceTank(picked);
					//selectModel = -1;
					break;
				case TOWER:
					instanceTower->InstanceTower(picked);
					//selectModel = -1;
					break;
				case CAR:
					instanceCar->InstanceCar(picked);
					//selectModel = -1;
					break;
				case KACHUJIN:
					instanceKachujin->InstanceKachujin(picked);
					//selectModel = -1;
					break;
				default:
					break;
			}
		}
	}
}

void ModelManager::SettingComboNames()
{
	comboName = "Tank";
	names.push_back(comboName);
	comboName = "Tower";
	names.push_back(comboName);
	comboName = "Car";
	names.push_back(comboName);
	comboName = "Kachujin";
	names.push_back(comboName);
}
