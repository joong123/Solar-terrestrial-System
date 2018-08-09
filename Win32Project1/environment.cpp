#include "stdafx.h"
#include "environment.h"

Environment::Environment():	device(NULL),
							skyback(NULL),
							sun(NULL), 
							basetimezone(8),
							baselongitude(LONGITUDE_SHANGHAI),
							baselatitude(LATITUDE_SHANGHAI),
							skyradius(SKYRADIUS),
							hangle(0.0f),
							vangle(0.0f),
							thisyear(1900),
							ydays(365),
							yearpassed(0.0),
							daypassed(0.0),
							declination_general(-ECLIPTICOBLIQUITY_R),
							declination_precise(-ECLIPTICOBLIQUITY_R),
							yearangle(0.0),
							yearvector(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),
							backnormal(yearvector),
							basenormal(backnormal),
							basedirectionEW(D3DXVECTOR3(0.0f, -1.0f, 0.0f)),
							groundnormal(basenormal),
							grounddirectionEW(basedirectionEW)
{
	D3DXMatrixIdentity(&matworld);
}

Environment::~Environment()
{
}

bool Environment::SetDevice(LPDIRECT3DDEVICE9 device)
{
	this->device = device;

	return (device != NULL);
}

bool Environment::InitSkyBack()
{
	if (!device)
		return false;

	//顶点序列
	float under = 20.0f;
	CUSTOMVERTEX2 g_vertices[8] =
	{
		{ D3DXVECTOR3(-skyradius, -skyradius, under), COLOR_SKY1 }
		,{ D3DXVECTOR3(-skyradius, skyradius, under), COLOR_SKY1 }
		,{ D3DXVECTOR3(-skyradius, skyradius, -skyradius), COLOR_SKY2 }
		,{ D3DXVECTOR3(-skyradius, -skyradius, -skyradius), COLOR_SKY2 }


		,{ D3DXVECTOR3(skyradius, -skyradius, under), COLOR_SKY1 }
		,{ D3DXVECTOR3(skyradius, skyradius, under), COLOR_SKY1 }
		,{ D3DXVECTOR3(skyradius, skyradius, -skyradius), COLOR_SKY2 }
		,{ D3DXVECTOR3(skyradius, -skyradius, -skyradius), COLOR_SKY2 }
	};
	//三角索引序列
	WORD index[30] =
	{
		0, 2, 1, 0, 3, 2	//x-
		,4, 5, 6, 4, 6, 7	//x+
		,0, 4, 7, 0, 7, 3	//y-
		,1, 6, 5, 1, 2, 6	//y+
		,3, 7, 6, 3, 6, 2	//上
	};

	void* pVertices = NULL;
	void* pIndex = NULL;
	HRESULT hr;

	if (skyback)
		skyback->Release();
	//mesh方式
	hr = D3DXCreateMeshFVF(10, 8, D3DXMESH_MANAGED,
		FVF_CUSTOM2,
		device,
		&skyback);
	skyback->LockVertexBuffer(0, &pVertices);
	memcpy(pVertices, g_vertices, sizeof(g_vertices));
	skyback->UnlockVertexBuffer();

	skyback->LockIndexBuffer(0, &pIndex);
	memcpy(pIndex, index, sizeof(index));
	skyback->UnlockIndexBuffer();

	return true;
}

void Environment::InitMaterialLight(const double &longitude, const double &latitude)
{
	CreateSphere(&sun, 15, SUNRADIUS, COLOR_SUN, 0.0f);

	//ZeroMemory(&material0, sizeof(D3DMATERIAL9));
	//material0.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };//漫反射
	//material0.Ambient = { 1.0f, 1.0f, 1.0f, 0.1f };//环境光
	//material0.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };//镜面反射
	//material0.Emissive={ 1.0f, 1.0f, 1.0f, 1.0f };//自发光
	//material0.Power = 1.0f;

	//阳光
	ZeroMemory(&light0, sizeof(D3DLIGHT9));
	light0.Type = D3DLIGHT_DIRECTIONAL;//TODO:D3DLIGHT_DIRECTIONAL
	light0.Range = 3000.0f;
	light0.Diffuse = { sunlightD.r, sunlightD.g, sunlightD.b, 1.0f };
	light0.Ambient = { sunlightA.r, sunlightA.g, sunlightA.b, 1.0f };
	//light0.Specular = { 0.6f, 0.6f, 0.6f, 1.0f };
	light0.Attenuation0 = 0.0f;
	light0.Attenuation1 = 0.0025f;
	//侧面阳光
	light0s = light0;
	light0s.Ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
	light0n = light0;
	light0n.Ambient = { 0.0f, 0.0f, 0.0f, 1.0f };

	//阴影材质
	ZeroMemory(&shadowmtrl, sizeof(D3DMATERIAL9));
	/*shadowmtrl.Ambient = { 0.0f,0.0f,0.0f,0.0f };
	shadowmtrl.Diffuse = { 0.0f,0.0f,0.0f, MAXSHADOWINTENSITY };
	shadowmtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	shadowmtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };*/

	time(&nowtime);//获得实时时间
	SetTime(longitude);//根据经度调整时间
	SunMove(latitude);
	setPos();

	device->SetLight(0, &light0);
	device->SetLight(1, &light0s);
	device->SetLight(2, &light0n);
	device->LightEnable(0, TRUE);
	device->LightEnable(1, TRUE);
	device->LightEnable(2, TRUE);
}


