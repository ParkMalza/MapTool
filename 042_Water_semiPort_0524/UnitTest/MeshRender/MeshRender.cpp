#include "stdafx.h"
#include "Viewer/Viewport.h"
#include "Environment/Terrain.h"
#include "Meshes/InstanceMesh.h"
#include "MeshRender.h"
#include "EnumGroup.h"
#include "Utilities/BinaryFile.h"

MeshRender::MeshRender(Shader * shader, vector<wstring>& textureNames, Terrain* terrain)
	:shader(shader), collPass(17), shadowPass(12), renderPass(16), cubePass(8)
{
	this->terrain = terrain;
	tempPos = D3DXVECTOR3(0, 0, 0);
	tempScale = D3DXVECTOR3(1, 1, 1);
	tempRotation = D3DXVECTOR3(0, Math::PI, 0);
	boxScale = D3DXVECTOR3(0, 0, 0);

	quadResize = D3DXVECTOR3(1.0f, 1.1f, 0.1f);
	cubeResize = D3DXVECTOR3(1.2f, 1.2f, 1.2f);
	gridResize = D3DXVECTOR3(1.0f, 0.1f, 1.0f);
	sphereResize = D3DXVECTOR3(1.0f, 1.1f, 1.0f);
	cylinderResize = D3DXVECTOR3(1.0f, 1.1f, 1.0f);

	textures = new TextureArray(textureNames, 256, 256);
	shader->AsSRV("Maps")->SetResource(textures->SRV()); //메쉬 원본 사진
	//쉐이더의 Map에 SRV 값 넣어준다.
	meshes.push_back(quad = new InstanceQuad(shader));
	meshes.push_back(cube = new InstanceCube(shader));
	meshes.push_back(grid = new InstanceGrid(shader));
	meshes.push_back(sphere = new InstanceSphere(shader, MESH_DEFAUL_LENGTH));
	meshes.push_back(cylinder = new InstanceCylinder(shader, MESH_DEFAUL_LENGTH, MESH_DEFAUL_LENGTH * 2));
	for (int i = 0; i < 5; i++)
	{
		quadBox[i] = new InstanceCollisionBox(shader, MESH_DEFAUL_LENGTH, MESH_DEFAUL_LENGTH, MESH_DEFAUL_LENGTH);
		quadBox[i]->Color(0, 1, 0);
		quadBox[i]->InstancePass(collPass);
		meshes.push_back(quadBox[i]);
	}

	quadImage = new Texture(L"Meshes/Quad.png");
	cubeImage = new Texture(L"Meshes/Cube.png");
	gridImage = new Texture(L"Meshes/Grid.png");
	sphereImage = new Texture(L"Meshes/Sphere.png");
	cylinderImage = new Texture(L"Meshes/Cylinder.png");

	whiteImage = new Texture(L"Meshes/OriginalTextures/1_White.png");
	currentImages.push_back(whiteImage);
	redImage = new Texture(L"Meshes/OriginalTextures/2_Red.png");
	currentImages.push_back(redImage);
	MagentaImage = new Texture(L"Meshes/OriginalTextures/3_Magenta.png");
	currentImages.push_back(MagentaImage);
	boxImage = new Texture(L"Meshes/OriginalTextures/4_Box.png");
	currentImages.push_back(boxImage);
	brickImage = new Texture(L"Meshes/OriginalTextures/5_Bricks.png");
	currentImages.push_back(brickImage);
	stoneImage = new Texture(L"Meshes/OriginalTextures/6_Stones.png");
	currentImages.push_back(stoneImage);
	floorImage = new Texture(L"Meshes/OriginalTextures/7_Floor.png");
	currentImages.push_back(floorImage);
	wallImage = new Texture(L"Meshes/OriginalTextures/8_Wall.png");
	currentImages.push_back(wallImage);

	for (int i = 0; i < 5; i++)
		isMeshSelect[i] = false;

	isChange = false;
	isShowCurrentTex = false;

	selectQuad = -1;
	selectCube = -1;
	selectGrid = -1;
	selectSphere = -1;
	selectCylinder = -1;
	selectMeshTexture = 0;
	instanceTexture = 0;
	comboTexture = -1;
	selectMesh = -1;

	saveIndex = 0;
	changeTexture = -1;

	imageX = 50;
	imageY = 50;

	//라이트
	specular = D3DXCOLOR(1, 1, 1, 20);
	mShader = new Shader(L"028_Specular.fx");


	for (UINT i = 0; i < textureNames.size(); i++)
	{
		wstring specName = Path::GetFileNameWithoutExtension(textureNames[i]);
		string comboName = String::ToString(specName);
		names.push_back(comboName);  // 콤보에 넣어질 이름이다.
		specName = L"meshes/" + specName + L"_Specular.png";
		specularTextures.push_back(specName);  //스페큘라 이미지다

		wstring normalName = Path::GetFileNameWithoutExtension(textureNames[i]);
		normalName = L"meshes/" + normalName + L"_Normal.png";
		normalTextures.push_back(normalName);  //노말 이미지!
	}

	box = new Material(shader);
	box->SpecularMaps(specularTextures);
	box->NormalMaps(normalTextures);

	boxNum = 0;
}

