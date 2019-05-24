#include "stdafx.h"
#include "PostEffectDemo.h"
#include "Environment/Sky.h"
#include "PostEffects/Default.h"
#include "PostEffects/Wiggle.h"
#include "PostEffects/Distortion.h"
#include "PostEffects/Sepia.h"
#include "PostEffects/Saturation.h"
#include "PostEffects/Sharpening.h"
#include "PostEffects/RadialBlur.h"
#include "PostEffects/Interlace.h"
#include "PostEffects/Vignette.h"

void PostEffectDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 0);
	Context::Get()->GetCamera()->Position(0, 46, -85);

	//화면 전체 길이
	float width = (UINT)D3D::GetDesc().Width;
	float height = (UINT)D3D::GetDesc().Height;

	//postEffect = new PostEffects::Default();
	//postEffect = new PostEffects::Wiggle();
	//postEffect = new PostEffects::Distortion();
	//postEffect = new PostEffects::Sepia();
	//postEffect = new PostEffects::Saturation();
	//postEffect = new PostEffects::Sharpening();
	//postEffect = new PostEffects::RadialBlur();
	postEffect = new PostEffects::Interlace();
	//postEffect = new PostEffects::Vignette();

	postEffect->GetTransform()->Position(width * 0.5f, height * 0.5f, 0);
	postEffect->GetTransform()->Scale(width, height, 1);


	renderTarget = new RenderTarget((UINT)width, (UINT)height);
	for (UINT i = 0; i < 10; i++)
		targets[i] = new RenderTarget((UINT)width, (UINT)height);

	depthStencil = new DepthStencil((UINT)width, (UINT)height);

	viewport = new Viewport(width, height);
	render2D = new Render2D();
	render2D->GetTransform()->Position(width * 0.25f, height * 0.5f, 0);
	render2D->GetTransform()->Scale(width * 0.5f, height, 1);

	finalRender2D = new Render2D();
	finalRender2D->GetTransform()->Position(width * 0.75f, height * 0.5f, 0);
	finalRender2D->GetTransform()->Scale(width/2, height, 1);

#ifdef  ALLOW_MULTI_VIEWPORT
	minimapTarget = new RenderTarget((UINT)width, (UINT)height);
	minimapDepth = new DepthStencil((UINT)width, (UINT)height);
	minimapViewPort = new Viewport(width, height);
	minimap2D = new Render2D();

	minimap2D->GetTransform()->Position(width - 125, 125, 0);
	minimap2D->GetTransform()->Scale(250, 250, 1);

#endif //  ALLOW_MULTI_VIEWPORT
	
	//Sky
	{
		sky = new Sky(L"Environment/GrassCube1024.dds");
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


}
void PostEffectDemo::Destroy()
{

}

void PostEffectDemo::Update()
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


	render2D->Update();
	finalRender2D->Update();
	postEffect->Update();

	//static float ratio = 1.0f;
	//ImGui::SliderFloat("Ratio", &ratio, 0.1f, 10.0f);

	//static float temp = 0.0f;
	//static float width = viewport->GetWidth();
	//static float height = viewport->GetHeight();
	//if (ratio != temp)
	//{
	//	temp = ratio;

	//	viewport->Set(width * ratio, height * ratio);
	//}
#ifdef  ALLOW_MULTI_VIEWPORT
	minimap2D->Update();
#endif //  ALLOW_MULTI_VIEWPORT
}

void PostEffectDemo::PreRender()
{
	//렌더 타겟
	renderTarget->Set(depthStencil->DSV());
	viewport->RSSetViewport();  

	RenderObject();

	targets[0]->Set(depthStencil->DSV());

	postEffect->SRV(renderTarget->SRV());
	postEffect->Render();

	finalSrv = targets[0]->SRV();  //타겟에서 최종적으로 그려진 걸 컨트롤하겠다

#ifdef  ALLOW_MULTI_VIEWPORT 
	minimapTarget->Set(minimapDepth->DSV());
	minimapViewPort->RSSetViewport();

	RenderObject();
#endif //  ALLOW_MULTI_VIEWPORT
}

void PostEffectDemo::Render()
{

}

void PostEffectDemo::PostRender()
{
	render2D->SRV(renderTarget->SRV());
	render2D->Render();

	finalRender2D->SRV(finalSrv);
	finalRender2D->Render();

#ifdef  ALLOW_MULTI_VIEWPORT
	minimap2D->SRV(minimapTarget->SRV());
	minimap2D->Render();
#endif //  ALLOW_MULTI_VIEWPORT
}

void PostEffectDemo::RenderObject()
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
