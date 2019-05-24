#include "Framework.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(UINT width, UINT height, bool bUseStencil)
{
	D3DDesc desc = D3D::GetDesc();

	this->width = (width < 1) ? (UINT)desc.Width : width;
	this->height = (height < 1) ? (UINT)desc.Height : height;


	D3D11_TEXTURE2D_DESC textureDesc;
	{
		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		textureDesc.Width = this->width;
		textureDesc.Height = this->height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = bUseStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &backBuffer);
		Check(hr)
	}

	//Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = bUseStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateDepthStencilView(backBuffer, &desc, &dsv);
		Check(hr)
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = bUseStencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(backBuffer, &desc, &srv);
		Check(hr)
	}
}

DepthStencil::~DepthStencil()
{
	SafeRelease(dsv);
	SafeRelease(backBuffer);
	SafeRelease(srv);
}

void DepthStencil::SaveTexture(wstring saveFile)
{
	HRESULT hr = D3DX11SaveTextureToFile
	(
		D3D::GetDC(), backBuffer, D3DX11_IFF_PNG, saveFile.c_str()
	);
	Check(hr)
}