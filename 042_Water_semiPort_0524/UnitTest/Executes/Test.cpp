#include "stdafx.h"
#include "Test.h"
#include "Utilities/Xml.h"
#include "Environment/Sky.h"
#include "Environment/Terrain.h"
#include "Environment/GeometryRain.h"
#include "Environment/GeometrySnow.h"
#include "MeshRender/MeshRender.h"
#include "BillBoardsRender/BillBoardsRender.h"
#include "BillBoardsRender/BillboardsInstancing.h"
#include "EditTerrain/BrushTerrain.h"
#include "EditTerrain/PaintTerrain.h"
#include "Models/ModelManager.h"
#include "RenderTarget/RTVMinimap.h"
#include "Viewer/Projector.h"
#include "Lights/Lights.h"

void Test::Initialize()
{
	//Context::Get()->GetCamera()->RotationDegree(12, 0);
	//Context::Get()->GetCamera()->Position(102, 23, -2);
	//RenderTarget
	integrationShader = new Shader(L"000_PPintegration2.fx");
	{
		renderTarget = new RTVMinimap();
	}
	//Sky
	{
		sky = new Sky(integrationShader, L"Environment/SnowCube1024.dds");
		bSnow = false;
		bRain = false;
	}

	//Weather
	{
		geoRain = new GeometryRain(D3DXVECTOR3(100, 100, 300), 1000);
		geoSnow = new GeometrySnow(D3DXVECTOR3(100, 100, 300), 1000);
	}
	//Terrain
	{

		//Projector
		wstring terrainBrushImage = L"Environment/MagicQuad.jpg";
		wstring terrainBrushImage2 = L"Environment/MagicCircle.png";
		vector<wstring> bImages;
		bImages.push_back(terrainBrushImage);
		bImages.push_back(terrainBrushImage2);
		projector = new Projector(integrationShader, bImages, 15, 15);

		startTerrain = L"HeightMaps/BaseMap256.png";
		paint = L"Terrain/Dirt.png";

		for (int i = 0; i < 5; i++)
			textureNames.push_back(paint);


		terrain = new Terrain(integrationShader, startTerrain, paint, textureNames);
		terrain->BaseMap(paint);
		terrain->LayerMap(paint);
		bTerrain = new BrushTerrain(projector, terrain);
		pTerrain = new PaintTerrain(projector, terrain);

	}
	//InstanceMesh
	{
		wstring path = L"../../_Textures/Meshes/OriginalTextures/";
		wstring filter = L"*.png";
		Path::GetFiles(&meshTextures, path, filter, false);
		for (UINT i = 0; i < meshTextures.size(); i++)
		{
			wstring temp = Path::GetFileName(meshTextures[i]);
			temp = L"Meshes/OriginalTextures/" + temp;
			meshTexturesNames.push_back(temp);
		}
		meshRender = new MeshRender(integrationShader, meshTexturesNames, terrain);
	}
	//Billboard
	{
		billboardsInstancing = new BillboardsInstancing(projector, integrationShader, terrain);
	}
	//Model
	{
		modelManager = new ModelManager(integrationShader, terrain);
	}
	//Lights
	{
		lights = new Lights(integrationShader);
	}
	////textureCube
	//{
		textureCube = NULL;
		sCubeSrv = integrationShader->AsSRV("CubeMap");
		isCube = false;
	//}
	////Shadow
	//{
		shadow = new Shadow(integrationShader, Vector3(130, 0, 0), 200, 2048, 2048);
	//}
}

void Test::Destroy()
{
	SafeRelease(sCubeSrv);
	SafeDelete(textureCube);
	SafeDelete(shadow);
	SafeDelete(projector);
	SafeDelete(lights);
	SafeDelete(modelManager);
	SafeDelete(billboardsInstancing);
	SafeDelete(meshRender);
	SafeDelete(pTerrain);
	SafeDelete(bTerrain);
	SafeDelete(terrain);
	SafeDelete(geoSnow);
	SafeDelete(geoRain);
	SafeDelete(sky);
	//SafeDelete(renderTarget);
	SafeDelete(integrationShader);
}

