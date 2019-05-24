#pragma once

enum PaintType
{
	LAYER1 = 0,
	LAYER2,
	LAYER3,
	LAYER4,
	ORIGIN
};
enum HeightEdit
{
	RAISE = 0,
	DOWN,
	FIX,
	SMOOTHING
};
enum ModelType
{
	TANK = 0,
	TOWER,
	CAR,
	KACHUJIN
};
enum MeshType
{
	QUAD = 0,
	CUBE,
	GRID,
	SPHERE,
	CYLINDER,
	QUADBOX,
	CUBEBOX,
	GRIDBOX,
	SPHEREBOX,
	CYLINDERBOX
};

enum Light
{
	POINTLIGHT = 0,
	SPOTLIGHT
};