#include "stdafx.h"
#include "Export.h"
#include "Assimp/Loader.h"

void Export::Initialize()
{
	//ExportTower();
	//ExportTank();
	//ExportCar();
	//ExportEclipse();

	ExportKachujin();
	//ExportUnityChan();
}

void Export::ExportTower()
{
	Loader* loader = new Loader();
	loader->ReadFile(L"Tower/Tower.fbx");
	loader->ExportMaterial(L"Tower/Tower");
	loader->ExportMesh(L"Tower/Tower");
	SafeDelete(loader);
}

void Export::ExportTank()
{
	Loader* loader = new Loader();
	loader->ReadFile(L"Tank/Tank.fbx");
	loader->ExportMaterial(L"Tank/Tank", false);
	loader->ExportMesh(L"Tank/Tank");
	SafeDelete(loader);
}

void Export::ExportCar()
{
	Loader* loader = new Loader();
	loader->ReadFile(L"Car/Car.obj");
	loader->ExportMaterial(L"Car/Car", false);
	loader->ExportMesh(L"Car/Car");
	SafeDelete(loader);
}

void Export::ExportEclipse()
{
	Loader* loader = new Loader();
	loader->ReadFile(L"Eclipse/Eclipse.fbx");
	loader->ExportMaterial(L"Eclipse/Eclipse", false);
	loader->ExportMesh(L"Eclipse/Eclipse");
	SafeDelete(loader);
}

void Export::ExportKachujin()
{
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

//void Export::ExportUnityChan()
//{
//	//ASSIMP가 부를 수 있는 최소 버전은 2014이후다.
//	Loader* loader = new Loader();
//	loader->ReadFile(L"UnityChan/unitychan.fbx");
//	loader->ExportMaterial(L"UnityChan/Mesh", false);
//	loader->ExportMesh(L"UnityChan/Mesh");
//	SafeDelete(loader);
//
//	loader = new Loader();
//	loader->ReadFile(L"UnityChan/unitychan_ARpose1.fbx");
//
//	vector<wstring> test;
//	loader->GetClipList(&test);
//
//	int a = 0;
//}
