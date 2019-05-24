#include "Framework.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(void * data, UINT dataSize)
	: data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer);
	Check(hr);
}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(buffer);
}

void ConstantBuffer::Apply()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, dataSize);
	}
	D3D::GetDC()->Unmap(buffer, 0);
}
