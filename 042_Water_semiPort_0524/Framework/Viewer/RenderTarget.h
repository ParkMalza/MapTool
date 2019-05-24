#pragma once

class RenderTarget
{
public:
	RenderTarget(UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	ID3D11ShaderResourceView* SRV() { return srv; }
	ID3D11RenderTargetView* RTV() { return rtv; }


	void SaveTexture(wstring file);

	void Set(ID3D11DepthStencilView * dsv = NULL);
	static void Sets(vector<RenderTarget *>& rtvs, class DepthStencil* dsv);


private:
	UINT width, height;
	DXGI_FORMAT format;

	ID3D11Texture2D* backBuffer;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;

};