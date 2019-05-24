#pragma once
#define MAX_POINT_LIGHT 32
struct  PointLight
{
	Color Color;
	Vector3 Position;
	float Range;
	float Intensity;
	float Padding[3];
};

#define MAX_SPOT_LIGHT 32
struct SpotLight
{
	Color Color;
	Vector3 Position;
	float Range;
	Vector3 Direction;
	float Angle;
	float Intensity;
	float Padding[3];
};
//게임에 필요한 요소들 집합소

class Context
{
public:
	static Context* Get();
	static void Create();
	static void Delete();

private:
	Context();
	~Context();

public:
	void Update();
	void Render();

	D3DXMATRIX ViewMatrix();
	D3DXMATRIX ProjectionMatrix();

	class Projection* GetProjection() { return projection; }
	class Viewport* GetViewport() { return viewport; }
	class Camera* GetCamera() { return camera; }

	void SetMainCamera() { subCamera = NULL; }
	void SetMainPerspective() { subPerspective = NULL; }

	void SetSubCamera(Camera* camera) { subCamera = camera; }
	void SetSubPerspective(Perspective* perspective) { subPerspective = perspective; }

	D3DXCOLOR& LightAmbient() { return lightAmbient; }
	D3DXCOLOR& LightSpecular() { return lightSpecular; }
	D3DXVECTOR3& LightDirection() { return lightDirection; }
	D3DXVECTOR3& LightPosition() { return lightPosition; }

	UINT PointLights(OUT PointLight* lights);
	void AddPointLight(PointLight& light);
	UINT GetPointLightCount() { return pointLightCount; }
	PointLight& GetPointLight(UINT index);
	void DeletePointLight(UINT index);

	UINT SpotLights(OUT SpotLight* lights);
	void AddSpotLight(SpotLight& light);
	UINT GetSpotLightCount() { return spotLightCount; }
	SpotLight& GetSpotLight(UINT index);
	void DeleteSpotLight(UINT index);

private:
	static Context* instance;

private:
	class Projection* projection;
	class Viewport* viewport;
	class Camera* camera;

	class Camera* subCamera;
	class Perspective* subPerspective;

	D3DXCOLOR lightAmbient;
	D3DXCOLOR lightSpecular;
	D3DXVECTOR3 lightDirection;
	D3DXVECTOR3 lightPosition;

	UINT pointIndex;
	UINT pointLightCount;
	PointLight pointLights[MAX_POINT_LIGHT];

	UINT spotIndex;
	UINT spotLightCount;
	SpotLight spotLights[MAX_SPOT_LIGHT];

private:
	vector<UINT> saveSpotIndex;
	vector<UINT> savePointIndex;
};