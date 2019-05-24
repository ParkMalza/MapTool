#include "Framework.h"
#include "Default.h"

PostEffects::Default::Default()
	:Render2D(L"032_Default.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Default");
	Mode[0] = "Normal";
	Mode[1] = "Inverse";
	Mode[2] = "GrayScale";
	Mode[3] = "Tone";
	Mode[4] = "Gamma";
	Mode[5] = "BitPlannerSlicing";
	Mode[6] = "None";
	Mode[7] = "None";
	Mode[8] = "None";
	Mode[9] = "None";
}

PostEffects::Default::~Default()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Default::Update()
{
	Super::Update();
	ImGui::InputInt(Mode[pass].c_str(), (int*)&pass);
	pass %= 6;

	if(pass == 3)
		ImGui::ColorEdit3("Tone", (float*)&desc.Tone);
	if(pass == 4)
		ImGui::ColorEdit3("Gamma", (float*)&desc.Gamma);
	if(pass == 5)
		ImGui::SliderInt("nBit", (int*)&desc.nBit, 0, 8);
}

void PostEffects::Default::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
