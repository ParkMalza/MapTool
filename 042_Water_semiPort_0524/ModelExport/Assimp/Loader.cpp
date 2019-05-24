#include "stdafx.h"
#include "Loader.h"
#include "Type.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

Loader::Loader()
{
	importer = new Assimp::Importer();
}

Loader::~Loader()
{
	SafeDelete(importer);
}

void Loader::ReadFile(wstring file)
{
	this->file = L"../../_Assets/" + file; // 위에 위에 _Assets 파일로..
										   //우리는 wstring, Assimp는 string 형으로 넘겨주기 때문에 맞춰야함
										   //임포터가 파일을 읽어서 씬을 리턴해줌
	scene = importer->ReadFile(String::ToString(this->file),  //(경로명, 파일을 우리가 원하는형태로 읽어들이기 위함 
		aiProcess_ConvertToLeftHanded						  // = 우린 왼손좌표계를 쓴다
		| aiProcess_Triangulate   // | 우린 삼각형으로 그려왔다. 하지만 그렇지 않은 형태들도 존재한다. 따라서 삼각형으로 바꿔줘!
		| aiProcess_GenUVCoords   // | UV 가 없는 모델 들도 uv계산해서 넘겨줘!
		| aiProcess_GenNormals   	// | 노말 벡터가 없는 모델 계산해서 넘겨줘!
		| aiProcess_CalcTangentSpace);   	// | 나중에다뤄)

	assert(scene != NULL);
}

void Loader::ExportMaterial(wstring savePath, bool bOverwrite)
{
	//넘어온 savePath는 폴더명/파일명
	//저장 되는 경로명
	savePath = L"../../_Textures/" + savePath + L".material";  //텍스쳐 폴더에서~ savepath + .material 확장자 추가

	ReadMaterial();
	WriteMaterial(savePath, bOverwrite);
}

void Loader::ReadMaterial()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		//assimp의 모든 자료형에는 앞에 ai가 붙는다.
		aiMaterial* readMaterial = scene->mMaterials[i];  //이중포인터, 정확히는 포인터의 배열

														  //우리가 저장할 자료형
		AsMaterial* material = new AsMaterial();  //Type에서 우리가 정의한 AsMaterial 자료형

		material->Name = readMaterial->GetName().C_Str();

		//색상
		aiColor3D color; //알파 없어서 3D
		float val; // float 형으로도 리턴됨

		readMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);  //매크로형에 3개가 이미 정의되어있음
		material->Ambient = D3DXCOLOR(color.r, color.g, color.b, 1.0f);  //ai_real = float3

		readMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);  //매크로형에 3개가 이미 정의되어있음
		material->Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);  //ai_real = float3

		readMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);  //매크로형에 3개가 이미 정의되어있음
		readMaterial->Get(AI_MATKEY_SHININESS, val);  //매크로형에 3개가 이미 정의되어있음
		material->Specular = D3DXCOLOR(color.r, color.g, color.b, 1.0f);  //ai_real = float3

		aiString file;  //파일의 경로명을 저장

		readMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();  // Diffuse 우리가 매일 쓸 텍스쳐

		readMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		readMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();
		//scene에 있던 material의 정보를 우리것으로 모두 저장해놨음

		materials.push_back(material);
		//material
		//Name, Ambient, Diffuse, Specular, Shiness, DiffuseFile, SpecularFile, NormalFile

	}
}

