#include "Framework.h"
#include "Material.h"

Material::Material()
{
	Initialize();
}

Material::Material(Shader * shader)
{
	Initialize();

	SetShader(shader);
}

void Material::Initialize()
{
	name = L"";

	diffuseMap = NULL;
	specularMap = NULL;
	normalMap = NULL;

	diffuseMaps = NULL;
	specularMaps = NULL;
	normalMaps = NULL;

	buffer = new ConstantBuffer(&colorDesc, sizeof(ColorDesc));
}

Material::~Material()
{
	SafeDelete(diffuseMap);
	SafeDelete(specularMap);
	SafeDelete(normalMap);

	SafeDelete(diffuseMaps);
	SafeDelete(specularMaps);
	SafeDelete(normalMaps);

	SafeDelete(buffer);
}

void Material::SetShader(Shader * shader)
{
	this->shader = shader;
	sBuffer = shader->AsConstantBuffer("CB_Material");

	sDiffuseMap = shader->AsSRV("DiffuseMap");
	sSpecularMap = shader->AsSRV("SpecularMap");
	sNormalMap = shader->AsSRV("NormalMap");

	sDiffuseMaps = shader->AsSRV("DiffuseMaps");
	sSpecularMaps = shader->AsSRV("SpecularMaps");
	sNormalMaps = shader->AsSRV("NormalMaps");
}

void Material::Ambient(D3DXCOLOR & color)
{
	colorDesc.Ambient = color;
}

void Material::Ambient(float r, float g, float b, float a)
{
	Ambient(D3DXCOLOR(r, g, b, a));
}

void Material::Diffuse(D3DXCOLOR & color)
{
	colorDesc.Diffuse = color;
}

void Material::Diffuse(float r, float g, float b, float a)
{
	Diffuse(D3DXCOLOR(r, g, b, a));
}

void Material::Specular(D3DXCOLOR & color)
{
	colorDesc.Specular = color;
}

void Material::Specular(float r, float g, float b, float a)
{
	Specular(D3DXCOLOR(r, g, b, a));
}

void Material::DiffuseMap(string file)
{
	DiffuseMap(String::ToWString(file));  //»çÁø
}

void Material::DiffuseMap(wstring file)
{
	SafeDelete(diffuseMap);

	diffuseMap = new Texture(file);
}

void Material::DiffuseMaps(vector<wstring>& textures)
{
	SafeDelete(diffuseMaps);

	diffuseMaps = new TextureArray(textures, 256, 256);
}

void Material::DiffuseMapNull()
{
	SafeDelete(diffuseMap);

	diffuseMap = NULL;
}

void Material::SpecularMap(string file)
{
	SpecularMap(String::ToWString(file));
}

void Material::SpecularMap(wstring file)
{
	SafeDelete(specularMap);

	specularMap = new Texture(file);
}

void Material::SpecularMaps(vector<wstring>& textures)
{
	SafeDelete(specularMaps);

	specularMaps = new TextureArray(textures, 256, 256);
}

void Material::SpecularMapNull()
{
	SafeDelete(specularMap);

	specularMap = NULL;
}

void Material::NormalMap(string file)
{
	NormalMap(String::ToWString(file));
}

void Material::NormalMap(wstring file)
{
	SafeDelete(normalMap);

	normalMap = new Texture(file);
}

void Material::NormalMaps(vector<wstring>& textures)
{
	SafeDelete(normalMaps);

	normalMaps = new TextureArray(textures, 256, 256);
}

void Material::NormalMapNull()
{
	SafeDelete(normalMap);

	normalMap = NULL;
}

void Material::Render()
{
	buffer->Apply();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	if (diffuseMap != NULL)
		sDiffuseMap->SetResource(diffuseMap->SRV());
	else
		sDiffuseMap->SetResource(NULL);

	if (diffuseMaps != NULL)
		sDiffuseMaps->SetResource(diffuseMaps->SRV());
	else
		sDiffuseMaps->SetResource(NULL);

	if (specularMap != NULL)
		sSpecularMap->SetResource(specularMap->SRV());
	else
		sSpecularMap->SetResource(NULL);

	if (specularMaps != NULL)
		sSpecularMaps->SetResource(specularMaps->SRV());
	else
		sSpecularMaps->SetResource(NULL);

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());
	else
		sNormalMap->SetResource(NULL);

	if (normalMaps != NULL)
		sNormalMaps->SetResource(normalMaps->SRV());
	else
		sNormalMaps->SetResource(NULL);
}
