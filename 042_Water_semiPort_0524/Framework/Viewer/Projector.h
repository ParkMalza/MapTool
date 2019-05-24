#pragma once

class Projector
{
public: //perspective ∫Ò¿≤
	Projector(Shader* shader, vector<wstring>& textureNames, UINT width = 1, UINT height = 1);
	~Projector();

	void Update(Vector3& position, int& type, UINT& range, Color& color);

	Camera* GetCamera() { return (Camera*)camera; }

private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;

		Color Color = D3DXCOLOR(1, 1, 1, 1);
	} desc;
private:
	Shader* shader;

	UINT width, height;

	class Fixity* camera;
	Projection* projection;
	Texture* texture;
	TextureArray* textures;

	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;
};