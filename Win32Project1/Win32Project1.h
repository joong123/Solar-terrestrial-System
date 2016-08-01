#pragma once

#include "resource.h"
#include "windows.h"
#include "worldviewer.h"
#include "worldblock.h"
#include "environment.h"
//#include "d3dfont.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>
#include <tchar.h>
#include <time.h>
#include <psapi.h>
#include <thread>

using std::wstring;

//颜色
#define COLOR_CLEAR				D3DCOLOR_XRGB(0, 0, 0 )
#define COLOR_BLUE1				D3DCOLOR_XRGB(100, 120, 255)
#define COLOR_RED1				D3DCOLOR_XRGB(255, 120, 100)
#define COLOR_RED2				D3DCOLOR_XRGB(255, 80, 80)
#define COLOR_GREY1				D3DCOLOR_XRGB(240, 240, 240)
#define COLOR_GREY2				D3DCOLOR_XRGB(230, 230, 230)
#define COLOR_DIRECTION			D3DCOLOR_XRGB(230, 230, 230)
	

//鼠标模式
#define CURSORMODE_NORMAL		1
#define CURSORMODE_CAPTURE		2

//运行模式
#define MODE_STOP				0
#define MODE_RUN				1
#define MODE_MENU				2
#define MODE_PAUSE				3

#define STOPCAPTUREFREQ			20		// STOP模式下主循环频率(降低频率)

#define INVALIDKEY_DELAY		0.4f	//非法字符显示持续时间（秒）

struct threadparam {
	/*LPD3DXFONT *font;
	float *fps;
	float *avgfps;
	float *frametime;
	time_t *loopcount;
	RECT *text;*/
	bool *start;
	POINT *pclientcenter;
	WViewer *pviewer;
	int *pcursormode; 
};

DWORD WINAPI ThreadProc(LPVOID lpParam)//子线程，处理运动，视角移动
{
	threadparam* tpp = (threadparam*)lpParam;
	POINT cursorpos, bias;
	/*LPD3DXFONT *font = tpp->font;
	float *fps = tpp->fps;
	float *avgfps = tpp->avgfps;
	float *frametime = tpp->frametime;
	time_t *loopcount = tpp->loopcount;
	RECT *text = tpp->text;*/
	bool *start = tpp->start;
	POINT *pclientcenter = tpp->pclientcenter;
	WViewer *pviewer = tpp->pviewer;
	int *pcursormode = tpp->pcursormode;

	WCHAR show[40];
	while (true)
	{
		if (*start)
		{
			pviewer->HandleMove();//处理视角移动
			if (*pcursormode == CURSORMODE_CAPTURE)//处理视角旋转
			{
				GetCursorPos(&cursorpos);
				SetCursorPos(pclientcenter->x, pclientcenter->y);//将指针重置到窗口中点

				bias = cursorpos;
				bias.x -= pclientcenter->x;
				bias.y -= pclientcenter->y;
				pviewer->Rotate(bias);
			}
			//处理太阳（月亮、星星、云等环境）移动
			
			/*if ((loopcount & 0x2) == 0)
			{*/
				if (moved)//更新全局平移矩阵，调整environment位置，保持与眼镜相对位置不变
				{
					D3DXMatrixIdentity(&ViewTranslation);
					D3DXMatrixTranslation(&ViewTranslation, viewpos.x, viewpos.y, 0.0f);//z方向不平移
				}
				if (shadowchanged || moved)
					matSun = ViewTranslation * sunTranslation;//更新太阳矩阵
			//}
			*start = false;
		}
		else
		{
			//Sleep(1);
		}
	}
}


//子线程
threadparam tp;//传递给子线程的参数
DWORD threadID;
HANDLE hThread;
bool startthread;						//是否允许子线程运行的标志
CRITICAL_SECTION cs;					//临界区

//窗口信息
HWND mainwnd;							//主窗口句柄
bool isiconic, isfullscreen, focused;	//窗口状态：是否最小化、是否全屏、是否有焦点
RECT wndrect, clientrect;				//主窗口区域，客户区域
POINT clientcenter;						//客户区中点，加速运算，每次鼠标移回这个位置
RECT originwndrect;						//存储原窗口尺寸，用于全屏恢复
int screenwidth, screenheight;			//屏幕长宽

//运行信息
time_t loopcount;						//循环计数
POINT cursorpos;						//鼠标坐标
float fps, avgfps, fpslimit;			//帧率, 帧率限制（<=0表示无限制）
float frametime;						//帧时间
char invalidkey; LARGE_INTEGER invalidtime;//非法按键记录
float memory1, memory2;					//内存占用量
tm Time;								//时间
//wchar_t testchar;						//测试屏幕显示效果的字符

//计时
LARGE_INTEGER frequency;				//计数器频率
LARGE_INTEGER stime, etime, stime2, etime2;

