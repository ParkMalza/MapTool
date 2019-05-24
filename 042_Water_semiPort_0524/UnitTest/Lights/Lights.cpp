#include "stdafx.h"
#include "Lights.h"
#include "Meshes/InstanceMesh.h"
#include "EnumGroup.h"
#include "Utilities/BinaryFile.h"

Lights::Lights(Shader* shader)
{
	for (int i = 0; i < 2; i++)
	{
		box[i] = new InstanceCollisionBox(shader, 2.0f, 2.0f, 2.0f);
		box[i]->InstancePass(8);
		box[i]->Color(0, 1, 0);
		boxes.push_back(box[i]);
	}
	maxCount = 32;
	for (int i = 0; i < LIGHT_TYPE_NUM; i++)
		isLight[i] = false;
	for (int i = 0; i < LIGHT_TYPE_NUM; i++)
		isLightSelected[i] = false;
	selectPoint = -1;
	selectSpot = -1;
	saveDrawCount = 0;
}

Lights::~Lights()
{
	for (InstanceMesh* box : box)
		SafeDelete(box);
}

void Lights::Update(Vector3& picked)
{
	AddPointLights(picked);
	AddSpotLights(picked);
	for (InstanceMesh* box : boxes)
		box->Update();
	SelectPointBox();
	SelectSpotBox();
	EditPointBox();
	EditSpotBox();
}

void Lights::Render()
{
	for (InstanceMesh* box : boxes)
		box->Render();
}

void Lights::SetOldPoint()
{
}

void Lights::AddPointLights(Vector3& picked)
{
	//if (ImGui::RadioButton("AddPointLight", active))
	//	isLight[POINTLIGHT] = true;
	//ImGui::RadioButton("AddSpotLight", isLight[SPOTLIGHT]);
	ImGui::Checkbox("AddPointLight", &isLight[POINTLIGHT]);
	ImGui::SameLine();
	ImGui::Checkbox("AddSpotLight", &isLight[SPOTLIGHT]);
	//(isLight[POINTLIGHT] == true) ? isLight[SPOTLIGHT] = false : true;
	if (isLight[POINTLIGHT] == true)
	{
		if (Mouse::Get()->Down(0) && picked.x > -1 && picked.z > -1)
		{
			UINT currentCount = Context::Get()->GetPointLightCount();
			if (currentCount < 32)
			{
				PointLight light = { Color(1, 0, 0, 1), Vector3(picked.x, 15.0f, picked.z), 20, 1.0f };
				Context::Get()->AddPointLight(light);
				BoxInstancing(POINTLIGHT, picked);
				//isLight[POINTLIGHT] = false;
			}
		}
	}
}

void Lights::AddSpotLights(Vector3 & picked)
{
	if (isLight[SPOTLIGHT] == true)
	{
		if (Mouse::Get()->Down(0) && picked.x > -1 && picked.z > -1)
		{
			UINT currentCount = Context::Get()->GetSpotLightCount();
			if (currentCount < 32)
			{
				SpotLight light = { Color(0, 0, 1, 0), Vector3(picked.x, 15.0f, picked.z), 30.0f, Vector3(0, -1, 0), 0.3f, 1.0f };
				Context::Get()->AddSpotLight(light);
				//BoxInstancing(SPOTLIGHT, picked);
				boxes[SPOTLIGHT]->MakeInstance(picked, 0);
				//isLight[SPOTLIGHT] = false;
			}
		}
	}
}

void Lights::BoxInstancing(UINT lightType, Vector3& picked)
{
	boxIndex = boxes[lightType]->PushTexture(0);
	boxTrans = boxes[lightType]->GetTransform(boxIndex);
	boxes[lightType]->Position(&boxPos);
	boxes[lightType]->Scale(&boxScale);
	boxes[lightType]->Rotation(&boxRotation);
	boxPos.x = picked.x;
	boxPos.y = 15.0f;
	boxPos.z = picked.z;
	boxTrans->Position(boxPos);
	boxTrans->Scale(boxScale);
	boxTrans->Rotation(boxRotation);
}

