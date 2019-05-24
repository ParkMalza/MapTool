#pragma once
#define MAX_BLOOM_COUNT 63

namespace PostEffects
{
	class Bloom : public Render2D
	{
	public:
		Bloom();
		~Bloom();

		void Update()override;
		void Render()override;

	public:
		void BlurOffset(float x, float y);
		void LuminosityMap(ID3D11ShaderResourceView* srv);

	private:
		void UpdateWeights();

	private:
		struct BloomDesc
		{
			Vector2 Offset = Vector2(0,0);
			float Weight = 0.0f;

			float Padding;
		};

		struct Desc
		{
			Vector2 MapSize;
			float Threshold = 0.25f;
			float Intensity = 1.15f;

			UINT Count = 8;

			Vector3 Padding;
			BloomDesc Valuse[MAX_BLOOM_COUNT];
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;

		ID3DX11EffectShaderResourceVariable* sLuminosityMap;
	};
}