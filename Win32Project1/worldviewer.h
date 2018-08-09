#pragma once

#include "D3Ddefine.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

//颜色
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
const float speednormal = WALKSPEED;		//正常速度
const float speeddelaytime = 0.04f;			//速度维持不变的时间。残余速度时间1
const float speedsmoothtime = 0.1f;			//速度降为0需要的时间。残余速度时间2
const float chasedist = 0.6f;				//追捕视角水平距离
const float chasescreenheight = 0.2f;		//追捕视角高度
const float overlookscreenheight = 300.0f;	//俯视高度

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
	D3DMATERIAL9 figuremtrl;			//人物材质
	LPDIRECT3DTEXTURE9 g_Texture;		//地面光照纹理

	//实时信息
	DoubleVec3 viewpos_d;
	DoubleVec3 displacement;			//位移
	double baselongitude, baselatitude;
	double longitude, latitude;			//眼睛经纬度TODO:旋转时修改
	double figurelongitude, figurelatitude;
	DoubleVec3 pos_d;					//double类型人物位置
	INDEX2D blockindex;					//眼睛位于哪个区块
	D3DXVECTOR3 inblockpos;
	D3DXMATRIX inblockTranslation;

	float figurehangle, figurevangle;	//人物朝向(0~360) & (-90~90)
	double hAngle;						//视角水平面角度	(0~360) x正向=0
	double basehangle;
	double earthhangle;					//在地球上当前位置，实际的指向
	double vAngle;						//视角纵向角度	(-90~90)
	D3DXVECTOR3 viewdirection;			//眼睛方向, 用于辅助计算at：eye + viewdirection = at
	D3DXVECTOR3 eye;
	D3DXVECTOR3 at;						//视线目标
	D3DXVECTOR3 up;
	float viewangle;					//视野角度
	float viewaspect;					//视野长宽比

	//物理运动
	LARGE_INTEGER frequency;			//计时器频率
	LARGE_INTEGER endtick;				//结束方向键时的ticks，用于残余速度计算
	LARGE_INTEGER lasttick;				//上一个tick，用于当前行走距离计算
	float speed;						//当前速度
	float speedrate;
	int remnantspeedmode;				//残余速度模式
	int curdirection;					//当前运动方向
	int lastdirection;					//当前方向为0时存储上一个方向，处理残余速度的运动
	bool wdown, sdown, adown, ddown;	//按键状态
	bool shiftdown;						//按键状态

	D3DXVECTOR3 figureeye;				//视角位置(人物眼睛相对人物模型位置)
	float viewradius;					//视野距离
	float sensitivity;					//旋转灵敏度
	double hcos, hsin, vsin, vcos, bhcos, bhsin;		//加速运算
	bool flashlight;

	int viewmode;						//view模式
	bool viewchanged;
	bool figuremoved;
	bool eyehmoved;
	bool bindview;

