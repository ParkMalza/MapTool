#pragma once
class GeometrySnow : public Renderer
{
public:
	GeometrySnow(D3DXVECTOR3 extent, UINT count);
	~GeometrySnow();

	void Update();
	void Render();



private:
	void CreateData();

private:
	struct BufferDesc
	{
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1);

		D3DXVECTOR3 Velocity = D3DXVECTOR3(0, -5, 0);
		float DrawDistance = 1000.f;

		D3DXVECTOR3 Origin = D3DXVECTOR3(0, 0, 0); //원점
		float Tubulence = 5;

		D3DXVECTOR3 Extent = D3DXVECTOR3(0, 0, 0); //큐브
		float Padding2;

	} bufferDesc;


	struct VertexSnow
	{
		D3DXVECTOR3 Position;
		//D3DXVECTOR2 Uv;
		D3DXVECTOR2 Random;
		float Scale;
	};

private:
	ConstantBuffer * buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	//VertexSnow * vertices;
	//UINT * indices;

	Texture* texture;

	UINT drawCount;

	vector<VertexSnow> vertices;
};