#include "stdafx.h"
#include "Kachujin.h"
#include "Environment/Terrain.h"

Kachujin::Kachujin(Shader * modelShader, Model * model, ModelAnimator * modelRender, Terrain* terrain)
	:modelShader(modelShader), model(model), kachujinRender(modelRender)
{
	this->terrain = terrain;
	rotation = Vector3(0, 0, 0);
	scale = Vector3(0.1f, 0.1f, 0.1f);

	changeMaxHigh.x = model->GetHighVal().x * 0.4f; //-x
	changeMaxHigh.y = model->GetHighVal().y * 0.95f;
	changeMaxHigh.z = model->GetHighVal().z;

	changeMaxLow.x = model->GetLowVal().x * 0.4f;
	changeMaxLow.y = model->GetLowVal().y;
	changeMaxLow.z = model->GetLowVal().z;

	float a = changeMaxHigh.x - changeMaxLow.x; //38.7, -38.7
	float v = changeMaxHigh.z - changeMaxLow.z;// 21.7, -17.6

	box = new InstanceCollisionBox(modelShader, changeMaxHigh, changeMaxLow);
	box->Color(0, 1, 0);
	box->InstancePass(17);

	speed = 20.0f;

	selectIndex = -1;
	isClick = false;

	for (int i = 0; i < 100; i++)
	{
		pattern[i] = 0;
		collTerm[i] = 0;
	}
	saveKachujin.reserve(100);
	saveBox.reserve(100);
}

Kachujin::~Kachujin()
{
	SafeDelete(box);
}

void Kachujin::Update(Terrain * terrain)
{
	kachujinRender->Update();
	//Move();
	//CheckCollision();
	box->Update();
	Selected();
	EditKachujin();
	CheckHeight();
}

void Kachujin::PreRender()
{
	kachujinRender->Pass(11);
	kachujinRender->Render();
}

void Kachujin::CubeRender()
{
	kachujinRender->Pass(1);
	kachujinRender->Render();
}

void Kachujin::Render()
{
	kachujinRender->Pass(15);
	kachujinRender->Render();
	box->Render();
}

void Kachujin::Move()
{
	for (UINT i = 0; i < kachujinMoves.size(); i++)
	{
		collTerm[i]++;
		editTrans = kachujinRender->GetTransform(i);
		boxEditTrans = box->GetTransform(i);

		Vector3 tempPos;

		editTrans->Position(&tempPos);
		editTrans->Scale(&scale);
		editTrans->Rotation(&rotation);

		if (tempPos.x >= 249)
			rotation.y = -rotation.y;
		else if (tempPos.x <= 1)
			rotation.y = -rotation.y;
		else if (tempPos.z >= 249)
		{
			if (rotation.y > 0)
				rotation.y = Math::PI - rotation.y;
			else
				rotation.y = -(Math::PI + rotation.y);
		}
		else if (tempPos.z <= 1)
		{
			if (rotation.y > 0)
				rotation.y = Math::PI - rotation.y;
			else
				rotation.y = -(Math::PI + rotation.y);
		}

		Clip = i;
		Clip = kachujinRender->Clip(Clip);
		switch (Clip)
		{
			case 0:
				speed = 0;
				break;
			case 1:
				speed = 15;
				break;
			case 2:
				speed = 30;
				break;
			case 3:
				speed = 0;
				break;
			case 4:
				break;
				speed = 0;
			default:
				break;
		}

		if (kachujinRender->Clip(i) == 0)
		{
			pattern[i]++;
			if (pattern[i] > 30)
			{
				Vector3 temp = Math::RandomVec3(0, 358.0f);
				rotation.y = temp.y; // 어떻게 값을 줘야 지들끼리 계속 안겹칠수 있지..
				editTrans->Rotation(rotation);
				boxEditTrans->Rotation(rotation);
				kachujinRender->PlayNextClip(i, 1);
				pattern[i] = 0;
			}
		}
		else if (kachujinRender->Clip(i) == 3)
		{
			pattern[i]++;
			if (pattern[i] > 300)
			{
				Vector3 temp = Math::RandomVec3(0, 358.0f);
				rotation.y = temp.y; // 어떻게 값을 줘야 지들끼리 계속 안겹칠수 있지..
				//그냥 충돌 사이사이에 충돌 쿨타임? 줘서 막아버렸음..
				editTrans->Rotation(rotation);
				boxEditTrans->Rotation(rotation);
				kachujinRender->PlayNextClip(i, 1);
				pattern[i] = 0;
			}
		}
		//
		tempPos.z -= cosf(rotation.y) * speed * Time::Delta();
		tempPos.x -= sinf(rotation.y) * speed * Time::Delta();
		//

		editTrans->Position(tempPos);
		editTrans->Scale(scale);
		editTrans->Rotation(rotation);

		boxEditTrans->Position(tempPos);
		boxEditTrans->Scale(scale);
		boxEditTrans->Rotation(rotation);

		kachujinRender->UpdateTransform();

		if (Keyboard::Get()->Down(VK_SPACE))
		{
			int ran = Math::RandomAtoB(0, kachujinMoves.size());
			kachujinRender->PlayNextClip(ran, 2);
			kachujinRender->UpdateTransform();
			break;
		}

	}
}

