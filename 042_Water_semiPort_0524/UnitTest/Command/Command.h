#pragma once
enum StackType
{
	STACK_BILLBOARD,
	STACK_BRUSH,
	STACK_PAINT,
	STACK_QUAD,
	STACK_CUBE,
	STACK_GRID,
	STACK_SPHERE,
	STACK_CYLINDER,
	STACK_QUAD_BOX,
	STACK_CUBE_BOX,
	STACK_GRID_BOX,
	STACK_SPHERE_BOX,
	STACK_CYLINDER_BOX,
};

class Command
{
public:
	virtual ~Command(){}

	virtual void Undo() = 0;
	virtual void Redo() = 0;
	virtual void Excute() = 0;
 };