void Test::Update()
{
	picked = terrain->GetPickedHeight();
	ImGui::LabelText("Picking", "%.2f, %.2f, %.2f", picked.x, picked.y, picked.z);

	sky->Update();
	terrain->Update();
	
	WeatherImgui();

	SetImgui();
	modelManager->Update(terrain);
	//billBoardRender->Update();
	billboardsInstancing->Update();
	meshRender->Update();
	lights->Update(picked);

	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down('Z'))
			COMMANDMANAGER->Undo();
		if (Keyboard::Get()->Down('Y'))
			COMMANDMANAGER->Redo();
	}
	renderTarget->Update();
	SetPass();

}

void Test::PreRender()
{
	
	PreRenderObject();
	//renderTarget->PreRender();
	//if(isCube == true)
		//renderTarget->SetViewport();
	//RenderObject();

	//추가
	//renderTarget->MultiViewPreRender();

	//renderTarget->MinimapRender();
	//if (isCube == true)
		//renderTarget->SetViewport();
	//RenderObject();
}

void Test::Render()
{
	RenderObject();
}

void Test::PostRender()
{
	//renderTarget->PostRender();
}

void Test::WeatherImgui()
{
	ImGui::Checkbox("Snow", &bSnow);
	ImGui::SameLine();
	ImGui::Checkbox("Rain", &bRain);

	if(bRain)
		geoRain->Update();
	if(bSnow)
		geoSnow->Update();
	ImGui::Separator();
}

void Test::SetImgui()
{
	ImGui::Begin("Menu");
	if(ImGui::BeginTabBar("Tabs"))
	{
		//Mesh
		meshRender->SetImgui(picked, terrain);  //메쉬 임구이
		//Height
		bTerrain->SetImgui(picked);
		//GrassBillBoard
		billboardsInstancing->SettingImgui(terrain, picked);
		//Paint
		pTerrain->SetImgui(picked);
		//Model
		modelManager->SetImgui(picked);
		ImGui::EndTabBar();
	} ImGui::Separator();
	
	if (ImGui::Button("Save"))  //전체 세이브
	{
		D3DDesc desc = D3D::GetDesc();
		function<void(wstring)> func = bind(&Test::Save, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::XmlFilter, L"../XmlFolder/", func, desc.Handle);
	}
	if (ImGui::Button("Load"))  //전체 로드
	{
		D3DDesc desc = D3D::GetDesc();
		function<void(wstring)> func = bind(&Test::Load, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::XmlFilter, L"../XmlFolder/", func, desc.Handle);
	}

	ImGui::End();
}

void Test::PreRenderObject()
{
	shadow->Set();
	terrain->PreRender();
	modelManager->PreRender();
	meshRender->PreRender();
	//billboardsInstancing->ShadowRender();
	if (modelManager->GetCarCount() >0)
	{
		if (isCube == false)
		{
			textureCube = new TextureCube(picked, 256, 256);
			isCube = true;
		}

		textureCube->Set(integrationShader);

		Perspective* perspective = textureCube->GetPerspective();
		Context::Get()->SetSubPerspective(perspective);
	}
	sky->Pass(3);
	sky->Render();

	terrain->CubeRender();
	modelManager->CubeRender();
	meshRender->CubeRender();
	//if (bRain)
		//geoRain->Render();
	//if (bSnow)
		//geoSnow->Render();
	//billBoardRender->Render();
	//lights->Render();
}

void Test::RenderObject()
{
	sky->Pass(4);
	sky->Render();
	terrain->Render();
	if (bRain)
		geoRain->Render();
	if (bSnow)
		geoSnow->Render();
	if (modelManager->GetCarCount() >0 && textureCube != NULL)
		sCubeSrv->SetResource(textureCube->SRV());
	modelManager->Render();
	billboardsInstancing->Render();
	meshRender->Render();
	lights->Render();
}

