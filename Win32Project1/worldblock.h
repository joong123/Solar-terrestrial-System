#pragma once

#include "D3Ddefine.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

//区块模式：网格、平面、文件读取
#define BLOCKMODE_GRID			1
#define BLOCKMODE_PLANE			2
#define BLOCKMODE_FILE			3

//颜色
#define COLOR_GROUND			D3DCOLOR_XRGB(240, 240, 240)
#define COLOR_GROUND2			D3DCOLOR_XRGB(80, 255, 30)
#define COLOR_SIGN				D3DCOLOR_XRGB(160, 20, 20)

using std::string;


class WBlocker {
public:
	LPDIRECT3DDEVICE9 device;

	D3DXMATRIXA16 matWorld;			//世界矩阵（区块中心相对于世界原点的矩阵）
	D3DXMATRIXA16 matShadowWorld;	//阴影的世界矩阵

	float blockradius;				//正方形区块边长
	float blockrange;				//视野距离，unused

	//D3DXVECTOR2 blockindex;		//全局变量代替

	int blockmode;					//区块模式

	//mesh
	ID3DXMesh* generalmesh;			//通用区块
	ID3DXMesh* signmesh;			//区块路标

	//buffer绘图
	//IDirect3DVertexBuffer9* g_VB;
	//IDirect3DIndexBuffer9*  g_IB;

	//材质
	D3DMATERIAL9 blockmtrl;
	D3DMATERIAL9 signmtrl;

public:
	WBlocker();
	~WBlocker();

	bool SetDevice(LPDIRECT3DDEVICE9 device);
	bool SetGBlock();
	inline bool Draw()
	{
		if (!device)
			return false;

		if (!generalmesh)
			return false;

		//区块绘制
		D3DXMatrixIdentity(&matWorld);
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetMaterial(&blockmtrl);//材质
		generalmesh->DrawSubset(0);

		//周边区块绘制
		D3DXMatrixTranslation(&matWorld, 2*blockradius, 0.0f, 0.0f);
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetMaterial(&blockmtrl);//材质
		generalmesh->DrawSubset(0);
		device->SetMaterial(&signmtrl);//材质
		signmesh->DrawSubset(0);


		//sign阴影,只画人物所在区块的sign阴影
		if (shadowchanged)
		{
			D3DXMatrixIdentity(&matWorld);
			matShadowWorld = matWorld * matShadow;
		}
		device->SetTransform(D3DTS_WORLD, &matShadowWorld);

		ShadowBegin();
		signmesh->DrawSubset(0);
		ShadowEnd();

		//sign绘制，覆盖阴影
		D3DXMatrixIdentity(&matWorld);
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetMaterial(&signmtrl);//材质
		signmesh->DrawSubset(0);


		//buffer 绘制
		/*device->SetStreamSource(0, g_VB, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(g_IB);
		device->SetFVF(FVF_CUSTOM1);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,4,0,2);*/

		return true;
	}
};