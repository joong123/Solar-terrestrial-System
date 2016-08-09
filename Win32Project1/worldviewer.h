#pragma once

#include "D3Ddefine.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

#define COLOR_FIGURE1					D3DCOLOR_XRGB(255, 140, 120)
//view模式
#define VIEWMODE_FIRSTPERSON			1
#define VIEWMODE_CHASE					2
#define VIEWMODE_FREE					3
#define VIEWMODE_OVERLOOK				4

//运动方向
#define DIRECTION_NONE					0
#define DIRECTION_TOP					1
#define DIRECTION_LEFT					2
#define DIRECTION_RIGHT					3
#define DIRECTION_DOWN					4
#define DIRECTION_TOPLEFT				5
#define DIRECTION_TOPRIGHT				6
#define DIRECTION_BOTTOMLEFT			7
#define DIRECTION_BOTTOMRIGHT			8

//残余速度模式
#define REMNANTSPEEDMODE_OFF			0
#define REMNANTSPEEDMODE_DELAY			1
#define REMNANTSPEEDMODE_SMOOTH			2
#define REMNANTSPEEDMODE_DELAYSMOOTH	3


using std::string;

//运动
const WCHAR walkdirection[10] = { L'○', L'↑', L'←', L'→', L'↓', L'↖', L'↗', L'↙', L'↘', L'﹡' };
const char viewdirection[][3] = { "E", "NE", "N", "NW", "W", "SW", "S", "SE", "E"};//视角方向
const float division = 0.04f;				//最小位移分度值
const float speednormal = WALKSPEED;		//正常速度
const float speeddelaytime = 0.04f;			//速度维持不变的时间。残余速度时间1
const float speedsmoothtime = 0.1f;			//速度降为0需要的时间。残余速度时间2
const float chasedist = 1.2f;				//追捕视角水平距离
const float chasescreenheight = 0.4f;		//追捕视角高度
const float overlookscreenheight = 300.0f;	//俯视高度

//共享变量
//extern D3DXVECTOR4 sunPos;
//extern D3DXMATRIX matShadow;
//extern D3DMATERIAL9 shadowmtrl;

class WViewer {
public:
	LPDIRECT3DDEVICE9 device;

	//绘图
	ID3DXMesh* figure;
	ID3DXMesh* figurelight;

	//矩阵
	D3DXMATRIXA16 matView;				//view矩阵
	D3DXMATRIXA16 matWorld;				//人物的世界矩阵（包括平移和水平旋转）
	D3DXMATRIXA16 matTranslation;		//人物平移矩阵
	D3DXMATRIXA16 matTranslation2;		//人物平移微小偏移，用于figurelight，防止两次渲染后重叠产生干扰线
	D3DXMATRIXA16 matHRotate;			//人物水平旋转矩阵
	D3DXMATRIXA16 matShadowWorld;		//影子矩阵

	//材质 & 纹理
	D3DMATERIAL9 figuremtrl;
	LPDIRECT3DTEXTURE9 g_Texture;

	//实时信息
	D3DXVECTOR3 pos;					//人物位置
	float figurehangle, figurevangle;	//人物朝向
	//D3DXVECTOR3 viewpos;				//眼睛真实位置，转为全局变量
	float hAngle;						//视角水平面角度	(0~360) x正向=0
	float vAngle;						//视角纵向角度	(-90~90)
	D3DXVECTOR3 displacement;			//位移
	D3DXVECTOR3 viewdirection;			//眼睛方向, added
	D3DXVECTOR3 at;						//视线目标
	D3DXVECTOR3 up;
	float viewangle;					//视野角度
	float viewaspect;					//视野长宽比

	//物理运动
	LARGE_INTEGER frequency;			//计时器频率
	LARGE_INTEGER endtick;				//结束方向键时的ticks，用于残余速度计算
	LARGE_INTEGER lasttick;				//上一个tick，用于当前行走距离计算
	float speed;						//当前速度
	int remnantspeedmode;				//残余速度模式
	int curdirection;					//当前运动方向
	int lastdirection;					//当前方向为0时存储上一个方向，处理残余速度的运动
	bool wdown, sdown, adown, ddown;	//按键状态
	bool shiftdown;						//按键状态

