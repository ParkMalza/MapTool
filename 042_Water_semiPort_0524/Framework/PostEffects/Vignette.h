#pragma once
#pragma once

namespace PostEffects
{
	class Vignette : public Render2D
	{
	public:
		Vignette();
		~Vignette();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Vector2 MapSize;
			Vector2 Scale = Vector2(1, 1);

			float Vignette = 1;
			float test = 0.0001f;

			float Padding[2];
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}