void Kachujin::CheckCollision()
{
	for (UINT i = 0; i < kachujinMoves.size(); i++)
	{
		for (UINT j = i + 1; j < kachujinMoves.size(); j++)
		{
			Vector3 tempPos, nextTempPos;
			Transform* tempTrans = kachujinRender->GetTransform(i);
			Transform* nextTempTrans = kachujinRender->GetTransform(j);
			tempTrans->Position(&tempPos);
			nextTempTrans->Position(&nextTempPos);

			if (tempPos.x + 5 > nextTempPos.x - 5 &&
				tempPos.x - 5 < nextTempPos.x + 5 &&
				tempPos.z + 5 > nextTempPos.z - 5 &&
				tempPos.z - 5 < nextTempPos.z + 5 && collTerm[i] > 100)
			{
				if (kachujinRender->Clip(i) == 1) //걷고 있을때 대기로 전환
					kachujinRender->PlayNextClip(i, 0);
				else if (kachujinRender->Clip(i) == 2) //뛰고 있다가 도발
					kachujinRender->PlayNextClip(i, 3);

				if (kachujinRender->Clip(j) == 1) //걷고 있을때 대기로 전환
					kachujinRender->PlayNextClip(j, 0);
				else if (kachujinRender->Clip(j) == 2) //뛰고 있다가 도발
					kachujinRender->PlayNextClip(j, 3);

				collTerm[i] = 0;
			}
		}
	}
}

void Kachujin::InstanceKachujin(D3DXVECTOR3 & picked)
{
	//카추진 정보
	UINT index = kachujinRender->AddTransform();
	Transform* transform = kachujinRender->GetTransform(index);
	Vector3 temp = Math::RandomVec3(0, 359.5f);
	position = picked;
	transform->Position(position);
	transform->Scale(scale);
	transform->Rotation(rotation.x, temp.y, rotation.z);

	kachujinMoves.push_back(transform);
	kachujinRender->PlayNextClip(index, 1);
	kachujinRender->UpdateTransform();

	//충돌 박스 정보
	UINT boxIndex = box->PushTexture(0);
	Transform* boxPos = box->GetTransform(boxIndex);

	boxPos->Position(position);
	boxPos->Scale(scale);
	boxPos->Rotation(rotation.x, temp.y, rotation.z);
}

void Kachujin::CheckHeight()
{
	for (UINT i = 0; i < kachujinRender->GetDrawCount(); i++)
	{
		editTrans = kachujinRender->GetTransform(i);
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
		kachujinRender->UpdateTransform();
	}
}

void Kachujin::EditKachujin()
{
	if (isClick == true)
	{
		ImGui::Begin("KachujinEdit");
		//카추진
		editTrans = kachujinRender->GetTransform(selectIndex);
		//박스
		boxEditTrans = box->GetTransform(selectIndex);

		editTrans->Position(&position);
		editTrans->Scale(&scale);
		editTrans->Rotation(&rotation);

		ImGui::SliderFloat3("position", (float*)&position, 0.2f, 254);
		ImGui::SliderFloat3("scale", (float*)&scale, 0.1f, 1.0f);
		ImGui::SliderFloat3("rotation", (float*)&rotation, 0, Math::PI);

		editTrans->Position(position);
		editTrans->Scale(scale);
		editTrans->Rotation(rotation);

		boxEditTrans->Position(position);
		boxEditTrans->Scale(scale);
		boxEditTrans->Rotation(rotation);

		kachujinRender->UpdateTransform();

		////삭제
		//if (Keyboard::Get()->Down(VK_DELETE))
		//{
		//	editTrans->Position(0, 0, 0);
		//	editTrans->Scale(0, 0, 0);
		//	editTrans->Rotation(0, 0, 0);

		//	boxEditTrans->Position(0, 0, 0);
		//	boxEditTrans->Scale(0, 0, 0);
		//	boxEditTrans->Rotation(0, 0, 0);

		//	kachujinRender->SaveDeleteIndex(selectIndex);
		//	box->SaveDeleteIndex(selectIndex);

		//	tankRender->UpdateTransform();
		//	isClick = false;
		//}
		ImGui::End();
	}
}

void Kachujin::Selected()
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

void Kachujin::Save(wstring fileName)
{
}

void Kachujin::Load(wstring fileName)
{
}