MeshRender::~MeshRender()
{
	SafeDelete(box);
	SafeDelete(mShader);
	for (Texture* curImage : currentImages)
		SafeDelete(curImage);
	SafeDelete(cylinderImage);
	SafeDelete(sphereImage);
	SafeDelete(gridImage);
	SafeDelete(cubeImage);
	SafeDelete(quadImage);
	SafeDelete(textures);
	for (InstanceMesh* mesh : meshes)
		SafeDelete(mesh);
}

void MeshRender::Update()
{
	for (InstanceMesh* mesh : meshes)
		mesh->Update();
	//선택
	SelectedQuad();
	SelectedCube();
	SelectedGrid();
	SelectedSphere();
	SelectedCylinder();
	//편집
	EditQuad();
	EditCube();
	EditGrid();
	EditSphere();
	EditCylinder();
	EditTexture();
	//인식
	QuadCheckHeight();
	CubeCheckHeight();
	GridCheckHeight();
	SphereCheckHeight();
	CylinderCheckHeight();

	//Specular Ggui
	ImGui::ColorEdit3("Specular", (float *)&specular);
	ImGui::SliderFloat("Shininess", &specular.a, 1.0f, 30.0f);
	box->Specular(specular);
}

void MeshRender::PreRender()
{
	for (int i = 0; i < 5; i++)
	{
		meshes[i]->InstancePass(shadowPass);
		meshes[i]->Render();
	}
}

void MeshRender::CubeRender()
{
	for (int i = 0; i < 5; i++)
	{
		meshes[i]->InstancePass(cubePass);
		meshes[i]->Render();
	}
}

void MeshRender::Render()
{
	box->Render();
	for (int i = 0; i < 5; i++)
	{
		meshes[i]->InstancePass(renderPass);
		meshes[i]->Render();
	}
	for (int i = 5; i < 10; i++)
	{
		meshes[i]->InstancePass(collPass);
		meshes[i]->Render();
	}

}

