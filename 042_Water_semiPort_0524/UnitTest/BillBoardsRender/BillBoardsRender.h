#pragma once

class Command;
class BillBoardsRender : public Command
{
public:
	BillBoardsRender(class Projector* projector, class Terrain* terrain);
	~BillBoardsRender();

	void Update();
	void Render();
	void SettingImgui(class Terrain* terrain, D3DXVECTOR3& picked);
	void SettingBillboard(class Terrain* terrain, D3DXVECTOR3& picked);
	void MakeRandomBillboard(D3D11_BOX & box, class Terrain* terrain, D3DXVECTOR3& picked);

	void AddGrass(wstring file);

	void Save(wstring name);
	void Load(wstring name);

	// Command을(를) 통해 상속됨
	virtual void Undo() override;

	virtual void Redo() override;

	virtual void Excute() override;

private:
	class Terrain* terrain;

	Texture* tex;  //어떤 풀 사용하고 있는지 이미지 받아오려고
	Shader* shader;
	class BillBoard* billBoard;
	class Projector* projector;

private:
	UINT selectGrass;  //몇번째 풀 선택했는지
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