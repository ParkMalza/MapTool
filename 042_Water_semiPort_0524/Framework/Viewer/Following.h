#pragma once
#pragma once
#include "Camera.h"

class Following : public Camera
{
public:
	Following();
	~Following();

	void Update() override;
	void Speed(float move, float rotation);


private:
	float moveSpeed;
	float rotationSpeed;

	D3DXVECTOR3 position;

	float distance;
};