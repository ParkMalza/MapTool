#pragma once
#pragma once

namespace PostEffects
{
	class Saturation : public Render2D
	{
	public:
		Saturation();
		~Saturation();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			float Saturation = 0.0f;
			Vector3 LuminanceWeights = Vector3(0.2126f, 0.7152f, 0.0722f);
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;
	};
}