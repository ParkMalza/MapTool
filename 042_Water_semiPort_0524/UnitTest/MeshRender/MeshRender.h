#pragma once
// 텍스쳐 내용들 배열로 받아야 해서 새로 클래스 만듦
class Command;

class MeshRender : public Command
{
public:
	MeshRender(Shader* shader, vector<wstring>& textureNames, class Terrain* terrain);
	~MeshRender();

	// Command을(를) 통해 상속됨
	virtual void Undo() override;
	virtual void Redo() override;
	virtual void Excute() override;

	void Update();
	void PreRender();
	void CubeRender();
	void Render();

	void Instancing(D3DXVECTOR3& picked, class Terrain* terrain, UINT mesh, UINT textureNum);
	void BoxInstancing(UINT boxNum, UINT meshNum, D3DXVECTOR3 reSize);
	//선택
	void SelectedQuad();
	void SelectedCube();
	void SelectedGrid();
	void SelectedSphere();
	void SelectedCylinder();

	//편집
	void EditQuad();
	void EditCube();
	void EditGrid();
	void EditSphere();
	void EditCylinder();
	void EditTexture();
	//지형인식
	void QuadCheckHeight();
	void CubeCheckHeight();
	void GridCheckHeight();
	void SphereCheckHeight();
	void CylinderCheckHeight();

	void GetState();
	void SetState();

	void UnProject();
	void SetSelectFalse();
	void SetImgui(D3DXVECTOR3& picked, class Terrain* terrain);
	void SettingMesh(D3DXVECTOR3& picked, Terrain* terrain);
	void Delete(UINT meshType, UINT selectNum);
	void ShowCurrentTexture();

	void Save(wstring fileName);
	void Load(wstring fileName);
private:
	class Terrain* terrain;
	Shader* meshShader;
	Shader* shader;

	class InstanceMesh* quad;
	class InstanceMesh* cube;
	class InstanceMesh* grid;
	class InstanceMesh* sphere;
	class InstanceMesh* cylinder;
	class InstanceMesh* quadBox[5];
	class vector<InstanceMesh*> meshes;
	
	TextureArray* textures;

private:
	D3DXVECTOR3 tempPos;
	D3DXVECTOR3 tempScale;
	D3DXVECTOR3 tempRotation;
	D3DXCOLOR tempColor;
	D3DXVECTOR3 boxScale;
	//리사이즈
	D3DXVECTOR3 quadResize;
	D3DXVECTOR3 cubeResize;
	D3DXVECTOR3 gridResize;
	D3DXVECTOR3 sphereResize;
	D3DXVECTOR3 cylinderResize;
	//언프 변수
	D3DXMATRIX W, V, P;
	D3DXVECTOR3 start, direction;
	D3DXVECTOR3 n, f;

	UINT boxIndex;
	
	Transform* boxTrans;
	Transform* editTrans;
	int selectMeshTexture;  //메쉬 선택했을시 나타날 이미지
	int comboTexture; // 임구이 눌렀을시
	int instanceTexture;  //메쉬에 입혀질 이미지
	UINT selectMesh;
	float imageX;
	float imageY;
	int changeTexture;
private:
	Texture* quadImage;
	Texture* cubeImage;
	Texture* gridImage;
	Texture* sphereImage;
	Texture* cylinderImage;

	Texture* whiteImage;
	Texture* redImage;
	Texture* MagentaImage;
	Texture* boxImage;
	Texture* brickImage;
	Texture* stoneImage;
	Texture* floorImage;
	Texture* wallImage;
	vector<Texture*> currentImages;
private:
	bool isMeshSelect[5];
	bool isChange;
	bool isShowCurrentTex;

	int selectQuad;
	int selectCube;
	int selectGrid;
	int selectSphere;
	int selectCylinder;

	int saveIndex;
	int boxNum;

	UINT cubePass;
	UINT collPass;
	UINT shadowPass;
	UINT renderPass;

private:
	//라이트
	Shader* mShader;
	Material* box;
	vector<wstring> meshTextures;
	vector<wstring> specularTextures;
	vector<wstring> normalTextures;
	wstring specNames;

	D3DXCOLOR specular;

	vector<string> names; //콤보용
};