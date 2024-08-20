#pragma once

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;

enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_MAINMANU, LEVEL_LOBY, LEVEL_STAGE3_1, LEVEL_STAGE3_2
			,LEVEL_STAGE4_1, LEVEL_STAGE4_2, LEVEL_IMGUI, LEVEL_END };

extern HINSTANCE	g_hInst;
extern HWND			g_hWnd;
extern LEVEL		g_eCurrentLevel;
extern bool		g_bGoNextLevel;
extern bool		g_bCameraAction;

#include "GameInstance.h"
extern CGameInstance*	g_pGameInstance;

namespace Client {}


using namespace Client;
#include <fstream>
#include <tchar.h>

#define DASHTIME 185
#define DISTANCE(DIR)	XMVectorGetX(XMVector4Length(DIR))
#define MAX_PATICLETEXTURE 51

#ifdef USE_IMGUI
extern bool g_bRender;
#endif