void Loader::WriteMaterial(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false)  //덮어쓰지 않겠다(false) 하면 기본값은 true.
	{
		if (Path::ExistFile(savePath) == true) //지금 넘어온 경로의 파일이 존재하면 그대로 지나간다.
			return;
	}

	string folder = String::ToString(Path::GetDirectoryName(savePath)); //경로에서 디렉토리 명 들만 가지고 온다. 파일명 제외!
	string file = String::ToString(Path::GetFileName(savePath));  //파일명(확장자 포함)만 가져옴

																  //저장하려는 폴더가 없을 경우 만들어줌
	Path::CreateFolders(folder); //가면서 해당 경로가 없으면 다 만들어줌.

								 // Xml 은 document로부터 시작한다. 항상!! 문서!! 로부터 시작한다.
	Xml::XMLDocument* document = new Xml::XMLDocument();

	// Xml 파일을 사용하겠다.
	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);  //document가 최상단이고 그 아래 붙여라.

	Xml::XMLElement* root = document->NewElement("Materials"); //모든 데이터는 얘로 부터 시작한다. ("이름")
															   // Element는 밑으로 생기는거고, attribute는 오른쪽으로 생기는거
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");  //얘네 있어야 xml파일로 읽는다.
																		  //<Materials xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
	document->LinkEndChild(root);

	for (AsMaterial* material : materials)
	{
		//여기서부터 자식들? 생성
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);  //바로 위 root에 붙여준다.
								   //    <Material> 아래로 적을 준비

		Xml::XMLElement* element = NULL;  //node 아래 적힐 놈

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str()); //파일의 이름
		node->LinkEndChild(element);  //노드아래 붙여준다.

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(material->DiffuseFile, folder).c_str());
		node->LinkEndChild(element); //노드아래 붙여준다.

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(material->SpecularFile, folder).c_str());
		node->LinkEndChild(element); //노드아래 붙여준다.

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(material->NormalFile, folder).c_str());
		node->LinkEndChild(element); //노드아래 붙여준다.


		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r); //SetAttribute : 옆으로 붙는거~
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);  //노드의 아래로 붙여준다.

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);  //노드의 아래로 붙여준다.

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);  //노드의 아래로 붙여준다.

  //노드의 아래로 붙여준다.

  //같은 노드의 아래에 붙은 애들은 형제가 된다ㅏㅏㅏㅏㅏㅏㅏㅏㅏㅏ.!!!

  //현재까지의 진행도 --->  document(최상단) - decl(특이한 놈.. xml파일 사용하겠다)
  //                           |
  //                          root  (Materials)
  //                           |
  //                          node  (Material)  ... root아래 붙어있다. 역시 document의 원소이다.
  //                           |
  //  element (Name) / element (DiffuseFile) / element (SpecularFile) / element (NormalFile) 
  //  element (Ambient)  /  element (Diffuse)  /  element (Specular)  /  element (Shininess)   ..node아래 붙어있다. 역시 document의 원소이다.

		SafeDelete(material);  //내 파일에 다 옮겨졌다~ 따라서 삭제해준다.
	}

	document->SaveFile((folder + file).c_str());
}

string Loader::WriteTexture(string file, string saveFolder)
{
	if (file.length() < 1) return "";  //없으면 리턴

	string fileName = Path::GetFileName(file);  //파일명만 가져옴(확장자 포함)
												// aiTexture -> assimp의 텍스쳐 정보를 가지고 있음
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());  //내장된 텍스쳐(압축되어있는 텍스쳐)

	string savePath = "";
	if (texture != NULL) //Null이 아니면 내장된 텍스쳐다. (압축)
	{
		savePath = saveFolder + Path::GetFileNameWithoutExtension(file) + ".png"; //png 파일로만 받아올꺼야!

		if (texture->mHeight < 1)  // 픽셀정보가 다 있기 때문에 그대로 저장만 해주면 파일이 된다.
		{
			FILE* fp;
			fopen_s(&fp, savePath.c_str(), "wb"); // ( 주소, 파일명, 바이너리로 쓰겠다)
			fwrite(texture->pcData, texture->mWidth, 1, fp); // (임의의 정보, 가로줄 크기, 세로줄 1개는 나온다, 파일 포인트)
			fclose(fp);  //파일 열었으면 닫아.
		}
		else
		{   // 자유형식으로 저장되어 있는 형식. 아직 샘플이...
			ID3D11Texture2D* dest;
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = texture->pcData;


			HRESULT hr;
			hr = D3D::GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);
			Check(hr);

			D3DX11SaveTextureToFileA(D3D::GetDC(), dest, D3DX11_IFF_PNG, savePath.c_str());
		}
	}
	else  //널이면 외장이다
	{
		string directory = Path::GetDirectoryName(String::ToString(this->file));
		string origin = directory + file;
		String::Replace(&origin, "\\", "/");  // 역슬래시를 / 로 바꿔

		if (Path::ExistFile(origin) == false) //복사 할 원본 파일이 없다면 파일이 없는걸로 간주하고 리턴시킴
			return "";

		savePath = saveFolder + fileName;  //넘겨받아온 saveFolder + 파일명만으로 줄인 fileName 더해줌
		CopyFileA(origin.c_str(), savePath.c_str(), FALSE);

		String::Replace(&savePath, "../../_Textures/", "");  //뒤에 텍스쳐 제거
	}

	return Path::GetFileName(savePath);
}

