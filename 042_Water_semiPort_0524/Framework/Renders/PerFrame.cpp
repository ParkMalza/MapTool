#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * shader)
	: shader(shader)
{
	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	lightBuffer = new ConstantBuffer(&lightDesc, sizeof(LightDesc));
	pointLightBuffer = new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	spotLightBuffer = new ConstantBuffer(&spotLightDesc, sizeof(SpotLightDesc));

	sBuffer = shader->AsConstantBuffer("CB_PerFrame");
	sLightBuffer = shader->AsConstantBuffer("CB_Light");
	sPointLightBuffer = shader->AsConstantBuffer("CB_PointLight");
	sSpotLightBuffer = shader->AsConstantBuffer("CB_SpotLight");

	D3DXMatrixIdentity(&bufferDesc.View);
	D3DXMatrixIdentity(&bufferDesc.ViewInverse);
	D3DXMatrixIdentity(&bufferDesc.Projection);
	D3DXMatrixIdentity(&bufferDesc.ViewProjection);
	D3DXMatrixIdentity(&bufferDesc.OrthoView);
	D3DXMatrixIdentity(&bufferDesc.FrontOrthoViewProjection);
	D3DXMatrixIdentity(&bufferDesc.UpOrthoViewProjection);
	D3DXMatrixIdentity(&bufferDesc.RightOrthoViewProjection);

	lightDesc.Ambient = D3DXCOLOR(0, 0, 0, 1);
	lightDesc.Specular = D3DXCOLOR(1, 1, 1, 1);
	lightDesc.Direction = D3DXVECTOR3(-1, -1, 1);
	lightDesc.Position = D3DXVECTOR3(0, 0, 0);
}

PerFrame::~PerFrame()
{
	SafeDelete(buffer);
	SafeDelete(lightBuffer);
	SafeDelete(pointLightBuffer);
	SafeDelete(spotLightBuffer);
}

void PerFrame::Update()
{
	bufferDesc.Time = Time::Get()->Running();

	lightDesc.Ambient = Context::Get()->LightAmbient();
	lightDesc.Specular = Context::Get()->LightSpecular();
	lightDesc.Position = Context::Get()->LightPosition();
	lightDesc.Direction = Context::Get()->LightDirection();

	pointLightDesc.Count = Context::Get()->PointLights(pointLightDesc.Lights);
	spotLightDesc.Count = Context::Get()->SpotLights(spotLightDesc.Lights);
}

void PerFrame::Render()
{
	bufferDesc.View = Context::Get()->ViewMatrix();
	D3DXMatrixInverse(&bufferDesc.ViewInverse, NULL, &bufferDesc.View);

	bufferDesc.Projection = Context::Get()->ProjectionMatrix();
	bufferDesc.ViewProjection = bufferDesc.View * bufferDesc.Projection;

	buffer->Apply();

	sBuffer->SetConstantBuffer(buffer->Buffer());

	//빛이 없을수도 있으므로 체크해줌
	if (sLightBuffer != NULL)
	{
		lightBuffer->Apply();
		sLightBuffer->SetConstantBuffer(lightBuffer->Buffer());
	}

	pointLightBuffer->Apply();
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());

	spotLightBuffer->Apply();
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());
}
