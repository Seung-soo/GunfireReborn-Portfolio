#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <typeinfo>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

#include <DirectXCollision.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <wincodec.h>
#include <assimp/Scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

using namespace DirectX;
using namespace Assimp;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <process.h>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <algorithm>

#include <time.h>
using namespace std;

// 메모리 누수 확인할 때 주석
//#define USE_IMGUI

#if !defined(USE_IMGUI)
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
#endif // !defined(USE_IMGUI)


#define DEAD				-1
#define NOPROBLEM			0
#define RETURN_OBJECTPOOL	2

#define ANIM_ONETIME 0
#define ANIM_ROOF 1

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

using namespace Engine;
#include <io.h>
