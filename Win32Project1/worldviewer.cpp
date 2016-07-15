#include "stdafx.h"
#include "worldviewer.h"

WViewer::WViewer()
{
	device = NULL;

	up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	viewpos = D3DXVECTOR3(0.0f, 0.0f, 0.0f );
	viewheight = 1.0f;
	viewdist = 1000.0f;
	viewangle = D3DX_PI / 3;
	viewaspect = 1.0f;
	hAngle = 0 * D3DX_PI;
	vAngle = 0.0f;

	SetViewmode(VIEWMODE_CHASE);
}

WViewer::~WViewer()
{
}

bool WViewer::SetDevice(LPDIRECT3DDEVICE9 device)
{
	this->device = device;

	return (device != NULL);
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
		switch (viewmode)
		{
		case VIEWMODE_FIRSTPERSON:
			viewpos = pos + D3DXVECTOR3(0.0f, 0.0f, -viewheight);
			at = viewpos + D3DXVECTOR3(cos(hAngle), sin(hAngle), 0.0f);
			up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case VIEWMODE_CHASE:
			viewpos = pos + D3DXVECTOR3(-chasedist*cos(hAngle), -chasedist*sin(hAngle), -viewheight);
			at = viewpos + D3DXVECTOR3(cos(hAngle), sin(hAngle), sin(vAngle));
			up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case VIEWMODE_FREE:
			viewpos = pos + D3DXVECTOR3(-chasedist*cos(hAngle), -chasedist*sin(hAngle), -viewheight);
			at = viewpos + D3DXVECTOR3(cos(hAngle), sin(hAngle), sin(vAngle));
			up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case VIEWMODE_OVERLOOK:
			viewpos = pos + D3DXVECTOR3(0.0f, 0.0f, -overlookheight);
			at = pos;
			up = D3DXVECTOR3(cos(hAngle), sin(hAngle), 0.0f);
			break;
		}
		return true;
	}
}

bool WViewer::SetView(LPDIRECT3DDEVICE9 dvc)
{
	if (!dvc)
	{
		if (!device)
			return false;

		D3DXMATRIXA16 V;
		D3DXMatrixLookAtLH(&V, &viewpos, &at, &up);
		device->SetTransform(D3DTS_VIEW, &V);

		D3DXMATRIXA16 proj;
		D3DXMatrixPerspectiveFovLH(&proj, viewangle,
			viewaspect, 0.0f, viewdist);
		device->SetTransform(D3DTS_PROJECTION, &proj);
	}
	else
	{

	}
	return true;
}

void WViewer::Walk(int key)
{
		if (viewmode == VIEWMODE_FIRSTPERSON || viewmode == VIEWMODE_CHASE)
		{
			if (key == VK_UP || key == 'W')//前进
			{
				pos += D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				viewpos += D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				at += D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
			}
			else if (key == VK_DOWN || key == 'S')
			{
				pos -= D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				viewpos -= D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				at -= D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
			}
			else if (key == VK_LEFT || key == 'A')
			{
				pos += D3DXVECTOR3(-division*sin(hAngle), division*cos(hAngle), 0.0f);
				viewpos += D3DXVECTOR3(-division*sin(hAngle), division*cos(hAngle), 0.0f);
				at += D3DXVECTOR3(-division*sin(hAngle), division*cos(hAngle), 0.0f);
			}
			else if (key == VK_RIGHT || key == 'D')
			{
				pos += D3DXVECTOR3(division*sin(hAngle), -division*cos(hAngle), 0.0f);
				viewpos += D3DXVECTOR3(division*sin(hAngle), -division*cos(hAngle), 0.0f);
				at += D3DXVECTOR3(division*sin(hAngle), -division*cos(hAngle), 0.0f);
			}
		}
		else if (viewmode == VIEWMODE_OVERLOOK)
		{
			if (key == VK_UP || key == 'W')//前进
			{
				pos += D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				viewpos += D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				at = pos;
			}
			else if (key == VK_DOWN || key == 'S')
			{
				pos -= D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				viewpos -= D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), 0.0f);
				at = pos;
			}
			else if (key == VK_LEFT || key == 'A')
			{
				pos += D3DXVECTOR3(-division*sin(hAngle), division*cos(hAngle), 0.0f);
				viewpos += D3DXVECTOR3(-division*sin(hAngle), division*cos(hAngle), 0.0f);
				at = pos;
			}
			else if (key == VK_RIGHT || key == 'D')
			{
				pos += D3DXVECTOR3(division*sin(hAngle), -division*cos(hAngle), 0.0f);
				viewpos += D3DXVECTOR3(division*sin(hAngle), -division*cos(hAngle), 0.0f);
				at = pos;
			}
		}
		else if (viewmode == VIEWMODE_FREE)
		{
			viewpos += D3DXVECTOR3(division*cos(hAngle), division*sin(hAngle), division*sin(vAngle));
		}
}

void WViewer::Rotate(POINT bias)
{
	if (viewmode == VIEWMODE_FIRSTPERSON || viewmode == VIEWMODE_CHASE)
	{
		hAngle += bias.x / viewdist;
		if (hAngle < 0)
			hAngle += 2 * D3DX_PI;
		if (hAngle >= 2 * D3DX_PI)
			hAngle -= 2 * D3DX_PI;

		vAngle += bias.y / viewdist;
		if (vAngle < -D3DX_PI/2)
			vAngle = -D3DX_PI / 2;
		if (vAngle > D3DX_PI / 2)
			vAngle = D3DX_PI / 2;
	}
	
	switch (viewmode)
	{
	case VIEWMODE_FIRSTPERSON:
		viewpos = pos + D3DXVECTOR3(0.0f, 0.0f, -viewheight);
		at = viewpos + D3DXVECTOR3(cos(hAngle), sin(hAngle), 0.0f);
		up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		break;
	case VIEWMODE_CHASE:
		viewpos = pos + D3DXVECTOR3(-chasedist*cos(hAngle), -chasedist*sin(hAngle), -viewheight);
		at = viewpos + D3DXVECTOR3(cos(hAngle), sin(hAngle), sin(vAngle));
		up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		break;
	case VIEWMODE_FREE:
		viewpos = pos + D3DXVECTOR3(-chasedist*cos(hAngle), -chasedist*sin(hAngle), -viewheight);
		at = viewpos + D3DXVECTOR3(cos(hAngle), sin(hAngle), sin(vAngle));
		up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		break;
	case VIEWMODE_OVERLOOK:
		viewpos = pos + D3DXVECTOR3(0.0f, 0.0f, -overlookheight);
		at = pos;
		up = D3DXVECTOR3(cos(hAngle), sin(hAngle), 0.0f);
		break;
	}
}

void WViewer::KeyControl(int key)
{
	switch (key)
	{
	case VK_UP:
		Walk(key);
		break;
	case 'W':
		Walk(key);
		break;
	case VK_DOWN:
		Walk(key);
		break;
	case 'S':
		Walk(key);
		break;
	case VK_LEFT:
		Walk(key);
		break;
	case 'A':
		Walk(key);
		break;
	case VK_RIGHT:
		Walk(key);
		break;
	case 'D':
		Walk(key);
		break;
	default:
		break;
	}
}