void Lights::SelectPointBox()
{
	UINT count = boxes[POINTLIGHT]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (boxes[POINTLIGHT]->Selected(start, direction, i))
				{
					isLightSelected[POINTLIGHT] = true;
					selectPoint = i;
					return;
				}
				else
				{
					
					if (ImGui::GetIO().WantCaptureMouse == true)
						isLightSelected[POINTLIGHT] = true;
					else
						isLightSelected[POINTLIGHT] = false;
				}
			}
		}
	}
}

void Lights::SelectSpotBox()
{
	UINT count = boxes[SPOTLIGHT]->GetDrawCount();
	if (count == 0)
		return;
	UnProject();
	if (ImGui::GetIO().WantCaptureMouse == false)
	{
		if (Mouse::Get()->Down(0))
		{
			for (UINT i = 0; i < count; i++)
			{
				if (boxes[SPOTLIGHT]->Selected(start, direction, i))
				{
					isLightSelected[SPOTLIGHT] = true;
					selectSpot = i;
					return;
				}
				else
				{
					if (ImGui::GetIO().WantCaptureMouse == true)
						isLightSelected[SPOTLIGHT] = true;
					else
						isLightSelected[SPOTLIGHT] = false;
				}
			}
		}
	}
}


void Lights::EditPointBox()
{
	if (isLightSelected[POINTLIGHT] == true)
	{
		ImGui::Begin("PointLight");

		boxTrans = boxes[POINTLIGHT]->GetTransform(selectPoint);
		boxTrans->Position(&boxPos);

		PointLight& light = Context::Get()->GetPointLight(selectPoint);
		ImGui::SliderFloat3("PointLight Position", (float *)&light.Position, 1, 254);
		ImGui::SliderFloat("PointLight Range", &light.Range, 1, 30);
		ImGui::SliderFloat("PointLight Intensity", &light.Intensity, 0.1f, 1.0f - 1e-6f);
		ImGui::ColorEdit4("PointLight Color", (float*)&light.Color);
		boxPos = light.Position;
		boxTrans->Position(boxPos);

		DeletePoint(POINTLIGHT, selectPoint);

		ImGui::End();
	}
}


void Lights::EditSpotBox()
{
	if (isLightSelected[SPOTLIGHT] == true)
	{
		ImGui::Begin("SpotLight");

		boxTrans = boxes[SPOTLIGHT]->GetTransform(selectSpot);

		boxTrans->Position(&boxPos);

		SpotLight& light = Context::Get()->GetSpotLight(selectSpot);
		ImGui::SliderFloat3("SpotLight Position", (float *)&light.Position, 1,254);
		ImGui::SliderFloat3("SpotLight Direction", (float *)&light.Direction, -1, 1);
		ImGui::SliderFloat("SpotLight Range", &light.Range, 1, 30);
		ImGui::SliderFloat("SpotLight Angle", &light.Angle, 0.1f, 30);
		ImGui::SliderFloat("SpotLight Intensity", &light.Intensity, 0.1f, 1.0f - 1e-6f);
		ImGui::ColorEdit4("SpotLight Color", (float*)&light.Color);

		boxPos = light.Position;
		boxTrans->Position(boxPos);

		DeleteSpot(SPOTLIGHT, selectSpot);

		ImGui::End();
	}
}

void Lights::DeleteSpot(UINT lightType, UINT selectNum)
{
	if (Keyboard::Get()->Down(VK_DELETE))
	{
		boxTrans->Position(0, 0, 0);
		boxTrans->Scale(0, 0, 0);
		boxTrans->Rotation(0, 0, 0);
		Context::Get()->DeleteSpotLight(selectNum);
		boxes[lightType]->SaveDeleteIndex(selectNum);
		isLightSelected[lightType] = false;
	}
}

