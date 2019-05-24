#pragma once

#include "InstanceMesh.h"

class InstanceCylinder : public InstanceMesh
{
public:
	InstanceCylinder(Shader* shader, float radius, float height, UINT sliceCount = 20, UINT stackCount = 20);
	~InstanceCylinder();

protected:
	void Create() override;

private:
	void BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices);
	void BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices);

private:
	float topRadius;  //윗면 반지름
	float bottomRadius;  //아랫면 반지름
	float height;  //높이겠지

	UINT sliceCount; //slicecount = 정점을 얼마나 세분화 할것인가 == 뚜껑과 바닥의 삼각형 수
	UINT stackCount;  //몸통의층수

	Texture* texture;
};