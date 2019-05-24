#include "Framework.h"
#include "Renderer.h"

Renderer::Renderer(Shader * shader)
	: shader(shader), bCreateShader(false)
{
	Initialize();
}

Renderer::Renderer(wstring shaderFile)
	: bCreateShader(true)
{
	shader = new Shader(shaderFile);

	Initialize();
}

void Renderer::Initialize()
{
	topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	pass = 0;
	vertexBuffer = NULL;
	indexBuffer = NULL;
	vertexCount = 0;
	indexCount = 0;

	perFrame = new PerFrame(shader);
	transform = new Transform(shader);
}

Renderer::~Renderer()
{
	SafeDelete(perFrame);
	SafeDelete(transform);

	if (bCreateShader == true)
		SafeDelete(shader);
}

void Renderer::Update()
{
	perFrame->Update();
	transform->Update();
}

void Renderer::Render()
{
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Render();

		if (indexBuffer != NULL)
			indexBuffer->Render();
	}

	D3D::GetDC()->IASetPrimitiveTopology(topology);

	perFrame->Render();
	transform->Render();
}