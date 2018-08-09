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

	MSG msg = { 0 };

	WCHAR title[40];
	// 修改版循环，效率更高
	while (msg.message != WM_QUIT)
	{
		QueryPerformanceCounter(&stime2);
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//实时日期 & 时间
		time(&nowtime); 
		if (nowtime != lasttime)
		{
			TIME_ZONE_INFORMATION tzi;
			GetTimeZoneInformation(&tzi);
			devicetimezone = tzi.Bias / -60;//设备时区
			_tzset();//更新时区，防止localtime()不更新

			gmtime_s(&dateGMT, &nowtime);//格林威治时间
			localtime_s(&devicedate, &nowtime);//转换设备时间
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
		/*QueryPerformanceCounter(&etime);
		if (fpslimit > 0)
		{
			float time = 1000.*(double)(etime.QuadPart - stime.QuadPart) / (double)frequency.QuadPart;
			if (1000 / fpslimit >  time)
			{
				Sleep(1000 / fpslimit - time);
			}
		}*/

		//更新帧率
		QueryPerformanceCounter(&etime);//记录结束时间
		if (nowtime != lasttime)//按秒计算fps
		{
			if (etime.QuadPart != stime.QuadPart)
				fps = (float)frequency.QuadPart / (etime.QuadPart - stime.QuadPart);

			float rate = 0.05f;
			if (avgfps == -1)//平均帧率
				avgfps = fps;
			else
				avgfps = avgfps*(1 - rate) + fps*rate;

			frametime = (float)(etime.QuadPart - stime2.QuadPart) / frequency.QuadPart;
			if (avgframetime == -1)//平均帧时间
				avgframetime = frametime;
			else
				avgframetime = avgframetime*(1 - rate) + frametime*rate;

			swprintf_s(title, L"world %.1f / %.1ffps %lld", fps, avgfps, lasttime);
			SetWindowText(mainwnd, title);
		}
		stime.QuadPart = etime.QuadPart;//记录开始时间


		lasttime = nowtime;//存储当前时间为上一时间
		//lastdate = nowdate;
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
	float wndscale = 0.6f;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		(int)(screenwidth*(1 - wndscale) / 2), (int)(screenheight*(1 - wndscale) / 2)
		, (int)(screenwidth*wndscale), (int)(screenheight*wndscale), nullptr, nullptr, hInstance, nullptr);

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
	viewer.SetViewmode(VIEWMODE_CHASE);

	//创建子线程
	//tp.start = &startthread;
	//tp.pclientcenter = &clientcenter;
	//tp.pviewer = &viewer;
	//tp.pcontrolmode = &controlmode;
	//InitializeCriticalSection(&cs);
	//hThread = CreateThread(NULL, 0, ThreadProc, &tp, 0, &threadID); // 创建线程

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
		/*MainLoop();
		ValidateRect(mainwnd,NULL);*/

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		if (controlmode == CONTROLMODE_NORMAL)//重获鼠标捕捉
		{
			CaptureControl();
			UpdateStatusStr();//更新窗口状态字符串
		}
		break;
	case WM_ACTIVATE://窗口最小化不产生此事件
		if (wParam == WA_INACTIVE)//结束鼠标捕捉，和Init中getforeground共同处理初始化窗口不接收键盘事件的not focused状态
		{
			focused = false;

			viewer.Inactivate();//中断viewer的键盘控制消息，重置为空

			ReleaseControl();
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
			if (isfullscreen)
			{
				FullScreen(!isfullscreen);
			}
			else
			{
				if (controlmode == CONTROLMODE_CAPTURE)
				{
					ReleaseControl();
					UpdateStatusStr();//更新窗口状态字符串
				}
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
		else if(controlmode == CONTROLMODE_CAPTURE)
			keyvalid = viewer.KeyControlDown(key);

		if (!keyvalid)//输入无效按键
		{
			invalidkey = key;
			QueryPerformanceCounter(&invalidtime);
		}
		if (key == 'B')//测试字符显示
			testchar++;
		else if (key == 'G')
			testchar--;
		break;
	case WM_KEYUP:
		key = wParam;
		key_state = lParam;

		viewer.KeyControlUp(key);
		break;
	case WM_SETCURSOR:
		if (controlmode == CONTROLMODE_CAPTURE)
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
		else if(controlmode == CONTROLMODE_NORMAL)
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	case WM_SIZE:
		WCHAR title[50];
		swprintf_s(title, L"world %.1f / %.1ffps %lld", fps, avgfps, lasttime);
		SetWindowText(mainwnd, title);
		Get2WndRect();//更新窗口区域
		if (controlmode == CONTROLMODE_CAPTURE)
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
		if (controlmode == CONTROLMODE_CAPTURE)
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
	time(&lasttime);
	localtime_s(&lastdate, &lasttime);
	time(&nowtime);
	localtime_s(&devicedate, &nowtime);
	//mainwnd;screenwidth;screenheight;//窗口句柄、屏幕长宽在InitInstance中获得

	//运行信息初始化
	loopcount = 0;
	cursorpos.x = -1;
	cursorpos.y = -1;
	fps = -1.0f;
	avgfps = -1.0f;//needed < 0
	fpslimit = -1.0f;
	avgframetime = -1.0f;
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
		controlmode = CONTROLMODE_NORMAL;
	else
		CaptureControl();
	//初始化信息显示策略
	fpsshow = true;
	infoshow = true;
	otherinfoshow = true;
	UpdateStatusStr();//初始化状态显示字符串

	SetTextRect();//设置文字显示区域

	anti = false;
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
			- clientrect.right + screenwidth + 12;
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
		HWND hwnd;
		hwnd = FindWindow(L"Shell_TrayWnd", NULL);
		ShowWindow(hwnd, SW_SHOW);
		hwnd = GetDlgItem(FindWindow(L"Shell_TrayWnd", NULL), 0x130);
		ShowWindow(hwnd, SW_SHOW);

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

void CaptureControl()
{
	controlmode = CONTROLMODE_CAPTURE;
	if(mode== MODE_RUN)
	{
		SetCursorPos(clientcenter.x, clientcenter.y);//将指针重置到窗口中点
		ClipCursor(&clientrect);
	}
}

void ReleaseControl()
{
	controlmode = CONTROLMODE_NORMAL;
	ClipCursor(NULL);
}

void D3DInit(HWND wnd, D3DMULTISAMPLE_TYPE ms)
{
	HRESULT hr;

	//Direct3D 接口对象
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	//caps
	if (FAILED(lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
		MessageBoxW(wnd, L"get caps FAILED!", L"", 0);
		

	//display mode
	hr = lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displaymode);
	if (FAILED(hr))
		MessageBoxW(wnd, L"get displaymode FAILED!", L"", 0);

	//D3DPRESENT_PARAMETERS
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth				= clientrect.right - clientrect.left;
	d3dpp.BackBufferHeight				= clientrect.bottom - clientrect.top;
	d3dpp.BackBufferFormat				= D3DFMT_A8R8G8B8;//加速，displaymode.Format
	d3dpp.BackBufferCount				= 2;
	d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow					= mainwnd;
	d3dpp.Windowed						= TRUE;
	d3dpp.EnableAutoDepthStencil		= TRUE;//深度缓冲
	d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;//加速用D3DFMT_D24S8不用D3DFMT_D16
	d3dpp.Flags							= 0;
	d3dpp.FullScreen_RefreshRateInHz	= 0;
	d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;//关闭垂直同步,(极大增加帧率，略增加内存占用，较大增加cpu占用率)
	d3dpp.MultiSampleType				= GetMultisampleType(lpD3D, displaymode);//设置多重采样模式D3DMULTISAMPLE_NONE
	d3dpp.MultiSampleQuality			= 0;
	if (d3dpp.MultiSampleType > ms)
		d3dpp.MultiSampleType = ms;
	multisample = d3dpp.MultiSampleType;
	swprintf_s(showms, _T("%d× MULTISAMPLE/ %dMAX     %d×%d"), multisample, maxmmultisample
		, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top);//设置multisample显示缓存字符串

	//device
	CreateDevice();	//创建设备
	DeviceInit();	//初始化设备模式

	//viewer
	viewer.SetDevice(device);
	viewer.SetFigure();
	viewer.InitProj((float)(clientrect.right - clientrect.left) / (clientrect.bottom - clientrect.top));
	//blocker
	blocker.SetDevice(device);
	blocker.SetGBlock();
	//environment
	environment.SetDevice(device);
	environment.InitSkyBack();
	environment.InitMaterialLight(viewer.longitude, viewer.latitude);

	depthbias = DEPTHBIAS;
	sdepthbias = SLOPESCALEDEPTHBIAS;
	if ((caps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS)
		&& (caps.RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS))
		depthbiasable = true;
	else
		depthbiasable = false;

	//创建字体
	//16-7-Calibri || 20-5-Raavi || 16-9-Kartika || 13-6-Arial Rounded MT Bold
	//20-9-PixelSix10
	D3DXCreateFontW(
		device,
		13, 5, 0, 1000, 0,			// 字体字符的宽高、是否加粗、Mipmap级别、是否为斜体	
		DEFAULT_CHARSET,        // 默认字符集
		OUT_DEFAULT_PRECIS,     // 输出精度，使用默认值
		CLEARTYPE_NATURAL_QUALITY,	// 文本质量NONANTIALIASED_QUALITY/CLEARTYPE_NATURAL_QUALITY
		DEFAULT_PITCH | FF_DONTCARE,
		L"Arial Rounded MT Bold",
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
		CLEARTYPE_NATURAL_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Tw Cen MT",
		&font3
	);
}

void CreateDevice()
{
	device = NULL;
	HRESULT hr;
	int vp;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	hr = lpD3D->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mainwnd
		, vp, &d3dpp, &device
	);

	if (FAILED(hr))
		MessageBoxW(mainwnd, L"Device create FAILED!", L"", 0);
}

void DeviceInit()
{
	//设置着色模式
	//device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);//D3DSHADE_PHONG
	//剔除模式：逆时针三角
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//开启光照
	//device->SetRenderState(D3DRS_LIGHTING, TRUE);
	//启用深度测试  
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//启用镜面反射光照模型  
	//device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	//启用顶点颜色用于光照计算
	//device->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	//设置颜色来源
	//device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	//device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	//device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	//反走样
	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);//开启多重采样,??关闭仍然有效果
	//device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);//未知
	//device->SetRenderState((_D3DRENDERSTATETYPE)40, TRUE);//不适用
	//雾
	_D3DFOGMODE fogmode = D3DFOG_LINEAR;//雾模式
	float density = 0.001f;
	float start = FOGSTART;
	float end = FOGEND;
	//device->SetRenderState(D3DRS_FOGENABLE, TRUE);
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
	//stencil
	//device->SetRenderState(D3DRS_STENCILENABLE, true);
	device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	device->SetRenderState(D3DRS_STENCILREF, 0x0);
	device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR); //increment to 1
	device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	//纹理过滤
	/*device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/
}

void SetTextRect()
{
	//text3, 4, 11未使用

	const int margin = 2, rowmargin = 6, bottommargin = 4;//边缘量
	const int rowscreenheight = 16, rowwidth = 200;//行高，行宽
	text.top = margin;//顶部左侧
	text.bottom = text.top + rowscreenheight;
	text.left = rowmargin;
	text.right = text.left + 300;
	const int multirows = 8;
	text2.top = text.bottom;//向下接text
	text2.bottom = text2.top + multirows*rowscreenheight;
	text2.left = rowmargin;
	text2.right = text2.left + rowwidth;
	//text3.top = text2.bottom;//向下接text2
	//text3.bottom = text3.top + rowscreenheight;
	//text3.left = rowmargin;
	//text3.right = text3.left + rowwidth;

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
	const int timerows = 9;
	text7.bottom = clientrect.bottom - clientrect.top - bottommargin;//底部右侧
	text7.top = text7.bottom - timerows * rowscreenheight;
	text7.right = clientrect.right - clientrect.left - rowmargin;
	text7.left = text7.right - 200;
	//特殊字符显示
	text11.bottom = text7.bottom - 18;//底部右侧
	text11.top = text7.bottom - 31;
	text11.right = text7.left + 19;
	text11.left = text7.left - 20;


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
	//D3D内容初始化
	viewer.SetView();
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

	//device地址没改变情况下，D3D内容初始化
	viewer.SetView();//重置view变换

	//viewer
	//viewer.SetDevice(device);//给viewer传递device
	//viewer.SetFigure();
	viewer.InitProj((float)(clientrect.right - clientrect.left) / (clientrect.bottom - clientrect.top));//重置投影变换
	
	//blocker
	//blocker.SetDevice(device);
	//blocker.SetGBlock();//不需要重新绘图

	//environment
	//environment.SetDevice(device);
	//environment.InitSkyBack();//不需要重新绘图
	environment.InitMaterialLight(viewer.longitude, viewer.latitude);//设置材质与光照
}

void MainLoop()
{
	if (!device)
		return;

	//物理处理，图形处理，移到子线程处理
	viewer.HandleMove();//处理视角移动
	if (controlmode == CONTROLMODE_CAPTURE)//处理视角旋转
	{
		GetCursorPos(&cursorpos);
		SetCursorPos(clientcenter.x, clientcenter.y);//将指针重置到窗口中点

		POINT bias = cursorpos;
		bias.x -= clientcenter.x;
		bias.y -= clientcenter.y;
		rotated = viewer.Rotate(bias);//视角旋转
	}

	viewer.ViewMove();//计算实时经纬度
	//viewpos信息更新
	if (viewer.bindview)
		viewer.BindView();

	//环境按n秒变化
	if (nowtime != lasttime || viewer.eyehmoved)
		environment.SetTime(viewer.longitude);//根据经度调整时间
	if (nowtime / 2 != lasttime / 2 || viewer.eyehmoved)
	{
		environment.SunMove(viewer.latitude);
		environment.setPos();

		viewer.eyehmoved = false;
	}

	// 视角处理，显示处理
	if (viewer.viewchanged)
	{
		viewer.RefreshView();//更新视角矩阵
		viewer.SetView();//设置视角，并重置viewchanged为false
	}


	// Clear the buffer.   
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, COLOR_CLEAR, 1.0f, 0);
	// Begin the scene. Start rendering.   
	device->BeginScene();

	//渲染状态初始化
	device->SetRenderState(D3DRS_LIGHTING, TRUE);//光照
	device->SetRenderState(D3DRS_FOGENABLE, TRUE);//雾

	//绘制
	//startthread = false;//结束子线程标志
	
	environment.Render();
	if (viewer.viewmode == VIEWMODE_OVERLOOK)
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);//临时关闭雾,TODO
	blocker.Render();
	viewer.Render();

	//重置标志
	//重置阴影标志为false
	shadowchanged = false;

	//startthread = true;//开始子线程标志

	//信息显示
	if (fpsshow)
		FpsShow();
	if (infoshow)
		InfoShow();
	if (otherinfoshow)
		OtherInfoShow();
		
	// End the scene. Stop rendering.   
	device->EndScene();

	HRESULT hr = 0;
	// Display the scene.  
	//QueryPerformanceCounter(&stime);
	hr = device->Present(NULL, NULL, NULL, NULL);
	//QueryPerformanceCounter(&etime);//单独记录present时间

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
	swprintf_s(show, _T("FPS  %.1f/ %.1f  %.3f/ %.3fms  %lld"), fps, avgfps, frametime*1000., avgframetime*1000., loopcount);
	font->DrawText(NULL, show, -1, &text, DT_LEFT | DT_NOCLIP | DT_SINGLELINE, COLOR_WHITE);
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
	if (controlmode == CONTROLMODE_CAPTURE)
		wcscpy_s(status2, L" captured: true ");
	else
		wcscpy_s(status2, L" captured: false ");
}

void InfoShow()
{
	WCHAR show2[512] = { 0 };
	//pos显示，(扫描更新)
	swprintf_s(show, _T("POS  %.3lf, %.3lf, %.3lf")
		, viewer.pos_d.x, viewer.pos_d.y, viewer.pos_d.z);
	//eye方位显示，(扫描更新)inblockTranslation
	swprintf_s(show2, _T("\n\rEYE  %.6lf, %.6lf, %.6lf")
		, viewer.viewpos_d.x, viewer.viewpos_d.y, viewer.viewpos_d.z);
	wcscat_s(show, show2);
	//inblockTranslation显示，(扫描更新)
	/*swprintf_s(show2, _T("\n\rCLIP  %.6f, %.6f, %.6f")
		, inblockpos->x, inblockpos->y, inblockpos->z);
	wcscat_s(show, show2);*/
	//视角显示，(扫描更新)
	swprintf_s(show2, _T("\n\rH %.2f°, V %.2f, EARTHH %.2f°  \t%S")
		, viewer.hAngle * RADIAN2DEGREE, viewer.vAngle * RADIAN2DEGREE
		, viewer.earthhangle * RADIAN2DEGREE
		, viewdirection[(int)((viewer.earthhangle + MYPI / 8) / MYPI * 4)]
	);
	if (abs((int)((viewer.earthhangle + MYPI / 8) / MYPI * 4)*MYPI / 4 - viewer.earthhangle)
		<= ANGLE_NEARRANGE / RADIAN2DEGREE)
		wcscat_s(show2, L"+");
	wcscat_s(show, show2);
	//位移显示，(扫描更新)
	swprintf_s(show2, _T("\n\r∆  %.4f, %.4f, %.4f")
		, viewer.displacement.x, viewer.displacement.y, viewer.displacement.z);
	wcscat_s(show, show2);
	//区块位置显示，(扫描更新)
	/*swprintf_s(show2, _T("\n\rBLOCK  %lld, %lld")
		, blockindex.x, blockindex.y);
	wcscat_s(show, show2);*/
	//viewmode、速度显示
	swprintf_s(show2, _T("\n\rVIEW  %S    SPEED  %.2f  %.2fm/s")
		, viewer.GetViewmodeStr().c_str(), viewer.speed, viewer.speed*viewer.speedrate / ONEMETER);
	wcscat_s(show, show2);
	//view参数
	swprintf_s(show2, _T("\n\rVIEWANGLE  %.1f°  ASPECT  %.3f")
		, viewer.viewangle * RADIAN2DEGREE, viewer.viewaspect);
	wcscat_s(show, show2);


	wcscat_s(show, L"\n\r");
	double absradian = 0;
	//经纬度显示
	swprintf_s(show2, _T("\n\rLONGITUDE  %03d°%02d'%05.2f\"")
		, GetRadianDValue(environment.baselongitude)
		, GetRadianMValue(environment.baselongitude)
		, GetRadianSValue(environment.baselongitude)
	);
	wcscat_s(show, show2);
	swprintf_s(show2, _T("  LATITUDE  %02d°%02d'%05.2f\"")
		, GetRadianDValue(environment.baselatitude)
		, GetRadianMValue(environment.baselatitude)
		, GetRadianSValue(environment.baselatitude)
	);
	wcscat_s(show, show2);
	//实时经纬度显示
	swprintf_s(show2, _T("\n\rlongitude  %02d°%02d'%05.2lf\"  latitude  %02d°%02d'%05.2lf\"  real:%16.13lf, %16.13lf")
		, GetRadianDValue(viewer.longitude)
		, GetRadianMValue(viewer.longitude)
		, GetRadianSValue(viewer.longitude)
		, GetRadianDValue(viewer.latitude)
		, GetRadianMValue(viewer.latitude)
		, GetRadianSValue(viewer.latitude)
		, viewer.longitude * RADIAN2DEGREE
		, viewer.latitude * RADIAN2DEGREE
	);
	wcscat_s(show, show2);
	//yearangle显示，(扫描更新)yearangle
	swprintf_s(show2, _T("\n\rYANGLE  %02d°%02d'%05.2f\"  DAYS  %.6lf/ %d")
		, GetRadianDValue(environment.yearangle)
		, GetRadianMValue(environment.yearangle)
		, GetRadianSValue(environment.yearangle)
		, environment.yearpassed, environment.ydays
	);
	wcscat_s(show, show2);
	//赤道平面上，与直射点反面同一经线的点，相对地轴需转过的角度显示
	swprintf_s(show2, _T("\n\rEQUATORANGLE  %02d°%02d'%05.2f\"")
		, GetRadianDValue(environment.equatorangle)
		, GetRadianMValue(environment.equatorangle)
		, GetRadianSValue(environment.equatorangle)
	);
	wcscat_s(show, show2);
	//赤纬
	swprintf_s(show2, _T("\n\rDECLINATION  %02d°%02d'%05.2f\"  P:%02d°%02d'%05.2f\"")
		, GetRadianDValue(environment.declination_general)
		, GetRadianMValue(environment.declination_general)
		, GetRadianSValue(environment.declination_general)
		, GetRadianDValue(environment.declination_precise)
		, GetRadianMValue(environment.declination_precise)
		, GetRadianSValue(environment.declination_precise)
	);
	wcscat_s(show, show2);
	//太阳位置显示，(扫描更新)
	swprintf_s(show2, _T("\n\rSUN  H %02d°%02d'%02d\"  V %02d°%02d'%02d\"   %S")
		, GetRadianDValue(environment.hangle)
		, GetRadianMValue(environment.hangle)
		, GetRadianSValueI(environment.hangle)
		, GetRadianDValue(environment.vangle)
		, GetRadianMValue(environment.vangle)
		, GetRadianSValueI(environment.vangle)
		, viewdirection[(int)((environment.hangle + MYPI / 8) / MYPI * 4)]
	);
	if (abs((int)((environment.hangle + MYPI / 8) / MYPI * 4)*MYPI / 4 - environment.hangle)
		<= ANGLE_NEARRANGE / RADIAN2DEGREE)
		wcscat_s(show2, L"+");//是否逼近一个方位TODO ERROR
	wcscat_s(show, show2);
	//光照指数显示，(扫描更新)
	swprintf_s(show2, _T("\n\rLIGHT  A %.4f%%  D %.4f%%")
		, environment.ambientindex * 100, environment.diffuseindex * 100);
	wcscat_s(show, show2);
	//法向量显示，(扫描更新)
	/*swprintf_s(show2, _T("\n\rNOR %.3f, %.3f, %.3f  LI %.3f")
		, environment.basenormal.x, environment.basenormal.y, environment.basenormal.z
		, ambientindex);
	wcscat_s(show, show2);*/
	//法向量显示，(扫描更新)
	swprintf_s(show2, _T("\n\rGNOR  %.4f, %.4f, %.4f  |%.6f|")
		, environment.groundnormal.x, environment.groundnormal.y, environment.groundnormal.z
		, D3DXVec3Length(&environment.groundnormal));
	wcscat_s(show, show2);
	//方向向量显示，(扫描更新)
	swprintf_s(show2, _T("\n\rEWDIR  %.4f, %.4f, %.4f  |%.6f|  %3.3E")
		, environment.grounddirectionEW.x, environment.grounddirectionEW.y, environment.grounddirectionEW.z
		, D3DXVec3Length(&environment.grounddirectionEW)
		, D3DXVec3Dot(&environment.grounddirectionEW, &environment.groundnormal)
	);
	wcscat_s(show, show2);
	//PI显示
	swprintf_s(show2, _T("\n\rπ  %.16lf (D3D)\n\rπ  %.16lf (MY)")
		, D3DX_PI
		, MYPI
	);
	wcscat_s(show, show2);
	font->DrawTextW(NULL, show, -1, &text2, DT_LEFT | DT_NOCLIP, COLOR_WHITE);//多行联合显示

	//行走状态、方向显示，扫描更新
	//swprintf_s(show, _T("%lc   %d"), testchar, testchar); //测试显示效果
	if (viewer.speed == 0.0f)
		swprintf_s(show, _T("%lc"), walkdirection[9]);
	else
		swprintf_s(show, _T("%lc"), walkdirection[viewer.curdirection]);
	if(rotated)
		wcscat_s(show, L">>");
	font2->DrawTextW(NULL, show, -1, &text6, DT_LEFT | DT_NOCLIP, COLOR_DIRECTION);
}

void OtherInfoShow()
{
	//多重采样模式和客户区尺寸显示，(全局字符串缓存，条件更新)
	font->DrawTextW(NULL, showms, -1, &text5, DT_LEFT | DT_NOCLIP, COLOR_GREY2);

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
			font2->DrawTextW(NULL, show, -1, &text12, DT_LEFT | DT_NOCLIP, COLOR_RED2);
		}
	}

	WCHAR show2[256] = { 0 };
	//冬至时区
	if(environment.basetimezone>=0)
		swprintf_s(show, _T("MIDWINTER(BASE) TIMEZONE (+%d)"), environment.basetimezone);
	else
		swprintf_s(show, _T("MIDWINTER(BASE) TIMEZONE (-%d)"), environment.basetimezone);
	wcscat_s(show, show2);

	//冬至时显示
	swprintf_s(show2, _T("\r\nMIDWINTERS %d-%02d-%02d  %S   %02d:%02d:%02d\r\nMIDWINTERE %d-%02d-%02d  %S   %02d:%02d:%02d")
		, environment.lastmidwinter.tm_year + 1900, environment.lastmidwinter.tm_mon + 1, environment.lastmidwinter.tm_mday
		, GetWDayStr(environment.lastmidwinter.tm_wday).c_str()
		, environment.lastmidwinter.tm_hour, environment.lastmidwinter.tm_min, environment.lastmidwinter.tm_sec
		, environment.nextmidwinter.tm_year + 1900, environment.nextmidwinter.tm_mon + 1, environment.nextmidwinter.tm_mday
		, GetWDayStr(environment.nextmidwinter.tm_wday).c_str()
		, environment.nextmidwinter.tm_hour, environment.nextmidwinter.tm_min, environment.nextmidwinter.tm_sec);//时间字符串
	wcscat_s(show, show2); 

	//回归年时长显示
	swprintf_s(show2, _T("\r\nTROPICAL YEAR LENGTH %lld sec")
		, (environment.tyearend - environment.tyearstart));
	wcscat_s(show, show2);

	//当前时间秒数显示，(定时更新)
	swprintf_s(show2, _T("\r\nSEC  %lld"), nowtime);
	wcscat_s(show, show2);

	//格林威治时间显示，(定时更新)
	swprintf_s(show2, _T("\r\n(0) GMT %d-%02d-%02d  %S   %02d:%02d:%02d")
		, dateGMT.tm_year + 1900, dateGMT.tm_mon + 1, dateGMT.tm_mday, GetWDayStr(dateGMT.tm_wday).c_str()
		, dateGMT.tm_hour, dateGMT.tm_min, dateGMT.tm_sec
		, devicedate.tm_year + 1900, devicedate.tm_mon + 1, devicedate.tm_mday, GetWDayStr(devicedate.tm_wday).c_str()
		, devicedate.tm_hour, devicedate.tm_min, devicedate.tm_sec
	);
	wcscat_s(show, show2);

	//设备时间
	if (devicetimezone > 0)
	{
		swprintf_s(show2, _T("\r\n(+%d) DEVICE %d-%02d-%02d  %S   %02d:%02d:%02d")
			, devicetimezone
			, devicedate.tm_year + 1900, devicedate.tm_mon + 1, devicedate.tm_mday, GetWDayStr(devicedate.tm_wday).c_str()
			, devicedate.tm_hour, devicedate.tm_min, devicedate.tm_sec
		);
	}
	else
	{
		swprintf_s(show2, _T("\r\n(%d) DEVICE %d-%02d-%02d  %S   %02d:%02d:%02d")
			, devicetimezone
			, devicedate.tm_year + 1900, devicedate.tm_mon + 1, devicedate.tm_mday, GetWDayStr(devicedate.tm_wday).c_str()
			, devicedate.tm_hour, devicedate.tm_min, devicedate.tm_sec
		);
	}
	wcscat_s(show, show2);

	//提供冬至时时区的时间
	swprintf_s(show2, _T("\r\nBASE %d-%02d-%02d  %S   %02d:%02d:%02d")
		, basedate.tm_year + 1900, basedate.tm_mon + 1, basedate.tm_mday, GetWDayStr(basedate.tm_wday).c_str()
		, basedate.tm_hour, basedate.tm_min, basedate.tm_sec
	);
	wcscat_s(show, show2);

	byte polarday = 0;

	if (MYPI / 2 - viewer.latitude <= abs(environment.declination_precise))
		polarday = 1;
	else if(MYPI / 2 + viewer.latitude <= abs(environment.declination_precise))
		polarday = 2;

	wcscat_s(show, L"\r\n");
	if (polarday == 1)
		wcscat_s(show, L"POLAR DAY  ");
	else if(polarday == 2)
		wcscat_s(show, L"POLAR NIGHT  ");

	//当前位置的准确时间
	short timezone;
	if (viewer.longitude >= 0)
		timezone = (int)(viewer.longitude / TIMEZONERADIAN + 0.5);
	else
		timezone = (int)(viewer.longitude / TIMEZONERADIAN - 0.5);
	if (timezone > 0)
	{
		swprintf_s(show2, _T("(+%d) REAL %d-%02d-%02d  %S   %02d:%02d:%02d")
			, timezone
			, realdate.tm_year + 1900, realdate.tm_mon + 1, realdate.tm_mday, GetWDayStr(realdate.tm_wday).c_str()
			, realdate.tm_hour, realdate.tm_min, realdate.tm_sec
		);//时间字符串
	}
	else
	{
		swprintf_s(show2, _T("(%d) REAL %d-%02d-%02d  %S   %02d:%02d:%02d")
			, timezone
			, realdate.tm_year + 1900, realdate.tm_mon + 1, realdate.tm_mday, GetWDayStr(realdate.tm_wday).c_str()
			, realdate.tm_hour, realdate.tm_min, realdate.tm_sec
		);//时间字符串
	}
	wcscat_s(show, show2);
	font3->DrawTextW(NULL, show, -1, &text7, DT_RIGHT | DT_BOTTOM | DT_NOCLIP, COLOR_GREY2);

	//特殊字符
	swprintf_s(show, _T("%lc"), L'￼');
	font2->DrawTextW(NULL, show, -1, &text11, DT_RIGHT | DT_NOCLIP, COLOR_GREY2);

	//内存占用量显示，(定时更新)
	if (nowtime != lasttime)
	{
		HANDLE handle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));

		memory1 = pmc.WorkingSetSize / 1000000.0f;//内存占用量
		memory2 = pmc.PagefileUsage / 1000000.0f;	//虚拟内存占用量
	}
	swprintf_s(show, _T("%.1fMB, %.1fMB"), memory1, memory2);
	font->DrawTextW(NULL, show, -1, &text8, DT_RIGHT | DT_NOCLIP, COLOR_GREY2);

	//窗口状态显示，(全局字符串缓存，条件更新)
	//UpdateStatusStr();
	font->DrawTextW(NULL, status1, -1, &text9, DT_RIGHT | DT_NOCLIP, COLOR_GREY2);
	font->DrawTextW(NULL, status2, -1, &text10, DT_RIGHT | DT_NOCLIP, COLOR_GREY2);
}

_D3DMULTISAMPLE_TYPE GetMultisampleType(LPDIRECT3D9 lp, D3DDISPLAYMODE dm)
{
	if (!lp)
		return D3DMULTISAMPLE_NONE;

	int multisample = 0;
	while (multisample <= 16)//循环测试，更新多重采样模式适用列表、最大多重采样，当前多重采样模式设为最大
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

inline int GetRadianDValue(double radian)
{
	return (int)(radian*RADIAN2DEGREE);
}

inline int GetRadianMValue(double radian)
{
	double absd = abs(radian)*RADIAN2DEGREE;
	return (int)floor((absd - (int)absd) * 60);
}

inline double GetRadianSValue(double radian)
{
	double absd = abs(radian)*RADIAN2DEGREE;
	double fraction = absd - (int)absd;
	return floor((((fraction * 60) - floor(fraction * 60)) * 60.) * 100) / 100;
}

inline int GetRadianSValueI(double radian)
{
	double absd = abs(radian)*RADIAN2DEGREE;
	double fraction = absd - (int)absd;
	return (int)(((fraction * 60) - (int)(fraction * 60)) * 60);
}
