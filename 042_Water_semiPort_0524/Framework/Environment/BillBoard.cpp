#include "Framework.h"
#include "BillBoard.h"
#include "Environment/Terrain.h"
#include "Utilities/BinaryFile.h"

BillBoard::BillBoard(Shader * shader, vector<wstring>& textureNames, Terrain* terrain)
	:Renderer(shader), fixedY(false)
{
	this->terrain = terrain;
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	textures = new TextureArray(textureNames, 256, 256);
	shader->AsSRV("Map")->SetResource(textures->SRV());
	//사진 외부에서 참조해보려고..
	//0번
	wstring temp = Path::GetFileName(textureNames[0]);
	temp = L"Grass/" + temp;
	grass = new Texture(temp);
	//1번
	temp = Path::GetFileName(textureNames[1]);
	temp = L"Grass/" + temp;
	grass1 = new Texture(temp);
	//2번
	temp = Path::GetFileName(textureNames[2]);
	temp = L"Grass/" + temp;
	grass2 = new Texture(temp);
	//3번
	temp = Path::GetFileName(textureNames[3]);
	temp = L"Grass/" + temp;
	grass3 = new Texture(temp);
	//4번
	temp = Path::GetFileName(textureNames[4]);
	temp = L"Grass/" + temp;
	grass4 = new Texture(temp);

}

BillBoard::~BillBoard()
{
	SafeDelete(grass4);
	SafeDelete(grass3);
	SafeDelete(grass2);
	SafeDelete(grass1);
	SafeDelete(grass);
	SafeDelete(textures);
}

void BillBoard::Add(D3DXVECTOR3 & position, D3DXVECTOR2 & scale)
{
	VertexScale vertex;
	vertex.Position = position;
	vertex.scale = scale;

	vertices.push_back(vertex);
}

void BillBoard::Add(D3DXVECTOR3 & position, D3DXVECTOR2 & scale, UINT & type)
{
	VertexScale vertex;
	vertex.Position = position;
	vertex.scale = scale;
	vertex.type = type;

	vertices.push_back(vertex);
}

void BillBoard::CreateBuffer()
{
	vertexBuffer = NULL;
	assert(vertexBuffer == NULL);

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexScale));
}

void BillBoard::Update()
{
	Renderer::Update();
	for (UINT i = 0; i < vertices.size(); i++)
	{
		float y = terrain->GetPickedHeight(vertices[i].Position);
		vertices[i].Position.y = y + (vertices[i].scale.y * 0.5f);
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, &vertices[0], sizeof(VertexScale) * vertices.size());
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}
}

void BillBoard::Render()
{
	Renderer::Render();
	shader->Draw(0, Pass(), vertices.size());
}

void BillBoard::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->Int(vertices.size());
	for (VertexScale v : vertices)
	{
		w->Vector3(v.Position);
		w->Vector2(v.scale);
		w->Int(v.type);
	}
	w->Close();
}

void BillBoard::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);

	int count = r->Int();
	for (int i = 0; i < count; i++)
	{
		VertexScale v;
		v.Position = r->Vector3();
		v.scale = r->Vector2();
		v.type = r->Int();
		vertices.push_back(v);

		vertexBuffer = NULL;
		assert(vertexBuffer == NULL);

		vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexScale));
	}
	r->Close();
}

void BillBoard::MakeNewGrass()
{

}

void BillBoard::SetOldBillBoard()
{
	oldBillBorad.clear();
	//누르기 직전 상태를 저장한다. 가장 최근 상태
	for (VertexScale billBoard : vertices)
	{
		VertexScale temp;
		temp.Position = billBoard.Position;
		temp.scale= billBoard.scale;
		temp.type= billBoard.type;

		oldBillBorad.push_back(temp);
	}
	//마우스 클릭시 UndoStack에 쌓는다
	undoStack.push_back(oldBillBorad);
	//인터넷에서 구글 -> 네이버 -> 뒤로가기 -> 다음 가면 네이버는 다시 못감
	if (redoStack.size() > 0)
		redoStack.clear();
}

void BillBoard::MakeOldBillBoard()
{
	oldBillBorad.clear();

	for (VertexScale billBoard : vertices)
	{
		VertexScale temp;
		temp.Position = billBoard.Position;
		temp.scale = billBoard.scale;
		temp.type = billBoard.type;

		oldBillBorad.push_back(temp);
	}
}

void BillBoard::MakeNewBillBoard()
{
	newBillBorad.clear();

	for (VertexScale billBoard : vertices)
	{
		VertexScale temp;
		temp.Position = billBoard.Position;
		temp.scale= billBoard.scale;
		temp.type = billBoard.type;

		newBillBorad.push_back(temp);
	}
}

void BillBoard::UndoBillBoard()  //c + z
{
	if (undoStack.empty())
		return;

	MakeNewBillBoard();
	redoStack.push_back(newBillBorad);  //현재의 상태를 새로 생성해 redo해 쌓는다

	it = undoStack.end() - 1;

	vertices.clear();

	for (VertexScale billBoard : *it)
	{
		VertexScale temp;

		temp.Position = billBoard.Position;
		temp.scale = billBoard.scale;
		temp.type= billBoard.type;

		vertices.push_back(temp);
	}
	undoStack.erase(it);
}

void BillBoard::RedoBillBoard()  //c + y
{
	if (redoStack.empty())
		return;

	MakeOldBillBoard();
	undoStack.push_back(oldBillBorad);

	it = redoStack.end() - 1;

	vertices.clear();

	for (VertexScale billBoard : *it)
	{
		VertexScale temp;

		temp.Position = billBoard.Position;
		temp.scale = billBoard.scale;
		temp.type = billBoard.type;

		vertices.push_back(temp);
	}
	redoStack.erase(it);
}
