#include "Framework.h"
#include "Terrain.h"
#include "Viewer/Viewport.h"
#include "Utilities/BinaryFile.h"

Terrain::Terrain(Shader * shader, wstring heightMap, wstring alphaMap, vector<wstring>& textureNames)
	: Renderer(shader)
	, baseMap(NULL), layerMap(NULL), alphaMap(NULL)
	,cubeMapPass(5),shadowPass(6), renderPass(7)
{
	this->heightMap = new Texture(heightMap);
	this->alphaMap = new Texture(alphaMap);
	alpha = 1;
	sIndex = shader->AsScalar("LayerAlpha");
	//사진 받아오기
	//1번
	wstring temp = Path::GetFileName(textureNames[0]);
	temp = L"Terrain/" + temp;
	layer[0] = new Texture(temp);
	//2번
	temp = Path::GetFileName(textureNames[1]);
	temp = L"Terrain/" + temp;
	layer[1] = new Texture(temp);
	//3번
	temp = Path::GetFileName(textureNames[2]);
	temp = L"Terrain/" + temp;
	layer[2] = new Texture(temp);
	//4번
	temp = Path::GetFileName(textureNames[3]);
	temp = L"Terrain/" + temp;
	layer[3] = new Texture(temp);

	baseTextureName = textureNames.at(0);
	alphaTextureName = alphaMap;

	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(TerrainVertex), 0);
	indexBuffer = new IndexBuffer(indices, indexCount);

	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_Brush");

	gridLineBuffer = new ConstantBuffer(&gridLineDesc, sizeof(GridLineDesc));
	sGridLineBuffer = shader->AsConstantBuffer("CB_GridLine");

	layersNames = textureNames;
	layerMaps = new TextureArray(layersNames, 256, 256);  //경로(../) 붙어서 layerMaps에 저장됨

	sBaseMap = shader->AsSRV("BaseMap");
	sLayerMap = shader->AsSRV("LayerMap");
	sAlphaMap = shader->AsSRV("AlphaMap");
	sLayerMaps = shader->AsSRV("LayerMaps");

}

Terrain::~Terrain()
{
	SafeDelete(layerMaps);
	SafeDelete(alphaMap);
	SafeDelete(layerMap);
	SafeDelete(baseMap);

	SafeDelete(gridLineBuffer);
	SafeDelete(brushBuffer);

	SafeDeleteArray(indices);
	SafeDeleteArray(vertices);

	SafeDelete(heightMap);
}

void Terrain::Update()
{
	Renderer::Update();
	sIndex->SetFloat(alpha);
}

void Terrain::PreRender()
{
	Renderer::Render();
	shader->DrawIndexed(0, shadowPass, indexCount);
}

void Terrain::CubeRender()
{
	Renderer::Render();
	if (sBrushBuffer != NULL)
	{
		brushBuffer->Apply();
		sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());
	}

	if (sGridLineBuffer != NULL)
	{
		gridLineBuffer->Apply();
		sGridLineBuffer->SetConstantBuffer(gridLineBuffer->Buffer());
	}

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL /*&& alphaMap != NULL*/)
	{
		sLayerMap->SetResource(layerMap->SRV());
		//sAlphaMap->SetResource(alphaMap->SRV());
	}
	if (layerMaps != NULL)
		sLayerMaps->SetResource(layerMaps->SRV());


	if (alphaMap != NULL)
		sAlphaMap->SetResource(alphaMap->SRV());
	shader->DrawIndexed(0, cubeMapPass, indexCount);
}

void Terrain::Render()
{
	Renderer::Render();

	if (sBrushBuffer != NULL)
	{
		brushBuffer->Apply();
		sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());
	}

	if (sGridLineBuffer != NULL)
	{
		gridLineBuffer->Apply();
		sGridLineBuffer->SetConstantBuffer(gridLineBuffer->Buffer());
	}

	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (layerMap != NULL /*&& alphaMap != NULL*/)
	{
		sLayerMap->SetResource(layerMap->SRV());
		//sAlphaMap->SetResource(alphaMap->SRV());
	}
	if (layerMaps != NULL)
		sLayerMaps->SetResource(layerMaps->SRV());


	if (alphaMap != NULL)
		sAlphaMap->SetResource(alphaMap->SRV());

	ImGui::Separator();
	//ImGui::Text("Terrain");
	//ImGui::SliderInt("Pass", (int *)&Pass(), 0, 1);
	shader->DrawIndexed(0, renderPass, indexCount);
}


