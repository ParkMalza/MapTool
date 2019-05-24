#pragma once

class Kachujin
{
public:
	Kachujin(Shader* shader,Model* model, ModelAnimator* modelRender, class Terrain* terrain);
	~Kachujin();

	void Update(class Terrain* terrain);
	void PreRender();
	void CubeRender();
	void Render();

	void Move();
	void CheckCollision();

	void InstanceKachujin(D3DXVECTOR3& picked);
	void CheckHeight();

	void Save(wstring fileName);
	void Load(wstring fileName);
private:
	void EditKachujin();
	void Selected();

private:
	class Terrain* terrain;
	Model* model;
	ModelAnimator* kachujinRender;

	Shader* modelShader;
	InstanceMesh* box;
	Transform* editTrans;
	Transform* boxEditTrans;

	vector<Transform*> kachujinMoves;

private:
	Vector3 position;
	Vector3 scale;
	Vector3 rotation;
	bool isClick;

	vector<Vector3> saveKachujin;
	vector<Vector3> saveBox;

	int selectIndex;
	int loadKachujinCount;
	int loadBoxCount;

	float speed;
	int Clip;
	Vector3 changeMaxHigh;
	Vector3 changeMaxLow;
	int pattern[100];
	int collTerm[100];
};
