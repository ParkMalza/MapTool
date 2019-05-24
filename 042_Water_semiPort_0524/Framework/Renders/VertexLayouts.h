#pragma once
#include "Framework.h"

//=======================================================================================

struct Vertex
{
	Vertex()
		: Position(0, 0, 0) {}

	D3DXVECTOR3 Position;
};

//=======================================================================================

struct VertexNormal
{
	VertexNormal()
		: Position(0, 0, 0)
		, Normal(0, 0, 0) {}

	D3DXVECTOR3	Position;
	D3DXVECTOR3 Normal;
};

//=======================================================================================

struct VertexColor
{
	VertexColor()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1) {}

	VertexColor(float x, float y, float z, float r, float g, float b)
	{
		Position.x = x;
		Position.y = y;
		Position.z = z;

		Color.r = r;
		Color.g = g;
		Color.b = b;
		Color.a = 1.0f;
	}

	D3DXVECTOR3	Position;
	D3DXCOLOR Color;
};

//=======================================================================================

struct VertexColorNormal
{
	VertexColorNormal()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1)
		, Normal(0, 0, 0) {}

	D3DXVECTOR3 Position;
	D3DXCOLOR Color;
	D3DXVECTOR3 Normal;
};

//=======================================================================================

struct VertexTexture
{
	VertexTexture()
		: Position(0, 0, 0)
		, Uv(0, 0) {}

	D3DXVECTOR3	Position;
	D3DXVECTOR2	Uv;
};

//=======================================================================================

struct VertexTextureColor
{
	VertexTextureColor()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Color(0, 0, 0, 1)
	{

	}

	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXCOLOR Color;
};

//=======================================================================================

struct VertexTextureColorNormal
{
	VertexTextureColorNormal()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Color(0, 0, 0, 1)
		, Normal(0, 0, 0)
	{

	}

	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXCOLOR Color;
	D3DXVECTOR3 Normal;
};

//=======================================================================================

struct VertexTextureNormal
{
	VertexTextureNormal()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0) {}

	VertexTextureNormal(float x, float y, float z, float u, float v, float nx, float ny, float nz)
	{
		Position = D3DXVECTOR3(x, y, z);
		Uv = D3DXVECTOR2(u, v);
		Normal = D3DXVECTOR3(nx, ny, nz);
	}

	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXVECTOR3 Normal;
};

//=======================================================================================

struct VertexTextureNormalType
{
	VertexTextureNormalType()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Type(0)
		, Normal(0, 0, 0) {}

	VertexTextureNormalType(float x, float y, float z, float u, float v, float nx, float ny, float nz, UINT type)
	{
		Position = D3DXVECTOR3(x, y, z);
		Uv = D3DXVECTOR2(u, v);
		Type = type;
		Normal = D3DXVECTOR3(nx, ny, nz);
	}

	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	UINT Type;
	D3DXVECTOR3 Normal;
};

//=======================================================================================

struct VertexColorTextureNormal
{
	VertexColorTextureNormal()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1)
		, Uv(0, 0)
		, Normal(0, 0, 0) {}

	D3DXVECTOR3 Position;
	D3DXCOLOR Color;
	D3DXVECTOR2 Uv;
	D3DXVECTOR3 Normal;
};

struct VertexColorTextureNormalTangent
{
	VertexColorTextureNormalTangent()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1)
		, Uv(0, 0)
		, Tangent(0, 0, 0)
		, Normal(0, 0, 0) {}

	D3DXVECTOR3 Position;
	D3DXCOLOR Color;
	D3DXVECTOR2 Uv;
	D3DXVECTOR3 Normal;
	D3DXVECTOR3 Tangent;
};
//=======================================================================================

struct VertexTextureNormalBlend
{
	VertexTextureNormalBlend()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0)
		, BlendIndices(0, 0, 0, 0)
		, BlendWeights(0, 0, 0, 0) {}

	D3DXVECTOR3 Position;
	D3DXVECTOR2 Uv;
	D3DXVECTOR3 Normal;
	D3DXVECTOR4 BlendIndices;
	D3DXVECTOR4 BlendWeights;
};

//=======================================================================================

struct VertexTextureNormalTangent
{
	VertexTextureNormalTangent()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0)
		, Tangent(0, 0, 0)
	{}

	VertexTextureNormalTangent(float px, float py, float pz, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz)
	{
		Position = D3DXVECTOR3(px, py, pz);
		Uv = D3DXVECTOR2(u, v);
		Normal = D3DXVECTOR3(nx, ny, nz);
		Tangent = D3DXVECTOR3(tx, ty, tz);
	}

	D3DXVECTOR3	Position;
	D3DXVECTOR2	Uv;
	D3DXVECTOR3	Normal;
	D3DXVECTOR3 Tangent;
};

//=======================================================================================

struct VertexTextureNormalTangentBlend
{
	VertexTextureNormalTangentBlend()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0)
		, Tangent(0, 0, 0)
		, BlendIndices(0, 0, 0, 0)
		, BlendWeights(0, 0, 0, 0)
	{}

	D3DXVECTOR3	Position;
	D3DXVECTOR2	Uv;
	D3DXVECTOR3	Normal;
	D3DXVECTOR3 Tangent;
	D3DXVECTOR4 BlendIndices;
	D3DXVECTOR4 BlendWeights;
};

//=======================================================================================

struct VertexCollisionBox
{
	VertexCollisionBox()
		:highestNoraml(0), minusX(0)
		, minusY(0), minusZ(0), plusX(0)
		, plusY(0), plusZ(0)
	{

	}
	float minusX, minusY, minusZ, plusX, plusY, plusZ;
	float highestNoraml;
};