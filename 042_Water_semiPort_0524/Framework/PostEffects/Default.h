#pragma once

namespace PostEffects
{
	class Default : public Render2D
	{
	public:
		Default();
		~Default();

		void Update()override;
		void Render()override;

	private:
		struct Desc
		{
			Color Tone = Color(1,1,1,1);
			Color Gamma = Color(1, 1, 1, 1);
			UINT nBit = 0;
			float padding[3];
		}desc;

		ConstantBuffer* buffer;
		ID3DX11EffectConstantBuffer* sBuffer;

		string Mode[10];
	};
}