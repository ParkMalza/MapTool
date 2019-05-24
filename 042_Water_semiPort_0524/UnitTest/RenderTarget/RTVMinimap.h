#pragma once
class RTVMinimap
{
public:
	RTVMinimap();
	~RTVMinimap();

	void Update();

	void PostRender();
	void PreRender();
	void MultiViewPreRender();
	void MinimapRender();
	void SetViewport();
	void SetImgui();

private:
	//메인 화면
	RenderTarget* renderTarget;  //오리지날
	DepthStencil* depthStencil;
	Viewport* viewport;
	Render2D* mainRender2D;
	//화면
	RenderTarget* orthoRenderTarget1;
	DepthStencil* orthoDepthStencil;
	Viewport* orthoViewport1;
	Render2D* orthoRender1;

	RenderTarget* targets[10];

	//각종 효과
	Render2D* deFault;
	Render2D* wiggle;
	Render2D* distortion;
	Render2D* sepia;
	Render2D* saturation;
	Render2D* sharpening;
	Render2D* radialBlur;
	Render2D* interLace;
	Render2D* vignette;
	Render2D* bloom; //9
	vector<Render2D*> postEffects;

#ifdef ALLOW_MULTI_VIEWPORT
	RenderTarget* minimapTarget;
	DepthStencil* minimapDepth;
	Viewport* minimapViewPort;
	Render2D* minimap2D;
#endif // ALLOW_MULTI_VIEWPORT

private:
	ID3D11ShaderResourceView* finalSrv;

private:
	bool isMapOpen;
	bool isDivideScreen;
	float width;
	float height;
	int selectEffects;

private:
	string effect;
	vector<string> effects;
};