float  Terrain::GetHeight(D3DXVECTOR3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return -1.0f;
	if (z < 0 || z > height) return -1.0f;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;


	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;
	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}


	return temp.y;
}

float Terrain::GetPickedHeight(D3DXVECTOR3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x > width) return -1.0f;
	if (z < 0 || z > height) return -1.0f;


	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + x + 1;
	index[3] = width * (z + 1) + x + 1;

	D3DXVECTOR3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;


	float u, v, distance;

	D3DXVECTOR3 start(position.x, 1000, position.z);
	D3DXVECTOR3 direction(0, -1, 0);

	D3DXVECTOR3 result(-1, -1, -1);
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

	return result.y;
}

D3DXVECTOR3 Terrain::GetPickedHeight()
{
	D3DXVECTOR3 start, direction;
	D3DXMATRIX V = Context::Get()->ViewMatrix();
	D3DXMATRIX P = Context::Get()->ProjectionMatrix();

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR3 n, f;

	mouse.z = 0.0f;
	Context::Get()->GetViewport()->Unproject(&n, mouse, transform->World(), V, P);

	mouse.z = 1.0f;
	Context::Get()->GetViewport()->Unproject(&f, mouse, transform->World(), V, P);

	direction = f - n;
	start = n;

	//Context::Get()->GetViewport()->GetRay(&start, &direction, world, V, P);

	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + x + 1;
			index[3] = width * (z + 1) + x + 1;

			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = vertices[index[i]].Position;


			float u, v, distance;

			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE)
				return p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		}
	}

	return D3DXVECTOR3(-1, -1, -1);
}

void Terrain::RaiseHeight(D3DXVECTOR3 & position, UINT type, UINT range, float speed)
{
	if (position.x < 0 || position.x > width || position.z < 0 || position.z > height)
		return;
	D3D11_BOX box;
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;


	if (type == 1)
	{
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				UINT index = width * z + x;
				vertices[index].Position.y += speed * Time::Delta();
			}
		}
	}
	else if (type == 2)
	{
		UINT radius = range;
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				UINT index = width * z + x;
				float dx = x - position.x;
				float dz = z - position.z;

				float dist = sqrt(dx * dx + dz * dz);
				if (dist <= radius)
				{
					vertices[index].Position.y += speed * Time::Delta();
				}
			}
		}
	}
	//CreateNormalData(); //높이가 바뀌었으니 노말데이터도 바뀐다! 다시 계산
	Mapping();
	
}

void Terrain::FallDownHeight(D3DXVECTOR3 & position, UINT type, UINT range, float speed)
{
	if (position.x < 0 || position.x > width || position.z < 0 || position.z > height)
		return;
	D3D11_BOX box;
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;

	if (type == 1)
	{
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				UINT index = width * z + x;

				vertices[index].Position.y -= speed * Time::Delta();

			}
		}
	}
	else if (type == 2)
	{
		UINT radius = range;
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				UINT index = width * z + x;
				float dx = x - position.x;
				float dz = z - position.z;

				float dist = sqrt(dx * dx + dz * dz);
				if (dist <= radius)
				{
					vertices[index].Position.y -= speed * Time::Delta();
				}
			}
		}
	}

	//CreateNormalData();
	Mapping();
}

void Terrain::LockHeight(D3DXVECTOR3 & position, UINT type, UINT range, float _height, float speed)
{
	if (position.x < 0 || position.x > width || position.z < 0 || position.z > height)
		return;
	D3D11_BOX box;
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;

	if (type == 1)
	{
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				UINT index = width * z + x;


				if (vertices[index].Position.y < _height)
				{
					vertices[index].Position.y += speed * Time::Delta();

					if (vertices[index].Position.y > _height)
						vertices[index].Position.y = _height;
				}
				else if (vertices[index].Position.y > _height)
				{
					vertices[index].Position.y -= speed * Time::Delta();

					if (vertices[index].Position.y < _height)
						vertices[index].Position.y = _height;
				}
			}
		}
	}
	else if (type == 2)
	{
		UINT radius = range;
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				UINT index = width * z + x;

				float dx = x - position.x;
				float dz = z - position.z;

				float dist = sqrt(dx * dx + dz * dz);

				if (dist <= radius)
				{
					if (vertices[index].Position.y < _height)
					{
						vertices[index].Position.y += speed * Time::Delta();

						if (vertices[index].Position.y > _height)
							vertices[index].Position.y = _height;
					}
					else if (vertices[index].Position.y > _height)
					{
						vertices[index].Position.y -= speed * Time::Delta();

						if (vertices[index].Position.y < _height)
							vertices[index].Position.y = _height;
					}
				}
			}
		}
	}
	//CreateNormalData();
	Mapping();
}

