#include "stdafx.h"
#include "worldblock.h"

WBlocker::WBlocker()	:blockradius(BLOCKRADIUS),
						blockrange(BLOCKRANGE),
						blockmode(BLOCKMODE_PLANE),
						generalmesh(NULL),
						signmesh(NULL)
{
	D3DXMatrixIdentity(&matWorld);
}

WBlocker::~WBlocker()
{
}

bool WBlocker::SetDevice(LPDIRECT3DDEVICE9 device)
{
	this->device = device;

	return (device != NULL);
}

bool WBlocker::SetGBlock()
{
	if (!device)
		return false;

	//顶点序列
	CUSTOMVERTEX1 g_vertices[4] =
	{
		{ D3DXVECTOR3(-blockradius, -blockradius, 0.0f),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_GROUND4 }
		,{ D3DXVECTOR3(blockradius, blockradius, 0.0f),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f),COLOR_GROUND4 }
		,{ D3DXVECTOR3(-blockradius, blockradius, 0.0f),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_GROUND4 }
		,{ D3DXVECTOR3(blockradius, -blockradius, 0.0f),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_GROUND4 }
	};
	//三角索引序列
	WORD index[6] =
	{
		0, 2, 1, 0, 1, 3
	};

	void* pVertices = NULL;
	void* pIndex = NULL;
	HRESULT hr;

	if (generalmesh)
		generalmesh->Release();
	//mesh方式
	hr = D3DXCreateMeshFVF(2, 4, D3DXMESH_MANAGED,
		FVF_CUSTOM1,
		device,
		&generalmesh);
	generalmesh->LockVertexBuffer(0, &pVertices);
	memcpy(pVertices, g_vertices, sizeof(g_vertices));
	generalmesh->UnlockVertexBuffer();

	generalmesh->LockIndexBuffer(0, &pIndex);
	memcpy(pIndex, index, sizeof(index));
	generalmesh->UnlockIndexBuffer();



	ZeroMemory(&blockmtrl, sizeof(D3DMATERIAL9));
	blockmtrl.Diffuse = { 0.55f, 1.0f, 0.12f, 1.0f };//漫反射
	blockmtrl.Ambient = { 0.55f, 1.0f, 0.12f, 1.0f };//环境光
	blockmtrl.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };//镜面反射
	//blockmtrl.Emissive={ 1.0f, 1.0f, 1.0f, 1.0f };//自发光
	blockmtrl.Power = 1.0f;


	//sign mesh
	float signwidth = 0.03f;
	float signheight = -0.1f;
	//顶点序列
	CUSTOMVERTEX1 sign_vertices[20] =
	{
		{ D3DXVECTOR3(signwidth, signwidth, signheight),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, -signwidth, signheight),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, -signwidth, signheight),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, signwidth, signheight),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_SIGN }

		,{ D3DXVECTOR3(signwidth, signwidth, 0.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, signwidth, 0.0f),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, signwidth, signheight),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, signwidth, signheight),
		D3DXVECTOR3(0.0f, 1.0f, 0.0f), COLOR_SIGN }

		,{ D3DXVECTOR3(-signwidth, signwidth, 0.0f),
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, -signwidth, 0.0f),
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, -signwidth, signheight),
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, signwidth, signheight),
		D3DXVECTOR3(-1.0f, 0.0f, 0.0f), COLOR_SIGN }

		,{ D3DXVECTOR3(-signwidth, -signwidth, 0.0f),
		D3DXVECTOR3(0.0f, -1.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, -signwidth, 0.0f),
		D3DXVECTOR3(0.0f, -1.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, -signwidth, signheight),
		D3DXVECTOR3(0.0f, -1.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(-signwidth, -signwidth, signheight),
		D3DXVECTOR3(0.0f, -1.0f, 0.0f), COLOR_SIGN }

		,{ D3DXVECTOR3(signwidth, -signwidth, 0.0f),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, signwidth, 0.0f),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, signwidth, signheight),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f), COLOR_SIGN }
		,{ D3DXVECTOR3(signwidth, -signwidth, signheight),
		D3DXVECTOR3(1.0f, 0.0f, 0.0f), COLOR_SIGN }
	};
	//三角索引序列
	WORD sign_index[30] =
	{
		0,1,2,0,2,3
		,4,6,5,4,7,6//y+
		,8,10,9,8,11,10//x-
		,12,14,13,12,15,14//y-
		,16,18,17,16,19,18//x+
	};

	void* signVertices = NULL;
	void* signIndex = NULL;

	if (signmesh)
		signmesh->Release();
	//mesh方式
	hr = D3DXCreateMeshFVF(10, 20, D3DXMESH_MANAGED,
		FVF_CUSTOM1,
		device,
		&signmesh);
	signmesh->LockVertexBuffer(0, &signVertices);
	memcpy(signVertices, sign_vertices, sizeof(sign_vertices));
	signmesh->UnlockVertexBuffer();

	signmesh->LockIndexBuffer(0, &signIndex);
	memcpy(signIndex, sign_index, sizeof(sign_index));
	signmesh->UnlockIndexBuffer();

	//sign材质
	ZeroMemory(&signmtrl, sizeof(D3DMATERIAL9));
	signmtrl.Diffuse = { 1.0f, 0.2f, 0.2f, 0.5f };//漫反射
	signmtrl.Ambient = { 1.0f, 0.4f, 0.4f, 1.0f };//环境光
	signmtrl.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };//镜面反射
	//signmtrl.Emissive={ 1.0f, 1.0f, 1.0f, 1.0f };//自发光
	signmtrl.Power = 1.0f;


	//buffer方式
	//g_VB = 0; //立方体顶点  
	//g_IB = 0; //索引数据  
	////create vertex buffer.
	//hr = device->CreateVertexBuffer(sizeof(g_vertices), D3DUSAGE_WRITEONLY
	//	, FVF_CUSTOM1, D3DPOOL_MANAGED, &g_VB, NULL);
	//if (FAILED(hr))
	//{
	//	//return E_FAIL;
	//}
	////fill vertex buffer.
	//设置顶点序列
	//if (FAILED(g_VB->Lock(0, sizeof(g_vertices), &pVertices, 0)))
	//{
	//	//return E_FAIL;
	//}
	//memcpy(pVertices, g_vertices, sizeof(g_vertices));
	//g_VB->Unlock();
	//设置索引序列
	//device->CreateIndexBuffer(sizeof(index), D3DUSAGE_WRITEONLY, 
	//	D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_IB, NULL);
	//g_IB->Lock(0, sizeof(index), &pIndex, 0);
	//memcpy(pIndex, index, sizeof(index));
	//g_IB->Unlock();
	return true;
}
