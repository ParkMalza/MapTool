#pragma once

class Sky
{
public:
	Sky();
	Sky(wstring cubeMapFile);
	Sky(Shader* shader, wstring cubeMapFile);
	~Sky();

	void Update();
	void Render();

	void Pass(UINT val) { sphere->Pass(val); }

private:
	struct DataDesc
	{
		D3DXCOLOR Center = D3DXCOLOR(0xFF0080FF);
		D3DXCOLOR Apex = D3DXCOLOR(0xFF9BCDFF);

		float Height = 4.0f;
	} dataDesc;
private:
	bool bCreateShader;
	Shader* shader;
	MeshSphere* sphere;

	ID3DX11EffectVariable* sData;

private:
	wstring cubeMapFile;
	ID3D11ShaderResourceView* cubeSrv;
	ID3DX11EffectShaderResourceVariable* sCubeSrv;
};