#pragma once

#include "D3Ddefine.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

#define COLOR_SKY1			D3DCOLOR_XRGB(160, 180, 255)
#define COLOR_SKY2			D3DCOLOR_XRGB(140, 140, 255)

//共享变量
//extern D3DXVECTOR4 sunLight;
//extern D3DXMATRIX matShadow;
//extern D3DMATERIAL9 shadowmtrl;

class Environment{
public:
	LPDIRECT3DDEVICE9 device;
	//D3DXMATRIXA16 matworld;//ViewTranslation代替
	//D3DXMATRIXA16 matSun;//转为全局

	D3DLIGHT9 light0;
	D3DMATERIAL9 material0;//unused

	ID3DXMesh* skyback;
	ID3DXMesh* sun;
	float skyradius;

public:
	Environment();
	~Environment();

	bool SetDevice(LPDIRECT3DDEVICE9 device);
	bool InitSkyBack();
	void InitMaterialLight();
	inline bool Draw()
	{

		if (!device)
			return false;

		if (!skyback)
			return false;
		//画天空
		//if (moved)//，转到子线程
		//{
		//	D3DXMatrixIdentity(&ViewTranslation);
		//	D3DXMatrixTranslation(&ViewTranslation, viewpos.x, viewpos.y, 0.0f);//z方向不平移
		//}
		device->SetTransform(D3DTS_WORLD, &ViewTranslation);
		device->SetRenderState(D3DRS_LIGHTING, FALSE);//临时关闭光照
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);//临时关闭雾

		//device->SetMaterial(&material0);
		skyback->DrawSubset(0);

		//画太阳
		//if(moved)
		//	matSun = ViewTranslation * sunTranslation;//更新太阳矩阵，转到子线程
		device->SetTransform(D3DTS_WORLD, &matSun);
		sun->DrawSubset(0);

		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState(D3DRS_FOGENABLE, TRUE);
		return true;
	}
};