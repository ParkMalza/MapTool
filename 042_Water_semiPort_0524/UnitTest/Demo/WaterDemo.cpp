#include "stdafx.h"
#include "WaterDemo.h"
#include "Environment/Sky.h"
#include "Environment/Water.h"

void WaterDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0, 0);
	Context::Get()->GetCamera()->Position(0, 46, -85);


	//Sky
	{
		sky = new Sky(L"Environment/GrassCube1024.dds");

		water = new Water(5, 50);
		shader = water->GetShader();
	}

	//Create Mesh
	{
		floor = new Material(shader);
		floor->Specular(1, 1, 1, 20);
		floor->DiffuseMap("Floor.png");
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");

		stone = new Material(shader);
		stone->Specular(1, 1, 1, 20);
		stone->Diffuse(0.8f, 0.7f, 0.7f);
		stone->DiffuseMap("Stones.png");
		stone->SpecularMap("Stones_Specular.png");
		stone->NormalMap("Stones_Normal.png");

		brick = new Material(shader);
		brick->Specular(1, 0, 0, 20);
		brick->DiffuseMap("Bricks.png");
		brick->SpecularMap("Bricks_Specular.png");
		brick->NormalMap("Bricks_Normal.png");

		wall = new Material(shader);
		wall->Specular(1, 0, 0, 20);
		wall->DiffuseMap("Wall.png");
		wall->SpecularMap("Wall_Specular.png");
		wall->NormalMap("Wall_Normal.png");


		cube = new MeshCube(shader);
		cube->GetTransform()->Position(0, 5.0f, 0);
		cube->GetTransform()->Scale(15.0f, 5.0f, 25.0);

		grid = new MeshGrid(shader, 3, 3);
		grid->GetTransform()->Position(0, 0, 0);
		grid->GetTransform()->Scale(20, 1, 20);


		for (UINT i = 0; i < 5; i++)
		{
			cylinder[i * 2] = new MeshCylinder(shader, 0.5f, 3.0f, 20, 20);
			cylinder[i * 2]->GetTransform()->Position(-30, 6.0f, -15.0f + (float)i * 15.0f);
			cylinder[i * 2]->GetTransform()->Scale(5, 5, 5);

			cylinder[i * 2 + 1] = new MeshCylinder(shader, 0.5f, 3.0f, 20, 20);
			cylinder[i * 2 + 1]->GetTransform()->Position(30, 6.0f, -15.0f + (float)i * 15.0f);
			cylinder[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);


			sphere[i * 2] = new MeshSphere(shader, 0.5f, 20, 20);
			sphere[i * 2]->GetTransform()->Position(-30.0f, 15.5f, -15.0f + i * 15.0f);
			sphere[i * 2]->GetTransform()->Scale(5, 5, 5);

			sphere[i * 2 + 1] = new MeshSphere(shader, 0.5f, 20, 20);
			sphere[i * 2 + 1]->GetTransform()->Position(30.0f, 15.5f, -15.0f + i * 15.0f);
			sphere[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);
		}
	}

	//Load Model
	{
		model[0] = new Model();
		model[0]->ReadMaterial(L"Eclipse/Eclipse");
		model[0]->ReadMesh(L"Eclipse/Eclipse");

		modelRender = new ModelRender(shader, model[0]);

		UINT index = modelRender->AddTransform();
		modelRender->GetTransform(index)->Position(0, 6.55f, -1);
		modelRender->GetTransform(index)->Scale(0.3f, 0.3f, 0.3f);
		modelRender->UpdateTransform();


		model[1] = new Model();
		model[1]->ReadMaterial(L"Kachujin/Mesh");
		model[1]->ReadMesh(L"Kachujin/Mesh");
		model[1]->ReadClip(L"Kachujin/Taunt");

		modelAnimator = new ModelAnimator(shader, model[1]);
		modelAnimator->Pass(1);

		index = modelAnimator->AddTransform();
		modelAnimator->GetTransform(index)->Position(-12.5, 0, 0);
		modelAnimator->GetTransform(index)->Scale(0.05f, 0.05f, 0.05f);
		modelAnimator->UpdateTransform();
	}



	//Add PointLight
	{
		PointLight light = { Color(1, 0, 0, 1), Vector3(20, 3, 0), 20, 1.0f };
		Context::Get()->AddPointLight(light);
	}

	//Add PointLight
	{
		PointLight light = { Color(0, 1, 0, 1), Vector3(-20, 3, 0), 15, 0.4f };
		Context::Get()->AddPointLight(light);
	}
}

void WaterDemo::Destroy()
{

}

void WaterDemo::Update()
{
	sky->Update();
	water->Update();

	for (UINT i = 0; i < 10; i++)
	{
		sphere[i]->Update();
		cylinder[i]->Update();
	}

	cube->Update();
	grid->Update();

	modelRender->Update();
	modelAnimator->Update();
}

void WaterDemo::PreRender()
{
	water->SetRefraction();
	{
		meshPass = 0;
		modelPass = 1;
		animPass = 2;


		sky->Render();

		wall->Render();
		for (UINT i = 0; i < 10; i++)
		{
			sphere[i]->Pass(meshPass);
			sphere[i]->Render();
		}

		brick->Render();
		for (UINT i = 0; i < 10; i++)
		{
			cylinder[i]->Pass(meshPass);
			cylinder[i]->Render();
		}

		stone->Render();
		cube->Pass(meshPass);
		cube->Render();

		floor->Render();
		grid->Pass(meshPass);
		grid->Render();


		modelRender->Pass(modelPass);
		modelRender->Render();

		modelAnimator->Pass(animPass);
		modelAnimator->Render();
	}

	water->SetReflection();
	{
		meshPass = 3;
		modelPass = 4;
		animPass = 5;

		sky->Render();

		wall->Render();
		for (UINT i = 0; i < 10; i++)
		{
			sphere[i]->Pass(meshPass);
			sphere[i]->Render();
		}

		brick->Render();
		for (UINT i = 0; i < 10; i++)
		{
			cylinder[i]->Pass(meshPass);
			cylinder[i]->Render();
		}

		stone->Render();
		cube->Pass(meshPass);
		cube->Render();

		floor->Render();
		grid->Pass(meshPass);
		grid->Render();


		modelRender->Pass(modelPass);
		modelRender->Render();

		modelAnimator->Pass(animPass);
		modelAnimator->Render();
	}
}

void WaterDemo::Render()
{
	meshPass = 6;
	modelPass = 7;
	animPass = 8;

	sky->Render();

	wall->Render();
	for (UINT i = 0; i < 10; i++)
	{
		sphere[i]->Pass(meshPass);
		sphere[i]->Render();
	}

	brick->Render();
	for (UINT i = 0; i < 10; i++)
	{
		cylinder[i]->Pass(meshPass);
		cylinder[i]->Render();
	}

	stone->Render();
	cube->Pass(meshPass);
	cube->Render();

	floor->Render();
	grid->Pass(meshPass);
	grid->Render();


	modelRender->Pass(modelPass);
	modelRender->Render();

	modelAnimator->Pass(animPass);
	modelAnimator->Render();

	water->Render();
}

void WaterDemo::PostRender()
{

}