void Terrain::Smoothing(D3DXVECTOR3 & position, UINT type, UINT range, float speed)
{
	if (position.x < 0 || position.x > width || position.z < 0 || position.z > height)
		return;
	D3D11_BOX box;
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;

	if (type == 1)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			for (UINT z = box.bottom; z <= box.top; z++)
			{	
				//UINT index = width * z + x;
				int adjacentSections = 0;
				float sectionsTotal = 0.0f;

				if ((x - 1) > box.left) // Check to left
				{	//x - 1, z
					sectionsTotal += vertices[width * z + (x-1)].Position.y;
					adjacentSections++;

					if ((z - 1) > box.bottom) // Check up and to the left
					{  //x - 1, z - 1
						sectionsTotal += vertices[width * (z-1) + (x - 1)].Position.y;
						adjacentSections++;
					}

					if ((z + 1) < box.top) // Check down and to the left
					{  //x - 1, z + 1
						sectionsTotal += vertices[width * (z + 1) + (x-1)].Position.y;
						adjacentSections++;
					}
				}
				if ((x + 1) < box.right) // Check to right
				{
					//UINT index = width * z + x;
					//x + 1, z
					sectionsTotal += vertices[width * z + (x + 1)].Position.y;
					adjacentSections++;

					if ((z - 1) > box.bottom) // Check up and to the right
					{  //x + 1, z - 1
						sectionsTotal += vertices[width * (z - 1) + (x + 1)].Position.y;
						adjacentSections++;
					}

					if ((z + 1) < box.top) // Check down and to the right
					{  //x + 1, z + 1
						sectionsTotal += vertices[width * (z + 1) + (x + 1)].Position.y;
						adjacentSections++;
					}
				}
				if ((z - 1) > box.bottom) // Check above
				{
					//UINT index = width * z + x;
					//x, z - 1
					sectionsTotal += vertices[width * (z - 1) + x].Position.y;
					adjacentSections++;
				}

				if ((z + 1) < box.top) // Check below
				{
					//x, z + 1
					sectionsTotal += vertices[width * (z + 1) + x].Position.y;
					adjacentSections++;
				}

				vertices[width * z + x].Position.y = (vertices[width * z + x].Position.y + (sectionsTotal / adjacentSections)) * 0.5f;

			}
		}
	}
	else if (type == 2)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			for (UINT z = box.bottom; z <= box.top; z++)
			{
				UINT index = width * z + x;
				float dx = x - position.x;
				float dz = z - position.z;

				float dist = sqrt(dx * dx + dz * dz);
				if (dist <= range)
				{
					int adjacentSections = 0;
					float sectionsTotal = 0.0f;

					if ((x - 1) > box.left) // Check to left
					{	//x - 1, z
						sectionsTotal += vertices[width * z + (x - 1)].Position.y;
						adjacentSections++;

						if ((z - 1) > box.bottom) // Check up and to the left
						{  //x - 1, z - 1
							sectionsTotal += vertices[width * (z - 1) + (x - 1)].Position.y;
							adjacentSections++;
						}

						if ((z + 1) < box.top) // Check down and to the left
						{  //x - 1, z + 1
							sectionsTotal += vertices[width * (z + 1) + (x - 1)].Position.y;
							adjacentSections++;
						}
					}
					if ((x + 1) < box.right) // Check to right
					{
						//UINT index = width * z + x;
						//x + 1, z
						sectionsTotal += vertices[width * z + (x + 1)].Position.y;
						adjacentSections++;

						if ((z - 1) > box.bottom) // Check up and to the right
						{  //x + 1, z - 1
							sectionsTotal += vertices[width * (z - 1) + (x + 1)].Position.y;
							adjacentSections++;
						}

						if ((z + 1) < box.top) // Check down and to the right
						{  //x + 1, z + 1
							sectionsTotal += vertices[width * (z + 1) + (x + 1)].Position.y;
							adjacentSections++;
						}
					}
					if ((z - 1) > box.bottom) // Check above
					{
						//UINT index = width * z + x;
						//x, z - 1
						sectionsTotal += vertices[width * (z - 1) + x].Position.y;
						adjacentSections++;
					}

					if ((z + 1) < box.top) // Check below
					{
						//x, z + 1
						sectionsTotal += vertices[width * (z + 1) + x].Position.y;
						adjacentSections++;
					}

					vertices[width * z + x].Position.y = (vertices[width * z + x].Position.y + (sectionsTotal / adjacentSections)) * 0.5f;

				}

			}
		}
	}
	CreateNormalData();
	Mapping();
}

