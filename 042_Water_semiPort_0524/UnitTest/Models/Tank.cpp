#include "stdafx.h"
#include "Tank.h"
#include "Environment\Terrain.h"
#include "Viewer\Viewport.h"
#include "Utilities/BinaryFile.h"

Tank::Tank(Shader* modelShader, Model * model, ModelRender* modelRendere, Terrain* terrain)
	:modelShader(modelShader), model(model), tankRender(modelRendere)
{
	this->terrain = terrain;
	rotation = D3DXVECTOR3(0, Math::PI, 0.0f);
	scale = D3DXVECTOR3(1, 1, 1);

	box = new InstanceCollisionBox(modelShader, this->model->GetHighVal()* 0.015f, this->model->GetLowVal() * 0.015f);
	box->Color(0, 1, 0);
	box->InstancePass(17);

	//speed = 20.0f;
	selectIndex = -1;
	isClick = false;

	saveTank.reserve(100);  //저장 될 공간 미리 할당.. 흠
	saveBox.reserve(100);
}

Tank::~Tank()
{
	SafeDelete(box);
}

void Tank::Update()
{
	tankRender->Update();
	box->Update();
	Selected();
	EditTank();
	CheckHeight();
}

void Tank::PreRender()
{
	tankRender->Pass(10);
	tankRender->Render();
}

void Tank::Render()
{
	//추가
	tankRender->Pass(14);
	tankRender->Render();
	box->Render();
}

void Tank::CubeRender()
{
	tankRender->Pass(0);
	tankRender->Render();
}

//void Tank::Move()
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

void Tank::Selected()
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

void Tank::CheckHeight()
{
	for (UINT i = 0; i < tankRender->GetDrawCount(); i++)
	{
		editTrans = tankRender->GetTransform(i);
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
		tankRender->UpdateTransform();
	}
}

void Tank::EditTank()
{
	if (isClick == true)
	{
		ImGui::Begin("TankEdit");
		//탱크
		editTrans = tankRender->GetTransform(selectIndex); //0
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

		tankRender->UpdateTransform();
		//삭제
		if (Keyboard::Get()->Down(VK_DELETE))
		{
			editTrans->Position(0, 0, 0);
			editTrans->Scale(0, 0, 0);
			editTrans->Rotation(0, 0, 0);

			boxEditTrans->Position(0, 0, 0);
			boxEditTrans->Scale(0, 0, 0);
			boxEditTrans->Rotation(0, 0, 0);

			tankRender->SaveDeleteIndex(selectIndex);
			box->SaveDeleteIndex(selectIndex);

			tankRender->UpdateTransform();
			isClick = false;
		}
	
		ImGui::End();
	}
}

void Tank::InstanceTank(D3DXVECTOR3& picked)
{
	UINT index = tankRender->AddTransform();
	Transform* transform = tankRender->GetTransform(index);
	//탱크 정보
	position = picked;
	transform->Position(position);
	scale = D3DXVECTOR3(1, 1, 1);
	rotation = D3DXVECTOR3(0, Math::PI, 0);
	transform->Scale(scale);
	transform->Rotation(rotation);

	tankRender->UpdateTransform();
	//박스 정보
	UINT boxIndex = box->PushTexture(0);  //0번그냥 넣어줌으로써 텍스쳐는 노상관
	Transform* boxPos = box->GetTransform(boxIndex);

	boxPos->Position(position);
	boxPos->Scale(scale);
	boxPos->Rotation(rotation);

}

void Tank::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);
	w->Int(tankRender->GetDrawCount());
	w->Int(box->GetDrawCount());

	D3DXVECTOR3 saveTankPos;
	D3DXVECTOR3 saveTankScale;
	D3DXVECTOR3 saveTankRotation;
	for (UINT i = 0; i < tankRender->GetDrawCount(); i++)
	{
		//탱크
		editTrans = tankRender->GetTransform(i);
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

void Tank::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);

	loadTankCount = r->Int();
	loadBoxCount = r->Int();
	//탱크
	for (int i = 0; i < loadTankCount; i++)
	{
		UINT index = tankRender->AddTransform();
		Transform* transform = tankRender->GetTransform(index);
		//위치
		position = r->Vector3();
		transform->Position(position);
		saveTank.push_back(position);
		//크기
		scale = r->Vector3();
		transform->Scale(scale);
		saveTank.push_back(scale);
		//회전
		rotation = r->Vector3();
		transform->Rotation(rotation);
		saveTank.push_back(rotation);

		tankRender->UpdateTransform();
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