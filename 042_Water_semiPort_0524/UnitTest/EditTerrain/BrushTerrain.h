#pragma once
class Command;

class BrushTerrain : public Command
{
public:
	BrushTerrain(class Projector* projector, class Terrain* terrain);
	~BrushTerrain();

	// Command을(를) 통해 상속됨
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Excute() override;

	void SetImgui(D3DXVECTOR3& picked);

private:
	void SettingMapHeight(D3DXVECTOR3& picked);

private:
	class Terrain* terrain;
	class Projector* projector;
private:
	int type;
	int range;
	float lockHeight;
	float speed;

	UINT selectHeight;
	D3DXCOLOR color;

	string comboName;
	vector<string> names;
};
