#pragma once

class GeometryRain : public Renderer
{
public:
	GeometryRain(D3DXVECTOR3& extent, UINT count);
	~GeometryRain();

	void Update();
	void Render();

private:
	void CreateData();

private:
	struct BufferDesc
	{
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1); //색깔
		D3DXVECTOR3 Velocity = D3DXVECTOR3(-10, -100, 0); //속도
		float DrawDistance = 1000.0f;  //그려질 범위??

		D3DXVECTOR3 Origin = D3DXVECTOR3(0, 0, 0);  //시작 지점
		float Padding;

		D3DXVECTOR3 Extent = D3DXVECTOR3(0, 0, 0);  //크기
		float Padding2;
	}bufferDesc;

	struct VertexRain  //쉐이더로 밀어줄 버텍스
	{
		D3DXVECTOR3 Position;
		//D3DXVECTOR2 Uv;
		D3DXVECTOR2 Scale;
	};
private:
	ConstantBuffer* buffer;// c버퍼 밀어주기
	ID3DX11EffectConstantBuffer* sBuffer;//

	//VertexRain* vertices;

	Texture* texture;//

	UINT drawCount;
	vector<VertexRain> vertices;
};