void Loader::ExportMesh(wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Models/" + savePath + L".mesh";

	ReadBoneData(scene->mRootNode, -1, -1);  //재귀 형식이므로 root부터 탄다. 최초인덱스 -1, parent -1
	ReadSkinData();
	WriteMehData(savePath, bOverwrite);
}

void Loader::ReadBoneData(aiNode * node, int index, int parent)
{
	AsBone* bone = new AsBone();
	bone->Index = index;  //최초 -1
	bone->Parent = parent;  //최초 -1
	bone->Name = node->mName.C_Str();  //bone의 이름

									   // SIMD : 하나의 명령어로 여러가지 DATA를 처리 (열 우선)
									   // MIMD : 여러개의 명령어로 여러가지 DATA를 처리
									   // MISD : 여러개의 명령어로 한가지의 DATA를 처리 (효율성 낮아.. 이론으로만 존재)

									   //SIMD는 오른쪽부터 데이터가 채워진다. 열 우선.
									   //DX10 부터 SIMD(하나의 명령어로 여러가지 일을 처리)

	D3DXMATRIX transform(node->mTransformation[0]); //FLOAT* 형으로 받을 수 있다. 따라서 시작주소를 넣어주면 알아서 복사해준다.
	D3DXMatrixTranspose(&bone->Transform, &transform);  //우리가 쓰는건 행우선 이므로 전치해서 넣어줘야함

	D3DXMATRIX temp;
	if (parent == -1) //최상단 루트 , 부모없음
		D3DXMatrixIdentity(&temp);  //그냥 
	else
		temp = bones[parent]->Transform;  //부모의 트랜스폼 가져옴

	bone->Transform = bone->Transform * temp;  //나의 트랜스폼 다시 계산
	//본 중에 하나만 움직일 수도 있으므로 다시 계산해준거야

	bones.push_back(bone);
	ReadMeshData(node, index); //자신의 bone의 인덱스를 mesh에 넘겨줌
	for (UINT i = 0; i < node->mNumChildren; i++)  //재귀탄다. 자식꺼 모두 돌아줌
		ReadBoneData(node->mChildren[i], bones.size(), index);  // index -> 다음자식의 부모번호가 현재의 번호다.
}

