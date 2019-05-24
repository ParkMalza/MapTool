#pragma once
#include "Camera.h"

class Freedom : public Camera
{
public:
	Freedom();
	~Freedom();

	void Update() override;

	void Speed(float move, float rotation);
	void SetForward(D3DXVECTOR3 _forward) { fix = _forward; }
	
private:
	float move;
	float rotation;

	D3DXVECTOR3 fix;
};