//模式
int mode, lastmode;						//运行模式, 存储先前模式（用于恢复）
int cursormode;							//鼠标捕捉模式（是否捕捉）
bool infoshow;							//是否显示信息
bool otherinfoshow;						//是否显示其他信息
bool fpsshow;							//是否显示fps

//D3D
LPDIRECT3D9	lpD3D;						//
LPDIRECT3DDEVICE9 device;				//设备
D3DPRESENT_PARAMETERS d3dpp;			//
D3DDISPLAYMODE displaymode;				//显示模式
LPDIRECT3DSURFACE9 surf;				//暂未使用
byte multisample, maxmmultisample;		//多重采样模式
bool mslist[17];						//多重采样模式适用列表

//信息显示
LPD3DXFONT font, font2, font3;			//字体
//CD3DFont d3dfont;
RECT text, text2, text3, text4, text5, text6, text7;//文字信息显示区域
RECT text8, text9, text10, text11, text12;
WCHAR show[100], showms[100], status1[100], status2[100];//文字信息缓存


//观察者
WViewer viewer;
//区块
WBlocker blocker;
//环境
Environment environment;

//
//信息共享
//
//眼睛位置
	//[WViewer构造函数中初始化，Walk()、SetViewmode()、KeyControlDown()和SetViewVector()中实时更新
	//	，子线程和SetViewmode()中更新ViewTranslation, SetView()中用来SetTransform(D3DTS_VIEW,)]
D3DXVECTOR3 viewpos;
//视角矩阵（全局平移矩阵），environment绘制所使用的水平平移矩阵
	//[子线程中根据moved用viewpos实时条件更新，子线程中根据moved和shadowchanged实时更新matSun
	//	，在WViewer构造函数中用viewpos初始化，在environment的InitMaterialLight()中初始化matSun]
D3DXMATRIX ViewTranslation;	
//视角是否改变，条件更新view
	//[SetViewmode()、Walk()、Rotate()中设置为true，MainLoop()中用于条件SetView()的依据
	//	并在所有元素绘制结束后设置为false，在InitInstance()中初始化]
bool viewchanged;
//视角是否移动，条件更新阴影矩阵，是viewchanged的子集
	//[子线程中用来条件更新ViewTranslation和matSun的依据，在WViewer的Draw()中用于条件更新matShadowWorld的依据
	//	，在Walk()中设置为true，在MainLoop()中所有元素绘制结束后设置为false，在InitInstance()中初始化为true]
bool moved;
//太阳世界矩阵（= ViewTranslation * sunTranslation）
	//[在子线程中根据shadowchanged || moved条件更新，在InitMaterialLight()中初始化为true
	//	在InitInstance()中初始化为true]
D3DXMATRIX matSun;
//太阳平移矩阵
	//[在子线程中用于条件更新matSun，在InitMaterialLight()中用sunLight初始化并更新matSun]
D3DXMATRIX sunTranslation;
D3DXMATRIX sunRotate;			//太阳旋转矩阵，unused
//太阳（月亮、星星、云等环境）位置是否改变，条件更新阴影矩阵
	//[在子线程中用于条件更新matSun，在MainLoop()中所有元素绘制结束后设置为false
	//，在InitMaterialLight()中初始化为true，在InitInstance()中初始化为true
	//，在WViewer的Draw()中条件更新matShadowWorld，在WBlock的Draw()中条件更新matShadowWorld]
bool shadowchanged;
//太阳位置
	//[在InitMaterialLight()中根据light0初始化并初始化sunTranslation和matShadow]
D3DXVECTOR4 sunLight;
//阴影矩阵
	//[在InitMaterialLight()中用sunLight和groundPlane初始化并用groundPlane.d垂直抬升，
	//，在WBlock的Draw()中根据shadowchanged条件更新matShadowWorld
//，在WViewer的Draw()中和shadowchanged条件更新matShadowWorld]
D3DXMATRIX matShadow;
//阴影材质
D3DMATERIAL9 shadowmtrl;

inline void MainLoop();			//主循环

void Init();					//参数、状态初始化

void D3DInit(HWND wnd, D3DMULTISAMPLE_TYPE ms);	//D3D初始化
void CreateDevice();			//创建设备
void DeviceInit();				//设备参数设置
void SetTextRect();				//设置显示文字信息的区域（根据窗口尺寸）
_D3DMULTISAMPLE_TYPE GetMultisampleType(LPDIRECT3D9 lp, D3DDISPLAYMODE dm);	//获得多重采样适用范围
void ChangeMultiSample();		//更改多重采样模式

void OnLostDevice(void);		//设备丢失处理
void OnResetDevice(void);		//设备重置处理

inline void FpsShow();			//显示fps
inline void InfoShow();			//显示信息
inline void OtherInfoShow();	//显示其他信息
inline void UpdateStatusStr();	//更新状态字符串

//时间
inline string GetWDayStr(int wday);	//得到星期几字符串
inline void Get2WndRect();			//获取实时窗口和客户区区域
//控制
bool FullScreen(bool tofull);	//设置全屏模式
void CaptureCursor();
void ReleaseCursor();
