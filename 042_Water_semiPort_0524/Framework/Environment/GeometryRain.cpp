#include "Framework.h"
#include "GeometryRain.h"

GeometryRain::GeometryRain(D3DXVECTOR3 & extent, UINT count)
	:Renderer(L"025_Geometry_Rain.fx"), drawCount(count)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	bufferDesc.Extent = extent;
	texture = new Texture(L"Environment/Rain.png");//
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());//

	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));//
	sBuffer = shader->AsConstantBuffer("CB_Geo_Rain");//

	CreateData();

}

GeometryRain::~GeometryRain()
{
	SafeDelete(texture);
	SafeRelease(sBuffer);
	SafeDelete(buffer);
}

void GeometryRain::Update()
{
	Renderer::Update();

	D3DXVECTOR3 position;
	Context::Get()->GetCamera()->Position(&position);

	bufferDesc.Origin = position;

	ImGui::Separator();

	ImGui::Text("Rain");

	//ImGui::SliderFloat3("Origin", (float*)&bufferDesc.Origin, 0, 200);
	ImGui::SliderFloat3("Velocity", (float*)&bufferDesc.Velocity, -200, 200);
//	ImGui::ColorEdit3("Color", (float*)&bufferDesc.Diffuse);

}

void GeometryRain::Render()
{
	buffer->Apply();
	Renderer::Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	vertexBuffer->Render();
	shader->Draw(0, 0, vertices.size());
}

void GeometryRain::CreateData()
{
	for (UINT i = 0; i < drawCount; i++)
	{
		D3DXVECTOR2 scale;
		scale.x = Math::Random(0.1f, 0.4f);
		scale.y = Math::Random(4.0f, 8.0f);

		D3DXVECTOR3 position;
		position.x = Math::Random(-bufferDesc.Extent.x, bufferDesc.Extent.x);
		position.y = Math::Random(-bufferDesc.Extent.y, bufferDesc.Extent.y);
		position.z = Math::Random(-bufferDesc.Extent.z, bufferDesc.Extent.z);

		VertexRain vertex;
		vertex.Position = position;
		vertex.Scale = scale;

		vertices.push_back(vertex);
	}//for(i)
	assert(vertexBuffer == NULL);
	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexRain));
}
