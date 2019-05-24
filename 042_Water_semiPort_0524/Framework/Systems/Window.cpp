#include "Framework.h"
#include "Window.h"
#include "IExecute.h"

IExecute* Window::mainExecute = NULL;

WPARAM Window::Run(IExecute * main)
{
	mainExecute = main;
	Create();

	D3DDesc desc = D3D::GetDesc();

	D3D::Create();
	Keyboard::Create();
	Mouse::Create();

	Time::Create();
	Time::Get()->Start();

	Gui::Create();
	Context::Create();

	mainExecute->Initialize();


	MSG msg = { 0 };
	while (true)

	{	
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //메세지가 있든 없든 패스
		{												//겟 메세지는 메세지가 있을때까지 대기
			if (msg.message == WM_QUIT) //빠져나옴
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			MainRender();
		}
	}
	mainExecute->Destroy();

	Context::Delete();
	Gui::Delete();
	Time::Delete();
	Mouse::Delete();
	Keyboard::Delete();
	D3D::Delete();

	Destroy();

	return msg.wParam;
}

void Window::Create()
{  //윈도우 생성
	D3DDesc desc = D3D::GetDesc();

	//WINDCLASSEX 와 EX차이 API 메뉴얼 찾아볼것

	WNDCLASSEX wndClass; //개발자 모드 지원. 풀스크린 모드
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.Instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	if (desc.bFullScreen == true)
	{
		DEVMODE devMode = { 0 };  //DEVMODE 개발자 옵션
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmPelsWidth = (DWORD)desc.Width;
		devMode.dmPelsHeight = (DWORD)desc.Height;
		devMode.dmBitsPerPel = 32;
		devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	}//잘 몰라도된다...

	desc.Handle = CreateWindowEx
	(
		WS_EX_APPWINDOW
		, desc.AppName.c_str()
		, desc.AppName.c_str()
		, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, NULL
		, (HMENU)NULL
		, desc.Instance
		, NULL
	);
	assert(desc.Handle != NULL);
	D3D::SetDesc(desc);


	RECT rect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.Width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.Height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	MoveWindow 
	(	//실제 윈도우 사이즈를 구할 때 제목표시줄과 선을 포함한 사이즈가 나온다.
		//이 함수를 통해 정확히 내가 사용할 사이즈를 사용하기 위함이다.
		desc.Handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, TRUE
	);
	ShowWindow(desc.Handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.Handle);  //가장 전면에 보여줄것
	SetFocus(desc.Handle);  //우리 윈도우에 포커스 둘것

	ShowCursor(true);
	//앞으로 우리가 건들 일 없다.
}

void Window::Destroy()
{
	D3DDesc desc = D3D::GetDesc();

	if (desc.bFullScreen == true)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(desc.Handle);

	UnregisterClass(desc.AppName.c_str(), desc.Instance);
}

LRESULT CALLBACK Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	Mouse::Get()->InputProc(message, wParam, lParam);

	if (Gui::Get()->MsgProc(handle, message, wParam, lParam))
		return true;

	if (message == WM_SIZE)
	{
		if (mainExecute != NULL)
		{
			float width = (float)LOWORD(lParam);
			float height = (float)HIWORD(lParam);

			if (D3D::Get() != NULL)
				D3D::Get()->ResizeScreen(width, height);

			mainExecute->ResizeScreen();
		}
	}

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

void Window::MainRender()
{
	Time::Get()->Update();

	if (ImGui::IsMouseHoveringAnyWindow() == false)
	{
		Keyboard::Get()->Update();
		Mouse::Get()->Update();
	}

	Gui::Get()->Update();
	mainExecute->Update();


	mainExecute->PreRender();

	//렌더링 구간
	D3DDesc desc = D3D::GetDesc();

	D3D::Get()->SetRenderTarget();
	D3D::Get()->Clear(desc.Background); //백버퍼 클리어
	{
		Context::Get()->Render();
		mainExecute->Render();
		mainExecute->PostRender();
		Gui::Get()->Render();
	}
	D3D::Get()->Present();  //백버퍼에 있는거를 화면에 보여준다.(내보낸다.)
}

