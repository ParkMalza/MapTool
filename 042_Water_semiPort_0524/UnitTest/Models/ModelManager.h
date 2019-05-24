#pragma once
class ModelManager
{
public:
	ModelManager(Shader* shader, class Terrain* terrain);
	~ModelManager();

	void Update(class Terrain* terrain);
	void CubeRender();
	void PreRender();
	void Render();

	void SetImgui(D3DXVECTOR3& picked);
	UINT GetCarCount() { return carCount; }

	void tankSave(wstring fileName);
	void towerSave(wstring fileName);
	void carSave(wstring fileName);
	void tankLoad(wstring fileName);
	void towerLoad(wstring fileName);
	void carLoad(wstring fileName);
private:
	void SettingModel(D3DXVECTOR3 & picked);
	void SettingComboNames();
private:
	//Export
	void ExportTower();
	void ExportTank();
	void ExportCar();
	void ExportKachujin();

	//Read
	void ReadTower(Shader* shader);
	void ReadTank(Shader* shader);
	void ReadCar(Shader* shader);
	void ReadKachujin(Shader* shader);

private:
	class Terrain* terrain;
	Shader* shader;
	//타워 임포트용
	Model* tower;
	//Shader* towerShader;
	ModelRender* towerRender;
	//탱크 임포트용
	Model* tank;
	//Shader* tankShader;
	ModelRender* tankRender;
	//자동차 임포트용
	Model* car;
	ModelRender* carRender;
	//카추진
	Model* kachujin;
	ModelAnimator* kachujinRender;
private:
	//실제 오브젝트
	class Tank* instanceTank;
	class Tower* instanceTower;
	class Car* instanceCar;
	class Kachujin* instanceKachujin;
private:
	UINT selectModel;
	string comboName;
	vector<string> names;

private:
	UINT carCount;
};