void Test::SetPass()
{
	static int pass = 0;
	ImGui::Text("Terrain");
	ImGui::SliderInt("Pass", (int *)&pass, 0, 1);
	terrain->Pass(pass);
}

//전체 세이브
void Test::Save(wstring savePath)
{
	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));
	file = file + ".xml";

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement(file.c_str());

	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	// mesh
	Xml::XMLElement* element = document->NewElement("Meshes");
	Xml::XMLElement* child = NULL;

	wstring mesh = Path::GetFileName(savePath);
	mesh = mesh + L".mesh";
	string save = String::ToString(mesh);

	D3DXVECTOR3 pos(0,0,0);
	D3DXVECTOR3 scale(0,0,0);
	D3DXVECTOR3 rotation(0,0,0);
	
	element->SetAttribute("file", save.c_str());
	// element의 자식 child 값 넣기
	child = document->NewElement("Cube");
	child->SetAttribute("CubePosX", pos.x);
	element->LinkEndChild(child);
	//
	root->LinkEndChild(element);

	meshRender->Save(mesh);  //실 저장

	//height
	element = document->NewElement("Height");
	wstring height = Path::GetFileName(savePath);
	height = height + L".height";
	save = String::ToString(height);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	terrain->Save(height);  //실 저장

	//grass
	element = document->NewElement("Grass");
	wstring grass = Path::GetFileName(savePath);
	grass = grass + L".grass";
	save = String::ToString(grass);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	billboardsInstancing->Save(grass);

	//Tank
	element = document->NewElement("Tank");
	wstring tank = Path::GetFileName(savePath);
	tank = tank + L".tank";
	save = String::ToString(tank);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	modelManager->tankSave(tank);  //실 저장

	//Tower
	element = document->NewElement("Tower");
	wstring tower = Path::GetFileName(savePath);
	tower = tower + L".tower";
	save = String::ToString(tower);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	modelManager->towerSave(tower);  //실 저장

	//Light
	element = document->NewElement("Light");
	wstring light = Path::GetFileName(savePath);
	light = light + L".light";
	save = String::ToString(light);
	element->SetAttribute("file", save.c_str());

	root->LinkEndChild(element);

	lights->Save(light);  //실 저장


	document->SaveFile((folder + file).c_str());
}
//전체 로드
void Test::Load(wstring savePath)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(savePath).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* element = root->FirstChildElement();
	Xml::XMLElement* child = element->FirstChildElement();
	D3DXVECTOR3 pos;
	//첫번째 참조 mesh
	{
		const char* mesh = element->Attribute("file");
		pos.x = child->FloatAttribute("CubePosX");  //save에서 CubePosX 이름맞춰서
		string loadMesh = mesh;

		wstring file = String::ToWString(loadMesh);
		meshRender->Load(file);

		element = element->NextSiblingElement();
	}
	//두번째 참조 height
	{
		const char* height = element->Attribute("file");
		string loadHeight = height;

		wstring file2 = String::ToWString(loadHeight);
		terrain->Load(file2);

		element = element->NextSiblingElement();
	}
	//3번째 참조 grass
	{
		const char* grass = element->Attribute("file");
		string loadGrass = grass;

		wstring file3 = String::ToWString(loadGrass);

		billboardsInstancing->Load(file3);

		element = element->NextSiblingElement();
	}
	//4번째 참조 tank
	{
		const char* tank = element->Attribute("file");
		string loadTank = tank;

		wstring file5 = String::ToWString(loadTank);
		modelManager->tankLoad(file5);

		element = element->NextSiblingElement();
	}
	//5번째 참조 tower
	{
		const char* tower = element->Attribute("file");
		string loadTower = tower;

		wstring file5 = String::ToWString(loadTower);
		modelManager->towerLoad(file5);

		element = element->NextSiblingElement();
	}
	//6번째 참조 light
	{
		const char* light = element->Attribute("file");
		string loadLight = light;

		wstring file6 = String::ToWString(loadLight);
		lights->Load(file6);

		element = element->NextSiblingElement();
	}
}
