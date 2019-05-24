#pragma once

class Loader
{
public:
	Loader();
	~Loader();

	void ReadFile(wstring file);

public:
	void ExportMaterial(wstring savePath, bool bOverwrite = true); // 파일이 이미 있으면 덮어써


private:
	void ReadMaterial(); //scene으로부터 읽어옴
	void WriteMaterial(wstring savePath, bool bOverwrite);
	string WriteTexture(string file, string saveFolder);
public:
	void ExportMesh(wstring savePath, bool bOverwrite = true);  //매쉬를 부른다.

private:
	void ReadBoneData(aiNode* node, int index, int parent);  //본 데이타 부름, 재귀
	void ReadMeshData(aiNode* node, int bone); 
	void ReadSkinData();
	UINT FindBoneIndex(string name);

	void WriteMehData(wstring savePath, bool bOverwrite);

public:
	//한 클립에 동작 하나
	void GetClipList(vector<wstring>* list);
	void ExportAnimClip(UINT index, wstring savePath, bool bOverwrite = true);

private:
	struct AsClip* ReadClipData(aiAnimation* animation);
	void ReadKeyframeData(struct AsClip* clip, aiNode* node, vector<struct AsClipNode>& aiNodeInfos);
	struct AsClipNode* FindClipNode(vector<AsClipNode>& aniNodeInfos, aiString name);

	void WriteClipData(struct AsClip* clip, wstring savePath, bool bOverwrite);

private:
	wstring file;
	Assimp::Importer* importer;  // 불러들이는 역할
	const aiScene* scene; //assimp의 모든 자료형에는 앞에 ai가 붙는다.

	vector<struct AsMaterial*> materials; //기본 클래스가 아니니까 앞에다가 전방선언
	vector<struct AsBone*> bones;
	vector <struct AsMesh*> meshes;
};