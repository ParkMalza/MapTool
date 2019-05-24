#pragma once

class TextureCube
{
public:
	TextureCube(Vector3& position, UINT width, UINT height);
	~TextureCube();


	void Set(Shader* shader);

	ID3D11ShaderResourceView* SRV() { return srv; }
	Camera* GetCamera(UINT index) {	return (Camera *)cameras[index]; }

	Perspective* GetPerspective() { return perspective; }

private:
	Vector3 position;

	UINT width, height;

	ID3D11Texture2D* rtvTexture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;

	ID3D11Texture2D* dsvTexture;
	ID3D11DepthStencilView* dsv;

	Matrix view[6];
	class Fixity* cameras[6];
	Perspective* perspective;

	Viewport* viewport;

	ID3DX11EffectMatrixVariable* sView;
};