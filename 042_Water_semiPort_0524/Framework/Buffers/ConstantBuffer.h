#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer(void* data, UINT dataSize);
	~ConstantBuffer();

	void Apply();

	ID3D11Buffer* Buffer() { return buffer; }

private:
	ID3D11Buffer* buffer;

	void* data;
	UINT dataSize;
};