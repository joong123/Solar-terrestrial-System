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

	// 修改版循环，效率更高
	while (true)
	{
		QueryPerformanceCounter(&stime2);
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)//退出程序
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//窗口是否最小化 TODO：利用消息机制
		if (IsIconic(mainwnd))
		{
			isiconic = true;
			if (mode != MODE_STOP)
			{
				lastmode = mode;
				mode = MODE_STOP;
			}
		}
		else
		{
			if (isiconic)//从最小化状态恢复
			{
				mode = lastmode;
				isiconic = false;
			}
		}

		WCHAR title[40];
		swprintf_s(title, L"world %.1f / %.1ffps", fps, avgfps);
		SetWindowText(mainwnd, title);
		//主循环
		switch (mode)
		{
		case MODE_RUN:
			loopcount++;
			MainLoop();
			break;
		case MODE_STOP:
			Sleep(1000 / STOPCAPTUREFREQ);//降低STOP模式下主循环频率
			break;
		case MODE_MENU:
			break;
		case MODE_PAUSE:
			break;
		}

		//限制帧率
		QueryPerformanceCounter(&etime);
		if (fpslimit > 0)
		{
			float time = 1000.*(double)(etime.QuadPart - stime.QuadPart) / (double)frequency.QuadPart;
			if (1000 / fpslimit >  time)
			{
				Sleep(1000 / fpslimit - time);
			}
		}

		//更新帧率
		QueryPerformanceCounter(&etime);//记录结束时间
		if ((loopcount & 0x3f) == 0)//每过n次计数计算fps
		{
			if (etime.QuadPart != stime.QuadPart)
				fps = (double)frequency.QuadPart
				/ (double)(etime.QuadPart - stime.QuadPart);

			float rate = 0.05f;
			if (avgfps == -1)//平均帧率
				avgfps = fps;
			else
				avgfps = avgfps*(1 - rate) + fps*rate;

			if (frametime == -1)//平均帧时间
				frametime = (double)(etime.QuadPart - stime2.QuadPart)
				/ (double)frequency.QuadPart;
			else
				frametime = frametime*(1 - rate) +
				(double)(etime.QuadPart - stime2.QuadPart)
				/ (double)frequency.QuadPart*rate;
		}
		stime.QuadPart = etime.QuadPart;//记录开始时间
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
	wcex.lpszMenuName = NULL;//不显示菜单栏//MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
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

	//设置初始窗口大小
	screenwidth = GetSystemMetrics(SM_CXSCREEN);
	screenheight = GetSystemMetrics(SM_CYSCREEN);
	float wndscale = 0.6;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		screenwidth*(1 - wndscale) / 2, screenheight*(1 - wndscale) / 2
		, screenwidth*wndscale, screenheight*wndscale, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	// 添加的自定义初始化
	mainwnd = hWnd;//存储主窗口句柄
	SetWindowText(mainwnd, L"world");
	Init();//初始化
	D3DInit(hWnd, D3DMULTISAMPLE_16_SAMPLES);//D3D初始化

	//D3D内容初始化
	moved = true;
	viewchanged = true;//设置视角更新参数
	shadowchanged = true;
	viewer.SetViewmode(VIEWMODE_CHASE);

	//创建子线程
	tp.start = &startthread;
	tp.pclientcenter = &clientcenter;
	tp.pviewer = &viewer;
	tp.pcursormode = &cursormode;
	InitializeCriticalSection(&cs);
	hThread = CreateThread(NULL, 0, ThreadProc, &tp, 0, &threadID); // 创建线程
	

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
	char key;
	int key_state;
	POINT cur, bias;
	bool keyvalid;

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
	case WM_LBUTTONDOWN:
		if (cursormode == CURSORMODE_NORMAL)//重获鼠标捕捉
		{
			CaptureCursor();
			UpdateStatusStr();//更新窗口状态字符串
		}
		break;
	case WM_ACTIVATE://窗口最小化不产生此事件
		if (wParam == WA_INACTIVE)//结束鼠标捕捉，和Init中getforeground共同处理初始化窗口不接收键盘事件的not focused状态
		{
			focused = false;

			viewer.Inactivate();//中断viewer的键盘控制消息，重置为空

			ReleaseCursor();
			UpdateStatusStr();//更新窗口状态字符串
		}
		else if (wParam == WA_ACTIVE)
		{
			focused = true; 
			UpdateStatusStr();//更新窗口状态字符串
		}
		else if (wParam == WA_CLICKACTIVE)
		{
			focused = true;
			UpdateStatusStr();//更新窗口状态字符串
		}
		break;
	case WM_KEYDOWN:
		key = wParam;
		//key_state = lParam;

		keyvalid = true;//初始化标志为合法按键
		
		if (key == VK_ESCAPE)//结束鼠标捕捉
		{
			if (cursormode == CURSORMODE_CAPTURE)
			{
				ReleaseCursor();
				UpdateStatusStr();//更新窗口状态字符串
			}
		}
		else if (key == 'F')
		{
			fpsshow = !fpsshow;
		}
		else if (key == 'I')
		{
			infoshow = !infoshow;
		}
		else if (key == 'M')
		{
			ChangeMultiSample();
			swprintf_s(showms, _T("%d× MULTISAMPLE/ %dMAX     %d×%d"), multisample, maxmmultisample
				, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top);
		}
		else if (key == VK_F3)
			otherinfoshow = !otherinfoshow;
		else if (key == VK_F4)
		{
			FullScreen(!isfullscreen);
			UpdateStatusStr();//更新窗口状态字符串
		}
		else
			keyvalid = viewer.KeyControlDown(key);

		if (!keyvalid)//输入无效按键
		{
			invalidkey = key;
			QueryPerformanceCounter(&invalidtime);
		}
		//if (key == 'T')//测试字符显示
		//	testchar++;
		//else if (key == 'R')
		//	testchar--;
		break;
	case WM_KEYUP:
		key = wParam;
		key_state = lParam;

		viewer.KeyControlUp(key);
		break;
	case WM_SETCURSOR:
		if (cursormode == CURSORMODE_CAPTURE)
		{
			switch (mode)
			{
			case MODE_RUN:
				SetCursor(LoadCursor(NULL, IDC_ICON));
				break;
			default:
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				break;
			}
		}
		else if(cursormode == CURSORMODE_NORMAL)
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	case WM_SIZE:
		Get2WndRect();//更新窗口区域
		if (cursormode == CURSORMODE_CAPTURE)
			ClipCursor(&clientrect);//重新设置捕获区域

		if (device)
		{
			//修改device长宽
			d3dpp.BackBufferWidth = clientrect.right - clientrect.left;
			d3dpp.BackBufferHeight = clientrect.bottom - clientrect.top;

			OnLostDevice();
			OnResetDevice();
		}
		SetTextRect();//设置text区域
		swprintf_s(showms, _T("%d× MULTISAMPLE/ %dMAX     %d×%d"), multisample, maxmmultisample
			, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top);
		break;
	case WM_MOVE:
		Get2WndRect();//更新窗口区域
		if (cursormode == CURSORMODE_CAPTURE)
			ClipCursor(&clientrect);//重新设置捕获区域
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