void Terrain::BaseMap(wstring file)
{
	SafeDelete(baseMap);
	baseTextureName = L"";
	baseTextureName = file;
	
	baseMap = new Texture(file);
}

void Terrain::GetBaeMap(Texture ** texture)
{
	*texture = baseMap;
}

void Terrain::LayerMap1(Texture ** texture)
{
	*texture = layer[0];
}

void Terrain::LayerMap2(Texture ** texture)
{
	*texture = layer[1];
}

void Terrain::LayerMap3(Texture ** texture)
{
	*texture = layer[2];
}

void Terrain::LayerMap4(Texture ** texture)
{
	*texture = layer[3];
}

void Terrain::LayerMap(wstring file)
{
	SafeDelete(layerMap);

	layerTextureName = L"";
	layerTextureName = file;

	layerMap = new Texture(file);
}

void Terrain::LayerMaps(wstring file, int i)
{
	SafeDelete(layerMaps);

	for (int a = 0; a < 5; a++)
	{
		layersNames[a] = Path::GetFileName(layersNames[a]); // textures에 저장된 이미지들 파일이름만 떼기
		layersNames[a] = L"Terrain/" + layersNames[a];  // L"Terrain/" 붙여서 저장
	}
	
	if (i < 4)
	{
		SafeDelete(layer[i]);
		layer[i] = new Texture(file);
	}
	
	layersNames[i] = file;
	layerMaps = new TextureArray(layersNames);  //새로이 저장
}

void Terrain::BrushLayer(D3DXVECTOR3 & position, UINT type, UINT range, float power, UINT layerNum)
{
	D3D11_BOX box;
	box.left = (UINT)position.x - range;
	box.top = (UINT)position.z + range;
	box.right = (UINT)position.x + range;
	box.bottom = (UINT)position.z - range;

	if (box.left < 0) box.left = 0;
	if (box.top >= height) box.top = height;
	if (box.right >= width) box.right = width;
	if (box.bottom < 0) box.bottom = 0;

	for (UINT z = box.bottom; z <= box.top; z++)
	{
		for (UINT x = box.left; x <= box.right; x++)
		{
			UINT index = width * z + x;

			if (type == 1)
			{
				switch (layerNum)
				{
					case 0:
						vertices[index].Color.r = alpha;
						break;
					case 1:
						vertices[index].Color.g = alpha;
						break;
					case 2:
						vertices[index].Color.b = alpha;
						break;
					case 3:
						vertices[index].Color.a = alpha;
						break;
					case 4:
						vertices[index].Color = Color(0,0,0,0);
						break;
				}
			}
			else if (type == 2)
			{
				float dx = position.x - x;
				float dz = position.z - z;
				float dist = sqrtf(dx * dx + dz * dz);

				if (dist <= range)
				{
					switch (layerNum)
					{
						case 0:
							vertices[index].Color.r = alpha ;
							break;
						case 1:
							vertices[index].Color.g = alpha;
							break;
						case 2:
							vertices[index].Color.b = alpha;
							break;
						case 3:
							vertices[index].Color.a = alpha;
							break;
						case 4:
							vertices[index].Color = Color(0, 0, 0, 0);
							break;
					}
				}
			}
		}
	}

	Mapping();
}

void Terrain::SettingAlpha(float alpha)
{
	this->alpha = alpha;
}

void Terrain::Save(wstring fileName)
{
	wstring savePath = L"../BinaryFolder/" + fileName;

	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);
	w->Float(alpha);
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			w->Float(vertices[index].Position.y);
			w->Float(vertices[index].Color.r);
			w->Float(vertices[index].Color.g);
			w->Float(vertices[index].Color.b);
			w->Float(vertices[index].Color.a);
		}
	}
	
	w->String(String::ToString(baseTextureName));
	w->String(String::ToString(layerTextureName));
	w->String(String::ToString(alphaTextureName));
	for (int i = 0; i < 5; i++)
	{
		layersNames[i] = Path::GetFileName(layersNames[i]);
		w->String(String::ToString(layersNames[i])); 
	}

	w->Close();
}