void Loader::ReadMeshData(aiNode * node, int bone)
{
	if (node->mNumMeshes < 1) return;  //메쉬가 없을수도 있엉. 없으면 안그릴꺼임

	AsMesh* asMesh = new AsMesh();
	asMesh->Name = node->mName.C_Str(); //메쉬의 이름은 본의 이름과 같다
	asMesh->BoneIndex = bone;  //본하고 연결할 번호

	for (UINT i = 0; i < node->mNumMeshes; i++)  // 본에 참조될 메쉬는 여러개 일수도 있는 법이다.
	{
		UINT index = node->mMeshes[i]; //씬에 붙어있는 매쉬의 번호
		aiMesh* mesh = scene->mMeshes[index];  //메쉬 씬에서 받아옴

		UINT startVertex = asMesh->Vertices.size();
		UINT startIndex = asMesh->Indices.size();

		for (UINT m = 0; m < mesh->mNumVertices; m++) //메쉬안에 정점들 가져오기~
		{
			Model::ModelVertex vertex;
			memcpy(&vertex.Position, &mesh->mVertices[m], sizeof(D3DXVECTOR3));
			memcpy(&vertex.Uv, &mesh->mTextureCoords[0][m], sizeof(D3DXVECTOR2));  //2차원 배열인 이유 -->  텍스쳐 하나 위에 또 텍스쳐를 올리는 경우도 있기때문 (레이어 라고 한다.)
			memcpy(&vertex.Normal, &mesh->mNormals[m], sizeof(D3DXVECTOR3));
			memcpy(&vertex.Tangent, &mesh->mTangents[m], sizeof(D3DXVECTOR3));  //탄젠트 값은 자동으로 들어온다.

			asMesh->Vertices.push_back(vertex);
		}//for(m)

		for (UINT f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace& face = mesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				asMesh->Indices.push_back(face.mIndices[k]);
				asMesh->Indices.back() += startVertex;
			}
		}

		AsMeshPart* meshPart = new AsMeshPart();

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		meshPart->Name = mesh->mName.C_Str();
		meshPart->MaterialName = material->GetName().C_Str();
		meshPart->StartVertex = startVertex;
		meshPart->StartIndex = startIndex;
		meshPart->VertexCount = mesh->mNumVertices;
		meshPart->IndexCount = mesh->mNumFaces * mesh->mFaces->mNumIndices;
		//우리가 사용하는 인덱스 번호 구하는 공식.. 3 x mNumIndices(삼각형의 개수)
		//사각형 하나에서 인덱스가 6개.. 
		//face는 삼각형의 정점의 개수 = 3개
		//mNumIndices는 삼각형의 개수

		asMesh->MeshParts.push_back(meshPart);
	}

	meshes.push_back(asMesh);
}

void Loader::ReadSkinData()
{
	for (UINT i = 0; i < scene->mNumMeshes; i++) //메쉬 개수 1개
	{
		aiMesh* aiMesh = scene->mMeshes[i]; //ai~ : assimp 모델
		if (aiMesh->HasBones() == false) continue; //메쉬가 본이 없는 경우가 이따아~?!
		//본에 참조될 메쉬가 아닌 경우 ex) 검이라든가 검이라든가 검이라든가
		//나중에 추가될거임
		AsMesh* mesh = meshes[i];
		//Name : Kachujin
		//BoneIndex : 66개
		//Mesh : NULL
		//Vertices : 25850개
		//Indices : 37830개
		//MeshParts : 1개

		vector<AsBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), AsBoneWeights());
		UINT num = boneWeights.size();
		//정점의 가중치 vector. 미리 정점의 개수만큼 할당한다. 정점마다 가중치가 
		//들어 갈 것이기 때문에 미리 확보해 둔다?
		for (UINT b = 0; b < aiMesh->mNumBones; b++) //본의 개수만큼 가져오면서 가중치 계산 .. 왜? -> 가중치가 본 Matrix에 의해서 계산되기때문
		{	//카츄진의 Bone 개수는 53개네
			aiBone* aibone = aiMesh->mBones[b]; //ai ~ : Assimp 의 Bone임.. mBones[0]은 "Hip", mNumWeights = 4020, mVertexId = 2635, mWeight = 0.6의 근사치여따
			UINT boneIndex = FindBoneIndex(aibone->mName.C_Str()); //"Hip"의 인덱스 번호가 1?(부모노드 다음이어서 1인듯) readBone에서 다 받아온, 우리가 저장해 놓은 번호를 받아오는듯

			for (UINT w = 0; w < aibone->mNumWeights; w++) //mNumWeights == 4020 -> 본의 가중치가 몇개 있느냐?
			{
				//최대 4개다
				UINT index = aibone->mWeights[w].mVertexId; // w == 0 일 때, index == 2653 == 가중치가 들어갈 정점의 번호!
				float weight = aibone->mWeights[w].mWeight;  // 아까 본 0.6의 근사치 값 받아왔네
				//결론 : 2653번(mVertexId)의 정점번호에다가 0.6정도(mWeight)의 근사치 값을 넣어주겠다!
				boneWeights[index].AddWeights(boneIndex, weight); //본의 인덱스와 가중치 넣어줌
			}
		}//for(b)  //정점에 가중치 모두 넣어줬음

		//우리꺼의 정점에다 가중치를 넣는다??
		for (UINT w = 0; w < boneWeights.size(); w++)  //boneWeights.size == 25850
		{
			boneWeights[w].Normalize(); //전체 가중치의 합은 1이된다.
			AsBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight); //blendWeight에다가 가중치 리턴해준다

			mesh->Vertices[w].BlendIndices = blendWeight.Indices; //이거 asMesh로 우리꺼임. 우리꺼에다가 다 받아옴
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