void Init()
{
	//窗口信息初始化
	if (IsIconic(mainwnd))
		isiconic = true;
	else
		isiconic = false;
	isfullscreen = false;
	focused = GetForegroundWindow() == mainwnd;
	Get2WndRect();
	originwndrect = wndrect;
	//mainwnd;screenwidth;screenheight;//窗口句柄、屏幕长宽在InitInstance中获得

	//运行信息初始化
	loopcount = 0;
	cursorpos.x = -1;
	cursorpos.y = -1;
	fps = -1.0f;
	avgfps = -1.0f;//needed < 0
	fpslimit = -1.0f;
	frametime = -1.0f;
	invalidkey = 0;//needed 0
	memory1 = 0.0f;
	memory2 = 0.0f;
	//testchar = 0;

	//计时
	QueryPerformanceFrequency(&frequency);
	stime.QuadPart = 0;
	etime.QuadPart = 0;
	stime2.QuadPart = 0;
	etime2.QuadPart = 0;

	//模式
	mode = MODE_RUN;//初始化运行模式
	lastmode = MODE_RUN;
	//初始化焦点状态、捕获状态
	if (!focused)//初始化捕获模式，(处理窗口不能接收事件的情况)
		cursormode = CURSORMODE_NORMAL;
	else
		CaptureCursor();
	//初始化信息显示策略
	infoshow = false;
	otherinfoshow = false;
	fpsshow = true;
	UpdateStatusStr();//初始化状态显示字符串

	SetTextRect();//设置文字显示区域
}

