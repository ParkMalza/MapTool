#include "Framework.h"
#include "Model.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "Model/ModelClip.h"
#include "Utilities/Xml.h"  //xml 받아오기 위한 헤더
#include "Utilities/BinaryFile.h"


Model::Model()
{
	HighValue = D3DXVECTOR3(0, 0, 0);
	LowValue = D3DXVECTOR3(0, 0, 0);
}

Model::~Model()
{
	for (Material* material : materials)
		SafeDelete(material);

	for (ModelBone* bone : bones)
		SafeDelete(bone);

	for (ModelMesh* mesh : meshes)
		SafeDelete(mesh);

	for (ModelClip* clip : clips)
		SafeDelete(clip);
}

Material * Model::MaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->Name() == name)
			return material;
	}

	return NULL;
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}

	return NULL;
}


ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}

	return NULL;
}

ModelClip * Model::ClipByName(wstring name)
{
	for (ModelClip* clip : clips)
	{
		if (clip->name == name)
			return clip;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Model::ReadMaterial(wstring file)
{
	//불러들여온 xml파일 읽는중...
	file = L"../../_Textures/" + file + L".material";  //뒤에  .material 추가

	Xml::XMLDocument* document = new Xml::XMLDocument();  //xml은 document로부터 시작한다.
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());  //정상적으로 불러지면 xml_success호출
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();  //document의 child . 맨위를 읽어들임 (버전들은 무시, 그 아래부터)
	Xml::XMLElement* materialNode = root->FirstChildElement();  //root의 child. root 다음 읽어들임

	// material이 몇개일지 모른다.
	do
	{
		// xml의 <Material> 아래 값들을 받아온다
		Material* material = new Material();


		Xml::XMLElement* node = NULL;  //값들 받아오기 위한 노드

		node = materialNode->FirstChildElement();  //node에서 내려가기 시작할꺼임 ㅋ
		material->Name(String::ToWString(node->GetText()));  //node의 이름 가져옴


		wstring directory = Path::GetDirectoryName(file);  //경로에서 디렉토리 명 들만 가지고 온다. 파일명 제외!
		String::Replace(&directory, L"../../_Textures", L"");  //텍스쳐 뺀거임, 우리 경로 붙이기 위해


		wstring texture = L"";
		//NextSiblingElement 은 같은 형제끼리 다음줄로 내려갈때 사용한다.
		node = node->NextSiblingElement();  //다음줄로 내려가
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)  //파일명이 있을때만 만들어준다
			material->DiffuseMap(directory + texture);  //Diffuse 읽었음

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->SpecularMap(directory + texture);  //Specular 읽었음

		node = node->NextSiblingElement();
		texture = String::ToWString(node->GetText());
		if (texture.length() > 0)
			material->NormalMap(directory + texture);  //Normal 읽었음


		D3DXCOLOR color;

		node = node->NextSiblingElement();  //역시나 다음으로
		// 우린 현재 그리는 과정이 ConstantBuffer를 거치는 형식으로 되어있다.
		
		// Ambient : 주위의, 은은한, 잔잔한 (네이버..ㅜ)
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Ambient(color); 
		// 이 부분에서 	material->Ambient(color) 를 타고 가보면
		//ConstantBuffer에 넣어주기 위한 desc의 D3DXCOLOR Ambient
		//에 값을 넣어주고 있다는것을 알 수 있다. 넣어준 값으로
		//material은 ConstantBuffer에 값을 밀어주고 있다.

		//diffuse : 널리퍼진, 분산된
		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Diffuse(color);
		//위의 Ambient와 같다.
		//Diffuse의 값을 넣어주고 material은 이를 ConstantBuffer에 밀어준다.

		//specular : 비추는, 반사하는
		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->Specular(color);
		//위와 동일하다. 
		//Specular의 값을 넣어주고 material은 이를 ConstantBuffer에 밀어준다.

		//역시나 material의 샤이니스에 Shininess의 값을 넣어주고
		//material은 ConstantBuffer로 밀어준다.

		materials.push_back(material);  //위의 과정을 거친 후 이를 vector로 관리 하기 위함

		materialNode = materialNode->NextSiblingElement();
	} while (materialNode != NULL);
}

