#pragma once

//#include <afx.h>
#include "resource.h"
#include "windows.h"
#include "worldviewer.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3dx9d.lib")

using std::wstring;

//颜色
#define COLOR_CLEAR				D3DCOLOR_XRGB(252, 252, 252)
#define COLOR_GROUND			D3DCOLOR_XRGB(220, 220, 220)
	
#define FVF_CUSTOM1				D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE


HWND mainwnd;//窗口信息
int width, height;
time_t ticks;
POINT cursor;

LPDIRECT3D9	lpD3D;//D3D设备
LPDIRECT3DDEVICE9 lpDevice;
D3DPRESENT_PARAMETERS d3dpp;
D3DDISPLAYMODE displaymode;
LPDIRECT3DSURFACE9 surf;
HRESULT hr;

LPD3DXFONT font;//字体
RECT text, text2;
WCHAR show[100];

//视角
D3DXVECTOR3 pos;			//位置
D3DXVECTOR3 viewdirection;	//视野方向

//绘图
IDirect3DVertexBuffer9* g_VB;
IDirect3DIndexBuffer9*  g_IB;

ID3DXMesh* mesh1;

//观察者
WViewer viewer;

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;     //顶点位置
	D3DXVECTOR3 normal;  //顶点法线

	DWORD color;
};

void D3DInit(HWND wnd);
void Light();			//材质和光照
void Pic();				//图元
void View();			//视角，矩阵变换
void Paint();			//绘制