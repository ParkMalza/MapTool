#pragma once

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <functional>
#include <iterator>
#include <thread>
#include <mutex>
#include <cmath>
using namespace std;

//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Effects11d.lib")

//ImGui
#include <ImGui_New/imgui.h>
#include <ImGui_New/imgui_impl_dx11.h>
#include <ImGui_New/imgui_impl_win32.h>
#pragma comment(lib, "ImGui_New/imgui.lib")

//DirectXTex
#include <DirectXTex.h>
#pragma comment(lib, "directxtex.lib")

#define Check(hr) { assert(SUCCEEDED(hr)); }
#define Super __super

#define SafeRelease(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SafeDelete(p){ if(p){ delete (p); (p) = NULL; } }
#define SafeDeleteArray(p){ if(p){ delete [] (p); (p) = NULL; } }
#define MESH_DEFAUL_LENGTH 5.0f
#define ALLOW_MULTI_VIEWPORT

typedef D3DXVECTOR2 Vector2;
typedef D3DXVECTOR3 Vector3;
typedef D3DXVECTOR4 Vector4;
typedef D3DXPLANE Plane;
typedef D3DXCOLOR Color;
typedef D3DXMATRIX Matrix;
typedef D3DXQUATERNION Quaternion;

#include "Systems/D3D.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Systems/Time.h"
#include "Systems/Gui.h"

#include "Buffers/ConstantBuffer.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"

#include "Viewer/Camera.h"
#include "Viewer/RenderTarget.h"
#include "Viewer/DepthStencil.h"
#include "Viewer/Viewport.h"
#include "Viewer/Projection.h"
#include "Viewer/Perspective.h"
#include "Viewer/Orthographic.h"

#include "Renders/Shader.h"
#include "Renders/Texture.h"
#include "Renders/VertexLayouts.h"
#include "Renders/Context.h"
#include "Renders/Material.h"
#include "Renders/PerFrame.h"
#include "Renders/Transform.h"
#include "Renders/Renderer.h"
#include "Renders/Render2D.h"
#include "Renders/TextureCube.h"
#include "Renders/Shadow.h"

#include "Meshes/MeshQuad.h"
#include "Meshes/MeshGrid.h"
#include "Meshes/MeshCube.h"
#include "Meshes/MeshCylinder.h"
#include "Meshes/MeshSphere.h"

#include "Utilities/Math.h"
#include "Utilities/String.h"
#include "Utilities/Path.h"

#include "Model/Model.h"
#include "Model/ModelRender.h"
#include "Model/ModelAnimator.h"

#include "Meshes/InstanceQuad.h"
#include "Meshes/InstanceCube.h"
#include "Meshes/InstanceGrid.h"
#include "Meshes/InstanceSphere.h"
#include "Meshes/InstanceCylinder.h"
#include "Meshes/CollisionBox.h"
#include "Meshes/InstanceCollisionBox.h"

#include "Singleton/Singleton.h"