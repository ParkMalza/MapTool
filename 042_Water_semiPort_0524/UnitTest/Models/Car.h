#pragma once
class Car
{
public:
	Car(Shader*, Model* modelinsShader, ModelRender* modelRendere, class Terrain* terrain);
	~Car();

	void Update(class Terrain* terrain);
	void CubeRender();
	void PreRender();
	void Render();

	//void Move();

	void Position(D3DXVECTOR3* position) { *position = this->position; }
	void Scale(D3DXVECTOR3* scale) { *scale = this->scale; }
	void Rotation(D3DXVECTOR3* rot) { *rot = this->rotation; }
	void Position(D3DXVECTOR3& position) { this->position = position; }
	void Scale(D3DXVECTOR3& scale) { this->scale = scale; }
	void Rotation(D3DXVECTOR3& rot) { this->rotation = rot; }
	UINT GetDrawCount() { return carRender->GetDrawCount(); }
	void GetTextureCube(TextureCube** val) { *val = textureCube; }

	void InstanceCar(D3DXVECTOR3& picked);
	void CheckHeight();

	void Save(wstring fileName);
	void Load(wstring fileName);
private:
	void EditCar();
	void Selected();
	//void SetImgui();
private:
	class Terrain* terrain;

	Model* model;
	ModelRender* carRender;

	Shader* modelShader;
	InstanceMesh* box;
	Transform* editTrans;
	Transform* boxEditTrans;
private:
	//float speed;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;
	D3DXMATRIX world;
	bool isClick;

	vector<D3DXVECTOR3> saveCar;
	vector<D3DXVECTOR3> saveBox;

	int selectIndex;
	int loadCarCount;
	int loadBoxCount;
private:
	TextureCube* textureCube;
	//ID3DX11EffectShaderResourceVariable* sCubeSrv;
};