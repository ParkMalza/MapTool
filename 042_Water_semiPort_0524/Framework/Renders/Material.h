#pragma once

class Material
{
public:
	Material();
	Material(Shader* shader);
	~Material();

	void SetShader(Shader* shader);
	Shader* GetShader() { return shader; }


	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	D3DXCOLOR Ambient() { return colorDesc.Ambient; }
	void Ambient(D3DXCOLOR& color);
	void Ambient(float r, float g, float b, float a = 1.0f);

	D3DXCOLOR Diffuse() { return colorDesc.Diffuse; }
	void Diffuse(D3DXCOLOR& color);
	void Diffuse(float r, float g, float b, float a = 1.0f);

	D3DXCOLOR Specular() { return colorDesc.Specular; }
	void Specular(D3DXCOLOR& color);
	void Specular(float r, float g, float b, float a = 1.0f);

	Texture* DiffuseMap() { return diffuseMap; }
	void DiffuseMap(string file);
	void DiffuseMap(wstring file);
	void DiffuseMaps(vector<wstring>& textures);
	void DiffuseMapNull();

	Texture* SpecularMap() { return specularMap; }
	void SpecularMap(string file);
	void SpecularMap(wstring file);
	void SpecularMaps(vector<wstring>& textures);
	void SpecularMapNull();

	Texture* NormalMap() { return normalMap; }
	void NormalMap(string file);
	void NormalMap(wstring file);
	void NormalMaps(vector<wstring>& textures);
	void NormalMapNull();

	void Render();

private:
	void Initialize();

private:
	struct ColorDesc
	{
		D3DXCOLOR Ambient = D3DXCOLOR(0, 0, 0, 1);
		D3DXCOLOR Diffuse = D3DXCOLOR(1, 1, 1, 1);
		D3DXCOLOR Specular = D3DXCOLOR(0, 0, 0, 1);

	} colorDesc;

private:
	Shader* shader;

	wstring name;

	Texture* diffuseMap;
	Texture* specularMap;
	Texture* normalMap;

	ConstantBuffer* buffer;

	ID3DX11EffectConstantBuffer* sBuffer;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
	ID3DX11EffectShaderResourceVariable* sSpecularMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;

private:
	TextureArray* diffuseMaps;
	TextureArray* specularMaps;
	TextureArray* normalMaps;

	ID3DX11EffectShaderResourceVariable* sDiffuseMaps;
	ID3DX11EffectShaderResourceVariable* sSpecularMaps;
	ID3DX11EffectShaderResourceVariable* sNormalMaps;
};