bool FullScreen(bool tofull)
{
	if (isfullscreen == tofull)
		return false;
	else if(tofull)//全屏化
	{
		originwndrect = wndrect;
		RECT m_FullScreenRect;
		m_FullScreenRect.left = wndrect.left - clientrect.left;
		m_FullScreenRect.top = wndrect.top - clientrect.top + 22;
		m_FullScreenRect.right = wndrect.right
			- clientrect.right + screenwidth;
		m_FullScreenRect.bottom = wndrect.bottom
			- clientrect.bottom + screenheight;

		//隐藏任务栏
		/*HWND hwnd;
		hwnd = FindWindow(L"Shell_TrayWnd", NULL);
		ShowWindow(hwnd, SW_HIDE);
		hwnd = GetDlgItem(FindWindow(L"Shell_TrayWnd", NULL), 0x130);
		ShowWindow(hwnd, SW_HIDE);*/


		LONG tmp = GetWindowLong(mainwnd, GWL_STYLE);
		tmp &= ~WS_BORDER; 
		tmp |= WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		SetWindowLong(mainwnd, GWL_STYLE, tmp);

		MoveWindow(mainwnd, m_FullScreenRect.left, m_FullScreenRect.top
			, m_FullScreenRect.right- m_FullScreenRect.left
			, m_FullScreenRect.bottom- m_FullScreenRect.top, TRUE);

		isfullscreen = true;
	}
	else//取消全屏
	{
		//显示任务栏
		/*HWND hwnd;
		hwnd = FindWindow(L"Shell_TrayWnd", NULL);
		ShowWindow(hwnd, SW_SHOW);
		hwnd = GetDlgItem(FindWindow(L"Shell_TrayWnd", NULL), 0x130);
		ShowWindow(hwnd, SW_SHOW);*/

		LONG tmp = GetWindowLong(mainwnd, GWL_STYLE);
		tmp |= WS_POPUPWINDOW;
		tmp |= WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		SetWindowLong(mainwnd, GWL_STYLE, tmp);

		MoveWindow(mainwnd, originwndrect.left, originwndrect.top
			, originwndrect.right - originwndrect.left
			, originwndrect.bottom - originwndrect.top, TRUE);
		SetFocus(mainwnd);
		isfullscreen = false;
	}
}

void CaptureCursor()
{
	cursormode = CURSORMODE_CAPTURE;
	if(mode== MODE_RUN)
	{
		SetCursorPos(clientcenter.x, clientcenter.y);//将指针重置到窗口中点
		ClipCursor(&clientrect);
	}
}

void ReleaseCursor()
{
	cursormode = CURSORMODE_NORMAL;
	ClipCursor(NULL);
}

