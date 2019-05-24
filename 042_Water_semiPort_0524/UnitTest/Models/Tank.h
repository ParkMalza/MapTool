#pragma once
class Tank
{
public:
	Tank(Shader*, Model* modelinsShader, ModelRender* modelRendere, class Terrain* terrain);
	~Tank();

	void Update();
	void PreRender();
	void Render();
	void CubeRender();

	//void Move();

	void Position(D3DXVECTOR3* position) { *position = this->position; }
	void Scale(D3DXVECTOR3* scale) { *scale = this->scale; }
	void Rotation(D3DXVECTOR3* rot) { *rot = this->rotation; }
	void Position(D3DXVECTOR3& position) { this->position = position; }
	void Scale(D3DXVECTOR3& scale) { this->scale = scale; }
	void Rotation(D3DXVECTOR3& rot) { this->rotation = rot; }

	void InstanceTank(D3DXVECTOR3& picked);

	void Save(wstring fileName);
	void Load(wstring fileName);

private:
	
	void EditTank();
	void Selected();
	void CheckHeight();
	//void SetImgui();

private:
	class Terrain* terrain;
	Model* model;
	ModelRender* tankRender;

	Shader* modelShader;
	InstanceMesh* box;
	Transform* editTrans;
	Transform* boxEditTrans;
private:
	//float speed;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;
	bool isClick;

	vector<D3DXVECTOR3> saveTank;
	vector<D3DXVECTOR3> saveBox;

	int selectIndex;
	int loadTankCount;
	int loadBoxCount;
};