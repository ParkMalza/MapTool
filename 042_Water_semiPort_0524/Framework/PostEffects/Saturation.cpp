#include "Framework.h"
#include "Saturation.h"

PostEffects::Saturation::Saturation()
	:Render2D(L"033_Saturation.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");

}

PostEffects::Saturation::~Saturation()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Saturation::Update()
{
	Super::Update();

	ImGui::PushButtonRepeat(true);
	{
		ImGui::InputFloat("Saturation", &desc.Saturation, 0.1f);
	}
	ImGui::PopButtonRepeat();
}

void PostEffects::Saturation::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
