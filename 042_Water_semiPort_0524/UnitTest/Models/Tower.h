#pragma once
class Tower
{
public:
	Tower(Shader* modelShader, Model* model, ModelRender* modelRendere, class Terrain* terrain);
	~Tower();

	void Update(class Terrain* terrain);
	void CubRender();
	void PreRender();
	void Render();
	
	void InstanceTower(D3DXVECTOR3& picked);
	ModelRender* GetRenderer() { return towerRender; }
	
	void CheckHeight();

	void Save(wstring fileName);
	void Load(wstring fileName);

private:
	void Selected();
	void EditTower();

private:
	class Terrain* terrain;
	Model* model;
	ModelRender* towerRender;

	InstanceMesh* box;
	Shader* modelShader;
	//ÆíÁý¿ë
	Transform* editTrans;
	Transform* boxEditTrans;
private:
	float y;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 boxScale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 changeYZHigh;
	D3DXVECTOR3 changeYZLow;

	vector<D3DXVECTOR3> saveTower;
	vector<D3DXVECTOR3> saveBox;

	bool isClick;
	int selectIndex;
	UINT loadTowerCount;
};