#include "Framework.h"
#include "Sepia.h"

PostEffects::Sepia::Sepia()
	:Render2D(L"033_Sepia.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");

	desc.Sepia = Matrix
	(
		0.393f, 0.769f, 0.189f, 0,
		0.349f, 0.686f, 0.168f, 0,
		0.272f, 0.534f, 0.131f, 0,
		0, 0, 0, 1
	);  //¿­¿ì¼±
	D3DXMatrixTranspose(&desc.Sepia, &desc.Sepia);
}

PostEffects::Sepia::~Sepia()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Sepia::Update()
{
	Super::Update();

	ImGui::PushButtonRepeat(true);
	{
		ImGui::InputInt("pass", (int*)&pass);
		pass %= 2;

		ImGui::InputFloat("Amount", &desc.Strength, 0.1f);
	}
	ImGui::PopButtonRepeat();
}

void PostEffects::Sepia::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}
