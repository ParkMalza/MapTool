#include "stdafx.h"
#include "LightDemo.h"
#include "Environment/Sky.h"

void LightDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0);
	Context::Get()->GetCamera()->Position(0, 46, -85);
	//Sky
	{
		sky = new Sky(L"Environment/SnowCube1024.dds");
	}

	//Create Mesh
	{
		specular = D3DXCOLOR(1, 1, 1, 20);

		shader = new Shader(L"028_Specular.fx");

		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");

		stone = new Material(shader);
		stone->Diffuse(0.8f, 0.7f, 0.7f);
		stone->DiffuseMap("Stones.png");
		stone->SpecularMap("Stones_Specular.png");
		stone->NormalMap("Stones_Normal.png");

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->Specular(1, 0, 0, 20);
		brick->SpecularMap("Bricks_Specular.png");
		brick->NormalMap("Bricks_Normal.png");

		wall = new Material(shader);
		wall->Specular(1, 0, 0, 20);
		wall->DiffuseMap("Wall.png");
		wall->SpecularMap("Wall_Specular.png");
		wall->NormalMap("Wall_Normal.png");
	}

	cube = new MeshCube(shader);
	cube->GetTransform()->Position(0, 5.0f, 0);
	cube->GetTransform()->Scale(20.0f, 10.0f, 20.0);

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

	//Add PointLight
	{
		PointLight light = { Color(1, 0, 0, 1), Vector3(20, 1, 0), 20, 1.0f };
		Context::Get()->AddPointLight(light);
	}
	//Point
	{
		SpotLight light = { Color(0, 0, 1, 0), Vector3(0, 5, 0), 3, Vector3(0, -1, 0), 0.3f, 1.0f };
		Context::Get()->AddSpotLight(light);
	}
}
void LightDemo::Destroy()
{

}

void LightDemo::Update()
{
	sky->Update();
	for (UINT i = 0; i < 10; i++)
	{
		sphere[i]->Update();
		cylinder[i]->Update();
	}

	cube->Update();
	grid->Update();

	//Specular Ggui
	ImGui::ColorEdit3("Specular", (float *)&specular);
	ImGui::SliderFloat("Shininess", &specular.a, 1.0f, 30.0f);

	floor->Specular(specular);
	stone->Specular(specular);
	wall->Specular(specular);
	brick->Specular(specular);

	//Point
	{
		PointLight& light = Context::Get()->GetPointLight(0);
		ImGui::SliderFloat3("PointLight Position", (float *)&light.Position, -30, 30);
		ImGui::SliderFloat("PointLight Range", &light.Range, 1, 30);
		ImGui::SliderFloat("PointLight Intensity", &light.Intensity, 0.1f, 1.0f - 1e-6f);
	}

	//Spot
	{
		SpotLight& light = Context::Get()->GetSpotLight(0);
		ImGui::SliderFloat3("SpotLight Position", (float *)&light.Position, -30, 30);
		ImGui::SliderFloat3("SpotLight Direction", (float *)&light.Direction, -1, 1);
		ImGui::SliderFloat("SpotLight Range", &light.Range, 1, 30);
		ImGui::SliderFloat("SpotLight Angle", &light.Angle, 0.1f, 30);
		ImGui::SliderFloat("SpotLight Intensity", &light.Intensity, 0.1f, 1.0f - 1e-6f);
	}
}

void LightDemo::Render()
{
	sky->Render();
	wall->Render();
	for (UINT i = 0; i < 10; i++)
		sphere[i]->Render();


	brick->Render();
	for (UINT i = 0; i < 10; i++)
		cylinder[i]->Render();


	stone->Render();
	cube->Render();

	floor->Render();
	grid->Render();
}
