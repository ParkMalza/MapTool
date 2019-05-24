#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"
#include "Viewer/Freedom.h"
#include "Demo/LightDemo.h"
#include "Executes/Test.h"
#include "Demo/EnvDemo.h"
#include "Demo/PostEffectDemo.h"
#include "Demo/CubeMapDemo.h"
#include "Demo/ProjectorDemo.h"
#include "Demo/ShadowDemo.h"
#include "Demo/WaterDemo.h"

void Main::Initialize()
{
	//Context::Get()->GetCamera()->RotationDegree(0, 30);
	//Context::Get()->GetCamera()->Position(0, 0, -50);
	((Freedom *)Context::Get()->GetCamera())->Speed(100, 2.5f);

	//Push(new LightDemo());
	//Push(new EnvDemo());
	Push(new Test());
	//Push(new PostEffectDemo());
	//Push(new CubeMapDemo());
	//Push(new ProjectorDemo());
	//Push(new ShadowDemo());
	//Push(new WaterDemo());
}	

void Main::Ready()
{

}

void Main::Destroy()
{
	for (IExecute* exe : executes)
	{
		exe->Destroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	Context::Get()->Update();

	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	string str = string("Frame Rate : ") + to_string(ImGui::GetIO().Framerate);
	Gui::Get()->RenderText(5, 5, 1, 1, 1, str);

	D3DXVECTOR3 camPos;
	Context::Get()->GetCamera()->Position(&camPos);

	D3DXVECTOR3 camDir;
	Context::Get()->GetCamera()->RotationDegree(&camDir);

	str = "Cam Position : ";
	str += to_string((int)camPos.x) + ", " + to_string((int)camPos.y) + ", " + to_string((int)camPos.z);
	Gui::Get()->RenderText(5, 20, 1, 1, 1, str);

	str = "Cam Rotation : ";
	str += to_string((int)camDir.x) + ", " + to_string((int)camDir.y);
	Gui::Get()->RenderText(5, 35, 1, 1, 1, str);

	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"©жди ╬Н╥ф╢о";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = D3DXCOLOR(0xFF6B6562);
	D3D::SetDesc(desc);


	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SafeDelete(main);

	return wParam;
}