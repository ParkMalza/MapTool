#pragma once

class Shadow
{
public:
	Shadow(Shader* shader, Vector3& position, float radius, UINT width = 512, UINT height = 512);
	~Shadow();

	void Set();

	ID3D11ShaderResourceView* SRV() { return renderTarget->SRV(); }
private:
	void UpdateVolume();

private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;

		Vector2 MapSize;
		float Bias = -0.0006f;

		UINT index;
	}desc;
private:
	Shader* shader;

	UINT width, height; //그림자 높이 넓이

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	Vector3 position;
	float radius;

	ID3D11SamplerState* samplerState;
};
