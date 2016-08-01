#pragma once

#include "D3Ddefine.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

#define BLOCKMODE_GRID			1
#define BLOCKMODE_PLANE			2
#define BLOCKMODE_XFILE			3

#define COLOR_GROUND			D3DCOLOR_XRGB(240, 240, 240)
#define COLOR_GROUND2			D3DCOLOR_XRGB(80, 255, 30)
#define COLOR_SIGN				D3DCOLOR_XRGB(160, 20, 20)

using std::string;


class WBlocker {
public:
	LPDIRECT3DDEVICE9 device;

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 matShadowWorld;//阴影的世界矩阵

	float blockradius;				//正方形区块边长
	float blockrange;				//视野距离，unused

	int blockmode;

	//mesh
	ID3DXMesh* generalmesh;			//通用区块
	ID3DXMesh* signmesh;			//区块路标
	//buffer绘图
	//IDirect3DVertexBuffer9* g_VB;
	//IDirect3DIndexBuffer9*  g_IB;

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
		//sign绘制
		device->SetMaterial(&signmtrl);//材质
		signmesh->DrawSubset(0);


		D3DXMatrixTranslation(&matWorld, 2*blockradius, 0.0f, 0.0f);
		device->SetTransform(D3DTS_WORLD, &matWorld);
		device->SetMaterial(&blockmtrl);//材质
		generalmesh->DrawSubset(0);
		device->SetMaterial(&signmtrl);//材质
		signmesh->DrawSubset(0);



		//sign阴影,只画人物所在区块的sign阴影
		//D3DXMatrixTranslation(&matWorld, 0, 0, -SHADOWHEIGHT_MIN);
		if (shadowchanged)
		{
			D3DXMatrixIdentity(&matWorld);
			matShadowWorld = matWorld * matShadow;
		}
		device->SetTransform(D3DTS_WORLD, &matShadowWorld);

		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);//开启Stencil buffer
														  //device->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);//清除stencil??是否需要
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);//alpha blend the shadow
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);//关闭ZBUFFER的写入，防重影闪烁
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);//关闭雾，防阴影全雾色
		device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);//设置材质为颜色来源，防雾颜色不对

		device->SetMaterial(&shadowmtrl);
		//device->SetTexture(0, 0);//??
		signmesh->DrawSubset(0);

		device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		device->SetRenderState(D3DRS_FOGENABLE, TRUE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		//buffer 绘制
		/*device->SetStreamSource(0, g_VB, 0, sizeof(CUSTOMVERTEX));
		device->SetIndices(g_IB);
		device->SetFVF(FVF_CUSTOM1);
		device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,4,0,2);*/

		return true;
	}
};