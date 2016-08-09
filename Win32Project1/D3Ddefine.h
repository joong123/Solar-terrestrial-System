#pragma once

#include <d3d9.h>
#include <d3dx9tex.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//顶点
#define FVF_CUSTOM1				(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define FVF_CUSTOM2				(D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define FVF_CUSTOM3				(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//物理定义
#define ONEMETER				0.1f				//1米的距离

//尺度定义
#define SKYRADIUS				1500.0f				//天空半径
#define VIEWRADIUS				10000.0f			//视线最远距离（远裁面），影响雾距离
#define BLOCKRADIUS				200.0f				//一个区块的半径（正方形半边长）
#define BLOCKRANGE				50.0f				//区块显示范围，超出范围的物体不绘制
#define FOGSTART				20.0f				//雾开始点
#define FOGEND					BLOCKRANGE			//雾100%浓度的距离，超出范围不可见
#define PERSPECTIVEZNEAR_MIN	0.02f				//最小近裁面0.03
#define SHADOWHEIGHT_MIN		0.0f				//阴影最小离地高度，小于容易闪烁，暂不使用depthbias代替
#define WALKSPEED				0.4f				//每秒运动的距离
#define DEFAULTFOVY				(D3DX_PI / 3)		//视锥上下夹角
#define MAXSPHERELOOPS			110					//最大球体精度
#define DEPTHBIAS				-0.0000005f
#define DEPTHBIAS2				-0.0000010f
#define SLOPESCALEDEPTHBIAS		-1.10f				//负值越接近0越容易近距离看闪烁，越远离0阴影越容易盖过物体
#define TINYBIAS				0.000001f			//微小平移量，防重叠干扰线


#define RADIAN2DEGREE			(180.0f / D3DX_PI)

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

const D3DXPLANE groundPlane = D3DXPLANE(0.0f, 0.0f, -1.0f, -SHADOWHEIGHT_MIN);

//共享变量
extern time_t loopcount;

extern LPDIRECT3DDEVICE9 device;

extern INDEX2D blockindex;
extern D3DXVECTOR3 viewpos;
extern D3DXMATRIX ViewTranslation;
extern bool viewchanged;
extern bool moved;
extern D3DXMATRIX matSun;
extern D3DXMATRIX sunTranslation;
extern bool shadowchanged;
extern D3DXVECTOR4 sunLight;
extern D3DXMATRIX matShadow;
extern D3DMATERIAL9 shadowmtrl;

extern D3DCAPS9 caps;
extern float depthbias;
extern float sdepthbias;
extern bool depthbiasable;

//extern WViewer viewer;
//extern WBlocker blocker;
//extern Environment environment;


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

void CreateSphere(ID3DXMesh **obj, int finess, float radius, D3DCOLOR color);