	D3DXVECTOR3 figureeye;				//视角位置(人物眼睛相对人物模型位置)
	float viewradius;					//视野距离
	float sensitivity;					//旋转灵敏度
	float hcos, hsin, vsin, vcos;		//加速运算
	bool flashlight;

	int viewmode;						//view模式

public:
	WViewer();
	~WViewer();

	void InitProj(float angle, float aspect);	//设置投影变换参数并设置变换
	void SetFigure();

	bool SetDevice(LPDIRECT3DDEVICE9 device);	//传递设备
	bool SetViewAngle(float angle);				//设置view角度
	bool SetAspect(float aspect);				//设置aspect
	bool SetViewmode(int mode);					//设置观察模式
	inline void SetViewVector()					//根据当前人物位置pos和观察模式，更新视线参数
	{
		switch (viewmode)
		{
		case VIEWMODE_FIRSTPERSON:
			viewpos = pos + D3DXVECTOR3(cos(figurehangle)*figureeye.x -sin(figurehangle)*figureeye.y
				, sin(figurehangle)*figureeye.x + cos(figurehangle)*figureeye.y, figureeye.z);
			at = viewpos + viewdirection;//z轴负向为世界天空方向，z值取负
			break;
		case VIEWMODE_CHASE:
			viewpos = pos + D3DXVECTOR3(-chasedist*hcos, -chasedist*hsin, -chasescreenheight);
			at = viewpos + viewdirection;
			break;
		case VIEWMODE_FREE:
			//viewpos = pos + D3DXVECTOR3(-chasedist*hcos, -chasedist*hsin, -chasescreenheight);
			at = viewpos + viewdirection;
			break;
		case VIEWMODE_OVERLOOK:
			//viewpos = pos + D3DXVECTOR3(0.0f, 0.0f, -overlookscreenheight);
			at = viewpos + viewdirection;//自由视角
			//at = pos;//限制垂直视角
			break;
		}
		//更新up
		if (vAngle == -D3DX_PI / 2)
			up = D3DXVECTOR3(hcos, hsin, 0.0f);
		else if (vAngle == D3DX_PI / 2)
			up = -D3DXVECTOR3(hcos, hsin, 0.0f);
		else
			up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	}
	inline bool SetView()				//根据实现参数进行视角变换
	{
		if (!device)
			return false;

		D3DXMatrixLookAtLH(&matView, &viewpos, &at, &up);
		device->SetTransform(D3DTS_VIEW, &matView);

		//viewchanged = false;
		return true;
	}
	bool SetProj();								//根据当前参数设置投影变换