void D3DInit(HWND wnd, D3DMULTISAMPLE_TYPE ms)
{
	HRESULT hr;

	//Direct3D 接口对象
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	//display mode
	hr = lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displaymode);
	if (FAILED(hr))
		MessageBoxW(wnd, L"get displaymode FAILED!", L"", 0);

	//D3DPRESENT_PARAMETERS
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.MultiSampleType = GetMultisampleType(lpD3D, displaymode);//设置多重采样模式
	if (d3dpp.MultiSampleType > ms)
		d3dpp.MultiSampleType = ms;
	multisample = d3dpp.MultiSampleType;
	swprintf_s(showms, _T("%d× MULTISAMPLE/ %dMAX     %d×%d"), multisample, maxmmultisample
		, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top);//设置multisample显示缓存字符串
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = displaymode.Format;
	//关闭垂直同步,(极大增加帧率，略增加内存占用，较大增加cpu占用率)
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; 
	d3dpp.EnableAutoDepthStencil = TRUE;//深度缓冲
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	//d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	//device
	CreateDevice();	//创建设备
	DeviceInit();	//初始化设备模式

	//viewer
	viewer.SetDevice(device);
	viewer.SetFigure();
	viewer.InitProj(D3DX_PI / 3
		, (float)(clientrect.right - clientrect.left) / (float)(clientrect.bottom - clientrect.top));
	//blocker
	blocker.SetDevice(device);
	blocker.SetGBlock();
	//environment
	environment.SetDevice(device);
	environment.InitSkyBack();
	environment.InitMaterialLight();

	//创建字体
	D3DXCreateFontW(
		device,
		16, 7, 0, 1, 0,			// 字体字符的宽高、是否加粗、Mipmap级别、是否为斜体	
		DEFAULT_CHARSET,        // 默认字符集
		OUT_TT_ONLY_PRECIS,     // 输出精度，使用默认值
		NONANTIALIASED_QUALITY,	// 文本质量
		DEFAULT_PITCH | FF_DONTCARE,
		L"Calibri",
		&font
	);
	D3DXCreateFontW(
		device,
		18, 7, 0, 1, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"微软雅黑",
		&font2
	);
	D3DXCreateFontW(
		device,
		15, 5, 0, 1, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Tw Cen MT",
		&font3
	);
}

void CreateDevice()
{
	device = NULL;
	HRESULT hr = lpD3D->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mainwnd
		, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &device
	);

	if (FAILED(hr))
		MessageBoxW(mainwnd, L"Device create FAILED!", L"", 0);
}

void DeviceInit()
{
	//设置着色模式
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_PHONG);
	//剔除模式：逆时针三角
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//不剔除
	//开启光照
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	//启用深度测试  
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//启用镜面反射光照模型  
	//device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	//启用顶点颜色用于光照计算
	device->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	//设置颜色来源
	//device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	//device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	//device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	//反走样
	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);//开启多重采样
	//device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);//未知
	//device->SetRenderState((_D3DRENDERSTATETYPE)40, TRUE);//不适用
	//雾
	_D3DFOGMODE fogmode = D3DFOG_LINEAR;//雾模式
	float density = 0.001f;
	float start = FOGSTART;
	float end = FOGEND;
	device->SetRenderState(D3DRS_FOGENABLE, TRUE);
	device->SetRenderState(D3DRS_FOGCOLOR, COLOR_SKY1);
	device->SetRenderState(D3DRS_FOGTABLEMODE, fogmode);
	if (fogmode == D3DFOG_LINEAR)
	{
		device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&start));
		device->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&end));
	}
	else
		device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&density));
	//ALPHABLEND
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void SetTextRect()
{
	//text4, 11未使用

	int margin = 2, rowmargin = 6;//边缘量
	int rowscreenheight = 16, rowwidth = 200;//行高，行宽
	text.top = margin;//顶部左侧
	text.bottom = text.top + rowscreenheight;
	text.left = rowmargin;
	text.right = text.left + 300;
	text2.top = text.bottom;//向下接text
	text2.bottom = text2.top + 3*rowscreenheight;
	text2.left = rowmargin;
	text2.right = text2.left + rowwidth;
	text3.top = text2.bottom;//向下接text2
	text3.bottom = text3.top + rowscreenheight;
	text3.left = rowmargin;
	text3.right = text3.left + rowwidth;

	text5.bottom = clientrect.bottom - clientrect.top - margin;//底部左侧
	text5.top = text5.bottom - rowscreenheight;
	text5.left = rowmargin;
	text5.right = text5.left + 300;
	text6.bottom = clientrect.bottom - clientrect.top - margin;//底部中间
	text6.top = text6.bottom - rowscreenheight;
	text6.left = (wndrect.right - wndrect.left) / 2 - rowwidth / 2;
	text6.right = text6.left + rowwidth;
	text12.bottom = clientrect.bottom - clientrect.top - margin;//底部中间
	text12.top = text12.bottom - rowscreenheight;
	text12.left = (wndrect.right - wndrect.left) / 2 - rowwidth / 2 - 40;
	text12.right = text12.left + 40;
	text7.bottom = clientrect.bottom - clientrect.top - margin;//底部右侧
	text7.top = text7.bottom - rowscreenheight;
	text7.right = clientrect.right - clientrect.left - rowmargin;
	text7.left = text7.right - 200;


	text8.top = margin;//顶部右侧
	text8.bottom = text8.top + rowscreenheight;
	text8.right = clientrect.right - clientrect.left - rowmargin;
	text8.left = text7.right - 300; 
	text9.top = text8.bottom;//向下接text8
	text9.bottom = text9.top + rowscreenheight;
	text9.right = clientrect.right - clientrect.left - rowmargin;
	text9.left = text7.right - 300;
	text10.top = text9.bottom;//向下接text9
	text10.bottom = text10.top + rowscreenheight;
	text10.right = clientrect.right - clientrect.left - rowmargin;
	text10.left = text7.right - 300;
}