void MeshRender::SettingMesh(D3DXVECTOR3& picked, Terrain* terrain)
{
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0) && picked.x >-1 && picked.z > -1 && picked.y > -1)
		{
			switch (selectMesh)
			{
				case QUAD: //QUAD 그리고, 선택한 comboTexture 번호 넘겨준다
					meshes[selectMesh]->SetOldMesh();
					COMMANDMANAGER->SetUndoCommand(STACK_QUAD, this);
					Instancing(picked, terrain, QUAD, instanceTexture);
					selectMesh = -1;//배치하고 기존 설정 없애기..
					break;
				case CUBE:
					meshes[selectMesh]->SetOldMesh();
					COMMANDMANAGER->SetUndoCommand(STACK_CUBE, this);
					Instancing(picked, terrain, CUBE, instanceTexture);
					selectMesh = -1;
					break;
				case GRID:
					meshes[selectMesh]->SetOldMesh();
					COMMANDMANAGER->SetUndoCommand(STACK_GRID, this);
					Instancing(picked, terrain, GRID, instanceTexture);
					selectMesh = -1;
					break;
				case SPHERE:
					meshes[selectMesh]->SetOldMesh();
					COMMANDMANAGER->SetUndoCommand(STACK_SPHERE, this);
					Instancing(picked, terrain, SPHERE, instanceTexture);
					selectMesh = -1;
					break;
				case CYLINDER:
					meshes[selectMesh]->SetOldMesh();
					COMMANDMANAGER->SetUndoCommand(STACK_CYLINDER, this);
					Instancing(picked, terrain, CYLINDER, instanceTexture);
					selectMesh = -1;
					break;
				default:
					break;
			}
		}
	}
}
void MeshRender::Instancing(D3DXVECTOR3& picked, Terrain* terrain, UINT mesh, UINT textureNum )
{
	switch (mesh)
	{
		case QUAD:
			meshes[0]->MakeInstance(picked, terrain, textureNum);
			BoxInstancing(QUADBOX, QUAD, quadResize);
			break;
		case CUBE:
			meshes[1]->MakeInstance(picked, terrain, textureNum);
			BoxInstancing(CUBEBOX, CUBE, cubeResize);
			break;
		case GRID:
			meshes[2]->MakeInstance(picked, terrain, textureNum);
			BoxInstancing(GRIDBOX, GRID, gridResize);
			break;
		case SPHERE:
			meshes[3]->MakeInstance(picked, terrain, textureNum);
			BoxInstancing(SPHEREBOX, SPHERE, sphereResize);
			break;
		case CYLINDER:
			meshes[4]->MakeInstance(picked, terrain, textureNum);
			BoxInstancing(CYLINDERBOX, CYLINDER, cylinderResize);
			break;
		default:
			break;
	}
}

void MeshRender::BoxInstancing(UINT boxNum, UINT meshNum, D3DXVECTOR3 reSize)
{
	boxIndex = meshes[boxNum]->PushTexture(0);
	boxTrans = meshes[boxNum]->GetTransform(boxIndex);
	meshes[meshNum]->Position(&tempPos);
	meshes[meshNum]->Scale(&tempScale);
	meshes[meshNum]->Rotation(&tempRotation);
	boxScale.x = tempScale.x * reSize.x;
	boxScale.y = tempScale.y * reSize.y;
	boxScale.z = tempScale.z * reSize.z;
	boxTrans->Position(tempPos);
	boxTrans->Scale(boxScale);
	boxTrans->Rotation(tempRotation);
}
///////////////////////////////////////////////////////////////////////////////
void MeshRender::SelectedQuad()
{
	UINT count = quadBox[QUAD]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();
	
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (quadBox[QUAD]->Selected(start, direction, i))
				{
					SetSelectFalse();
					isMeshSelect[QUAD] = true; //해당 메쉬 피킹
					isShowCurrentTex = true; //해당 메쉬 텍스쳐 보이기
					selectQuad = i;  //몇번째 픽했는지
					comboTexture = -1; //임구이 콤보에서 텍스쳐 안보이고 해당메쉬 텍스쳐로 보이기
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isMeshSelect[QUAD] = true;
					else
					{
						isMeshSelect[QUAD] = false;
						comboTexture = -1;
					}
				}
			}
		}
	}
}

void MeshRender::SelectedCube()
{
	UINT count = quadBox[CUBE]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (quadBox[CUBE]->Selected(start, direction, i))
				{
					SetSelectFalse();
					isMeshSelect[CUBE] = true;
					isShowCurrentTex = true;
					selectCube = i;
					comboTexture = -1;
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isMeshSelect[CUBE] = true;
					else
					{
						isMeshSelect[CUBE] = false;
						comboTexture = -1;
					}
						
				}
			}
		}
	}
}

