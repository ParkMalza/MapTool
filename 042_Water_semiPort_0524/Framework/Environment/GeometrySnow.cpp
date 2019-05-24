#include "Framework.h"
#include "GeometrySnow.h"


GeometrySnow::GeometrySnow(D3DXVECTOR3 extent, UINT count)
	: Renderer(L"025_Geometry_Snow.fx"), drawCount(count)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	bufferDesc.Extent = extent;

	texture = new Texture(L"Environment/Snow.png");
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());

	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = shader->AsConstantBuffer("CB_Snow");

	CreateData();
}

GeometrySnow::~GeometrySnow()
{
	SafeDelete(texture);
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}


void GeometrySnow::Update()
{
	Renderer::Update();

	D3DXVECTOR3 position(0, 0, 0);
	Context::Get()->GetCamera()->Position(&position);
	bufferDesc.Origin = position;

	ImGui::Separator();
	ImGui::Text("GeometrySnow");

	//ImGui::SliderFloat3("Origin", (float *)&bufferDesc.Origin, 0, 200);
	ImGui::SliderFloat3("Velocity", (float *)&bufferDesc.Velocity, -200, 200);
	ImGui::SliderFloat("Turbulence", &bufferDesc.Tubulence, -20, 20);
	//ImGui::ColorEdit3("Color", (float *)&bufferDesc.Diffuse);
}

void GeometrySnow::Render()
{
	Renderer::Render();

	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	shader->Draw(0, Pass(), vertices.size());
}

void GeometrySnow::CreateData()
{
	//Create VertexData
	{
		for (UINT i = 0; i < drawCount; i++)
		{
			D3DXVECTOR3 position;
			position.x = Math::Random(-bufferDesc.Extent.x, bufferDesc.Extent.x);
			position.y = Math::Random(-bufferDesc.Extent.y, bufferDesc.Extent.y);
			position.z = Math::Random(-bufferDesc.Extent.z, bufferDesc.Extent.z);

			D3DXVECTOR2 random;
			random.x = Math::Random(0.0f, 1.0f);
			random.y = Math::Random(0.0f, 1.0f);

			float scale = Math::Random(0.2f, 0.6f);

			VertexSnow vertex;
			vertex.Position = position;
			vertex.Scale = scale;
			vertex.Random = random;

			vertices.push_back(vertex);
		}//for(i)

		assert(vertexBuffer == NULL);
		vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexSnow));
	}
}
