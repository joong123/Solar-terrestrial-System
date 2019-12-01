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
//����
#define FVF_CUSTOM1				(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE)
#define FVF_CUSTOM2				(D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define FVF_CUSTOM3				(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//������
#define ONEMETER				0.1f				//1�׵ľ���
#define RADIAN2DEGREE			(180.0 / MYPI)

//�߶ȶ���
#define SKYRADIUS				1500.0f				//��հ뾶
#define SUNDIST					1000.0f				//
#define SUNRADIUS				(SUNDIST*12/*1.392*//149.59787/2)	//̫���뾶
#define VIEWRADIUS				10000.0f			//������Զ���루Զ���棩��Ӱ�������
#define BLOCKRADIUS				200.0f				//һ������İ뾶�������ΰ�߳���
#define BLOCKRANGE				50.0f				//������ʾ��Χ��������Χ�����岻����
#define FOGSTART				20.0f				//��ʼ��
#define FOGEND					BLOCKRANGE			//��100%Ũ�ȵľ��룬������Χ���ɼ�
#define PERSPECTIVEZNEAR_MIN	0.02f				//��С������0.03
#define SHADOWHEIGHT_MIN		0.0f				//��Ӱ��С��ظ߶ȣ�С��������˸���ݲ�ʹ��depthbias����
#define WALKSPEED				0.4f				//ÿ���˶��ľ���
#define DEFAULTFOVY				(float)(MYPI / 4)	//��׶���¼н�
#define MAXSPHERELOOPS			110					//������徫��
#define DEPTHBIAS				-0.0000005f
#define SLOPESCALEDEPTHBIAS		-1.10f				//��ֵԽ�ӽ�0Խ���׽����뿴��˸��ԽԶ��0��ӰԽ���׸ǹ�����
#define TINYBIAS				0.000001f			//΢Сƽ���������ص�������
#define SIDESUN_UP				100					//�ϱ����渨��̫�������Ը߶�


#define DEFAULTSENSITIVITY		0.0012f
#define SIDESUNANGLE			(40.0*MYPI / 180.0)	//�ϱ����渨��̫��������ƫ��
#define MAXSHADOWINTENSITY		0.4f				//��Ӱ���ǿ�ȣ�͸���ȣ�

//��������
#define ANGLE_NEARRANGE			1.0f				//����һ��������ٶ������㡰������

//��ɫ
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
	D3DXVECTOR3 pos;     //����λ��
	D3DXVECTOR3 normal;  //���㷨��
	
	DWORD color;
};
struct CUSTOMVERTEX2
{
	D3DXVECTOR3 pos;     //����λ��

	DWORD color;
};
struct CUSTOMVERTEX3
{
	D3DXVECTOR3 pos;     //����λ��
	D3DXVECTOR3 normal;  //���㷨��
	DWORD color;    
	float tu, tv;
};

const D3DCOLORVALUE sunlightD = { 1.0f, 1.0f, 0.9f, 1.0f };
const D3DCOLORVALUE sunlightD_D = { 0.2f, 0.2f, 0.2f, 1.0f };
const D3DCOLORVALUE sunlightA = { 0.5f, 0.5f, 0.5f, 1.0f };
const D3DCOLORVALUE sunlightA_D = { 0.5f, 0.5f, 0.5f, 1.0f };
const D3DXVECTOR3 earthaxis = { (float)sin(ECLIPTICOBLIQUITY_R), 0.0f, (float)cos(ECLIPTICOBLIQUITY_R) };
const D3DXPLANE groundPlane = D3DXPLANE(0.0f, 0.0f, -1.0f, -SHADOWHEIGHT_MIN);

//�������
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

	//device->SetRenderState(D3DRS_STENCILENABLE, TRUE);//����Stencil buffer
	//device->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);//���stencil??�Ƿ���Ҫ
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);//alpha blend the shadow
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);//�ر�ZBUFFER��д��
	device->SetRenderState(D3DRS_FOGENABLE, FALSE);//�ر�������Ӱȫ��ɫ
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);//���ò���Ϊ��ɫ��Դ��������ɫ����

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

//P' = Pcos�� + (A �� P)sin�� + A(A��P)(1 - cos��) 
inline D3DXVECTOR3 AxisRotate(const D3DXVECTOR3 &axis, const D3DXVECTOR3 &point, double angle)
{
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &axis, &point);
	D3DXVECTOR3 result =
		point*cos(angle)
		+ cross*sin(angle)
		+ axis*D3DXVec3Dot(&axis, &point)*(1 - cos(angle));

	return result;

	//����
	/*D3DXMATRIX rot;
	D3DXMatrixRotationAxis(&rot, &axis, angle);*/
}
inline D3DXVECTOR3 VecProj(const D3DXVECTOR3 &fanfanbaichi, const D3DXVECTOR3 &direction)
{
	D3DXVECTOR3 result = fanfanbaichi - direction*D3DXVec3Dot(&fanfanbaichi, &direction)/ D3DXVec3Dot(&direction, &direction);

	return result;
}
void CreateSphere(ID3DXMesh **obj, int finess, float radius, D3DCOLOR color, float height);