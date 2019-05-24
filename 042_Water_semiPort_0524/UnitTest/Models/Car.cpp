#include "stdafx.h"
#include "Car.h"
#include "Environment\Terrain.h"
#include "Viewer\Viewport.h"
#include "Utilities/BinaryFile.h"

Car::Car(Shader* modelShader,  Model * model, ModelRender* modelRendere, Terrain* terrain)
	:modelShader(modelShader), model(model), carRender(modelRendere)
{
	this->terrain = terrain;
//	sCubeSrv = this->modelShader->AsSRV("CubeMap");
	textureCube = NULL;
	rotation = D3DXVECTOR3(0, Math::PI, 0.0f);
	scale = D3DXVECTOR3(1, 1, 1);
	D3DXVECTOR3 changeHigh;
	D3DXVECTOR3 changeLow;
	changeHigh.x = this->model->GetHighVal().x * 0.4f; //-x
	changeHigh.y = this->model->GetHighVal().y * 2.1f; //y
	changeHigh.z = this->model->GetHighVal().z * 1.75f; // -z
	changeLow.x = this->model->GetLowVal().x * 0.3f; //x
	changeLow.y = this->model->GetLowVal().y * 0.0005f; //-y
	changeLow.z = this->model->GetLowVal().z * 2.55f; //z

	box = new InstanceCollisionBox(modelShader, changeHigh, changeLow);
	box->Color(0, 1, 0);
	box->InstancePass(17);

	D3DXMatrixIdentity(&world);

	//speed = 20.0f;
	selectIndex = -1;
	isClick = false;

	saveCar.reserve(100);  //저장 될 공간 미리 할당.. 흠
	saveBox.reserve(100);
}

Car::~Car()
{
	SafeDelete(box);
}

void Car::Update(Terrain * terrain)
{
	carRender->Update();
	box->Update();
	Selected();
	EditCar();
	CheckHeight();
}

void Car::CubeRender()
{
	//carRender->Pass(2);
	//carRender->Render();
}

void Car::PreRender()
{
	//if (textureCube != NULL)
	//{
	//	textureCube->Set(modelShader);

	//	Perspective* perspective = textureCube->GetPerspective();
	//	Context::Get()->SetSubPerspective(perspective);
	//}
	carRender->Pass(10);
	carRender->Render();
	//box->Render();
}

void Car::Render()
{
	//추가
	//if(textureCube != NULL)
	//	sCubeSrv->SetResource(textureCube->SRV());

	carRender->Pass(2);
	carRender->Render();
	box->Render();
}

//void Car::Move()
//{
//	if (Mouse::Get()->Press(0))
//	{
//		if (Keyboard::Get()->Press('W'))
//		{
//			position.z -= cosf(rotation.y) * speed * Time::Delta();
//			position.x -= sinf(rotation.y) * speed * Time::Delta();
//		}
//		if (Keyboard::Get()->Press('S'))
//		{
//			position.z += cosf(rotation.y) * speed * Time::Delta();
//			position.x += sinf(rotation.y) * speed * Time::Delta();
//		}
//		if (Keyboard::Get()->Press('D'))
//		{
//			rotation.y += speed * Time::Delta();
//		}
//		if (Keyboard::Get()->Press('A'))
//		{
//			rotation.y -= speed * Time::Delta();
//		}
//	}
//}

void Car::Selected()
{
	UINT count = box->GetDrawCount();  //없으면 바로 탈출
	if (count == 0)
		return;
	D3DXMATRIX W, V, P;
	D3DXVECTOR3 start, direction;

	D3DXMatrixIdentity(&W);
	V = Context::Get()->ViewMatrix();
	P = Context::Get()->ProjectionMatrix();

	Viewport* vp = Context::Get()->GetViewport();

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR3 n, f;

	mouse.z = 0.0f;
	Context::Get()->GetViewport()->Unproject(&n, mouse, W, V, P);

	mouse.z = 1.0f;
	Context::Get()->GetViewport()->Unproject(&f, mouse, W, V, P);

	direction = f - n;

	start = n;

	//임구이 조절시 isClick 해제 안되면서 화면상 다른걸 누르면 해당 픽킹 해제
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (box->Selected(start, direction, i))
				{
					isClick = true;
					selectIndex = i;
					break;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isClick = true;
					else
						isClick = false;
				}
			}
		}
	}
}

