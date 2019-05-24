#pragma once
#define MAX_RADIAL_BLUR_COUNT 32

namespace PostEffects
{
	class RadialBlur : public Render2D
	{
	public:
		RadialBlur();
		~RadialBlur();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Vector2 MapSize;
			UINT BlurCount = 8;

			float Radius = 0.0f;
			float Amount = 0.04f;
			float addError = 0.00001f;

			Vector2 Padding;
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}