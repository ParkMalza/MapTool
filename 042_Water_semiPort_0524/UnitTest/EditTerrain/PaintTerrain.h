#pragma once

class PaintTerrain : public Command
{
public:
	PaintTerrain(class Projector* projector, class Terrain* terrain);
	~PaintTerrain();

	void SetImgui(D3DXVECTOR3& picked);
	
	// Command을(를) 통해 상속됨
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Excute() override;

private:
	void OpenBaseTexture(wstring file);
	void OpenLayerTexture(wstring file);
	void OpenLayer1(wstring file);
	void OpenLayer2(wstring file);
	void OpenLayer3(wstring file);
	void OpenLayer4(wstring file);
	void Paint(D3DXVECTOR3& picked);
private:
	class Terrain* terrain;
	class Projector* projector;
	Texture* tex;
private:
	wstring paint;
	wstring baseTexture;
	wstring layerTexture;
	wstring layer1;
	wstring layer2;
	wstring layer3;
	wstring layer4;

	UINT brushType;
	float imageX;
	float imageY;
	float power;
private:
	int type;
	Color color;
	int range;
	float alpha;
	
	bool isGridView;
	float gridLineThick;
	float gridLineSize;
};