	inline void HandleSpeed()					//处理当前速度
	{
		if (speed == 0.0f)
			return;

		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		float time = (float)(now.QuadPart - endtick.QuadPart) / (float)frequency.QuadPart;
		if (curdirection == DIRECTION_NONE)//如果不在行走，才处理速度，行走期间保持速度恒定
		{
			switch (remnantspeedmode)
			{
			case REMNANTSPEEDMODE_OFF:
				speed = 0.0f;
				break;
			case REMNANTSPEEDMODE_DELAY:
				if (time < speeddelaytime)
				{
					return;
				}
				else
					speed = 0.0f;
				break;
			case REMNANTSPEEDMODE_SMOOTH:
				if (time < speedsmoothtime)
				{
					float percent = (speedsmoothtime - time)
						/ speedsmoothtime;
					speed = speednormal*percent;
				}
				else
					speed = 0.0f;
				break;
			case REMNANTSPEEDMODE_DELAYSMOOTH:
				if (time < speeddelaytime)
				{
					return;
				}
				else
				{
					float percent = ((speeddelaytime + speedsmoothtime) - time) / speedsmoothtime;
					if (percent > 0)
						speed = speednormal*percent;
					else
						speed = 0.0f;
				}
				break;
			}
		}
	}
	inline void Walk()							//根据speed和curdirection进行行走
	{
		if (speed == 0.0f)//没速度则返回
			return;

		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		float dist = speed*(double)(now.QuadPart - lasttick.QuadPart) / (double)frequency.QuadPart;//移动距离
		lasttick = now;//记录为上一次行走时间点
		if (shiftdown)
			dist *= 20;
		//如果当前运动方向为NONE，使用存储的上一个运动方向
		int direction = (curdirection == DIRECTION_NONE) ? lastdirection : curdirection;
		if (viewmode == VIEWMODE_FIRSTPERSON || viewmode == VIEWMODE_CHASE)
		{
			if (direction == DIRECTION_TOP)//前进
			{
				pos += D3DXVECTOR3(dist*hcos, dist*hsin, 0.0f);
				displacement = D3DXVECTOR3(dist*hcos, dist*hsin, 0.0f);
			}
			else if (direction == DIRECTION_DOWN)
			{
				pos += D3DXVECTOR3(-dist*hcos, -dist*hsin, 0.0f);
				displacement = D3DXVECTOR3(-dist*hcos, -dist*hsin, 0.0f);
			}
			else if (direction == DIRECTION_LEFT)
			{
				pos += D3DXVECTOR3(-dist*hsin, dist*hcos, 0.0f);
				displacement = D3DXVECTOR3(-dist*hsin, dist*hcos, 0.0f);
			}
			else if (direction == DIRECTION_RIGHT)
			{
				pos += D3DXVECTOR3(dist*hsin, -dist*hcos, 0.0f);
				displacement = D3DXVECTOR3(dist*hsin, -dist*hcos, 0.0f);
			}
			else if (direction == DIRECTION_TOPLEFT)
			{
				pos += D3DXVECTOR3(dist*cos(hAngle + D3DX_PI / 8), dist*sin(hAngle + D3DX_PI / 8), 0.0f);
				displacement = D3DXVECTOR3(dist*cos(hAngle + D3DX_PI / 8), dist*sin(hAngle + D3DX_PI / 8), 0.0f);
			}
			else if (direction == DIRECTION_TOPRIGHT)
			{
				pos += D3DXVECTOR3(dist*cos(hAngle - D3DX_PI / 8), dist*sin(hAngle - D3DX_PI / 8), 0.0f); 
				displacement = D3DXVECTOR3(dist*cos(hAngle - D3DX_PI / 8), dist*sin(hAngle - D3DX_PI / 8), 0.0f);
			}
			else if (direction == DIRECTION_BOTTOMLEFT)
			{
				pos += D3DXVECTOR3(-dist*cos(hAngle - D3DX_PI / 8), -dist*sin(hAngle - D3DX_PI / 8), 0.0f);
				displacement = D3DXVECTOR3(-dist*cos(hAngle - D3DX_PI / 8), -dist*sin(hAngle - D3DX_PI / 8), 0.0f);
			}
			else if (direction == DIRECTION_BOTTOMRIGHT)
			{
				pos += D3DXVECTOR3(-dist*cos(hAngle + D3DX_PI / 8), -dist*sin(hAngle + D3DX_PI / 8), 0.0f);
				displacement = D3DXVECTOR3(-dist*cos(hAngle + D3DX_PI / 8), -dist*sin(hAngle + D3DX_PI / 8), 0.0f);
			}
			//更新平移矩阵
			D3DXMatrixIdentity(&matTranslation);
			D3DXMatrixTranslation(&matTranslation, pos.x, pos.y, pos.z);
			//更新平移偏转矩阵
			D3DXMatrixIdentity(&matTranslation2);
			D3DXMatrixTranslation(&matTranslation2, pos.x + TINYBIAS, pos.y, 0);
			//更新世界矩阵
			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixMultiply(&matWorld, &matTranslation, &matWorld);
			D3DXMatrixMultiply(&matWorld, &matHRotate, &matWorld);
		}
		else if (viewmode == VIEWMODE_OVERLOOK)
		{
			float accerate = 20.0f;
			if (direction == DIRECTION_TOP)//前进
			{
				viewpos += accerate*D3DXVECTOR3(dist*hcos, dist*hsin, 0.0f);
			}
			else if (direction == DIRECTION_DOWN)
			{
				viewpos -= accerate*D3DXVECTOR3(dist*hcos, dist*hsin, 0.0f);
			}
			else if (direction == DIRECTION_LEFT)
			{
				viewpos += accerate*D3DXVECTOR3(-dist*hsin, dist*hcos, 0.0f);
			}
			else if (direction == DIRECTION_RIGHT)
			{
				viewpos += accerate*D3DXVECTOR3(dist*hsin, -dist*hcos, 0.0f);
			}
			else if (direction == DIRECTION_TOPLEFT)
			{
				viewpos += accerate*D3DXVECTOR3(dist*cos(hAngle + D3DX_PI / 8), dist*sin(hAngle + D3DX_PI / 8), 0.0f);
			}
			else if (direction == DIRECTION_TOPRIGHT)
			{
				viewpos += accerate*D3DXVECTOR3(dist*cos(hAngle - D3DX_PI / 8), dist*sin(hAngle - D3DX_PI / 8), 0.0f);
			}
			else if (direction == DIRECTION_BOTTOMLEFT)
			{
				viewpos += accerate*D3DXVECTOR3(-dist*cos(hAngle - D3DX_PI / 8), -dist*sin(hAngle - D3DX_PI / 8), 0.0f);
			}
			else if (direction == DIRECTION_BOTTOMRIGHT)
			{
				viewpos += accerate*D3DXVECTOR3(-dist*cos(hAngle + D3DX_PI / 8), -dist*sin(hAngle + D3DX_PI / 8), 0.0f);
			}
			displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
		else if (viewmode == VIEWMODE_FREE)
		{
			if (direction == DIRECTION_TOP)//前进
			{
				viewpos += D3DXVECTOR3(dist*hcos*vcos, dist*hsin*vcos, -dist*vsin);
			}
			else if (direction == DIRECTION_DOWN)
			{
				viewpos -= D3DXVECTOR3(dist*hcos*vcos, dist*hsin*vcos, -dist*vsin);
			}
			else if (direction == DIRECTION_LEFT)
			{
				viewpos += D3DXVECTOR3(-dist*hsin, dist*hcos, 0.0f);
			}
			else if (direction == DIRECTION_RIGHT)
			{
				viewpos += D3DXVECTOR3(dist*hsin, -dist*hcos, 0.0f);
			}
			else if (direction == DIRECTION_TOPLEFT)
			{
				viewpos += D3DXVECTOR3(dist*cos(hAngle + D3DX_PI / 8)*vcos, dist*sin(hAngle + D3DX_PI / 8)*vcos, -dist*vsin / 2);
			}
			else if (direction == DIRECTION_TOPRIGHT)
			{
				viewpos += D3DXVECTOR3(dist*cos(hAngle - D3DX_PI / 8)*vcos, dist*sin(hAngle - D3DX_PI / 8)*vcos, -dist*vsin / 2);
			}
			else if (direction == DIRECTION_BOTTOMLEFT)
			{
				viewpos += D3DXVECTOR3(-dist*cos(hAngle - D3DX_PI / 8)*vcos, -dist*sin(hAngle - D3DX_PI / 8)*vcos, -dist*vsin / 2);
			}
			else if (direction == DIRECTION_BOTTOMRIGHT)
			{
				viewpos += D3DXVECTOR3(-dist*cos(hAngle + D3DX_PI / 8)*vcos, -dist*sin(hAngle + D3DX_PI / 8)*vcos, -dist*vsin / 2);
			}
			displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
		//更新区块位置
		blockindex.x = floor((viewpos.x + 200) / 400);
		blockindex.y = floor((viewpos.y + 200) / 400);

		SetViewVector();

		viewchanged = true;//设置视角更新标志
		moved = true;//设置视角移动标志
		//SetView();// WHETHER NEEDED
	}
	inline void HandleMove()					//处理当前移动
	{
		HandleSpeed();
		Walk();
	}
	inline void Rotate(POINT bias)				//处理当前旋转
	{
		hAngle += -bias.x * sensitivity;
		if (hAngle < 0)
			hAngle += 2 * D3DX_PI;
		if (hAngle >= 2 * D3DX_PI)
			hAngle -= 2 * D3DX_PI;

		vAngle -= bias.y * sensitivity;
		if (vAngle < -D3DX_PI / 2)
			vAngle = -D3DX_PI / 2;
		if (vAngle > D3DX_PI / 2)
			vAngle = D3DX_PI / 2;

		if (viewmode == VIEWMODE_CHASE || viewmode == VIEWMODE_FIRSTPERSON)
		{
			//同步人物角度
			figurehangle = hAngle;
			figurevangle = vAngle;

			//更新水平旋转矩阵
			D3DXMatrixIdentity(&matHRotate);
			D3DXMatrixRotationZ(&matHRotate, figurehangle);
			//更新世界矩阵
			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixMultiply(&matWorld, &matTranslation, &matWorld);
			D3DXMatrixMultiply(&matWorld, &matHRotate, &matWorld);

			//更新区块位置
			blockindex.x = floor((viewpos.x + 200) / 400);
			blockindex.y = floor((viewpos.y + 200) / 400);
		}
		hcos = cos(hAngle);
		hsin = sin(hAngle);
		vsin = sin(vAngle);
		vcos = cos(vAngle);
		viewdirection = D3DXVECTOR3(hcos*vcos, hsin*vcos, -vsin);

		SetViewVector();
		viewchanged = true;//设置视角更新参数
		//SetView();// WHETHER NEEDED
	}
	void DirectionControl();					//根据实时按键状态更新curdirection,并设置speed的启动
	bool KeyControlDown(int key);				//按下按键的处理
	void KeyControlUp(int key);					//抬起按键的处理
	void Inactivate();							//窗口失去焦点后的处理
	string GetViewmodeStr();					//获得当前view模式的字符串
	int ChangeViewmode();						//切换view模式
	inline void Draw()
	{
		if (!device)
			return;

		if (!figure)
			return;

		//
		//观察者绘制
		//
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetMaterial(&figuremtrl);
		figure->DrawSubset(0);

		//
		//shadow 绘制
		//
		if (shadowchanged || moved)
			matShadowWorld = matTranslation * matShadow;
		device->SetTransform(D3DTS_WORLD, &matShadowWorld);

		ShadowBegin();
		figure->DrawSubset(0);
		ShadowEnd();


		if (flashlight)
		{
			DrawLight();
		}

	}
	inline void DrawLight()
	{
		if (!figurelight)
			return;

		device->SetTransform(D3DTS_WORLD, &matTranslation);

		//物体光源贴图
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		//device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		//搭配1：D3DBLEND_DESTCOLOR & D3DBLEND_SRCALPHA
		//搭配2：D3DBLEND_SRCALPHA & D3DBLEND_INVSRCALPHA || D3DBLEND_INVDESTCOLOR & D3DBLEND_SRCALPHA
		//搭配3：D3DBLEND_SRCALPHA & D3DBLEND_SRCALPHA
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);//关闭ZBUFFER的写入，防重影闪烁
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);//关闭雾，防阴影全雾色
		device->SetRenderState(D3DRS_LIGHTING, FALSE);//关闭光照

		device->SetTexture(0, g_Texture); 
		float db2 = DEPTHBIAS;
		float sdb2 = SLOPESCALEDEPTHBIAS;
		if (depthbiasable)
		{
			device->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)&db2));
			device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&sdb2));
		}
		figurelight->DrawSubset(0); 
		device->SetTransform(D3DTS_WORLD, &matTranslation2);
		//figurelight->DrawSubset(0);
		device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
		device->SetRenderState(D3DRS_DEPTHBIAS, 0);
		device->SetTexture(0, 0);

		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState(D3DRS_FOGENABLE, TRUE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);//关闭ZBUFFER的写入，防重影闪烁
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	}
};