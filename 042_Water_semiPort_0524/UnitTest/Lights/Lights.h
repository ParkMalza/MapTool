#pragma once
#define LIGHT_TYPE_NUM 2

class Command;

class Lights: public Command
{
public:
	Lights(Shader* shader);
	~Lights();

	// Command을(를) 통해 상속됨
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Excute() override;

	void Update(Vector3& picked);
	void Render();

	void SetOldPoint();

	void Save(wstring fileName);
	void Load(wstring fileName);
private:
	void AddPointLights(Vector3& picked);
	void AddSpotLights(Vector3& picked);
	void BoxInstancing(UINT lightType, Vector3& picked);
	void SelectPointBox();
	void SelectSpotBox();
	void EditPointBox();
	void EditSpotBox();
	void DeleteSpot(UINT lightType, UINT selectNum);
	void DeletePoint(UINT lightType, UINT selectNum);

	void UnProject();

private:
	class InstanceMesh* box[2];
	vector<InstanceMesh*> boxes;
	Transform* boxTrans;
private:
	Vector3 boxPos;
	Vector3 boxScale;
	Vector3 boxRotation;
	Vector3 pointLightPos;
	UINT colorIndex;
	UINT boxIndex;
	UINT maxCount;
	bool isLight[2];
	bool isLightSelected[2];
	int selectPoint;
	int selectSpot;
	int saveDrawCount;
private:
	//언프 변수
	D3DXMATRIX W, V, P;
	D3DXVECTOR3 start, direction;
	D3DXVECTOR3 n, f;

};