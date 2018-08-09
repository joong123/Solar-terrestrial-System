#pragma once

#include <d3d9.h>
#include <d3dx9tex.h>
#include <time.h>
#include "mytime.h"
#include "mygeography.h"
#include "mytype.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#define MINDOUBLE_DECIMAL		0.00000000000000010000000000000001
//顶点
#define FVF_CUSTOM1				(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define FVF_CUSTOM2				(D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define FVF_CUSTOM3				(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//物理定义
#define ONEMETER				0.1f				//1米的距离
#define RADIAN2DEGREE			(180.0 / MYPI)

//尺度定义
#define SKYRADIUS				1500.0f				//天空半径
#define SUNDIST					1000.0f				//
#define SUNRADIUS				(SUNDIST*1.392/149.59787/2)	//太阳半径
#define VIEWRADIUS				10000.0f			//视线最远距离（远裁面），影响雾距离
#define BLOCKRADIUS				200.0f				//一个区块的半径（正方形半边长）
#define BLOCKRANGE				50.0f				//区块显示范围，超出范围的物体不绘制
#define FOGSTART				20.0f				//雾开始点
#define FOGEND					BLOCKRANGE			//雾100%浓度的距离，超出范围不可见
#define PERSPECTIVEZNEAR_MIN	0.02f				//最小近裁面0.03
#define SHADOWHEIGHT_MIN		0.0f				//阴影最小离地高度，小于容易闪烁，暂不使用depthbias代替
#define WALKSPEED				0.4f				//每秒运动的距离
#define DEFAULTFOVY				(float)(MYPI / 4)	//视锥上下夹角
#define MAXSPHERELOOPS			110					//最大球体精度
#define DEPTHBIAS				-0.0000005f
#define SLOPESCALEDEPTHBIAS		-1.10f				//负值越接近0越容易近距离看闪烁，越远离0阴影越容易盖过物体
#define TINYBIAS				0.000001f			//微小平移量，防重叠干扰线
#define SIDESUN_UP				100					//南北侧面辅助太阳光的相对高度


#define DEFAULTSENSITIVITY		0.0012f
#define SIDESUNANGLE			(40.0*MYPI / 180.0)	//南北侧面辅助太阳光的相对偏角
#define MAXSHADOWINTENSITY		0.4f				//阴影最大强度（透明度）

//其他参数
#define ANGLE_NEARRANGE			1.0f				//靠近一个方向多少度以内算“靠近”

//颜色
#define COLOR_BLACK				D3DCOLOR_XRGB(0, 0, 0)
#define COLOR_WHITE				D3DCOLOR_XRGB(255, 255, 255)

#define COLOR_SUN				D3DCOLOR_XRGB(254, 254, 248)
#define COLOR_YELLOWGRASS		D3DCOLOR_XRGB(145, 185, 22)


struct INDEX2D
{
	long long int x;
	long long int y;
};

struct CUSTOMVERTEX1
{
	D3DXVECTOR3 pos;     //顶点位置
	D3DXVECTOR3 normal;  //顶点法线
	
	DWORD color;
};
struct CUSTOMVERTEX2
{
	D3DXVECTOR3 pos;     //顶点位置

	DWORD color;
};
struct CUSTOMVERTEX3
{
	D3DXVECTOR3 pos;     //顶点位置
	D3DXVECTOR3 normal;  //顶点法线
	DWORD color;    
	float tu, tv;
};

const D3DCOLORVALUE sunlightD = { 1.0f, 1.0f, 0.9f, 1.0f };
const D3DCOLORVALUE sunlightD_D = { 0.2f, 0.2f, 0.2f, 1.0f };
const D3DCOLORVALUE sunlightA = { 0.5f, 0.5f, 0.5f, 1.0f };
const D3DCOLORVALUE sunlightA_D = { 0.5f, 0.5f, 0.5f, 1.0f };
const D3DXVECTOR3 earthaxis = { (float)sin(ECLIPTICOBLIQUITY_R), 0.0f, (float)cos(ECLIPTICOBLIQUITY_R) };
const D3DXPLANE groundPlane = D3DXPLANE(0.0f, 0.0f, -1.0f, -SHADOWHEIGHT_MIN);

//共享变量
extern LPDIRECT3DDEVICE9 device;
extern time_t nowtime, basetime, lasttime, realtime;
extern tm devicedate, basedate, lastdate, realdate;
extern int devicetimezone;

extern bool anti;
extern bool shadowchanged;
extern D3DXMATRIX matShadow;
extern D3DMATERIAL9 shadowmtrl;
extern bool sundown;

extern D3DCAPS9 caps;
extern float depthbias;
extern float sdepthbias;
extern bool depthbiasable;


inline void ShadowBegin()
{
	if (!device)
		return;

	//device->SetRenderState(D3DRS_STENCILENABLE, TRUE);//开启Stencil buffer
	//device->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);//清除stencil??是否需要
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);//alpha blend the shadow
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);//关闭ZBUFFER的写入
	device->SetRenderState(D3DRS_FOGENABLE, FALSE);//关闭雾，防阴影全雾色
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);//设置材质为颜色来源，防雾颜色不对

	//depthbias
	if (depthbiasable)
	{
		device->SetRenderState(D3DRS_DEPTHBIAS, *((DWORD*)&depthbias));
		device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *((DWORD*)&sdepthbias));
	}
	device->SetMaterial(&shadowmtrl);
}
inline void ShadowEnd()
{
	device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	device->SetRenderState(D3DRS_DEPTHBIAS, 0);

	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	device->SetRenderState(D3DRS_FOGENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}

//P' = Pcosθ + (A × P)sinθ + A(A·P)(1 - cosθ) 
inline D3DXVECTOR3 AxisRotate(const D3DXVECTOR3 &axis, const D3DXVECTOR3 &point, double angle)
{
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &axis, &point);
	D3DXVECTOR3 result =
		point*cos(angle)
		+ cross*sin(angle)
		+ axis*D3DXVec3Dot(&axis, &point)*(1 - cos(angle));

	return result;

	//矩阵法
	/*D3DXMATRIX rot;
	D3DXMatrixRotationAxis(&rot, &axis, angle);*/
}
inline D3DXVECTOR3 VecProj(const D3DXVECTOR3 &fanfanbaichi, const D3DXVECTOR3 &direction)
{
	D3DXVECTOR3 result = fanfanbaichi - direction*D3DXVec3Dot(&fanfanbaichi, &direction)/ D3DXVec3Dot(&direction, &direction);

	return result;
}
void CreateSphere(ID3DXMesh **obj, int finess, float radius, D3DCOLOR color, float height);