UINT Loader::FindBoneIndex(string name)  //"Hip"이 넘어왔네
{
	for (AsBone* bone : bones) //as~ : 우리가 이미 다 받아놓은 Bone임. bones.size() == 67임. 아마도 맨 상위 부모노드로 인해 위에서 BoneIndex 66개 + 부모노드 1개 된듯
	{
		if (bone->Name == name)
			return bone->Index; //"Hip" 일때 부모노드 다음이니까 1 넘겨주네요
	}

	return 0;
}

void Loader::WriteMehData(wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false)  //덮어쓰지 않겠다 하면 기본값은 true.
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter * w = new BinaryWriter();  //쓸때는 writer, 읽을때는 reader

	w->Open(savePath);  //파일을 열기 위해

	w->UInt(bones.size());

	//AsBone과 똑같이.. 순서지켜서
	for (AsBone* bone : bones)  //본의 개수만큼 돌면서 가져오기
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		SafeDelete(bone);
	}

	w->UInt(meshes.size());
	for (AsMesh* meshData : meshes)
	{
		//매쉬
		w->String(meshData->Name);
		w->Int(meshData->BoneIndex);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::ModelVertex) * meshData->Vertices.size());
		//바이트 단위로 저장하겠다

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());

		//매쉬 파트
		w->UInt(meshData->MeshParts.size());
		for (AsMeshPart* part : meshData->MeshParts)
		{
			w->String(part->Name);
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCount);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);

			SafeDelete(part);
		}

		SafeDelete(meshData);
	}

	w->Close();
	SafeDelete(w);
}

//한 클립에 동작 하나
void Loader::GetClipList(vector<wstring>* list)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		list->push_back(String::ToWString(anim->mName.C_Str()));

	}
}

void Loader::ExportAnimClip(UINT index, wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Models/" + savePath + L".clip"; // Kachujin/Idle
	//scene->mAnimations 
	//mName = mixamo.com
	//mDuration = 59
	//mTicksperSecond = 30
	//mNumChannels = 42
	AsClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath, bOverwrite);
}

