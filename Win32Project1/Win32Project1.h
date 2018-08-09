#pragma once

#include "resource.h"
//#include "windows.h"
#include "worldviewer.h"
#include "worldblock.h"
#include "environment.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>
#include <tchar.h>
#include <psapi.h>
#include <thread>

using std::wstring;

//颜色
#define COLOR_CLEAR					D3DCOLOR_XRGB(0, 0, 0 )
#define COLOR_RED1					D3DCOLOR_XRGB(255, 120, 100)
#define COLOR_RED2					D3DCOLOR_XRGB(255, 80, 80)
#define COLOR_BLUE1					D3DCOLOR_XRGB(100, 120, 255)
#define COLOR_GREY1					D3DCOLOR_XRGB(240, 240, 240)
#define COLOR_GREY2					D3DCOLOR_XRGB(230, 230, 230)
#define COLOR_DIRECTION				D3DCOLOR_XRGB(230, 230, 230)
	
//控制模式（是否接受鼠标键盘控制）
#define CONTROLMODE_NORMAL			1
#define CONTROLMODE_CAPTURE			2

//运行模式
#define MODE_STOP					0
#define MODE_RUN					1
#define MODE_MENU					2
#define MODE_PAUSE					3

#define STOPCAPTUREFREQ				40		// STOP模式下主循环频率(降低频率)

#define INVALIDKEY_DELAY			0.4f	//非法字符显示持续时间（秒）

struct threadparam {
	bool *start;
	POINT *pclientcenter;
	WViewer *pviewer;
	int *pcontrolmode; 
};

DWORD WINAPI ThreadProc(LPVOID lpParam)//子线程，处理运动，视角移动
{
	POINT cursorpos, bias;

	threadparam* tpp = (threadparam*)lpParam;
	bool *start = tpp->start;
	POINT *pclientcenter = tpp->pclientcenter;
	WViewer *pviewer = tpp->pviewer;
	int *pcontrolmode = tpp->pcontrolmode;

	while (true)
	{
		if (*start)
		{
			pviewer->HandleMove();//处理视角移动
			if (*pcontrolmode == CONTROLMODE_CAPTURE)//处理视角旋转
			{
				GetCursorPos(&cursorpos);
				SetCursorPos(pclientcenter->x, pclientcenter->y);//将指针重置到窗口中点

				bias = cursorpos;
				bias.x -= pclientcenter->x;
				bias.y -= pclientcenter->y;
				pviewer->Rotate(bias);
			}			
			
			//if (viewer.viewchanged)//更新全局平移矩阵，保持environment与眼睛相对位置不变
			//{
			//	//D3DXMatrixIdentity(&ViewTranslation); 
			//	//D3DXMatrixTranslation(&ViewTranslation, viewpos.x, viewpos.y, 0.0f);//z方向不平移
			//	D3DXMatrixIdentity(&inblockTranslation);
			//	D3DXMatrixTranslation(&inblockTranslation
			//		, inblockpos.x, inblockpos.y, 0.0f);//z方向不平移
			//}
			//if (shadowchanged || moved)
			//	matSun = inblockTranslation * sunTranslation;//更新太阳矩阵，保持太阳与眼睛相对位置不变

			//环境按n秒变化
			//environment.ViewMove();//计算实时经纬度
			//if (nowtime / 2 != lasttime / 2)
			//{
			//	localtime_s(&nowdate, &nowtime);//读取当前时间
			//	environment.SunMove();
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
threadparam tp;							//传递给子线程的参数
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
float fps, avgfps, fpslimit;			//帧率, 帧率限制（fpslimit<=0表示无限制）
float frametime, avgframetime;			//帧时间
char invalidkey; LARGE_INTEGER invalidtime;//非法按键记录
float memory1, memory2;					//内存占用量
tm dateGMT, devicedate, basedate, lastdate, realdate;//时间。basedate是提供冬至日时区的时间
time_t nowtime, basetime, lasttime, realtime;		//时间
int devicetimezone;						//设备的时区
wchar_t testchar;						//测试屏幕显示效果的字符
bool rotated;							//视角是否旋转

//计时
LARGE_INTEGER frequency;				//计数器频率
LARGE_INTEGER stime, etime, stime2, etime2;

//模式
int mode, lastmode;						//运行模式; 存储先前模式（用于恢复）
int controlmode;						//鼠标捕捉模式
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
D3DCAPS9 caps;							//设备能力
float depthbias;						//阴影zdepth抬离地面，防止zfighting
float sdepthbias;						//防止静距离阴影不可见
bool depthbiasable;						//depthbias是否可用

//信息显示
LPD3DXFONT font, font2, font3;			//字体
RECT text, text2, text3, text4, text5, text6, text7;//文字信息显示区域
RECT text8, text9, text10, text11, text12;
WCHAR show[1024], showms[100], status1[100], status2[100];//文字信息缓存


//观察者
WViewer viewer;
//区块
WBlocker blocker;
//环境
Environment environment;

//
//信息共享
//
bool anti;
bool shadowchanged;
D3DXMATRIX matShadow;				//阴影矩阵
D3DMATERIAL9 shadowmtrl;			//阴影材质
bool sundown;						//太阳是否落山

//主循环
inline void MainLoop();			

//初始化
void Init();					//参数、状态初始化
void D3DInit(HWND wnd, D3DMULTISAMPLE_TYPE ms);	//D3D初始化
void CreateDevice();			//创建设备
void DeviceInit();				//设备参数设置
void SetTextRect();				//设置显示文字信息的区域（根据窗口尺寸）
_D3DMULTISAMPLE_TYPE GetMultisampleType(LPDIRECT3D9 lp, D3DDISPLAYMODE dm);	//获得多重采样适用范围，并设置最大多重采样
void ChangeMultiSample();		//更改多重采样模式

//设备处理
void OnLostDevice();			//设备丢失处理
void OnResetDevice();			//设备重置处理

//显示
inline void FpsShow();			//显示fps
inline void InfoShow();			//显示信息
inline void OtherInfoShow();	//显示其他信息
inline void UpdateStatusStr();	//更新状态,k字符串

//时间
inline string GetWDayStr(int wday);	//得到星期几字符串
inline void Get2WndRect();			//获取实时窗口和客户区区域
inline int GetRadianDValue(double radian);
inline int GetRadianMValue(double radian);
inline double GetRadianSValue(double radian);
inline int GetRadianSValueI(double radian);

//控制
bool FullScreen(bool tofull);	//设置全屏模式
void CaptureControl();
void ReleaseControl();
