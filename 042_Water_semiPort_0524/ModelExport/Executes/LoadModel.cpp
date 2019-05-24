#include "stdafx.h"
#include "LoadModel.h"
#include "Assimp/Loader.h"

void LoadModel::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(14, 0);
	Context::Get()->GetCamera()->Position(0, 12, -34);

	shader = new Shader(L"039_Model_Instance.fx");
	towerRender = NULL;
	ReadTower();

	tankRender = NULL;
	ReadTank();

	carRender = NULL;
	ReadCar();

	kachujinRender = NULL;
	ReadKachujin();
}

void LoadModel::Destroy()
{
	SafeDelete(tower);
	SafeDelete(towerRender);

	SafeDelete(tank);
	SafeDelete(tankRender);
}

void LoadModel::Update()
{
	if (towerRender != NULL) towerRender->Update();
	if (tankRender != NULL) tankRender->Update();
	if (carRender != NULL) carRender->Update();
	if (kachujinRender != NULL)
	{
		static UINT instance = 0;
		static UINT clip = 0;
		static float blendTime = 1.0f;

		ImGui::InputInt("Instance", (int *)&instance);
		instance %= 20;

		ImGui::InputInt("Clip", (int *)&clip);
		clip %= 4;

		ImGui::InputFloat("Tween", &blendTime, 0.01f);

		if (ImGui::Button("Change"))
			kachujinRender->PlayNextClip(instance, clip, blendTime);
		kachujinRender->Update();
	}
}

void LoadModel::Render()
{
	if (towerRender != NULL) towerRender->Render();
	if (tankRender != NULL) tankRender->Render();
	if (carRender != NULL) carRender->Render();
	if (kachujinRender != NULL) kachujinRender->Render();
}

void LoadModel::ReadTower()
{
	tower = new Model();
	tower->ReadMaterial(L"Tower/Tower");
	tower->ReadMesh(L"Tower/Tower");

	towerRender = new ModelRender(shader, tower);
	for (int i = -100; i <= 100; i += 10)
	{
		UINT index = towerRender->AddTransform();
		Transform* transform = towerRender->GetTransform(index);

		D3DXVECTOR3 position((float)i, 0, 50);
		D3DXVECTOR3 scale = Math::RandomVec3(0.01f, 0.02f);
		D3DXVECTOR3 rotation = Math::RandomVec3(0.0f, 180.0f);

		transform->Position(position);
		transform->Scale(scale);
		transform->RotationDegree(0, Math::Random(0.0f, 359.0f), 0);
	}
	towerRender->UpdateTransform();

}

void LoadModel::ReadTank()
{
	tank = new Model();
	tank->ReadMaterial(L"Tank/Tank");
	tank->ReadMesh(L"Tank/Tank");

	tankRender = new ModelRender(shader, tank);
	for (int i = -100; i <= 100; i += 10)
	{
		UINT index = tankRender->AddTransform();
		Transform* transform = tankRender->GetTransform(index);

		D3DXVECTOR3 position((float)i * 5, 0, 30);

		D3DXVECTOR3 scale = Math::RandomVec3(2, 2);
		D3DXVECTOR3 rotation = Math::RandomVec3(0.0f, 180.0f);

		transform->Position(position);
		transform->Scale(scale);
		transform->RotationDegree(0, Math::Random(0.0f, 359.0f), 0);
	}
	tankRender->UpdateTransform();
}

void LoadModel::ReadCar()
{
	car = new Model();
	car->ReadMaterial(L"Eclipse/Eclipse");
	car->ReadMesh(L"Eclipse/Eclipse");

	carRender = new ModelRender(shader, car);
	for (int i = -100; i <= 100; i += 10)
	{
		UINT index = carRender->AddTransform();
		Transform* transform = carRender->GetTransform(index);

		D3DXVECTOR3 position((float)i, 0, 10);

		D3DXVECTOR3 scale = Math::RandomVec3(0.2f, 0.2f);
		D3DXVECTOR3 rotation = Math::RandomVec3(0.0f, 180.0f);

		transform->Position(position);
		transform->Scale(scale);
		transform->RotationDegree(0, Math::Random(0.0f, 359.0f), 0);
	}
	carRender->UpdateTransform();
}

void LoadModel::ReadKachujin()
{
	kachujin = new Model();
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadMesh(L"Kachujin/Mesh");

	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walking");
	kachujin->ReadClip(L"Kachujin/Running");
	kachujin->ReadClip(L"Kachujin/Taunt"); 
	kachujin->ReadClip(L"Kachujin/Threatening");

	kachujinRender = new ModelAnimator(shader, kachujin);
	kachujinRender->Pass(1);
	for (int i = -100; i <= 100; i += 10)
	{
		UINT index = kachujinRender->AddTransform();
		Transform* transform = kachujinRender->GetTransform(index);

		D3DXVECTOR3 position((float)i, 0, 0);

		D3DXVECTOR3 scale = Math::RandomVec3(0.025f, 0.025f);

		transform->Position(position);
		transform->Scale(scale);
		transform->RotationDegree(0, Math::Random(0.0f, 0.0f), 0);
		int num = Math::Random(0, 3);
		kachujinRender->PlayNextClip(index, num, 1.0f);
	}
	kachujinRender->UpdateTransform();
}
