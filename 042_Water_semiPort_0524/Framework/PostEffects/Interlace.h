#pragma once
#pragma once

namespace PostEffects
{
	class Interlace : public Render2D
	{
	public:
		Interlace();
		~Interlace();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Vector2 MapSize;
			float Interlace = 1;
			UINT Line = 2;
			Vector3 LuminanceWeights = Vector3(0.2126f, 0.7152f, 0.0722f);
			float Padding;
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}