void MeshRender::SelectedGrid()
{
	UINT count = quadBox[GRID]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();
	
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (quadBox[GRID]->Selected(start, direction, i))
				{
					SetSelectFalse();
					isMeshSelect[GRID] = true;
					isShowCurrentTex = true;
					selectGrid = i;
					comboTexture = -1;
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isMeshSelect[GRID] = true;
					else
					{
						isMeshSelect[GRID] = false;
						comboTexture = -1;
					}
				}
			}
		}
	}
}

void MeshRender::SelectedSphere()
{
	UINT count = quadBox[SPHERE]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (quadBox[SPHERE]->Selected(start, direction, i))
				{
					SetSelectFalse();
					isMeshSelect[SPHERE] = true;
					isShowCurrentTex = true;
					selectSphere = i;
					comboTexture = -1;
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isMeshSelect[SPHERE] = true;
					else
					{
						isMeshSelect[SPHERE] = false;
						comboTexture = -1;
					}
				}
			}
		}
	}
}

void MeshRender::SelectedCylinder()
{
	UINT count = quadBox[CYLINDER]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();

	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (quadBox[CYLINDER]->Selected(start, direction, i))
				{
					SetSelectFalse();
					isMeshSelect[CYLINDER] = true;
					isShowCurrentTex = true;
					selectCylinder = i;
					comboTexture = -1;
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isMeshSelect[CYLINDER] = true;
					else
					{
						isMeshSelect[CYLINDER] = false;
						comboTexture = -1;
					}
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void MeshRender::EditQuad()
{
	if (isMeshSelect[QUAD] == true)
	{
		ImGui::Begin("MeshEdit");
		
		boxTrans = quadBox[QUAD]->GetTransform(selectQuad); //0
		editTrans = meshes[QUAD]->GetTransform(selectQuad);
		if (isMeshSelect[QUAD])
			selectMeshTexture =  meshes[QUAD]->GetTexture(selectQuad);

		GetState();
		Color color;
		meshes[QUAD]->Colors(&tempColor, selectQuad);

		ImGui::SliderFloat3("position", (float*)&tempPos, 3, 253);
		ImGui::SliderFloat3("scale", (float*)&tempScale, 1, 20);
		ImGui::SliderFloat3("rotation", (float*)&tempRotation, 0, Math::PI);
		ImGui::ColorEdit3("color", (float*)&tempColor);

		meshes[QUAD]->Colors(tempColor, selectQuad);
		boxScale.x = tempScale.x * quadResize.x;
		boxScale.y = tempScale.y * quadResize.y;
		boxScale.z = tempScale.z * quadResize.z;

		SetState();

		Delete(QUAD, selectQuad);

		ImGui::End();
	}
}

void MeshRender::EditCube()
{
	if (isMeshSelect[CUBE] == true)
	{
		ImGui::Begin("MeshEdit");
		
		boxTrans = quadBox[CUBE]->GetTransform(selectCube); //0
		editTrans = meshes[CUBE]->GetTransform(selectCube);
		if(isMeshSelect[CUBE])
			selectMeshTexture = meshes[CUBE]->GetTexture(selectCube);

		GetState();
		Color color;
		meshes[CUBE]->Colors(&tempColor, selectCube);

		ImGui::SliderFloat3("position", (float*)&tempPos, 3, 253);
		ImGui::SliderFloat3("scale", (float*)&tempScale, 1, 20);
		ImGui::SliderFloat3("rotation", (float*)&tempRotation, 0, Math::PI);
		ImGui::ColorEdit3("color", (float*)&tempColor);

		meshes[CUBE]->Colors(tempColor, selectCube);
		boxScale.x = tempScale.x * cubeResize.x;
		boxScale.y = tempScale.y * cubeResize.y;
		boxScale.z = tempScale.z * cubeResize.z;

		SetState();

		Delete(CUBE, selectCube);

		ImGui::End();
	}
}

void MeshRender::EditGrid()
{
	if (isMeshSelect[GRID] == true)
	{
		ImGui::Begin("MeshEdit");
		
		boxTrans = quadBox[GRID]->GetTransform(selectGrid); //0
		editTrans = meshes[GRID]->GetTransform(selectGrid);
		if (isMeshSelect[GRID] == true)
			selectMeshTexture = meshes[GRID]->GetTexture(selectGrid);

		GetState();
		Color color;
		meshes[GRID]->Colors(&tempColor, selectGrid);

		ImGui::SliderFloat3("position", (float*)&tempPos, 3, 253);
		ImGui::SliderFloat3("scale", (float*)&tempScale, 1, 20);
		ImGui::SliderFloat3("rotation", (float*)&tempRotation, 0, Math::PI);
		ImGui::ColorEdit3("color", (float*)&tempColor);

		meshes[GRID]->Colors(tempColor, selectGrid);
		boxScale.x = tempScale.x * gridResize.x;
		boxScale.y = tempScale.y * gridResize.y;
		boxScale.z = tempScale.z * gridResize.z;

		SetState();

		Delete(GRID, selectGrid);

		ImGui::End();
	}
}

void MeshRender::EditSphere()
{
	if (isMeshSelect[SPHERE] == true)
	{
		ImGui::Begin("MeshEdit");
		
		boxTrans = quadBox[SPHERE]->GetTransform(selectSphere); //0
		editTrans = meshes[SPHERE]->GetTransform(selectSphere);
		if (isMeshSelect[SPHERE] == true)
			selectMeshTexture = meshes[SPHERE]->GetTexture(selectSphere);

		GetState();
		meshes[SPHERE]->Colors(&tempColor, selectSphere);

		ImGui::SliderFloat3("position", (float*)&tempPos, 3, 253);
		ImGui::SliderFloat3("scale", (float*)&tempScale, 1, 20);
		ImGui::SliderFloat3("rotation", (float*)&tempRotation, 0, Math::PI);
		ImGui::ColorEdit3("color", (float*)&tempColor);

		meshes[SPHERE]->Colors(tempColor, selectSphere);
		boxScale.x = tempScale.x * sphereResize.x;
		boxScale.y = tempScale.y * sphereResize.y;
		boxScale.z = tempScale.z * sphereResize.z;

		SetState();

		Delete(SPHERE, selectSphere);

		ImGui::End();
	}
}

void MeshRender::EditCylinder()
{
	if (isMeshSelect[CYLINDER] == true)
	{
		ImGui::Begin("MeshEdit");
		
		boxTrans = quadBox[CYLINDER]->GetTransform(selectCylinder); //0
		editTrans = meshes[CYLINDER]->GetTransform(selectCylinder);
		if (isMeshSelect[CYLINDER] == true)
			selectMeshTexture = meshes[CYLINDER]->GetTexture(selectCylinder);

		GetState();
		meshes[CYLINDER]->Colors(&tempColor, selectCylinder);

		ImGui::SliderFloat3("position", (float*)&tempPos, 3, 253);
		ImGui::SliderFloat3("scale", (float*)&tempScale, 1, 20);
		ImGui::SliderFloat3("rotation", (float*)&tempRotation, 0, Math::PI);
		ImGui::ColorEdit3("color", (float*)&tempColor);

		meshes[CYLINDER]->Colors(tempColor, selectCylinder);
		boxScale.x = tempScale.x * cylinderResize.x;
		boxScale.y = tempScale.y * cylinderResize.y;
		boxScale.z = tempScale.z * cylinderResize.z;

		SetState();

		Delete(CYLINDER, selectCylinder);

		ImGui::End();
	}
}

void MeshRender::EditTexture()
{
	ImGui::Checkbox("EditTexture", &isChange);
	if (isMeshSelect[QUAD] == true && isChange == true)
	{
		meshes[QUAD]->ChangeTexture(selectQuad, instanceTexture);
		selectMesh = -1;  //선택 한거 풀어줘야..
		isChange = false;
	}
	if (isMeshSelect[CUBE] == true && isChange == true)
	{
		meshes[CUBE]->ChangeTexture(selectCube, instanceTexture);
		selectMesh = -1;
		isChange = false;
	}
	if (isMeshSelect[GRID] == true && isChange == true)
	{
		meshes[GRID]->ChangeTexture(selectGrid, instanceTexture);
		selectMesh = -1;
		isChange = false;
	}
	if (isMeshSelect[SPHERE] == true && isChange == true)
	{
		meshes[SPHERE]->ChangeTexture(selectSphere, instanceTexture);
		selectMesh = -1;
		isChange = false;
	}
	if (isMeshSelect[CYLINDER] == true && isChange == true)
	{
		meshes[CYLINDER]->ChangeTexture(selectCylinder, instanceTexture);
		selectMesh = -1;
		isChange = false;
	}
}
void MeshRender::QuadCheckHeight()
{
	for (UINT i = 0; i < meshes[QUAD]->GetDrawCount(); i++)
	{
		Transform* meshTrans = meshes[QUAD]->GetTransform(i);
		Transform* boxTrans = meshes[QUADBOX]->GetTransform(i);
		Vector3 tempPos;
		Vector3 tempScalse;
		meshTrans->Position(&tempPos);
		meshTrans->Scale(&tempScale);
		boxTrans->Position(&tempPos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y + (tempScale.y * 5.0f);
		meshTrans->Position(tempPos);
		boxTrans->Position(tempPos);
	}
}
void MeshRender::CubeCheckHeight()
{
	for (UINT i = 0; i < meshes[CUBE]->GetDrawCount(); i++)
	{
		Transform* meshTrans = meshes[CUBE]->GetTransform(i);
		Transform* boxTrans = meshes[CUBEBOX]->GetTransform(i);
		Vector3 tempPos;
		Vector3 tempScalse;
		meshTrans->Position(&tempPos);
		meshTrans->Scale(&tempScale);
		boxTrans->Position(&tempPos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y + (tempScale.y * 5.0f);
		meshTrans->Position(tempPos);
		boxTrans->Position(tempPos);
	}
}
void MeshRender::GridCheckHeight()
{
	for (UINT i = 0; i < meshes[GRID]->GetDrawCount(); i++)
	{
		Transform* meshTrans = meshes[GRID]->GetTransform(i);
		Transform* boxTrans = meshes[GRIDBOX]->GetTransform(i);
		Vector3 tempPos;
		Vector3 tempScalse;
		meshTrans->Position(&tempPos);
		meshTrans->Scale(&tempScale);
		boxTrans->Position(&tempPos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y + (tempScale.y * 5.0f);
		meshTrans->Position(tempPos);
		boxTrans->Position(tempPos);
	}
}
void MeshRender::SphereCheckHeight()
{
	for (UINT i = 0; i < meshes[SPHERE]->GetDrawCount(); i++)
	{
		Transform* meshTrans = meshes[SPHERE]->GetTransform(i);
		Transform* boxTrans = meshes[SPHEREBOX]->GetTransform(i);
		Vector3 tempPos;
		Vector3 tempScalse;
		meshTrans->Position(&tempPos);
		meshTrans->Scale(&tempScale);
		boxTrans->Position(&tempPos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y + (tempScale.y * 5.0f);
		meshTrans->Position(tempPos);
		boxTrans->Position(tempPos);
	}
}
void MeshRender::CylinderCheckHeight()
{
	for (UINT i = 0; i < meshes[CYLINDER]->GetDrawCount(); i++)
	{
		Transform* meshTrans = meshes[CYLINDER]->GetTransform(i);
		Transform* boxTrans = meshes[CYLINDERBOX]->GetTransform(i);
		Vector3 tempPos;
		Vector3 tempScalse;
		meshTrans->Position(&tempPos);
		meshTrans->Scale(&tempScale);
		boxTrans->Position(&tempPos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y + (tempScale.y * 5.0f);
		meshTrans->Position(tempPos);
		boxTrans->Position(tempPos);
	}
}
///////////////////////////////////////////////////////////////////////////////
void MeshRender::GetState()
{
	boxTrans->Position(&tempPos);
	boxTrans->Scale(&boxScale);
	boxTrans->Rotation(&tempRotation);
	
	editTrans->Position(&tempPos);
	editTrans->Scale(&tempScale);
	editTrans->Rotation(&tempRotation);
}

void MeshRender::SetState()
{
	boxTrans->Position(tempPos);
	boxTrans->Scale(boxScale);
	boxTrans->Rotation(tempRotation);

	editTrans->Position(tempPos);
	editTrans->Scale(tempScale);
	editTrans->Rotation(tempRotation);
}

void MeshRender::UnProject()
{
	D3DXMatrixIdentity(&W);
	V = Context::Get()->ViewMatrix();
	P = Context::Get()->ProjectionMatrix();

	Viewport* vp = Context::Get()->GetViewport();

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	mouse.z = 0.0f;
	Context::Get()->GetViewport()->Unproject(&n, mouse, W, V, P);

	mouse.z = 1.0f;
	Context::Get()->GetViewport()->Unproject(&f, mouse, W, V, P);

	direction = f - n;

	start = n;
}
///////////////////////////////////////////////////////////////////////////////
void MeshRender::SetSelectFalse()
{
	for (int i = 0; i < 5; i++)
		isMeshSelect[i] = false;
}

void MeshRender::SetImgui(D3DXVECTOR3& picked, Terrain* terrain)
{
	static const char* current_texture = NULL;
	if (ImGui::BeginTabItem("MeshesEditor"))
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Mesh");

		if (ImGui::ImageButton(quadImage->SRV(), ImVec2(imageX, imageY)) == true)
			selectMesh = 0; ImGui::SameLine();
		if (ImGui::ImageButton(cubeImage->SRV(), ImVec2(imageX, imageY)) == true)
			selectMesh = 1; ImGui::SameLine();
		if (ImGui::ImageButton(gridImage->SRV(), ImVec2(imageX, imageY)) == true)
			selectMesh = 2; ImGui::SameLine();
		if (ImGui::ImageButton(sphereImage->SRV(), ImVec2(imageX, imageY)) == true)
			selectMesh = 3; ImGui::SameLine();
		if (ImGui::ImageButton(cylinderImage->SRV(), ImVec2(imageX, imageY)) == true)
			selectMesh = 4;

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MeshTexture");

		if (ImGui::BeginCombo("##combo", current_texture))
		{
			for (UINT n = 0; n < names.size(); n++)
			{
				bool is_selected = (current_texture == names[n].c_str());
				if (ImGui::Selectable(names[n].c_str(), is_selected))
				{
					current_texture = names[n].c_str();
					instanceTexture = comboTexture = selectMeshTexture = n;
					isShowCurrentTex = false;
				}
			}
			ImGui::EndCombo();
		}

		ShowCurrentTexture();

		SettingMesh(picked, terrain);
		ImGui::EndTabItem();
	}
}

//메쉬 삭제
void MeshRender::Delete(UINT meshType, UINT selectNum)
{
	if (Keyboard::Get()->Down(VK_DELETE))
	{
		boxTrans->Position(0, 0, 0);
		boxTrans->Scale(0, 0, 0);
		boxTrans->Rotation(0, 0, 0);

		editTrans->Position(0, 0, 0);
		editTrans->Scale(0, 0, 0);
		editTrans->Rotation(0, 0, 0);

		quadBox[meshType]->SaveDeleteIndex(selectNum);
		meshes[meshType]->SaveDeleteIndex(selectNum);
		isMeshSelect[meshType] = false;
	}
}

void MeshRender::ShowCurrentTexture()
{
	if (isMeshSelect[QUAD] == true && isShowCurrentTex == true)
	{
		ImGui::Image(currentImages[selectMeshTexture]->SRV(), ImVec2(130, 130));
	}
	else if(isMeshSelect[CUBE] == true && isShowCurrentTex == true)
	{
		ImGui::Image(currentImages[selectMeshTexture]->SRV(), ImVec2(130, 130));
	}
	else if (isMeshSelect[GRID] == true && isShowCurrentTex == true)
	{
		ImGui::Image(currentImages[selectMeshTexture]->SRV(), ImVec2(130, 130));
	}
	else if (isMeshSelect[SPHERE] == true && isShowCurrentTex == true)
	{
		ImGui::Image(currentImages[selectMeshTexture]->SRV(), ImVec2(130, 130));
	}
	else if (isMeshSelect[CYLINDER] == true && isShowCurrentTex == true)
	{
		ImGui::Image(currentImages[selectMeshTexture]->SRV(), ImVec2(130, 130));
	}
	if (comboTexture >= 0 && isShowCurrentTex == false)
	{
		ImGui::Image(currentImages[comboTexture]->SRV(), ImVec2(130, 130));
	}
}

void MeshRender::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	Transform* saveTransform;
	D3DXVECTOR3 savePos;
	D3DXVECTOR3 saveScale;
	D3DXVECTOR3 saveRotation;
	D3DXCOLOR saveColor;

	int TextureType = 0;

	w->Color3f(specular);
	w->Float(specular.a);
	
	for (InstanceMesh* mesh : meshes)
	{
		saveIndex = mesh->GetDrawCount();
		w->Int(saveIndex); //각 메쉬 카운트 저장..
	}

	for (InstanceMesh* mesh : meshes)
	{
		for (UINT i = 0; i < mesh->GetDrawCount(); i++)
		{
			//몇번 텍스쳐를 쓸지 가져옴
			TextureType = mesh->GetTexture(i);
			//텍스쳐 저장
			w->Int(TextureType);
			//트랜스폼 위치 가져오기
			saveTransform = mesh->GetTransform(i);
			saveTransform->Position(&savePos);
			saveTransform->Scale(&saveScale);
			saveTransform->Rotation(&saveRotation);
			mesh->Colors(&saveColor, i);
			//위치 저장
			w->Vector3(savePos);
			w->Vector3(saveScale);
			w->Vector3(saveRotation);
			w->Color3f(saveColor);
		}
	}
	
	w->Close();
}

void MeshRender::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);

	specular = r->Color3f();
	specular.a = r->Float();

	UINT index[10];
	int i = -1;

	for (int i = 0; i < 10; i++)  //각 메쉬 드로우 카운트 받기
		index[i] = r->Int();

	for (InstanceMesh* mesh : meshes)
	{
		i++;
		for (UINT j = 0; j < index[i]; j++)
		{
			UINT index = mesh->PushTexture(r->Int());
			Transform* trans = mesh->GetTransform(index);

			trans->Position(r->Vector3());
			trans->Scale(r->Vector3());
			trans->Rotation(r->Vector3());
			mesh->Colors(r->Color3f(), index);
		}
	}

	r->Close();
}


void MeshRender::Undo()
{
	changeTexture = -1;
	StackType type = COMMANDMANAGER->GetUndoType();

	if (type == STACK_QUAD)
	{
		meshes[QUAD]->Undo();
		//isMeshSelect[QUAD] = false;
	}
	else if (type == STACK_CUBE)
		meshes[CUBE]->Undo();
	else if (type == STACK_GRID)
		meshes[GRID]->Undo();
	else if (type == STACK_SPHERE)
		meshes[SPHERE]->Undo();
	else if (type == STACK_CYLINDER)
		meshes[CYLINDER]->Undo();

	//meshes[selectMesh]->MeshtoNew();
}

void MeshRender::Redo()
{
	StackType type = COMMANDMANAGER->GetRedoType();

	if (type == STACK_QUAD)
		meshes[QUAD]->Redo();
	else if (type == STACK_CUBE)
		meshes[CUBE]->Redo();
	else if (type == STACK_GRID)
		meshes[GRID]->Redo();
	else if (type == STACK_SPHERE)
		meshes[SPHERE]->Redo();
	else if (type == STACK_CYLINDER)
		meshes[CYLINDER]->Redo();

}

void MeshRender::Excute()
{

}

