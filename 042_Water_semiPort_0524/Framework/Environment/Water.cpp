#include "Framework.h"
#include "Water.h"
#include "Viewer/Fixity.h"

Water::Water(float height, float radius)
	:Renderer(L"042_Water.fx")
	, height(height), radius(radius)
	, waveSpeed(0.003f)
	,mapScale(1024, 1024)//렌더 타겟 사이즈
{
	desc.NormalMapTile = Vector2(0.01f, 0.02f);
	desc.WaveTranslation = 0.0f;
	desc.WaveScale = 0.03f;
	desc.RefractionColor = Color(0.0f, 0.8f, 1.0f, 1.0f);
	desc.Shininess = 200;

	D3DXMatrixIdentity(&desc.ReflectionMatrix);

	waveMap = new Texture(L"Environment/Wave.dds");

	reflection = new RenderTarget((UINT)mapScale.x, (UINT)mapScale.y);
	refraction = new RenderTarget((UINT)mapScale.x, (UINT)mapScale.y);
	depthStencil = new DepthStencil((UINT)mapScale.x, (UINT)mapScale.y);
	viewport = new Viewport(mapScale.x, mapScale.y);

	sReflectionMap = shader->AsSRV("ReflectionMap");
	sRefractionMap = shader->AsSRV("RefractionMap");

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-radius, 0.0f, -radius);
	vertices[1].Position = Vector3(-radius, 0.0f, +radius);
	vertices[2].Position = Vector3(+radius, 0.0f, -radius);
	vertices[3].Position = Vector3(+radius, 0.0f, +radius);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	UINT indices[6] = { 0,1,2,2,1,3 };

	vertexCount = 4;
	indexCount = 6;

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = new IndexBuffer(indices, indexCount);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_WaterRender");

	sWaveMap = shader->AsSRV("WaveMap");
	sWaveMap->SetResource(waveMap->SRV());

	sClipPlane = shader->AsVector("WaterClipPlane");

	reflectionCamera = new Fixity();
}

Water::~Water()
{
	SafeDelete(waveMap);

	SafeDelete(reflectionCamera);
	SafeDelete(reflection);
	SafeDelete(refraction);
	SafeDelete(depthStencil);
	SafeDelete(viewport);
}

void Water::Update()
{
	Super::Update();
	ImGui::InputFloat("MapTileX", (float*)&desc.NormalMapTile.x, 0.01f); //물의 파장 크기
	ImGui::InputFloat("MapTileY", (float*)&desc.NormalMapTile.y, 0.01f); 
	ImGui::InputFloat("WaveScale", (float*)&desc.WaveScale, 0.01f);  //물 아래 이미지들 굴절정도
	ImGui::InputFloat("Shininess", (float*)&desc.Shininess, 1.0f);
	ImGui::ColorEdit4("RefractionColor", (float*)&desc.RefractionColor, 0.1f);

	desc.WaveTranslation += waveSpeed;
	if (desc.WaveTranslation > 1.0f)
		desc.WaveTranslation -= 1.0f;

	Matrix world;
	GetTransform()->Position(0, height, 0);

	Vector3 position, rotation;
	Context::Get()->GetCamera()->Rotation(&rotation);
	Context::Get()->GetCamera()->Position(&position);

	rotation.x *= -1;
	reflectionCamera->Rotation(rotation);
	position.y = -position.y + (height * 2.0f);
	reflectionCamera->Position(position);

	reflectionCamera->GetMatrix(&desc.ReflectionMatrix);
}
void Water::Render()
{
	Renderer::Render();

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sReflectionMap->SetResource(reflection->SRV());
	sRefractionMap->SetResource(refraction->SRV());

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shader->DrawIndexed(0, 9, indexCount);
}

void Water::SetRefraction()
{
	Plane clipPlane = Plane(0, -1, 0, height + 0.1f);
	sClipPlane->SetFloatVector(clipPlane);

	refraction->Set(depthStencil->DSV());
	viewport->RSSetViewport();
}

void Water::SetReflection()
{
	Plane clipPlane = Plane(0, 1, 0, -height);
	sClipPlane->SetFloatVector(clipPlane);

	Context::Get()->SetSubCamera(reflectionCamera);

	reflection->Set(depthStencil->DSV());
	viewport->RSSetViewport();
}