void Terrain::Load(wstring fileName)
{
	wstring loadFile = L"../BinaryFolder/" + fileName;

	BinaryReader* r = new BinaryReader();
	r->Open(loadFile);
	alpha = r->Float();
	for (UINT i = 0; i < vertexCount; i++)
	{
		vertices[i].Position.y = r->Float();
		vertices[i].Color.r = r->Float();
		vertices[i].Color.g = r->Float();
		vertices[i].Color.b = r->Float();
		vertices[i].Color.a = r->Float();
	}


	baseTextureName = String::ToWString(r->String());
	layerTextureName = String::ToWString(r->String());
	alphaTextureName = String::ToWString(r->String());

	for (int i = 0; i < 5; i++)
	{
		layersNames[i] = String::ToWString(r->String());
		layersNames[i] = L"Terrain/" + layersNames[i];
	}


	BaseMap(baseTextureName);
	layerMap = new Texture(layerTextureName);
	alphaMap = new Texture(alphaTextureName);
	
	SafeDelete(layerMaps);
	layerMaps = new TextureArray(layersNames);

	CreateNormalData();
	Mapping();

	r->Close();
}

void Terrain::Mapping()
{
	//vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(TerrainVertex), 0);
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy(subResource.pData, vertices, sizeof(TerrainVertex) * vertexCount);
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
}

void Terrain::SetOldTerrain()
{
	oldTerrainVertices = new TerrainVertex[vertexCount];
	memcpy(oldTerrainVertices, vertices, sizeof(TerrainVertex) * vertexCount);

	undoStack.push_back(oldTerrainVertices);

	if (redoStack.size() > 0)
		redoStack.clear();
}

void Terrain::MakeOldTerrain()
{
	oldTerrainVertices = new TerrainVertex[vertexCount];
	memcpy(oldTerrainVertices, vertices, sizeof(TerrainVertex) * vertexCount);
}

void Terrain::MakeNewTerrain()
{
	newTerrainVertices = new TerrainVertex[vertexCount];
	memcpy(newTerrainVertices, vertices, sizeof(TerrainVertex) * vertexCount);
}

void Terrain::UndoTerrain()
{
	if (undoStack.empty())
		return;

	MakeNewTerrain();
	redoStack.push_back(newTerrainVertices);

	it = undoStack.end() - 1;
	memcpy(vertices, *it, sizeof(TerrainVertex) * vertexCount);
	undoStack.erase(it);

	CreateNormalData();
	Mapping();
}

void Terrain::RedoTerrain()
{
	if (redoStack.empty())
		return;

	MakeOldTerrain();
	undoStack.push_back(oldTerrainVertices);

	it = redoStack.end() - 1;
	memcpy(vertices, *it, sizeof(TerrainVertex) * vertexCount);
	redoStack.erase(it);

	CreateNormalData();
	Mapping();
}

void Terrain::CreateVertexData()
{
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();


	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount];
	oldTerrainVertices = new TerrainVertex[vertexCount];
	newTerrainVertices = new TerrainVertex[vertexCount];

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = (float)(heights[pixel].r * 255.0f) / 10.0f;
			vertices[index].Position.z = (float)z;

			vertices[index].Uv.x = (float)x / (float)width * 3;
			vertices[index].Uv.y = (float)(height - 1 - z) / (float)height * 3;

			vertices[index].Color = D3DXCOLOR(0, 0, 0, 0);
			vertices[index].Tangent = D3DXVECTOR3(1, 0, 0); //안줘도 조명 깔리네?
		}
	}
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT z = 0; z < height - 1; z++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = width * z + x;
			indices[index + 1] = width * (z + 1) + x;
			indices[index + 2] = width * z + x + 1;
			indices[index + 3] = width * z + x + 1;
			indices[index + 4] = width * (z + 1) + x;
			indices[index + 5] = width * (z + 1) + x + 1;

			index += 6;
		}
	}
}

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		TerrainVertex v0 = vertices[index0];
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];


		D3DXVECTOR3 d1 = v1.Position - v0.Position;
		D3DXVECTOR3 d2 = v2.Position - v0.Position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
}