#include "Framework.h"
#include "RadialBlur.h"

PostEffects::RadialBlur::RadialBlur()
	:Render2D(L"033_RadialBlur.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");

	desc.MapSize = Vector2(D3D::GetDesc().Width, D3D::GetDesc().Height);
}

PostEffects::RadialBlur::~RadialBlur()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::RadialBlur::Update()
{
	Super::Update();

	ImGui::PushButtonRepeat(true);
	{
		ImGui::InputInt("RadialBlur", (int*)&desc.BlurCount);
		desc.BlurCount = (desc.BlurCount < 1) ? 1 : desc.BlurCount;
		desc.BlurCount = (desc.BlurCount >= MAX_RADIAL_BLUR_COUNT) ? MAX_RADIAL_BLUR_COUNT : desc.BlurCount;

		ImGui::InputFloat("Radius", &desc.Radius, 0.1f);
		ImGui::InputFloat("Amount", &desc.Amount, 0.01f);
		ImGui::InputFloat("addError", &desc.addError, 0.01f);
	}
	ImGui::PopButtonRepeat();
}

void PostEffects::RadialBlur::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
