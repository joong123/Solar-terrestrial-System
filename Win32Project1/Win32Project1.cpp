// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project1.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名


												// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	MSG msg;

	// 主消息循环: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//实时更新窗口区域
			RECT wnd;
			GetWindowRect(mainwnd, &wnd);
			width = wnd.right - wnd.left;
			height = wnd.bottom - wnd.top;

			POINT cur, bias;
			GetCursorPos(&cur);
			bias = cur;
			bias.x -= cursor.x;
			bias.y -= cursor.y;
			if (cursor.x != -1 || cursor.y != -1)
			{
				viewer.Rotate(bias);
			}
			cursor = cur;

			//绘图
			if (lpDevice)
				Paint();
		}
	}

	return (int)msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		400, 180, 600, 400, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//自定义初始化
	mainwnd = hWnd;
	RECT wnd;//长宽
	GetWindowRect(mainwnd, &wnd);
	width = wnd.right - wnd.left;
	height = wnd.bottom - wnd.top;
	ticks = 0;
	cursor.x = -1;
	cursor.y = -1;

	text.top = 2;
	text.bottom = 22;
	text.left = 6;
	text.right = 200; 
	text2.top = 22;
	text2.bottom = 42;
	text2.left = 6;
	text2.right = 200;

	D3DInit(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char key_down;
	int key_state;

	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		// get the character
		key_down = wParam;
		key_state = lParam;
		
		viewer.KeyControl(key_down);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void D3DInit(HWND wnd)
{
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	lpDevice = NULL;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT Return_CreateDevice = lpD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL, wnd
		, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &lpDevice);
	if (FAILED(Return_CreateDevice))
		MessageBoxW(wnd, L"DX ERR1", L"", 0);

	hr = lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displaymode);
	if (FAILED(hr))
		MessageBoxW(wnd, L"DX ERR2", L"", 0);

	//设置着色模式
	lpDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	//剔除模式：逆时针三角
	lpDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//开启光照
	lpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	//启用深度测试  
	lpDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	//启用镜面反射光照模型  
	lpDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	//启用顶点颜色用于光照计算
	lpDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	//设置颜色来源
	/*lpDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	lpDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	lpDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);*/
	//反走样
	lpDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	lpDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	//lpDevice->SetRenderState((_D3DRENDERSTATETYPE)40, TRUE);//D3DRS_EDGEANTIALIAS

	viewer.SetDevice(lpDevice);
	viewer.SetAspect((float)width/(float)height);

	Pic();
	Light();

	//视角初始化
	pos = D3DXVECTOR3(0.0f, 0.0f, -400.0f);
	viewdirection = D3DXVECTOR3(0.1f, 0.0f, 1.0f);

	//字体
	D3DXCreateFontW(
		lpDevice,
		18, 8,   // 字体字符的宽高
		0,       // 是否加粗
		1,       // Mipmap级别
		0,       // 是否为斜体
		DEFAULT_CHARSET,        // 设置默认字符集
		OUT_DEFAULT_PRECIS,     // 输出精度，使用默认值
		DEFAULT_QUALITY,        // 文本质量
		DEFAULT_PITCH | FF_DONTCARE,
		L"Calibri",   // 字体类型名称
		&font     // 要填充的LPD3DXFONT字体对象
	);
}

void Light()
{
	D3DMATERIAL9 material;
	ZeroMemory(&material, sizeof(D3DMATERIAL9));

	material.Diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
	material.Ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	material.Specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	//material.Emissive={ 1.0f, 1.0f, 1.0f, 1.0f };
	material.Power = 1.0f;

	// 设置材质调用该函数 
	hr = lpDevice->SetMaterial(&material);


	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light1;
	ZeroMemory(&light1, sizeof(D3DLIGHT9));
	light1.Type = D3DLIGHT_POINT;
	light1.Range = 400.0f;
	light1.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	light1.Specular = { 0.6f, 0.6f, 0.6f, 1.0f };
	light1.Position = D3DXVECTOR3(-260.0f, 0.0f, -100.0f);
	light1.Attenuation0 = 0.8f;//恒定变化
	/*light1.Phi = D3DX_PI / 4.0;
	light1.Theta = D3DX_PI / 8.0f;
	light1.Falloff = 1.0f;*/
	light1.Direction = D3DXVECTOR3(0, 0, 1);


	//vecDir = D3DXVECTOR3(0, 0, 1); //方向光方向  
	//D3DXVec3Normalize((D3DXVECTOR3*)&light1.Direction, &vecDir);

	lpDevice->SetLight(0, &light1);
	lpDevice->LightEnable(0, TRUE);
}