void Lights::DeletePoint(UINT lightType, UINT selectNum)
{
	if (Keyboard::Get()->Down(VK_DELETE))
	{
		boxTrans->Position(0, 0, 0);
		boxTrans->Scale(0, 0, 0);
		boxTrans->Rotation(0, 0, 0);
		Context::Get()->DeletePointLight(selectNum);
		boxes[lightType]->SaveDeleteIndex(selectNum);
		isLightSelected[lightType] = false;
	}
}

void Lights::UnProject()
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

void Lights::Undo()
{
}

void Lights::Redo()
{
}

void Lights::Excute()
{
}

void Lights::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	Transform* saveTrans;
	D3DXVECTOR3 savePos;
	D3DXVECTOR3 saveScale;
	D3DXVECTOR3 saveRotation;

	//pointLight
	saveDrawCount = boxes[POINTLIGHT]->GetDrawCount();
	w->Int(saveDrawCount); //포인트 라이드 박스 개수 저장

	for (int i = 0; i < saveDrawCount; i++)
	{
		saveTrans = boxes[POINTLIGHT]->GetTransform(i);
		saveTrans->Position(&savePos);
		saveTrans->Scale(&saveScale);
		saveTrans->Rotation(&saveRotation);
		//위치 저장
		w->Vector3(savePos);
		w->Vector3(saveScale);
		w->Vector3(saveRotation);
		//빛의 정보
		w->Color4f(Context::Get()->GetPointLight(i).Color);
		w->Vector3(Context::Get()->GetPointLight(i).Position);
		w->Float(Context::Get()->GetPointLight(i).Range);
		w->Float(Context::Get()->GetPointLight(i).Intensity);
	}

	//spotLight
	saveDrawCount = boxes[SPOTLIGHT]->GetDrawCount();
	w->Int(saveDrawCount);

	for (int i = 0; i < saveDrawCount; i++)
	{
		saveTrans = boxes[SPOTLIGHT]->GetTransform(i);
		saveTrans->Position(&savePos);
		saveTrans->Scale(&saveScale);
		saveTrans->Rotation(&saveRotation);
		//위치 저장
		w->Vector3(savePos);
		w->Vector3(saveScale);
		w->Vector3(saveRotation);
		//빛의 정보
		w->Color4f(Context::Get()->GetSpotLight(i).Color);
		w->Vector3(Context::Get()->GetSpotLight(i).Position);
		w->Float(Context::Get()->GetSpotLight(i).Range);
		w->Float(Context::Get()->GetSpotLight(i).Angle);
		w->Float(Context::Get()->GetSpotLight(i).Intensity);
	}
	w->Close();
}

void Lights::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);

	//pointLight
	saveDrawCount = r->Int();
	for (int i = 0; i < saveDrawCount; i++)
	{
		//충돌 박스
		boxIndex = boxes[POINTLIGHT]->PushTexture(0);
		boxTrans = boxes[POINTLIGHT]->GetTransform(boxIndex);

		boxTrans->Position(r->Vector3());
		boxTrans->Scale(r->Vector3());
		boxTrans->Rotation(r->Vector3());

		//point위치
		Color color = r->Color4f();
		Vector3 pos = r->Vector3();
		float tempRange = r->Float();
		float tempIntensity = r->Float();
		PointLight light = { color, pos, tempRange, tempIntensity };
		Context::Get()->AddPointLight(light);
	}

	//spotLight
	saveDrawCount = r->Int();
	for (int i = 0; i < saveDrawCount; i++)
	{
		boxIndex = boxes[SPOTLIGHT]->PushTexture(0);
		boxTrans = boxes[SPOTLIGHT]->GetTransform(boxIndex);

		boxTrans->Position(r->Vector3());
		boxTrans->Scale(r->Vector3());
		boxTrans->Rotation(r->Vector3());

		//spotPoint위치
		Color color = r->Color4f();
		Vector3 pos = r->Vector3();
		float tempRange = r->Float();
		float tempAngle = r->Float();
		float tempIntensity = r->Float();
		SpotLight light = { color, pos, tempRange, Vector3(0, -1, 0), tempAngle, tempIntensity };
		Context::Get()->AddSpotLight(light);
	}

	r->Close();
}