public:
	WViewer();
	~WViewer();

	void InitProj(float aspect);	//设置投影变换参数并设置变换
	void SetFigure();

	bool SetDevice(LPDIRECT3DDEVICE9 device);	//传递设备
	bool SetViewAngle(float angle);				//设置view角度
	bool SetAspect(float aspect);				//设置aspect
	void SetViewmode(int mode);					//设置观察模式
	inline void Refreshtri()
	{
		hcos = cos(hAngle);
		hsin = sin(hAngle);
		vsin = sin(vAngle);
		vcos = cos(vAngle);
	}
	inline void BindView()
	{
		switch (viewmode)
		{
		case VIEWMODE_FIRSTPERSON:
			viewpos_d = pos_d + DoubleVec3(hcos*figureeye.x -hsin*figureeye.y
				, hsin*figureeye.x + hcos*figureeye.y, figureeye.z);
			break;
		case VIEWMODE_CHASE:
			viewpos_d = pos_d + DoubleVec3(-chasedist*hcos, -chasedist*hsin, -chasescreenheight);
			break;
		case VIEWMODE_FREE:
			//viewpos_d = pos_d + DoubleVec3(-chasedist*hcos, -chasedist*hsin, -chasescreenheight);
			break;
		case VIEWMODE_OVERLOOK:
			//viewpos_d = pos_d + DoubleVec3(0.0f, 0.0f, -overlookscreenheight);
			//at = inblockpos;//限制垂直视角
			break;
		}

		bindview = false;
	}
	inline void SetBlockInfo()
	{
		//更新区块位置
		blockindex.x = (long long)floor((viewpos_d.x + BLOCKRADIUS) / (long long)(2 * BLOCKRADIUS));
		blockindex.y = (long long)floor((viewpos_d.y + BLOCKRADIUS) / (long long)(2 * BLOCKRADIUS));
		//更新inblockpos，实际眼睛位置
		inblockpos = D3DXVECTOR3(
			(float)(viewpos_d.x - blockindex.x * 2 * BLOCKRADIUS)
			, (float)(viewpos_d.y - blockindex.y * 2 * BLOCKRADIUS)
			, 0.0f);
		//更新平移矩阵
		D3DXMatrixIdentity(&inblockTranslation);
		D3DXMatrixTranslation(&inblockTranslation, inblockpos.x, inblockpos.y, 0.0f);//z方向不平移

		//eyehmoved = false;
	}
	inline void SetViewVector()					//根据当前人物位置pos_d和观察模式，更新视线参数
	{
		//eye = D3DXVECTOR3(inblockpos.x, inblockpos.y, viewpos_d.z);
		if(viewmode==VIEWMODE_CHASE)//经纬坐标策略，不使用区块
			eye = D3DXVECTOR3(-chasedist*hcos, -chasedist*hsin, -chasescreenheight);
		else if(viewmode==VIEWMODE_FIRSTPERSON)
			eye = D3DXVECTOR3(hcos*figureeye.x - hsin*figureeye.y, hsin*figureeye.x + hcos*figureeye.y, figureeye.z);
		else if(viewmode==VIEWMODE_FREE ||viewmode==VIEWMODE_OVERLOOK)
			eye = D3DXVECTOR3(0.0f, 0.0f, viewpos_d.z);

		//更新up
		if (vAngle == -MYPI / 2)
			up = D3DXVECTOR3((float)(hcos), (float)(hsin), 0.0f);
		else if (vAngle == MYPI / 2)
			up = -D3DXVECTOR3((float)(hcos), (float)(hsin), 0.0f);
		else
			up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		viewdirection = D3DXVECTOR3((float)(hcos*vcos), (float)(hsin*vcos), (float)(-vsin));
		//更新at
		at = eye + viewdirection;//自由视角
	}
	inline bool SetView()//更新视角矩阵
	{
		if (!device)
			return false;

		/*if (viewchanged)
		{
			SetViewVector();
			D3DXMatrixLookAtLH(&matView, &eye, &at, &up);
			viewchanged = false;
		}*/

		device->SetTransform(D3DTS_VIEW, &matView);

		return true;
	}
	inline bool RefreshView()
	{
		SetViewVector();
		D3DXMatrixLookAtLH(&matView, &eye, &at, &up);

		viewchanged = false;

		return true;
	}
	bool SetProj();								//根据当前参数设置投影变换

	inline void HandleSpeed()					//处理当前速度
	{
		if (speed == 0.0f)
			return;

		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		float time = (float)(now.QuadPart - endtick.QuadPart) / frequency.QuadPart;
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
		{
			displacement = DoubleVec3(0.0, 0.0, 0.0f);
			return;
		}

		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		double dist = speed*(double)(now.QuadPart - lasttick.QuadPart) / frequency.QuadPart;//移动距离
		lasttick = now;//记录为上一次行走时间点
		dist *= speedrate;
		//如果当前运动方向为NONE，使用存储的上一个运动方向
		int direction = (curdirection == DIRECTION_NONE) ? lastdirection : curdirection;
		if (viewmode == VIEWMODE_FIRSTPERSON || viewmode == VIEWMODE_CHASE)
		{
			if (direction == DIRECTION_TOP)//前进
			{
				displacement.x = dist*hcos;
				displacement.y = dist*hsin;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_DOWN)
			{
				displacement.x = -dist*hcos;
				displacement.y = -dist*hsin;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_LEFT)
			{
				displacement.x = -dist*hsin;
				displacement.y = dist*hcos;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_RIGHT)
			{
				displacement.x = dist*hsin;
				displacement.y = -dist*hcos;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_TOPLEFT)
			{
				double biash = hAngle + MYPI / 8;
				displacement.x = dist*cos(biash);
				displacement.y = dist*sin(biash);
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_TOPRIGHT)
			{
				double biash = hAngle - MYPI / 8;
				displacement.x = dist*cos(biash);
				displacement.y = dist*sin(biash);
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_BOTTOMLEFT)
			{
				double biash = hAngle - MYPI / 8;
				displacement.x = -dist*cos(biash);
				displacement.y = -dist*sin(biash);
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_BOTTOMRIGHT)
			{
				double biash = hAngle + MYPI / 8;
				displacement.x = -dist*cos(biash);
				displacement.y = -dist*sin(biash);
				displacement.z = 0.0;
			}
			pos_d.x += displacement.x;
			pos_d.y += displacement.y;
			pos_d.z += displacement.z;
		}
		else if (viewmode == VIEWMODE_OVERLOOK)
		{
			if (direction == DIRECTION_TOP)//前进
			{
				displacement.x = dist*hcos;
				displacement.y = dist*hsin;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_DOWN)
			{
				displacement.x = -dist*hcos;
				displacement.y = -dist*hsin;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_LEFT)
			{
				displacement.x = -dist*hcos;
				displacement.y = dist*hsin;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_RIGHT)
			{
				displacement.x = dist*hcos;
				displacement.y = -dist*hsin;
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_TOPLEFT)
			{
				double biash = hAngle + MYPI / 8;
				displacement.x = dist*cos(biash);
				displacement.y = dist*sin(biash);
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_TOPRIGHT)
			{
				double biash = hAngle - MYPI / 8;
				displacement.x = dist*cos(biash);
				displacement.y = dist*sin(biash);
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_BOTTOMLEFT)
			{
				double biash = hAngle - MYPI / 8;
				displacement.x = -dist*cos(biash);
				displacement.y = -dist*sin(biash);
				displacement.z = 0.0;
			}
			else if (direction == DIRECTION_BOTTOMRIGHT)
			{
				double biash = hAngle + MYPI / 8;
				displacement.x = -dist*cos(biash);
				displacement.y = -dist*sin(biash);
				displacement.z = 0.0;
			}
			viewpos_d.x += displacement.x;
			viewpos_d.y += displacement.y;
			viewpos_d.z += displacement.z;
		}
		else if (viewmode == VIEWMODE_FREE)
		{
			if (direction == DIRECTION_TOP)//前进
			{
				displacement.x = dist*hcos*vcos;
				displacement.y = dist*hsin*vcos;
				displacement.z = -dist*vsin;
			}
			else if (direction == DIRECTION_DOWN)
			{
				displacement.x = -dist*hcos*vcos;
				displacement.y = -dist*hsin*vcos;
				displacement.z = dist*vsin;
			}
			else if (direction == DIRECTION_LEFT)
			{
				displacement.x = -dist*hsin;
				displacement.y = dist*hcos;
				displacement.z = 0.0f;
			}
			else if (direction == DIRECTION_RIGHT)
			{
				displacement.x = dist*hsin;
				displacement.y = -dist*hcos;
				displacement.z = 0.0f;
			}
			else if (direction == DIRECTION_TOPLEFT)
			{
				double biash = hAngle + MYPI / 8;
				displacement.x = dist*cos(biash)*vcos;
				displacement.y = dist*sin(biash)*vcos;
				displacement.z = -dist*vsin / 2;
			}
			else if (direction == DIRECTION_TOPRIGHT)
			{
				double biash = hAngle - MYPI / 8;
				displacement.x = dist*cos(biash)*vcos;
				displacement.y = dist*sin(biash)*vcos;
				displacement.z = -dist*vsin / 2;
			}
			else if (direction == DIRECTION_BOTTOMLEFT)
			{
				double biash = hAngle - MYPI / 8;
				displacement.x = -dist*cos(biash)*vcos;
				displacement.y = -dist*sin(biash)*vcos;
				displacement.z = dist*vsin / 2;
			}
			else if (direction == DIRECTION_BOTTOMRIGHT)
			{
				double biash = hAngle + MYPI / 8;
				displacement.x = -dist*cos(biash)*vcos;
				displacement.y = -dist*sin(biash)*vcos;
				displacement.z = dist*vsin / 2;
			}
			viewpos_d.x += displacement.x;
			viewpos_d.y += displacement.y;
			viewpos_d.z += displacement.z;
		}

		if (viewmode == VIEWMODE_FIRSTPERSON || viewmode == VIEWMODE_CHASE)
		{
			//moved = true; //经纬坐标策略，不设置
			bindview = true;
		}
		else
			figuremoved = true;
		eyehmoved = true;
		viewchanged = true;
	}
	inline void HandleMove()					//处理当前移动
	{
		HandleSpeed();
		Walk();
	}
	inline bool Rotate(POINT bias)				//处理当前旋转
	{
		if (bias.x == 0 && bias.y == 0)
		{
			return false;
		}
		double oldh = hAngle, oldv = vAngle;
		double hbias = -bias.x * sensitivity;
		hAngle += hbias;
		if (hAngle < 0)
			hAngle += 2 * MYPI;
		if (hAngle >= 2 * MYPI)
			hAngle -= 2 * MYPI;

		vAngle -= bias.y * sensitivity;
		if (vAngle < -MYPI / 2)
			vAngle = -MYPI / 2;
		if (vAngle > MYPI / 2)
			vAngle = MYPI / 2;

		Refreshtri();

		if (viewmode == VIEWMODE_CHASE || viewmode == VIEWMODE_FIRSTPERSON)
		{
			//同步人物角度
			figurehangle += -bias.x * sensitivity;
			figurevangle += -bias.y * sensitivity;
			if (figurehangle < 0)
				figurehangle += (float)(2 * MYPI);
			if (figurehangle >= (float)(2 * MYPI))
				figurehangle -= (float)(2 * MYPI);
			
			if (figurevangle < -(float)(MYPI / 2))
				figurevangle = -(float)(MYPI / 2);
			if (figurevangle >(float)(MYPI / 2))
				figurevangle = (float)(MYPI / 2);

			double radius;
			if (viewmode == VIEWMODE_CHASE)
				radius = chasedist;
			else
				radius = sqrt(figureeye.x*figureeye.x + figureeye.y*figureeye.y);

			/*displacement = DoubleVec3(
				radius*(cos(hAngle)-cos(oldh))
				, radius*(sin(vAngle) - cos(oldv))*cos(oldh)
				, 0.0f);*/
		}
		else
		{
			;//眼睛位置没改变，不更新人物矩阵，不更新区块位置
		}

		
		if (viewmode == VIEWMODE_CHASE || viewmode == VIEWMODE_FIRSTPERSON)
		{
			eyehmoved = true;
			bindview = true;
			figuremoved = true;
		}
		viewchanged = true;//设置视角更新参数

		return true;
	}
	inline void ViewMove()//根据viewpos和基准经纬度计算实时经纬度，在SunMove()前调用
	{
		//过极点时如果3D世界视角也偏转，取消注释
		//if (anti)
		//	displacement = -displacement;


		//经纬度
		if (cos(latitude) == 0)
			latitude = latitude
			+ sqrt(displacement.x*displacement.x + displacement.y*displacement.y) / ONEMETER / AVGRADIUS;
		else
		{
			latitude = latitude + displacement.y / ONEMETER / AVGRADIUS;
			longitude = longitude + displacement.x / ONEMETER / AVGRADIUS / cos(latitude);
		}
		

		double unnormalizedlatitude = latitude;

		//规范化经纬度
		NormalizeLongitudeLatitude(longitude, latitude);

		//更新地球角度
		if (latitude == MYPI / 2)
			earthhangle = 3 * MYPI / 2;
		else if (latitude == -MYPI / 2)
			earthhangle = MYPI / 2;
		else
			earthhangle = hAngle;

		if (unnormalizedlatitude > MYPI / 2 || unnormalizedlatitude < -MYPI / 2)
		{
			//经度转向
			if (longitude > 0)
				longitude -= MYPI;
			else
				longitude += MYPI;

			anti = !anti;


			if (hAngle < MYPI)
				earthhangle = hAngle + MYPI;
			else
				earthhangle = hAngle - MYPI;

			//过极点时，3D世界视角也偏转
			hAngle = earthhangle;
			Refreshtri();
			bindview = true;
		}


		//if (anti)//越过（南北）极点，角度翻转
		//{
		//	if (hAngle < MYPI)
		//		earthhangle = hAngle + MYPI;
		//	else
		//		earthhangle = hAngle - MYPI; 
		//	
		//	//viewchanged = true;
		//	//hAngle = earthhangle;//TODO:3D世界视角角度是否也翻转
		//}
		//else
		//	earthhangle = hAngle;
	}
	void DirectionControl();					//根据实时按键状态更新curdirection,并设置speed的启动
	bool KeyControlDown(int key);				//按下按键的处理
	void KeyControlUp(int key);					//抬起按键的处理
	void Inactivate();							//窗口失去焦点后的处理
	string GetViewmodeStr();					//获得当前view模式的字符串
	int ChangeViewmode();						//切换view模式
	inline void SetFigureMat()
	{
		//更新平移矩阵
		D3DXMatrixIdentity(&matTranslation); 
		//D3DXMatrixTranslation(&matTranslation
		//	, (float)(pos_d.x - blockindex.x * 2 * BLOCKRADIUS)
		//	, (float)(pos_d.y - blockindex.y * 2 * BLOCKRADIUS)
		//	, 0.0f);//垂直方向不限制，TODO垂直方向traslation采用double
		//			//更新平移偏转矩阵
		//D3DXMatrixIdentity(&matTranslation2);
		//D3DXMatrixTranslation(&matTranslation2
		//	, (float)(pos_d.x - blockindex.x * 2 * BLOCKRADIUS) + TINYBIAS
		//	, (float)(pos_d.y - blockindex.y * 2 * BLOCKRADIUS)
		//	, 0);
		if (viewmode == VIEWMODE_CHASE|| viewmode == VIEWMODE_FIRSTPERSON)
		{
			D3DXMatrixTranslation(&matTranslation
				, 0.0f
				, 0.0f
				, 0.0f);//垂直方向不限制，TODO垂直方向traslation采用double
						//更新平移偏转矩阵
			D3DXMatrixIdentity(&matTranslation2);
			D3DXMatrixTranslation(&matTranslation2
				, 0.0f + TINYBIAS
				, 0.0f
				, 0);
		}
		else if (viewmode == VIEWMODE_FREE || viewmode == VIEWMODE_OVERLOOK)
		{
			D3DXMatrixTranslation(&matTranslation
				, (float)(pos_d.x - viewpos_d.x)
				, (float)(pos_d.y - viewpos_d.y)
				, 0.0f);//垂直方向不限制，TODO垂直方向traslation采用double
						//更新平移偏转矩阵
			D3DXMatrixIdentity(&matTranslation2);
			D3DXMatrixTranslation(&matTranslation2
				, (float)(pos_d.x - viewpos_d.x) + TINYBIAS
				, (float)(pos_d.y - viewpos_d.y)
				, 0);
		}
		//更新水平旋转矩阵
		D3DXMatrixIdentity(&matHRotate);
		D3DXMatrixRotationZ(&matHRotate, figurehangle);
		//更新世界矩阵
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixMultiply(&matWorld, &matTranslation, &matWorld);
		D3DXMatrixMultiply(&matWorld, &matHRotate, &matWorld);
		//更新阴影矩阵
		matShadowWorld = matShadow*matTranslation;

		figuremoved = false;
	}
	inline void Render()
	{
		if (!device)
			return;

		if (!figure)
			return;

		if (figuremoved)
			SetFigureMat();
		//
		//shadow 绘制
		//
		if (!sundown)
		{
			if (shadowchanged)
				matShadowWorld = matShadow*matTranslation;//TODO可能算了两遍
			device->SetTransform(D3DTS_WORLD, &matShadowWorld);

			ShadowBegin();
			figure->DrawSubset(0);
			ShadowEnd();
		}

		//
		//观察者绘制
		//
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetMaterial(&figuremtrl);
		figure->DrawSubset(0);

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
		figurelight->DrawSubset(0);
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