void ChangeMultiSample()
{
	multisample = d3dpp.MultiSampleType + 1;
	while (multisample != d3dpp.MultiSampleType)//得到下一个合适的多重采样模式
	{
		if (mslist[multisample] == true)
			break;
		multisample++;
		if (multisample > D3DMULTISAMPLE_16_SAMPLES || multisample > maxmmultisample)//越界或超过最大多重采样
			multisample = D3DMULTISAMPLE_NONE;
	}
	//d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)multisample;

	//清除所有创建的D3D变量(D3DPOOL_MANAGED、D3DPOOL_DEFAULT加载的资源都需要释放、重建)
	lpD3D->Release();
	font->Release();
	font2->Release();
	font3->Release();
	device->Release();
	//重新创建D3D变量
	D3DInit(mainwnd, (D3DMULTISAMPLE_TYPE)multisample);
}

void OnLostDevice()
{
	//D3DPOOL_DEFAULT加载的资源(LPD3DXFont、LPD3DXSprite)需要先释放，后重建。
	font->OnLostDevice();
	font2->OnLostDevice();
	font3->OnLostDevice();
}

void OnResetDevice(void)
{
	if (FAILED(device->Reset(&d3dpp)))
	{
		return;
	}

	//D3DPOOL_DEFAULT加载的资源(LPD3DXFont、LPD3DXSprite)需要先释放，后重建。
	font->OnResetDevice();
	font2->OnResetDevice();
	font3->OnResetDevice();

	DeviceInit();//设备参数初始化

	//viewer
	viewer.SetDevice(device);//给viewer传递device
	viewer.SetView();//重置view变换
	//重置投影变换
	viewer.InitProj(D3DX_PI / 3, (float)(clientrect.right - clientrect.left) / (float)(clientrect.bottom - clientrect.top));
	
	//blocker
	blocker.SetDevice(device);
	//blocker.SetGBlock();//不需要重新绘图

	//environment
	environment.SetDevice(device);
	//environment.InitSkyBack();//不需要重新绘图
	environment.InitMaterialLight();
}

