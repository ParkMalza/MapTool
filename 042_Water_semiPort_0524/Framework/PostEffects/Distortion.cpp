#include "Framework.h"
#include "Distortion.h"

PostEffects::Distortion::Distortion()
	:Render2D(L"033_Distortion.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");

}

PostEffects::Distortion::~Distortion()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Distortion::Update()
{
	Super::Update();

	ImGui::PushButtonRepeat(true);
	{
		ImGui::InputInt("pass", (int*)&pass);
		pass %= 3;

		ImGui::InputFloat("Amount", &desc.Noise, 0.1f);
		ImGui::InputFloat("Test", &desc.test, 0.001f);
	}
	ImGui::PopButtonRepeat();
}

void PostEffects::Distortion::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
