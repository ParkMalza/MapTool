#pragma once

class Viewport
{
public:
	Viewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	~Viewport();

	void RSSetViewport();
	void Set(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);

	float GetWidth() { return width; }
	float GetHeight() { return height; }

	//2D¿¡¼­ 3D·Î
	void GetRay(OUT D3DXVECTOR3* position, OUT D3DXVECTOR3* direction, D3DXMATRIX& W, D3DXMATRIX& V, D3DXMATRIX& P);
	void Project(OUT D3DXVECTOR3* position, D3DXVECTOR3& source, D3DXMATRIX& W, D3DXMATRIX& V, D3DXMATRIX& P);
	//3D ÁÂÇ¥¸¦ 2D ÁÂÇ¥·Î
	void Unproject(OUT D3DXVECTOR3* position, D3DXVECTOR3& source, D3DXMATRIX& W, D3DXMATRIX& V, D3DXMATRIX& P);
	//2D ÁÂÇ¥¸¦ 3D ÁÂÇ¥·Î
private:
	float x, y;
	float width, height;
	float minDepth, maxDepth;

	D3D11_VIEWPORT viewport;
};