void MainLoop()
{
	if (!device)
		return;

	//物理处理，图形处理，移到子线程处理
	//viewer.HandleMove();//处理视角移动
	//if (cursormode == CURSORMODE_CAPTURE)//处理视角旋转
	//{
	//	GetCursorPos(&cursorpos);

	//	POINT bias = cursorpos;
	//	bias.x -= clientcenter.x;
	//	bias.y -= clientcenter.y;
	//	viewer.Rotate(bias);//视角旋转
	//	//将指针重置到窗口中点
	//	SetCursorPos(clientcenter.x, clientcenter.y);
	//}


	// 视角处理，显示处理
	if(viewchanged)
		viewer.SetView();//设置视角，并重置viewchanged为false
	// Clear the buffer.   
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COLOR_CLEAR, 1.0f, 0);
	// Begin the scene. Start rendering.   
	if (SUCCEEDED(device->BeginScene()))
	{
		//渲染状态初始化
		device->SetRenderState(D3DRS_LIGHTING, TRUE);//光照
		device->SetRenderState(D3DRS_FOGENABLE, TRUE);//雾

		//绘制
		if (viewer.viewmode == VIEWMODE_OVERLOOK)
			device->SetRenderState(D3DRS_FOGENABLE, FALSE);//临时关闭雾,TODO
		startthread = false;//结束子线程标志
		blocker.Draw();
		viewer.Draw();
		environment.Draw();

		//重置标志
		viewchanged = false;//重置view改变标志为false
		moved = false;
		shadowchanged = false;//重置阴影标志为false

		startthread = true;//开始子线程标志

		//信息显示
		if (fpsshow)
			FpsShow();
		if (infoshow)
			InfoShow();
		if (otherinfoshow)
			OtherInfoShow();
		
		// End the scene. Stop rendering.   
		device->EndScene();
	}

	HRESULT hr;
	// Display the scene.   
	hr = device->Present(NULL, NULL, NULL, NULL);

	// Render failed, try to reset device
	if (hr == D3DERR_DEVICELOST)
	{
		if (device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)//处理设备丢失
		{
			OnLostDevice();
			OnResetDevice();
		}
	}
}

void FpsShow()
{
	//fps显示，扫描更新
	swprintf_s(show, _T("FPS  %.1f/ %.1f  %.3fms  %lld"), fps, avgfps, frametime*1000., loopcount);
	font->DrawTextW(NULL, show, -1, &text, DT_LEFT, COLOR_RED1);
}

void UpdateStatusStr()
{
	if (focused)
		wcscpy_s(status1, L"focused: true ");
	else
		wcscpy_s(status1, L"focused: false ");
	if (isfullscreen)
		wcscat_s(status1, L" fullscreen: true ");
	else
		wcscat_s(status1, L" fullscreen: false ");
	if (cursormode == CURSORMODE_CAPTURE)
		wcscpy_s(status2, L" captured: true ");
	else
		wcscpy_s(status2, L" captured: false ");
}

void InfoShow()
{
	WCHAR show2[100] = { 0 };
	//pos显示，(扫描更新)
	swprintf_s(show, _T("POS %.3f, %.3f, %.3f")
		, viewer.pos.x, viewer.pos.y, viewer.pos.z);
	//eye方位显示，(扫描更新)
	swprintf_s(show2, _T("\n\rEYE %.3f, %.3f, %.3f")
		, viewpos.x, viewpos.y, viewpos.z);
	wcscat_s(show, show2);
	//视角显示，(扫描更新)
	swprintf_s(show2, _T("\n\rH %.2f°, V %.2f°")
		, viewer.hAngle * RADIAN2DEGREE, viewer.vAngle * RADIAN2DEGREE);
	wcscat_s(show, show2);
	font->DrawTextW(NULL, show, -1, &text2, DT_LEFT, COLOR_WHITE);//三行联合显示

	//viewmode、速度显示
	swprintf_s(show, _T("VIEW %S    SPEED %.2f")
		, viewer.GetViewmodeStr().c_str(), viewer.speed);
	font->DrawTextW(NULL, show, -1, &text3, DT_LEFT, COLOR_GREY1);

	//行走状态、方向显示，扫描更新
	//swprintf_s(show, _T("%lc   %d"), testchar, testchar); //测试显示效果
	if (viewer.speed == 0.0f)
		swprintf_s(show, _T("%lc"), walkdirection[9]);
	else
		swprintf_s(show, _T("%lc"), walkdirection[viewer.curdirection]);
	font2->DrawTextW(NULL, show, -1, &text6, DT_CENTER, COLOR_DIRECTION);
}

