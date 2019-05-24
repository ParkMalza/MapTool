#pragma once
#pragma once
#include "InstanceMesh.h"

class InstanceCollisionBox : public InstanceMesh
{
public:
	InstanceCollisionBox(Shader* shader, D3DXVECTOR3 highPos, D3DXVECTOR3 lowPos);
	InstanceCollisionBox(Shader* shader, float width, float height, float depth);
	~InstanceCollisionBox();

	bool Selected(D3DXVECTOR3 start, D3DXVECTOR3 direction, UINT index);
protected:
	void Create() override;

private:
	D3DXVECTOR3 high;
	D3DXVECTOR3 low;
};