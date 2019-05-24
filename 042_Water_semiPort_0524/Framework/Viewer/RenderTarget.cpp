#include "Framework.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format)
	: format(format)
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
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &backBuffer);
		Check(hr)
	}

	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateRenderTargetView(backBuffer, &rtvDesc, &rtv);
		Check(hr)
	}

	//CreateSRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(backBuffer, &srvDesc, &srv);
		Check(hr)
	}

}

RenderTarget::~RenderTarget()
{
	SafeRelease(rtv);
	SafeRelease(srv);
	SafeRelease(backBuffer);
}

void RenderTarget::SaveTexture(wstring file)
{
	HRESULT hr = D3DX11SaveTextureToFile
	(
		D3D::GetDC(), backBuffer, D3DX11_IFF_PNG, file.c_str()
	);
	Check(hr)
}

void RenderTarget::Set(ID3D11DepthStencilView * dsv)
{
	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(D3DXCOLOR(0, 0, 0, 1), rtv, dsv);
}

void RenderTarget::Sets(vector<RenderTarget*>& rtvs, DepthStencil * dsv)
{
	vector<ID3D11RenderTargetView *> views;
	for (UINT i = 0; i < rtvs.size(); i++)
		views.push_back(rtvs[i]->RTV());


	D3D::GetDC()->OMSetRenderTargets(views.size(), &views[0], dsv->DSV());

	for (UINT i = 0; i < views.size(); i++)
		D3D::Get()->Clear(D3DXCOLOR(0, 0, 0, 1), views[i], dsv->DSV());
}