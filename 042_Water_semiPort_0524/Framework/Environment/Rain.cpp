#include "Framework.h"
#include "Rain.h"

Rain::Rain(D3DXVECTOR3 & extent, UINT count)
	:Renderer(L"025_Rain.fx"), drawCount(count)
{
	bufferDesc.Extent = extent;
	texture = new Texture(L"Environment/Rain.png");//
	shader->AsSRV("DiffuseMap")->SetResource(texture->SRV());//
	
	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));//
	sBuffer = shader->AsConstantBuffer("CB_Rain");//


	CreateData();//

	vertexBuffer = new VertexBuffer(vertices, drawCount * 4, sizeof(VertexRain));//
	indexBuffer = new IndexBuffer(indices, drawCount * 6);//

}

Rain::~Rain()
{
	SafeDelete(texture);
	SafeDeleteArray(indices);
	SafeDeleteArray(vertices);
	SafeDelete(indexBuffer);
	SafeDelete(vertexBuffer);
	SafeDelete(shader);
	SafeDelete(buffer);
}

void Rain::Update()
{
	Renderer::Update();

	D3DXVECTOR3 position(0, 0, 0);
	Context::Get()->GetCamera()->Position(&position);
	bufferDesc.Origin = position;

	ImGui::Separator();

	ImGui::Text("Rain");

	ImGui::SliderFloat3("Origin", (float*)&bufferDesc.Origin, 0, 200);
	ImGui::SliderFloat3("Velocity", (float*)&bufferDesc.Velocity, -200, 200);
	ImGui::ColorEdit3("Color", (float*)&bufferDesc.Diffuse);
}

void Rain::Render()
{
	buffer->Apply();
	Renderer::Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());


	vertexBuffer->Render();
	indexBuffer->Render();

	shader->DrawIndexed(0, 0, drawCount * 6);
}

void Rain::CreateData()
{
	vertices = new VertexRain[drawCount * 4];
	indices = new UINT[drawCount * 6];


	//Create VertexData
	{
		for (UINT i = 0; i < drawCount * 4; i += 4)
		{
			D3DXVECTOR2 scale;
			scale.x = Math::Random(0.1f, 0.4f);
			scale.y = Math::Random(4.0f, 8.0f);

			D3DXVECTOR3 position;
			position.x = Math::Random(-bufferDesc.Extent.x, bufferDesc.Extent.x);
			position.y = Math::Random(-bufferDesc.Extent.y, bufferDesc.Extent.y);
			position.z = Math::Random(-bufferDesc.Extent.z, bufferDesc.Extent.z);


			vertices[i + 0].Position = position;
			vertices[i + 1].Position = position;
			vertices[i + 2].Position = position;
			vertices[i + 3].Position = position;

			vertices[i + 0].Uv = D3DXVECTOR2(0, 1);
			vertices[i + 1].Uv = D3DXVECTOR2(0, 0);
			vertices[i + 2].Uv = D3DXVECTOR2(1, 1);
			vertices[i + 3].Uv = D3DXVECTOR2(1, 0);

			vertices[i + 0].Scale = scale;
			vertices[i + 1].Scale = scale;
			vertices[i + 2].Scale = scale;
			vertices[i + 3].Scale = scale;
		}//for(i)

		for (UINT i = 0; i < drawCount; i++)
		{
			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 2;
			indices[i * 6 + 4] = i * 4 + 1;
			indices[i * 6 + 5] = i * 4 + 3;
		}//for(i)
	}
}