AsClip * Loader::ReadClipData(aiAnimation * animation)
{
	AsClip* clip = new AsClip();
	clip->Name = animation->mName.C_Str(); // mixamo.com
	clip->FrameRate = (float)animation->mTicksPerSecond; //30
	clip->FrameCount = (UINT)animation->mDuration + 1; //무조건 올림 해버림, 60

	vector<AsClipNode> aniNodeInfos;//최상 루트에서 아래로 내려오면서 계산하는 방식을 사용 할 것이다. 정운동학(Forward Kinematics)사용
									//역운동학(Inverse Kinematics)도 있음 : 거의 대부분 엔진은 IK를 사용한다.
									//캐릭터가 산같은곳을 올라갈때 역운동학은 발이 정확하게 땅에 붙을 수 있다. 끝에서부터 루트로 계산해서 올라간다.

	// tweening : 한동작에서 다음동작으로 까지 스무스 하게 넘어가게 하기 위함
	// blending : 애니메이션끼리 섞어서 사용한다. 걷는것과 손을 흔드는 동작을 동시에 할 수 있다. 2개의 채널을 동시에 돌림
	//채널은 Blending을 위해 사용된다.
	for (UINT i = 0; i < animation->mNumChannels/*42개*/; i++) 
	{													
		aiNodeAnim* aniNode = animation->mChannels[i]; //0번 "Head" , 각3개 키 모두 60개

		AsClipNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName; //본 이름 저장해두기 , 0번 "Head" 이름 가지고 키프레임 저장할거임ㅋ

		//키 값중 맥스만 보관한 다음, 그 키값을 기준으로 모자르면 채워넣을것이다. 항상 키값이 존재하는걸로!
		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumRotationKeys); //60
		keyCount = max(keyCount, aniNode->mNumScalingKeys);

		AsKeyframeData frameData;
		for (UINT k = 0; k < keyCount; k++)
		{
			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size(); //0
			//fabsf : 절대값
			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON) // 아주작은 수 Epsilon보다 그 차가 작다면 키 값이 존재한다고 간주
			{
				aiVectorKey posKey = aniNode->mPositionKeys[k]; //키 받아옴
				memcpy_s(&frameData.Translation, sizeof(Vector3), &posKey.mValue, sizeof(aiVector3D));
				frameData.Time = (float)aniNode->mPositionKeys[k].mTime;

				bFound = true;
			}

			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON) //
			{
				aiQuatKey Key = aniNode->mRotationKeys[k];
				//쿼터니온은 복사하면 안된다.. 이유는 모름.. 찾아보자
				frameData.Rotation.x = Key.mValue.x;  //키값 받아오는중입니다...
				frameData.Rotation.y = Key.mValue.y;
				frameData.Rotation.z = Key.mValue.z;
				frameData.Rotation.w = Key.mValue.w;

				frameData.Time = (float)aniNode->mRotationKeys[k].mTime;  //0

				bFound = true;
			}

			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON) //
			{
				aiVectorKey posKey = aniNode->mScalingKeys[k];
				memcpy_s(&frameData.Scale, sizeof(Vector3), &posKey.mValue, sizeof(aiVector3D));
				frameData.Time = (float)aniNode->mScalingKeys[k].mTime;

				bFound = true;
			}
			if (bFound == true)
				aniNodeInfo.Keyframe.push_back(frameData);
		}
		if (aniNodeInfo.Keyframe.size() < clip->FrameCount)  //전체프레임 수보다 작으면 다 안채워진거야.. 그래서 채울꺼임
		{
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();
			AsKeyframeData keyframe = aniNodeInfo.Keyframe.back();
			
			for (UINT i = 0; i < count; i++)
				aniNodeInfo.Keyframe.push_back(keyframe);  //마지막 keyframe으로 남은 count만큼 채워넣는다
			
		}
		clip->Duration = max(clip->Duration, aniNodeInfo.Keyframe.back().Time);

		aniNodeInfos.push_back(aniNodeInfo);  //노드별로 키프레임 정보 저장해 주는곳이다
	}
	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos); //재귀! 우리꺼에 저장시작

	return clip;
}

void Loader::ReadKeyframeData(AsClip * clip, aiNode * node, vector<struct AsClipNode>& aiNodeInfos)
{
	AsKeyframe* keyframe = new AsKeyframe();
	keyframe->BoneName = node->mName.C_Str();  // "RootNode" 부터 시작하네, 다음 "Hips"

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		AsClipNode* aniNode = FindClipNode(aiNodeInfos, node->mName);

		AsKeyframeData frameData;
		if (aniNode != NULL)
		{
			frameData = aniNode->Keyframe[i];
		}
		else
		{
			Matrix transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform); //우리는 행 우선이라서

			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform); //분해
			frameData.Time = (float)i;
		}
		keyframe->Transforms.push_back(frameData);; //실제 운용할 데이타?
	}

	clip->Keyframes.push_back(keyframe);

	for (UINT i = 0; i < node->mNumChildren; i++) //mNumChildren : 뭘 읽었냐에 따라 계속 다름
		ReadKeyframeData(clip, node->mChildren[i], aiNodeInfos);
}

AsClipNode * Loader::FindClipNode(vector<AsClipNode>& aniNodeInfos, aiString name)
{
	for (UINT i = 0; i < aniNodeInfos.size(); i++)
	{
		if (aniNodeInfos[i].Name == name)
			return &aniNodeInfos[i];
	}
	return NULL;
}

void Loader::WriteClipData(AsClip * clip, wstring savePath, bool bOverwrite)
{
	if (bOverwrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}


	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (AsKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(AsKeyframeData) * keyframe->Transforms.size());

		SafeDelete(keyframe);
	}

	w->Close();
	SafeDelete(w);
}
