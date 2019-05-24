#pragma once
#pragma once

namespace PostEffects
{
	class Wiggle : public Render2D
	{
	public:
		Wiggle();
		~Wiggle();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Vector2 Offset = Vector2(1,1);
			Vector2 Amount = Vector2(0.1f, 0.1f);
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}