void Car::EditCar()
{
	if (isClick == true)
	{
		ImGui::Begin("CarEdit");
		//탱크
		editTrans = carRender->GetTransform(selectIndex); //0
		boxEditTrans = box->GetTransform(selectIndex);

		editTrans->Position(&position);
		editTrans->Scale(&scale);
		editTrans->Rotation(&rotation);

		ImGui::SliderFloat3("position", (float*)&position, 0.2f, 254);
		ImGui::SliderFloat3("scale", (float*)&scale, 1, 20);
		ImGui::SliderFloat3("rotation", (float*)&rotation, 0, Math::PI);

		editTrans->Position(position);
		editTrans->Scale(scale);
		editTrans->Rotation(rotation);
		//박스
		boxEditTrans->Position(position);
		boxEditTrans->Scale(scale);
		boxEditTrans->Rotation(rotation);

		carRender->UpdateTransform();
		//삭제
		if (Keyboard::Get()->Down(VK_DELETE))
		{
			//editTrans->Position(0, 0, 0);
			//editTrans->Scale(0, 0, 0);
			//editTrans->Rotation(0, 0, 0);

			//boxEditTrans->Position(0, 0, 0);
			//boxEditTrans->Scale(0, 0, 0);
			//boxEditTrans->Rotation(0, 0, 0);

			carRender->SaveDeleteIndex(selectIndex);
			box->DeleteCollboxIndex(selectIndex);

			carRender->UpdateTransform();
			isClick = false;
		}

		ImGui::End();
	}
}

void Car::InstanceCar(D3DXVECTOR3& picked)
{
	UINT index = carRender->AddTransform();
	Transform* transform = carRender->GetTransform(index);
	//탱크 정보
	position = picked;
	transform->Position(position);
	scale = D3DXVECTOR3(0.5, 0.5, 0.5);
	rotation = D3DXVECTOR3(0, Math::PI, 0);
	transform->Scale(scale);
	transform->Rotation(rotation);

	carRender->UpdateTransform();
	//박스 정보
	UINT boxIndex = box->Push(); 
	Transform* boxPos = box->GetTransform(boxIndex);
	boxPos->Position(position);
	boxPos->Scale(scale);
	boxPos->Rotation(rotation);

	//textureCube = new TextureCube(position, 512, 512);
}

void Car::CheckHeight()
{
	for (UINT i = 0; i < carRender->GetDrawCount(); i++)
	{
		editTrans = carRender->GetTransform(i);
		boxEditTrans = box->GetTransform(i);
		Vector3 tempPos;
		Vector3 boxpos;
		editTrans->Position(&tempPos);
		boxEditTrans->Position(&boxpos);
		float y = terrain->GetPickedHeight(tempPos);
		tempPos.y = y;
		boxpos = tempPos;
		editTrans->Position(tempPos);
		boxEditTrans->Position(boxpos);
		carRender->UpdateTransform();
	}
}

void Car::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);
	w->Int(carRender->GetDrawCount());
	w->Int(box->GetDrawCount());

	D3DXVECTOR3 saveTankPos;
	D3DXVECTOR3 saveTankScale;
	D3DXVECTOR3 saveTankRotation;
	for (UINT i = 0; i < carRender->GetDrawCount(); i++)
	{
		//탱크
		editTrans = carRender->GetTransform(i);
		editTrans->Position(&saveTankPos);
		editTrans->Scale(&saveTankScale);
		editTrans->Rotation(&saveTankRotation);
		w->Vector3(saveTankPos);
		w->Vector3(saveTankScale);
		w->Vector3(saveTankRotation);
	}
	for (UINT i = 0; i < box->GetDrawCount(); i++)
	{
		//박스
		boxEditTrans = box->GetTransform(i);
		boxEditTrans->Position(&saveTankPos);
		boxEditTrans->Scale(&saveTankScale);
		boxEditTrans->Rotation(&saveTankRotation);
		w->Vector3(saveTankPos);
		w->Vector3(saveTankScale);
		w->Vector3(saveTankRotation);
	}

	w->Close();
}

void Car::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);

	loadCarCount = r->Int();
	loadBoxCount = r->Int();
	//탱크
	for (int i = 0; i < loadCarCount; i++)
	{
		UINT index = carRender->AddTransform();
		Transform* transform = carRender->GetTransform(index);
		//위치
		position = r->Vector3();
		transform->Position(position);
		saveCar.push_back(position);
		//크기
		scale = r->Vector3();
		transform->Scale(scale);
		saveCar.push_back(scale);
		//회전
		rotation = r->Vector3();
		transform->Rotation(rotation);
		saveCar.push_back(rotation);

		carRender->UpdateTransform();
	}
	for (int i = 0; i < loadBoxCount; i++)
	{
		UINT boxIndex = box->Push();
		Transform* boxPos = box->GetTransform(boxIndex);
		//위치
		position = r->Vector3();
		boxPos->Position(position);
		saveBox.push_back(position);
		//크기
		scale = r->Vector3();
		boxPos->Scale(scale);
		saveBox.push_back(scale);
		//회전
		rotation = r->Vector3();
		boxPos->Rotation(rotation);
		saveBox.push_back(rotation);
	}
	r->Close();
}