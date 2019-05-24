#include "Framework.h"
#include "TextureCube.h"
#include "Viewer/Fixity.h"

TextureCube::TextureCube(Vector3&  position, UINT width, UINT height)
	: position(position), width(width), height(height)
	,sView(NULL)
{
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM; //픽셀값을 쓰므로 0~1까지의 값을 사용하기 위해

	//Create Texture2D - RTV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.Format = rtvFormat;
		desc.Usage = D3D11_USAGE_DEFAULT; //GPU에서 읽고 쓰기 가능
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &rtvTexture));
	}

	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY; //6면 텍스쳐를 사용하므로 배열
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateRenderTargetView(rtvTexture, &desc, &rtv));
		
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; //shader에서 텍스쳐 큐브로 쓰므로
		desc.TextureCube.MipLevels = 1;

		D3D::GetDevice()->CreateShaderResourceView(rtvTexture, &desc, &srv);
	}


	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT; // 전부다 깊이로 쓸꺼임
	//CreateTexture - DSV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 6;
		desc.SampleDesc.Count = 1;
		desc.Format = dsvFormat;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &dsvTexture));
	}

	//Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = dsvFormat;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv));
	}


	viewport = new Viewport((float)width, (float)height);

	//Create Camera
	{
		float x = position.x;
		float y = position.y;
		float z = position.z;

		Vector3 direction[6] =
		{
			Vector3(x + 1, y, z), Vector3(x - 1, y, z),
			Vector3(x, y + 1, z), Vector3(x, y - 1, z),
			Vector3(x, y, z + 1), Vector3(x, y, z - 1),
		};

		Vector3 up[6] =
		{
			Vector3(0, 1, 0), Vector3(0, 1, 0),
			Vector3(0, 0, -1), Vector3(0, 0, 1),
			Vector3(0, 1, 0), Vector3(0, 1, 0),
		};

		for (UINT i = 0; i < 6; i++)
		{
			D3DXMatrixLookAtLH(&view[i], &position, &direction[i], &up[i]);

			cameras[i] = new Fixity();
			cameras[i]->SetView(view[i]);
		}
	}
	//ortho그래픽은 완전한 직교 투영 
	perspective = new Perspective(1, 1, 1, 1500, Math::PI * 0.5f);
}

TextureCube::~TextureCube()
{
	SafeRelease(rtvTexture);
	SafeRelease(srv);

	SafeRelease(rtv);

	SafeRelease(dsvTexture);
	SafeRelease(dsv);

	for (UINT i = 0; i < 6; i++)
		SafeDelete(cameras[i]);

	SafeDelete(viewport);
}

void TextureCube::Set(Shader* shader)
{
	if (sView == NULL)
		sView = shader->AsMatrix("Views");

	sView->SetMatrixArray((float*)view, 0, 6);

	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, dsv);

	viewport->RSSetViewport(); //레스터라이징에 붙는다.
}
