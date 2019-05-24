#include "Framework.h"
#include "Sky.h"

Sky::Sky()
	:cubeSrv(NULL), bCreateShader(true)
{
	shader = new Shader(L"031_Sky.fx");
	sData = shader->Variable("Data");
	sphere = new MeshSphere(shader, 0.5f);
}

Sky::Sky(wstring cubeMapFile)
	:bCreateShader(true)
{
	shader = new Shader(L"031_Sky.fx");
	sData = shader->Variable("Data");
	sphere = new MeshSphere(shader, 0.5f);
	sphere->GetTransform()->Scale(1000, 1000, 1000);
	sphere->Pass(1);

	wstring temp = L"../../_Textures/" + cubeMapFile;
	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), temp.c_str(), NULL, NULL, &cubeSrv, NULL
	));

	sCubeSrv = shader->AsSRV("SkyCubeMap");
}

Sky::Sky(Shader * shader, wstring cubeMapFile)
	:shader(shader), bCreateShader(false)
{
	sData = shader->Variable("Data");
	sphere = new MeshSphere(shader, 0.5f);
	sphere->GetTransform()->Scale(1000, 1000, 1000);
	sphere->Pass(1);

	wstring temp = L"../../_Textures/" + cubeMapFile;
	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), temp.c_str(), NULL, NULL, &cubeSrv, NULL
	));

	sCubeSrv = shader->AsSRV("SkyCubeMap");
}

Sky::~Sky()
{
	
	if(bCreateShader == true)
		SafeDelete(shader);
	SafeDelete(sphere);
}

void Sky::Update()
{
	D3DXVECTOR3 position;
	Context::Get()->GetCamera()->Position(&position);

	sphere->GetTransform()->Position(position);
	sphere->Update();
}

void Sky::Render()
{
	sData->SetRawValue(&dataDesc, 0, sizeof(DataDesc));

	if (sCubeSrv != NULL)
		sCubeSrv->SetResource(cubeSrv);

	sphere->Render();
}
