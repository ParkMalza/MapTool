#pragma once
//매 프레임마다 밀어줘야 할 정보들
class PerFrame
{
public:
	PerFrame(Shader* shader);
	~PerFrame();

public:
	void Update();
	void Render();

private:
	//cbuffer의 데이터와 일치해야함
	struct BufferDesc
	{
		D3DXMATRIX View;
		D3DXMATRIX ViewInverse;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewProjection;

		D3DXMATRIX OrthoView;
		D3DXMATRIX FrontOrthoViewProjection;
		D3DXMATRIX UpOrthoViewProjection;
		D3DXMATRIX RightOrthoViewProjection;

		float Time;
		float Padding[3];
	} bufferDesc;

	struct LightDesc
	{
		D3DXCOLOR Ambient;
		D3DXCOLOR Specular;

		D3DXVECTOR3 Direction;
		float Padding;

		D3DXVECTOR3 Position;
		float Padding2;
	} lightDesc;

	struct PointLightDesc
	{
		UINT Count = 0;
		float Padding[3];

		PointLight Lights[MAX_POINT_LIGHT];
	} pointLightDesc;

	struct SpotLightDesc
	{
		UINT Count = 0;
		float Padding[3];

		SpotLight Lights[MAX_SPOT_LIGHT];
	} spotLightDesc;
private:
	Shader* shader;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ConstantBuffer* lightBuffer;
	ID3DX11EffectConstantBuffer* sLightBuffer;

	ConstantBuffer* pointLightBuffer;
	ID3DX11EffectConstantBuffer* sPointLightBuffer;

	ConstantBuffer* spotLightBuffer;
	ID3DX11EffectConstantBuffer* sSpotLightBuffer;
};