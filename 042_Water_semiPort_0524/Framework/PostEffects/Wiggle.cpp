#include "Framework.h"
#include "Wiggle.h"

PostEffects::Wiggle::Wiggle()
	:Render2D(L"033_Wiggle.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");

}

PostEffects::Wiggle::~Wiggle()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Wiggle::Update()
{
	Super::Update();
	ImGui::SliderFloat2("Offset", (float*)desc.Offset, 0.1f, 1.0f);
	ImGui::SliderFloat2("Amount", (float*)desc.Amount, 0.1f, 100.0f);
}

void PostEffects::Wiggle::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
