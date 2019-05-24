#pragma once

class Snow : public Renderer
{
public:
	Snow(D3DXVECTOR3& extent, UINT count);
	~Snow();

	void Update();
	void Render();

private:
	void CreateData();
private:
	struct BufferDesc
	{
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1); //색깔
		D3DXVECTOR3 Velocity = D3DXVECTOR3(0, -5, 0); //속도
		float DrawDistance = 1000.0f;  //그려질 범위??

		D3DXVECTOR3 Origin = D3DXVECTOR3(0, 0, 0);  //시작 지점
		float Turbulence = 5;

		D3DXVECTOR3 Extent = D3DXVECTOR3(0, 0, 0);  //크기
		float Padding2;
	}bufferDesc;

	struct VertexSnow  //쉐이더로 밀어줄 버텍스
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR2 Uv;
		D3DXVECTOR2 Random;
		float Scale;
	};
private:
	ConstantBuffer* buffer;// c버퍼 밀어주기
	ID3DX11EffectConstantBuffer* sBuffer;//

	VertexBuffer* vertexBuffer;//
	IndexBuffer* indexBuffer;//

	VertexSnow* vertices;
	UINT* indices;

	Texture* texture;//

	UINT drawCount;
};