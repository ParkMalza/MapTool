#pragma once

class BillboardsInstancing : public Command
{
public:
	BillboardsInstancing(class Projector* projector, Shader* shader, class Terrain* terrain);
	~BillboardsInstancing();

	void Update();
	void Render();
	void ShadowRender();
	void SettingImgui(class Terrain* terrain, Vector3& picked);
	void SettingBillboard(class Terrain* terrain, Vector3& picked);
	void MakeRandomBillboard(D3D11_BOX & box, class Terrain* terrain, D3DXVECTOR3& picked);

	void Save(wstring name);
	void Load(wstring name);



	// Command을(를) 통해 상속됨
	virtual void Undo() override;

	virtual void Redo() override;

	virtual void Excute() override;

private:
	class Terrain* terrain;
	Texture* texture;
	Shader* shader;
	class BillboardInstance* billboard;
	class Projector* projector;

private:
	UINT selectGrass;
	vector<wstring> getFolderImage;  //폴더에서 가져올 이름
	vector<wstring> grassNames;  //사용할 이름

private:
	float imageX;  //풀 이미지 크기
	float imageY;
	UINT grassType;
	UINT brushType;
	UINT brushRange;
	UINT grassNum;
	D3DXCOLOR brushColor;
};