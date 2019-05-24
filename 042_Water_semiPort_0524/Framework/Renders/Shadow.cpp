#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(Shader * shader, Vector3 & position, float radius, UINT width, UINT height)
	: shader(shader), position(position), radius(radius), width(width), height(height)
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Shadow");

	renderTarget = new RenderTarget(width, height); //색상 정보
	depthStencil = new DepthStencil(width, height, true); // 깊이 정보
	viewport = new Viewport((float)width, (float)height);

	desc.MapSize = Vector2((float)width, (float)height);

	shader->AsSRV("ShadowMap")->SetResource(depthStencil->SRV()); //깊이 정보를 넣어줘야 한다.

	//Create SamplerState
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		//desc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC; //비등방성
		desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;  //<= depth
		desc.MaxAnisotropy = 1;
		//desc.MaxAnisotropy = 8;
		desc.MinLOD = 0;
		desc.MaxLOD = FLT_MAX;

		D3D::GetDevice()->CreateSamplerState(&desc, &samplerState);

		shader->AsSampler("ShadowSampler")->SetSampler(0, samplerState);
	}
}

Shadow::~Shadow()
{
	SafeDelete(buffer);

	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);
}

void Shadow::Set()
{
	//ImGui::InputInt("Index", (int*)&desc.index);
	//desc.index %= 3;

	//ImGui::InputFloat("Bias", &desc.Bias, 0.0001f, 1.0f, "%0.4f");

	UpdateVolume();

	renderTarget->Set(depthStencil->DSV());
	viewport->RSSetViewport();

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}

void Shadow::UpdateVolume()
{
	Vector3 up = Vector3(0, 1, 0);
	Vector3 direction = Context::Get()->LightDirection();
	Vector3 position = -2.0f * radius * direction;

	D3DXMatrixLookAtLH(&desc.View, &position, &this->position, &up);


	Vector3 cube;
	D3DXVec3TransformCoord(&cube, &this->position, &desc.View);

	float l = cube.x - radius;
	float b = cube.y - radius;
	float n = cube.z - radius;

	float r = cube.x + radius;
	float t = cube.y + radius;
	float f = cube.z + radius;

	D3DXMatrixOrthoLH(&desc.Projection, r - l, t - b, n, f);
}
