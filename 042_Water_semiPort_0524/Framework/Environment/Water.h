#pragma once

class Water : public Renderer
{
	
public:
	Water(float height, float radius); 
	~Water();

	void Update() override;
	void Render() override;

	void SetRefraction();
	void SetReflection();

private:
	struct Desc
	{
		Matrix ReflectionMatrix;
		Color RefractionColor; //±¼Àý¿ë ÄÃ·¯
		Vector2 NormalMapTile;

		float WaveTranslation;
		float WaveScale;
		float Shininess;

		float Padding[3];
	}desc;

private:
	float height, radius;
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* waveMap;
	ID3DX11EffectShaderResourceVariable* sWaveMap; //³ë¸»¸Ê

	
	float waveSpeed;
	Vector2 mapScale;

	class Fixity* reflectionCamera;
	RenderTarget* reflection;
	RenderTarget* refraction;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sReflectionMap;
	ID3DX11EffectShaderResourceVariable* sRefractionMap;
	ID3DX11EffectVectorVariable* sClipPlane;
};	