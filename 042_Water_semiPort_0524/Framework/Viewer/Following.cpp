#include "Framework.h"
#include "Following.h"


Following::Following()
	: moveSpeed(10), rotationSpeed(4), distance(100), position(0, 0, 0)
{

}

Following::~Following()
{
}

void Following::Update()
{
	D3DXVECTOR3& f = Forward();
	D3DXVECTOR3& r = Right();
	D3DXVECTOR3& u = Up();


	//D3DXVECTOR3 position;
	//Position(&position);
	//if (Keyboard::Get()->Press('W'))
	//	position += f * moveSpeed * Time::Delta();
	//else if (Keyboard::Get()->Press('S'))
	//	position += -f * moveSpeed * Time::Delta();

	//if (Keyboard::Get()->Press('D'))
	//	position += r * moveSpeed * Time::Delta();
	//else if (Keyboard::Get()->Press('A'))
	//	position += -r * moveSpeed * Time::Delta();

	//if (Keyboard::Get()->Press('E'))
	//	position += u * moveSpeed * Time::Delta();
	//else if (Keyboard::Get()->Press('Q'))
	//	position += -u * moveSpeed * Time::Delta();

	//Position(position);


	position = fix;
	position -= f * distance;
	Position(position);

	if (Mouse::Get()->Press(1))
	{
		D3DXVECTOR3 R;
		Rotation(&R);
		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

		R.x += val.y * rotationSpeed * Time::Delta();
		R.y += val.x * rotationSpeed * Time::Delta();
		R.z = 0;
		Rotation(R);
	}

	if (Keyboard::Get()->Press('Z'))
	{
		if (distance > 5)
			distance -= 40.0f * Time::Delta();;
	}
	if (Keyboard::Get()->Press('C'))
	{
		if (distance <= 100)
			distance += 40.0f * Time::Delta();;
	}
}

void Following::Speed(float move, float rotation)
{
	moveSpeed = move;
	rotationSpeed = rotation;
}
