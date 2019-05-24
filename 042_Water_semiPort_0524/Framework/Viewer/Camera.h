
#pragma once

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual void Update() = 0;

public:
	void Position(float x, float y, float z);
	void Position(Vector3& vec);
	void Position(Vector3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	void Rotation(Vector3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	void RotationDegree(Vector3* vec);

	void GetMatrix(Matrix* matrix);

	Vector3& Forward() { return forward; }
	Vector3& Right() { return right; }
	Vector3& Up() { return up; }

	void SetForward(D3DXVECTOR3 _forward) { fix = _forward; }
protected:
	virtual void Move();
	virtual void Rotation();

	Vector3 fix;
protected:
	void View();

private:
	Vector3 position;
	Vector3 rotation;

	Vector3 forward;
	Vector3 up;
	Vector3 right;

	Matrix matRotation;

protected:
	Matrix matView;
};

//#pragma once
//
//class Camera
//{
//public:
//	Camera();
//	virtual ~Camera();
//
//	virtual void Update() = 0;
//
//public:
//	void Position(float x, float y, float z);
//	void Position(D3DXVECTOR3& vec);
//	void Position(D3DXVECTOR3* vec);
//
//	void Rotation(float x, float y);
//	void Rotation(D3DXVECTOR2& vec);
//	void Rotation(D3DXVECTOR2* vec);
//
//	void RotationDegree(float x, float y);
//	void RotationDegree(D3DXVECTOR2& vec);
//	void RotationDegree(D3DXVECTOR2* vec);
//
//	D3DXVECTOR3& Forward() { return forward; }
//	D3DXVECTOR3& Right() { return right; }
//	D3DXVECTOR3& Up() { return up; }
//
//	void GetMatrix(D3DXMATRIX* mat);
//
//	void SetForward(D3DXVECTOR3 _forward) { fix = _forward; }
//
//protected:
//	virtual void Move();
//	virtual void Rotation();
//
//	D3DXVECTOR3 fix;
//
//protected:
//	void View();
//
//	D3DXMATRIX matView;
//private:
//	D3DXVECTOR3 position;
//	D3DXVECTOR2 rotation;
//
//	D3DXVECTOR3 forward;
//	D3DXVECTOR3 right;
//	D3DXVECTOR3 up;
//
//	D3DXMATRIX matRotation;
//
//};