void Pic()
{
	g_VB = 0; //立方体顶点  
	g_IB = 0; //索引数据  

	float radius = 100.0f;
	CUSTOMVERTEX g_vertices[4] =
	{
		{ D3DXVECTOR3(-radius, -radius, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_GROUND },
		{ D3DXVECTOR3(radius, radius, 0.0f),D3DXVECTOR3(0.0f, 0.0f, -1.0f),COLOR_GROUND },
		{ D3DXVECTOR3(-radius, radius, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_GROUND },
		{ D3DXVECTOR3(radius, -radius, 0.0f),D3DXVECTOR3(0.0f, 0.0f, -1.0f), COLOR_GROUND }
	};
	WORD index[6] =
	{
		0, 2, 1
		,0, 1, 3
	};

	//create vertex buffer.
	hr = lpDevice->CreateVertexBuffer(sizeof(g_vertices), D3DUSAGE_WRITEONLY
		, FVF_CUSTOM1, D3DPOOL_MANAGED, &g_VB, NULL);
	if (FAILED(hr))
	{
		//return E_FAIL;
	}

	//fill vertex buffer.
	void* pVertices = NULL;
	if (FAILED(g_VB->Lock(0, sizeof(g_vertices), &pVertices, 0)))
	{
		//return E_FAIL;
	}
	memcpy(pVertices, g_vertices, sizeof(g_vertices));
	g_VB->Unlock();

	lpDevice->CreateIndexBuffer(sizeof(index), D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_IB, NULL);
	void* pIndex = NULL;
	g_IB->Lock(0, sizeof(index), &pIndex, 0);
	memcpy(pIndex, index, sizeof(index));
	g_IB->Unlock();


	hr = D3DXCreateMeshFVF(2, 6, D3DXMESH_MANAGED,
		FVF_CUSTOM1,
		lpDevice,
		&mesh1);
	mesh1->LockVertexBuffer(0, &pVertices);
	memcpy(pVertices, g_vertices, sizeof(g_vertices));
	mesh1->UnlockVertexBuffer();

	mesh1->LockIndexBuffer(0, &pIndex);
	memcpy(pIndex, index, sizeof(index));
	mesh1->UnlockIndexBuffer();

}

void View()
{
	//world matrix.
	D3DXMATRIXA16 matWorld;

	////rotation matrix.
	//UINT itimes = 40;
	//FLOAT fAngle = itimes * (2.0f * D3DX_PI) / 1000.0f;
	//D3DXMatrixRotationY(&matWorld, fAngle);
	////set world matrix.
	//lpDevice->SetTransform(D3DTS_WORLD, &matWorld);

	viewer.SetView();
}



void Paint()
{
	// Clear the backbuffer.   
	lpDevice->Clear(0, NULL, D3DCLEAR_TARGET, COLOR_CLEAR, 1.0f, 0);

	// Begin the scene. Start rendering.   
	if (SUCCEEDED(lpDevice->BeginScene()))
	{
		View();

		//buffer 物体绘图
		/*lpDevice->SetStreamSource(0, g_VB, 0, sizeof(CUSTOMVERTEX));
		lpDevice->SetIndices(g_IB);
		lpDevice->SetFVF(FVF_CUSTOM1);
		lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,4,0,2);*/

		//mesh 物体绘图
		mesh1->DrawSubset(0);


		//计时
		time_t nowticks = GetTickCount64();

		wsprintf(show, _T(""));
		if (ticks != nowticks)
		{
			float fps = 1000. / (nowticks - ticks);
			wsprintf(show, _T("%d"), (int)fps);
		}
		else
		{
			//wsprintf(show, _T("#inf"));
		}
		ticks = nowticks;

		font->DrawTextW(NULL, show,
			-1, &text, DT_LEFT,
			D3DCOLOR_XRGB(100, 120, 240));
		int x, y, z;
		x = (int)(viewer.viewpos.x * 10);
		y = (int)(viewer.viewpos.y * 10);
		z = (int)(viewer.viewpos.z * 10);
		wsprintf(show, _T("%d.%d, %d.%d, %d.%d"), (int)(viewer.viewpos.x), abs(x % 10)
			, (int)(viewer.viewpos.y), abs(y % 10), (int)(viewer.viewpos.z), abs(z % 10));
		font->DrawTextW(NULL, show,
			-1, &text2, DT_LEFT,
			D3DCOLOR_XRGB(100, 120, 240));

		// End the scene. Stop rendering.   
		lpDevice->EndScene();
	}

	// Display the scene.   
	lpDevice->Present(NULL, NULL, NULL, NULL);
}
