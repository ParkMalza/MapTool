#pragma once

class Transform
{
public:
	Transform();
	Transform(Shader* shader);
	~Transform();

	void Set(Transform* transform);
	void SetShader(Shader* shader);

	void Position(float x, float y, float z);
	void Position(D3DXVECTOR3& vec);
	void Position(D3DXVECTOR3* vec);

	void Scale(float x, float y, float z);
	void Scale(D3DXVECTOR3& vec);
	void Scale(D3DXVECTOR3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(D3DXVECTOR3& vec);
	void Rotation(D3DXVECTOR3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(D3DXVECTOR3* vec);

	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	void World(D3DXMATRIX& matrix) { bufferDesc.World = matrix; }
	D3DXMATRIX& World() { return bufferDesc.World; }

private:
	void UpdateWorld();

public:
	void Update();
	void Render();

private:
	struct BufferDesc
	{
		D3DXMATRIX World;
	} bufferDesc;

private:
	Shader* shader;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;


	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;
};