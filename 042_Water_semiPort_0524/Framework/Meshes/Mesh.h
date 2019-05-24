#pragma once

class Mesh : public Renderer
{
public:
	typedef VertexTextureNormalTangent MeshVertex;

public:
	Mesh(Shader* shader);
	virtual ~Mesh();

	void Render();

protected:
	virtual void Create() = 0;

protected:
	MeshVertex* vertices;
	UINT* indices;
};