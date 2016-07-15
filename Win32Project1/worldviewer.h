#pragma once

#include <d3d9.h>
#include <d3dx9tex.h>

#define VIEWMODE_FIRSTPERSON	1
#define VIEWMODE_CHASE			2
#define VIEWMODE_FREE			3
#define VIEWMODE_OVERLOOK		4

//运动
const float division = 0.1f;
const float chasedist = 4.0f;
const float chaseheight = 3.0f;
const float overlookheight = 400.0f;

class WViewer {
public:
	LPDIRECT3DDEVICE9 device;

	D3DXVECTOR3 up;

	D3DXVECTOR3 pos;		//观察者位置
	D3DXVECTOR3 at;
	D3DXVECTOR3 viewpos;

	float viewheight;		//视角高度(人物高度)
	float viewdist;			//视野距离
	float viewangle;		//视野角度
	float viewaspect;		//视野长宽比
	float hAngle;			//水平面角度	(0~360) x正向=0
	float vAngle;			//纵向角度	(-90~90)

	int viewmode;			//观察模式

public:
	WViewer();
	~WViewer();

	bool SetDevice(LPDIRECT3DDEVICE9 device);
	bool SetAspect(float aspect);
	bool SetViewmode(int mode);
	bool SetView(LPDIRECT3DDEVICE9 dvc = NULL);

	void Walk(int key);
	void Rotate(POINT bias);
	void KeyControl(int key);
};