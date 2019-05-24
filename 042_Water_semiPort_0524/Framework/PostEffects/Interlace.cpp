#include "Framework.h"
#include "Interlace.h"

PostEffects::Interlace::Interlace()
	:Render2D(L"033_Interlace.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");

	desc.MapSize = Vector2((float)D3D::GetDesc().Width, (float)D3D::GetDesc().Height);
}

PostEffects::Interlace::~Interlace()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Interlace::Update()
{
	Super::Update();

	ImGui::InputFloat("Interlace", &desc.Interlace, 0.1f);
	ImGui::InputInt("Line", (int *)&desc.Line);
	desc.Line = desc.Line < 2 ? 2 : desc.Line;  //2보다 작으면 안된다!
}

void PostEffects::Interlace::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
