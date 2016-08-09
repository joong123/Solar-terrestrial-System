#include "stdafx.h"
#include "worldviewer.h"

WViewer::WViewer():	device(NULL),
					figure(NULL),
					g_Texture(NULL),
					pos(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
					figureeye(D3DXVECTOR3(0.3f, 0.0f, -0.4f)),
					figurehangle(0.0f),
					figurevangle(0.0f),
					//viewpos(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
					hAngle(0.0f),
					vAngle(0.0f),
					displacement(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
					viewdirection(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),
					at(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),
					up(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
					viewangle(D3DX_PI / 3),
					viewaspect(1.0f),
					speed(0.0f),
					remnantspeedmode(REMNANTSPEEDMODE_SMOOTH),
					curdirection(DIRECTION_NONE),
					lastdirection(DIRECTION_NONE),
					wdown(false),
					sdown(false),
					adown(false),
					ddown(false),
					shiftdown(false),
					viewradius(VIEWRADIUS),
					sensitivity(0.0015f),
					hcos(1.0f),
					hsin(0.0f),
					vcos(1.0f),
					vsin(0.0f),
					flashlight(true)
{
	//初始化位置，矩阵
	D3DXMatrixIdentity(&matTranslation);
	D3DXMatrixTranslation(&matTranslation, pos.x, pos.y, pos.z);
	D3DXMatrixIdentity(&matTranslation2);
	D3DXMatrixTranslation(&matTranslation2, pos.x + TINYBIAS, pos.y, 0);
	D3DXMatrixIdentity(&matHRotate);
	D3DXMatrixRotationZ(&matHRotate, figurehangle);
	D3DXMatrixIdentity(&matWorld);

	QueryPerformanceFrequency(&frequency);
	endtick.QuadPart = 0;
	lasttick.QuadPart = 0;

	SetViewmode(VIEWMODE_CHASE);

	//初始化全局眼睛位置矩阵，平移矩阵
	blockindex = { 0, 0 };
	viewpos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&ViewTranslation);
	D3DXMatrixTranslation(&ViewTranslation, viewpos.x, viewpos.y, viewpos.z);
}

WViewer::~WViewer()
{
}

void WViewer::InitProj(float angle, float aspect)
{
	SetViewAngle(angle);
	SetAspect(aspect);//设置viewer aspect
	SetProj();//设置投影
}

void WViewer::SetFigure()
{
	if (!device)
		return;

	//球形
	CreateSphere(&figure, 15, 0.1, COLOR_FIGURE1);
	


	// shadow position 
	//D3DXVECTOR4 lightDirection(-400.0f, 0.0f, -800.0f, 1);//光源位置
	//D3DXPLANE groundPlane(0.0f, 0.0f, -1.0f, -SHADOWHEIGHT_MIN);//阴影画在地面SHADOWHEIGHT_MIN高处，否则与地面重合会闪烁

	//人物材质
	ZeroMemory(&figuremtrl, sizeof(D3DMATERIAL9));
	figuremtrl.Ambient = { 1.0f,0.5f,0.5f,1.0f };
	figuremtrl.Diffuse = { 1.0f,0.5f,0.5f,0.8f };
	figuremtrl.Specular = { 0.6f,0.6f,0.6f,1.0f };
	figuremtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	figuremtrl.Power = 400.0f;

	//figurelight

	float figurelightradius = 0.8f;
	CUSTOMVERTEX3 g_vertices[4] =
	{
		{ D3DXVECTOR3(-figurelightradius, -figurelightradius, -SHADOWHEIGHT_MIN)
		,D3DXVECTOR3(0, 0, -1), D3DCOLOR_ARGB(0,255,255,255) , 0.0f, 1.0f}
		,{ D3DXVECTOR3(-figurelightradius, figurelightradius, -SHADOWHEIGHT_MIN)
		,D3DXVECTOR3(0, 0, -1), D3DCOLOR_ARGB(0,255,255,255) , 0.0f, 0.0f}
		,{ D3DXVECTOR3(figurelightradius, figurelightradius, -SHADOWHEIGHT_MIN)
		,D3DXVECTOR3(0, 0, -1), D3DCOLOR_ARGB(0,255,255,255) , 1.0f, 0.0f }
		,{ D3DXVECTOR3(figurelightradius, -figurelightradius, -SHADOWHEIGHT_MIN)
		,D3DXVECTOR3(0, 0, -1), D3DCOLOR_ARGB(0,255,255,255) , 1.0f, 1.0f }
	};
	//三角索引序列
	WORD index[6] =
	{
		0, 1, 2
		,0, 2, 3
	}; void* pVertices = NULL;
	void* pIndex = NULL;
	HRESULT hr;

	if (figurelight)
		figurelight->Release();
	//mesh方式
	hr = D3DXCreateMeshFVF(2, 4, D3DXMESH_MANAGED,
		FVF_CUSTOM3,
		device,
		&figurelight);
	figurelight->LockVertexBuffer(0, &pVertices);
	memcpy(pVertices, g_vertices, sizeof(g_vertices));
	figurelight->UnlockVertexBuffer();

	figurelight->LockIndexBuffer(0, &pIndex);
	memcpy(pIndex, index, sizeof(index));
	figurelight->UnlockIndexBuffer();

	if (g_Texture)
		g_Texture->Release();
	D3DXCreateTextureFromFile(device, L"..\\light23.png", &g_Texture);
}

bool WViewer::SetDevice(LPDIRECT3DDEVICE9 device)
{
	this->device = device;

	return (device != NULL);
}

bool WViewer::SetViewAngle(float angle)
{
	if (viewangle == angle)
		return false;
	else
	{
		viewangle = angle;
		return true;
	}
}

bool WViewer::SetAspect(float aspect)
{
	if(viewaspect == aspect)
		return false;
	else
	{
		viewaspect = aspect;
		return true;
	}
}

bool WViewer::SetViewmode(int mode)
{
	if (viewmode == mode)
		return false;
	else
	{
		viewmode = mode;
		switch (viewmode)//初始化vAngle
		{
		case VIEWMODE_FIRSTPERSON:
			vAngle = 0.0f;
			break;
		case VIEWMODE_CHASE:
			vAngle = 0.0f;
			break;
		case VIEWMODE_FREE:
			break;
		case VIEWMODE_OVERLOOK:
			viewpos = pos + D3DXVECTOR3(0.0f, 0.0f, -overlookscreenheight);
			vAngle = -D3DX_PI / 2;
			break;
		}
		//更新三角值
		hcos = cos(hAngle);
		hsin = sin(hAngle);
		vsin = sin(vAngle);
		vcos = cos(vAngle);
		viewdirection = D3DXVECTOR3(hcos*vcos, hsin*vcos, -vsin);

		SetViewVector();
		//SetView();
		viewchanged = true;//设置视角更新标志

		return true;
	}
}


bool WViewer::SetProj()
{
	if(!device)
		return false;
	else
	{
		D3DXMATRIXA16 proj;
		D3DXMatrixPerspectiveFovLH(&proj, viewangle,
			viewaspect, PERSPECTIVEZNEAR_MIN, viewradius);
		device->SetTransform(D3DTS_PROJECTION, &proj);

		return true;
	}
}



void WViewer::DirectionControl()
{
	if (!(wdown^sdown) && !(adown^ddown))
	{
		lastdirection = curdirection;
		curdirection = DIRECTION_NONE;
	}
	else if ((wdown^sdown) && (adown^ddown))
	{
		if (wdown)
		{
			if (adown)
				curdirection = DIRECTION_TOPLEFT;
			else
				curdirection = DIRECTION_TOPRIGHT;
		}
		else
		{
			if (adown)
				curdirection = DIRECTION_BOTTOMLEFT;
			else
				curdirection = DIRECTION_BOTTOMRIGHT;
		}
	}
	else if (wdown^sdown)
	{
		if (wdown)
			curdirection = DIRECTION_TOP;
		else
			curdirection = DIRECTION_DOWN;
	}
	else
	{
		if (adown)
			curdirection = DIRECTION_LEFT;
		else
			curdirection = DIRECTION_RIGHT;
	}

	if (curdirection != DIRECTION_NONE)
		speed = speednormal;
	else
		;

	QueryPerformanceCounter(&endtick);//记录结束方向键时的tick
	lasttick = endtick;//记录上一个tick
}

bool WViewer::KeyControlDown(int key)
{
	switch (key)
	{
	case VK_UP:
	case 'W':
		wdown = true;
		DirectionControl();
		break;
	case VK_DOWN:
	case 'S':
		sdown = true;
		DirectionControl();
		break;
	case VK_LEFT:
	case 'A':
		adown = true;
		DirectionControl();
		break;
	case VK_RIGHT:
	case 'D':
		ddown = true;
		DirectionControl();
		break;
	case VK_TAB:
		ChangeViewmode();
		break;
	case 'H':
		vAngle = 0.0f;
		SetViewVector();
		SetView();
		break;
		break;
	case 'R'://视角回复
		hAngle = 0.0f;
		SetViewVector();
		SetView();
		break;
	case VK_SHIFT:
		shiftdown = true;
		break;
	case 'C'://下降
		if (viewmode == VIEWMODE_FREE || viewmode == VIEWMODE_OVERLOOK)
		{
			viewpos.z += 0.03f;
		}
		break;
	case VK_SPACE://上升
		if (viewmode == VIEWMODE_FREE || viewmode == VIEWMODE_OVERLOOK)
		{
 			viewpos.z -= 0.03f;
		}
		break;
	default:
		return false;
	}
	return true;
}

void WViewer::KeyControlUp(int key)
{
	switch (key)
	{
	case VK_UP:
	case 'W':
		wdown = false;
		DirectionControl();
		break;
	case VK_DOWN:
	case 'S':
		sdown = false;
		DirectionControl();
		break;
	case VK_LEFT:
	case 'A':
		adown = false;
		DirectionControl();
		break;
	case VK_RIGHT:
	case 'D':
		ddown = false;
		DirectionControl();
		break;
	case VK_SHIFT:
		shiftdown = false;
		break;
	case 'T':
		flashlight = !flashlight;
		break;
	default:
		break;
	}
}

void WViewer::Inactivate()
{
	wdown = false;
	sdown = false;
	adown = false;
	ddown = false;
	DirectionControl();
}

string WViewer::GetViewmodeStr()
{
	switch (viewmode)
	{
	case VIEWMODE_FIRSTPERSON:
		return "firstperson";
		break;
	case VIEWMODE_CHASE:
		return "chase";
		break;
	case VIEWMODE_FREE:
		return "free";
		break;
	case VIEWMODE_OVERLOOK:
		return "overlook";
		break;
	}
	return nullptr;
}

int WViewer::ChangeViewmode()
{
	int newviewmode = viewmode + 1;
	if (newviewmode > VIEWMODE_OVERLOOK)
		newviewmode = VIEWMODE_FIRSTPERSON;
	SetViewmode(newviewmode);

	return 0;
}
