#pragma once

class IndexBuffer
{
public:
	IndexBuffer(UINT* data, UINT count);
	~IndexBuffer();

	UINT Count() { return count; }
	ID3D11Buffer* Buffer() { return buffer; }

	void Render();

private:
	ID3D11Buffer* buffer;

	UINT* data;
	UINT count;
};