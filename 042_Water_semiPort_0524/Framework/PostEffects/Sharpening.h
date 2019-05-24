#pragma once
#pragma once

namespace PostEffects
{
	class Sharpening : public Render2D
	{
	public:
		Sharpening();
		~Sharpening();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Vector2 MapSize;
			float Sharpening = 0.0f;

			float Padding;
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}