void OtherInfoShow()
{
	//多重采样模式和客户区尺寸显示，(全局字符串缓存，条件更新)
	font->DrawTextW(NULL, showms, -1, &text5, DT_LEFT, COLOR_GREY2);

	//按下非法按键，显示提醒
	if (invalidkey)
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		if ((double)(now.QuadPart - invalidtime.QuadPart) / (double)frequency.QuadPart > INVALIDKEY_DELAY)
		{
			invalidkey = 0;
		}
		else
		{
			swprintf_s(show, _T("%lc"), invalidkey);
			font2->DrawTextW(NULL, show, -1, &text12, DT_LEFT, COLOR_RED2);
		}
	}

	//当前时间显示，(定时更新)
	if ((loopcount & 0x3F) == 0)
	{
		time_t rawtime;
		time(&rawtime);
		localtime_s(&Time, &rawtime);//读取当前时间
	}
	swprintf_s(show, _T("%d-%02d-%02d  %S   %02d:%02d:%02d")
		, Time.tm_year + 1900, Time.tm_mon + 1, Time.tm_mday, GetWDayStr(Time.tm_wday).c_str()
		, Time.tm_hour, Time.tm_min, Time.tm_sec);//时间字符串
	font3->DrawTextW(NULL, show, -1, &text7, DT_RIGHT, COLOR_GREY2);

	//内存占用量显示，(定时更新)
	if ((loopcount & 0xFF) == 0)
	{
		HANDLE handle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));

		memory1 = pmc.WorkingSetSize / 1000000.;//内存占用量
		memory2 = pmc.PagefileUsage / 1000000.;	//虚拟内存占用量
	}
	swprintf_s(show, _T("%.1fMB, %.1fMB"), memory1, memory2);
	font->DrawTextW(NULL, show, -1, &text8, DT_RIGHT, COLOR_GREY2);

	//窗口状态显示，(全局字符串缓存，条件更新)
	//UpdateStatusStr();
	font->DrawTextW(NULL, status1, -1, &text9, DT_RIGHT, COLOR_GREY2);
	font->DrawTextW(NULL, status2, -1, &text10, DT_RIGHT, COLOR_GREY2);
}

_D3DMULTISAMPLE_TYPE GetMultisampleType(LPDIRECT3D9 lp, D3DDISPLAYMODE dm)
{
	if (!lp)
		return D3DMULTISAMPLE_NONE;

	int multisample = 0;
	while (multisample<=16)//循环测试，更新多重采样模式适用列表、最大多重采样，当前多重采样模式设为最大
	{
		if (lp->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL
			, dm.Format, true, (_D3DMULTISAMPLE_TYPE)multisample, NULL) != D3D_OK)
			mslist[multisample] = false;
		else
		{
			maxmmultisample = multisample;
			mslist[multisample] = true;
		}

		multisample++;
	}

	return (_D3DMULTISAMPLE_TYPE)maxmmultisample;
}

string GetWDayStr(int wday)
{
	switch (wday)
	{
	case 0:
		return "SUN";
		break;
	case 1:
		return "MON";
		break;
	case 2:
		return "TUE";
		break;
	case 3:
		return "WED";
		break;
	case 4:
		return "THR";
		break;
	case 5:
		return "FRI";
		break;
	case 6:
		return "SAT";
		break;
	default:
		return "UNK";
	}
}

void Get2WndRect()
{
	GetClientRect(mainwnd, &clientrect);//得到client区域尺寸
	POINT clienttl = { 0, 0 };
	ClientToScreen(mainwnd, &clienttl);//获得client区域左上角的屏幕坐标
	//得到client真实屏幕区域
	clientrect.left = clienttl.x;
	clientrect.top = clienttl.y;
	clientrect.right += clientrect.left;
	clientrect.bottom += clientrect.top;
	clientcenter.x = (clientrect.left + clientrect.right) / 2;
	clientcenter.y = (clientrect.top + clientrect.bottom) / 2;

	GetWindowRect(mainwnd, &wndrect);//得到窗口区域
}
