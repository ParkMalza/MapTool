#include "Framework.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(UINT * data, UINT count)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(UINT) * count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer);
	Check(hr);
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(buffer);
}

void IndexBuffer::Render()
{
	D3D::GetDC()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}
