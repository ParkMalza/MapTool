#pragma once

class Renderer
{
public:
	Renderer(Shader* shader);
	Renderer(wstring shaderFile);

	virtual ~Renderer();

	Shader* GetShader() { return shader; }

	UINT& Pass() { return pass; }
	void Pass(UINT val) { pass = val; }

	virtual void Update();
	virtual void Render();

	Transform* GetTransform() { return transform; }

private:
	void Initialize();

protected:
	void Topology(D3D11_PRIMITIVE_TOPOLOGY val) { topology = val; }

protected:
	Transform* transform;
	Shader* shader;

	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;

	UINT vertexCount;
	UINT indexCount;

private:
	bool bCreateShader;

	D3D11_PRIMITIVE_TOPOLOGY topology;
	UINT pass;

	PerFrame* perFrame;
};