void Model::ReadMesh(wstring file)
{
	file = L"../../_Models/" + file + L".mesh";  //이거 읽어!

	//
	BinaryReader* r = new BinaryReader();

	r->Open(file); // 열어서

	UINT count = 0;
	count = r->UInt();  //읽어낼 대상의 크기

	for (UINT i = 0; i < count; i++)  //크기만큼 읽어내서
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();

		bones.push_back(bone);  //받아온 다음 저장
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();

		mesh->name = String::ToWString(r->String());
		mesh->boneIndex = r->Int();

		//VertexData
		{
			UINT count = r->UInt();

			vector<Model::ModelVertex> vertices;
			vertices.assign(count, Model::ModelVertex());
			

			void* ptr = (void *)&(vertices[0]);
			r->Byte(&ptr, sizeof(Model::ModelVertex) * count);
			//미리 사이즈 만큼 공간 확보해놓고 읽어들인거임

			mesh->vertices = new Model::ModelVertex[count];
			mesh->vertexCount = count;
			copy
			(
				vertices.begin(), vertices.end(),
				stdext::checked_array_iterator<Model::ModelVertex *>(mesh->vertices, count)
				//벡터 데이터로 복사
			);
		}

		//IndexData
		{
			UINT count = r->UInt();

			vector<UINT> indices;
			indices.assign(count, UINT());

			void* ptr = (void *)&(indices[0]);
			r->Byte(&ptr, sizeof(UINT) * count);


			mesh->indices = new UINT[count];
			mesh->indexCount = count;
			copy
			(
				indices.begin(), indices.end(),
				stdext::checked_array_iterator<UINT *>(mesh->indices, count)
			);
		}


		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;

			meshPart->name = String::ToWString(r->String());
			meshPart->materialName = String::ToWString(r->String());

			meshPart->startVertex = r->UInt();
			meshPart->vertexCount = r->UInt();

			meshPart->startIndex = r->UInt();
			meshPart->indexCount = r->UInt();

			mesh->meshParts.push_back(meshPart);
		}//for(k)

		meshes.push_back(mesh);
	}//for(i)

	r->Close();
	SafeDelete(r);

	BindingBone();
	BindingMesh();  //연결시작
	SetHighLowVal();
}

void Model::ReadClip(wstring file)
{
	file = L"../../_Models/" + file + L".clip";

	BinaryReader* r = new BinaryReader();
	r->Open(file);


	ModelClip* clip = new ModelClip();

	clip->name = String::ToWString(r->String());
	clip->duration = r->Float();
	clip->frameRate = r->Float();
	clip->frameCount = r->UInt();

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->BoneName = String::ToWString(r->String());


		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->Transforms.assign(size, ModelKeyframeData());

			void* ptr = (void *)&keyframe->Transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyframeData) * size);
		}

		clip->keyframeMap[keyframe->BoneName] = keyframe;
	}

	r->Close();
	SafeDelete(r);

	clips.push_back(clip);
}

void Model::BindingBone()
{
	root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)  
		{
			bone->parent = bones[bone->parentIndex];  //자기 부모꺼 할당
			bone->parent->childs.push_back(bone);  //부모의 자식에다가 자기 할당
		}
		else
			bone->parent = NULL;//부모가 없으면
	}
}

void Model::BindingMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelBone* bone : bones)
		{
			if (mesh->boneIndex == bone->index)  //본 번호가지고 메쉬에 연결
			{
				mesh->bone = bone;

				break;
			}
		}

		mesh->Binding(this);
	}
}

void Model::SetHighLowVal()
{
	UINT tempCount = 0;

	for (ModelMesh* meshData : meshes)
	{
		UINT count = meshData->vertexCount;

		for (UINT i = 0; i < count; i++)
		{
			float tempX = meshData->vertices[i].Position.x;
			float tempY = meshData->vertices[i].Position.y;
			float tempZ = meshData->vertices[i].Position.z;

			if (HighValue.x <= tempX)
				HighValue.x = tempX;
			if (HighValue.y <= tempY)
				HighValue.y = tempY;
			if (HighValue.z <= tempZ)
				HighValue.z = tempZ;

			if (LowValue.x >= tempX)
				LowValue.x = tempX;
			if (LowValue.y >= tempY)
				LowValue.y = tempY;
			if (LowValue.z >= tempZ)
				LowValue.z = tempZ;
		}
	}
}
