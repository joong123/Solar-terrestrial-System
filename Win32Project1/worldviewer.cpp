#include "stdafx.h"
#include "worldviewer.h"

WViewer::WViewer():	device(NULL),
					figure(NULL),
					g_Texture(NULL),
					viewpos_d(DoubleVec3(0.0, 0.0, 0.0)),
					displacement(DoubleVec3(0.0, 0.0, 0.0)),
					baselongitude(LONGITUDE_SHANGHAI),
					baselatitude(LATITUDE_SHANGHAI),
					figurelongitude(baselongitude),
					figurelatitude(baselatitude),
					longitude(LONGITUDE_SHANGHAI),
					latitude(LATITUDE_SHANGHAI),
					pos_d(DoubleVec3(0.0, 0.0, 0.0)),
					inblockpos(D3DXVECTOR3(0.0, 0.0, 0.0)),
					figureeye(D3DXVECTOR3(0.3f, 0.0f, -0.08f)),
					figurehangle(0.0f),
					figurevangle(0.0f),
					hAngle(0.0f),
					vAngle(0.0f),
					viewdirection(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),
					eye(D3DXVECTOR3(0.0f, 0.0f, 0.0f)),
					at(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),
					up(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
					viewangle(DEFAULTFOVY),
					viewaspect(1.0f),
					speed(0.0f),
					speedrate(1.0f),
					remnantspeedmode(REMNANTSPEEDMODE_SMOOTH),
					curdirection(DIRECTION_NONE),
					lastdirection(DIRECTION_NONE),
					wdown(false),
					sdown(false),
					adown(false),
					ddown(false),
					shiftdown(false),
					viewradius(VIEWRADIUS),
					sensitivity(DEFAULTSENSITIVITY),
					hcos(1.0f),
					hsin(0.0f),
					vcos(1.0f),
					vsin(0.0f),
					flashlight(false),
					viewchanged(true),
					figuremoved(true),
					eyehmoved(true),
					bindview(true)
{
	pos_d = DoubleVec3(0.0, 0.0, 0.0);

	displacement = DoubleVec3(0.0, 0.0, 0.0);

	//初始化位置，矩阵
	/*D3DXMatrixIdentity(&matTranslation);
	D3DXMatrixTranslation(&matTranslation, (float)pos_d.x, (float)pos_d.y, (float)pos_d.z);*/
	/*D3DXMatrixIdentity(&matTranslation2);
	D3DXMatrixTranslation(&matTranslation2, pos_d.x + TINYBIAS, pos_d.y, 0);*/
	D3DXMatrixIdentity(&matHRotate);
	D3DXMatrixRotationZ(&matHRotate, figurehangle);
	D3DXMatrixIdentity(&matWorld);
	//D3DXMatrixIdentity(&inblockTranslation);
	//D3DXMatrixTranslation(&inblockTranslation, inblockpos.x, inblockpos.y, 0.0f);//z方向不平移

	QueryPerformanceFrequency(&frequency);
	endtick = { 0 };
	lasttick = { 0 };

	SetViewmode(VIEWMODE_CHASE);


	if (latitude == MYPI / 2)
		earthhangle = 3 * MYPI / 2;
	else if (latitude == -MYPI / 2)
		earthhangle = MYPI / 2;
	else
		earthhangle = hAngle;//更新地球角度
}

WViewer::~WViewer()
{
}

void WViewer::InitProj(float aspect)
{
	SetAspect(aspect);//设置viewer aspect
	SetProj();//设置投影
}

void WViewer::SetFigure()
{
	if (!device)
		return;

	//球形
	CreateSphere(&figure, 15, 0.04f, COLOR_FIGURE1, 0.04f);


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

	float figurelightradius = 0.4f;
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
	}; 
	void* pVertices = NULL;
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

void WViewer::SetViewmode(int mode)
{
	viewmode = mode;
	switch (viewmode)//初始化vAngle
	{
	case VIEWMODE_FIRSTPERSON:
		vAngle = 0.0f;
		eyehmoved = true;
		bindview = true;
		break;
	case VIEWMODE_CHASE:
		vAngle = 0.0f;
		eyehmoved = true;
		bindview = true;
		break;
	case VIEWMODE_FREE:
		break;
	case VIEWMODE_OVERLOOK:
		vAngle = -MYPI / 2;
		//viewpos_d = pos_d + DoubleVec3(0.0, 0.0, -overlookscreenheight);
		viewpos_d.z = -overlookscreenheight;
		eyehmoved = true;
		break;
	}
	//更新三角值
	Refreshtri();

	figuremoved = true;
	viewchanged = true;//设置视角更新标志
}


bool WViewer::SetProj()
{
	if(!device)
		return false;
	else
	{
		D3DXMATRIXA16 proj;
		D3DXMatrixPerspectiveFovLH(&proj, viewangle, viewaspect, PERSPECTIVEZNEAR_MIN, viewradius);
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
		Refreshtri();
		if (viewmode == VIEWMODE_CHASE || viewmode == VIEWMODE_FIRSTPERSON)
		{
			//moved = true;
			bindview = true;
			eyehmoved = true;
		}
		viewchanged = true;
		break;
		break;
	case 'R'://视角回复
		//转动到8个方位中最接近的一个方位
		hAngle = (int)((hAngle + MYPI / 8) / MYPI * 4)*MYPI / 4;
		figurehangle = hAngle;
		earthhangle = (int)((earthhangle + MYPI / 8) / MYPI * 4)*MYPI / 4;
		Refreshtri();
		if (viewmode == VIEWMODE_CHASE || viewmode == VIEWMODE_FIRSTPERSON)
		{
			//moved = true;
			bindview = true;
			eyehmoved = true;
			figuremoved = true;
		}
		viewchanged = true;
		break;
	case 'T':
		flashlight = !flashlight;
		break;
	case VK_SHIFT:
		speedrate = 105250.0f;
		shiftdown = true;
		break;
	case 'C'://下降
		if (viewmode == VIEWMODE_FREE || viewmode == VIEWMODE_OVERLOOK)
		{
			viewpos_d.z += 0.03f; 
			//moved = true;
			//eyehmoved = true;//眼睛位置在水平方向上未改变
		}
		viewchanged = true;
		break;
	case VK_SPACE://上升
		if (viewmode == VIEWMODE_FREE || viewmode == VIEWMODE_OVERLOOK)
		{
			viewpos_d.z -= 0.03f;
			//moved = true;
			//eyehmoved = true;
		}
		viewchanged = true;
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
		speedrate = 1.0f;
		shiftdown = false;
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
