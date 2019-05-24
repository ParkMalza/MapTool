#include "Framework.h"
#include "Bloom.h"

PostEffects::Bloom::Bloom()
	:Render2D(L"034_Bloom.fx")
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Values");
	sLuminosityMap = shader->AsSRV("LuminosityMap");
}

PostEffects::Bloom::~Bloom()
{
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void PostEffects::Bloom::Update()
{
	Super::Update();
	ImGui::InputInt("BlurCount", (int *)&desc.Count, 2);
	desc.Count %= 33;
	desc.Count = desc.Count < 2 ? 2 : desc.Count;

	ImGui::SliderFloat("Threshold", &desc.Threshold, 0.01f, 1.0f);
	ImGui::SliderFloat("Intensity", &desc.Intensity, 0.1f, 10.0f);

	UpdateWeights();
}

void PostEffects::Bloom::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
	Super::Render();
}

void PostEffects::Bloom::BlurOffset(float x, float y)
{
	//x가 1이 들어오면 (y = 0 일 떄) x * (count * 2.0f + 1.5f) 가 누적되가겠다. 
	desc.Valuse[0].Offset = Vector2(0, 0);
	for (UINT i = 0; i < desc.Count / 2; i++)
	{
		Vector2 offset = Vector2(x, y) * (i * 2.0f + 1.5f); 
		desc.Valuse[i * 2 + 1].Offset = offset;
		desc.Valuse[i * 2 + 2].Offset = -offset; //가운데 기준으로 왼쪽 오른쪽에 넣기위해
	}
}

void PostEffects::Bloom::LuminosityMap(ID3D11ShaderResourceView * srv)
{
	sLuminosityMap->SetResource(srv);
}

void PostEffects::Bloom::UpdateWeights()
{
	desc.Valuse[0].Weight = Math::Gaussian(0, desc.Count);

	float totalWeight = desc.Valuse[0].Weight;
	for (UINT i = 0; i < desc.Count / 2; i++)
	{
		float weight = Math::Gaussian((float)(i + 1), desc.Count);
		desc.Valuse[i * 2 + 1].Weight = weight;
		desc.Valuse[i * 2 + 2].Weight = weight;

		totalWeight += weight * 2.0f;
	}

	for (UINT i = 0; i < desc.Count; i++)
		desc.Valuse[i].Weight /= totalWeight; //평균
}
