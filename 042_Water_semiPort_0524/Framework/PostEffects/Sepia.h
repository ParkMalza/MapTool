#pragma once
#pragma once

namespace PostEffects
{
	class Sepia : public Render2D
	{
	public:
		Sepia();
		~Sepia();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Matrix Sepia;
			float Strength = 0.5f;
			Vector3 Padding;
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}