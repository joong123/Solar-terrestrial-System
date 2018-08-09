#pragma once

#include "D3Ddefine.h"

#include <d3d9.h>
#include <d3dx9tex.h>
#include <string>

#define COLOR_SKY1			D3DCOLOR_XRGB(160, 180, 255)
#define COLOR_SKY2			D3DCOLOR_XRGB(140, 140, 255)

class Environment{
public:
	LPDIRECT3DDEVICE9 device;
	D3DXMATRIXA16 matworld;
	//D3DXMATRIXA16 matSun;//转为全局变量

	D3DLIGHT9 light0, light0s, light0n;//阳光及侧面辅助阳光
	D3DMATERIAL9 material0;//unused

	ID3DXMesh* skyback;					//天空背景
	ID3DXMesh* sun;

	int basetimezone;					//提供冬至日的时区
	double baselongitude, baselatitude;	//基准经纬度：（0，0）坐标处经纬度
	float skyradius;					//天空半径
	double hangle, vangle;				//阳光角度


	tm lastmidwinter, nextmidwinter;
	time_t tyearstart, tyearend;
	int thisyear;						//年份
	unsigned int ydays;					//一年的天数
	double yearpassed;					//一年已度过的比例
	double daypassed;					//一天已度过的比例
	double declination_general, declination_precise;	//赤纬角
	double yearangle;					//一年中地日连线转过的角度
	D3DXVECTOR3 yearvector;				//一年中地日连线向量
	D3DXVECTOR3 backnormal;				//负赤纬角处0点时刻的法向量
	D3DXVECTOR3 sunproj;
	D3DXVECTOR3 basenormal, basedirectionEW;	//对于longitude指定纬度的地点，0点时刻法线和东西方向向量
	D3DXVECTOR3 groundnormal, grounddirectionEW;//对于longitude指定纬度的地点，实时法线和东西方向向量
	double blockangleEW, blockangleSN;			//地面东西向量角与南北向量角
	double equatorangle;				//赤道平面上，与直射点反面同一经线的点，相对地轴需转过的角度


	float ambientindex;
	float diffuseindex;
	D3DXVECTOR4 sunLight;
	D3DXMATRIX sunTranslation;
public:
	Environment();
	~Environment();

	bool SetDevice(LPDIRECT3DDEVICE9 device);
	bool InitSkyBack();
	void InitMaterialLight(const double &longitude, const double &latitude);
	inline void RefreshLight()//更新光照指数
	{
		float speedregion = 0.06f;	//太阳刚下山后，多少倍π弧度内是diffuse递减为0的区域；ambient递减为stage1end倍的区域。
		float stage1end = 0.10f;	//太阳刚下山后，speedregion倍π弧度后ambientindex递减结果的倍数。
		float stage2end = 0.06f;

		if (vangle <= MYPI)
		{
			diffuseindex = ambientindex = (float)(0.01 + abs(cos(vangle - MYPI / 2) + 1) / 2);
			diffuseindex = ambientindex = 1;
		}
		else if (vangle < (1 + speedregion)*MYPI)//太阳下山后第一级递减时间
		{
			diffuseindex = (float)(((1 + speedregion)*MYPI - vangle) / speedregion / MYPI);
			ambientindex = (float)(stage1end + ((1 + speedregion)*MYPI - vangle)
				/ speedregion / MYPI*(1 - stage1end));
		}
		else if (vangle > (2 - speedregion)*MYPI && vangle < 2 * MYPI)//太阳快升起前最后一级递增时间
		{
			diffuseindex = (float)((vangle - (2 - speedregion)*MYPI) / speedregion / MYPI);
			ambientindex = (float)(stage1end + (vangle - (2 - speedregion)*MYPI)
				/ speedregion / MYPI*(1 - stage1end));
		}
		else//太阳仅提供环境光
		{
			diffuseindex = 0;
			ambientindex = (float)(stage2end + (stage1end - stage2end)*abs(1.5*MYPI - vangle)
					/ (0.5 - speedregion) / MYPI);
		}
	}
	/*
	计算：该年有几天，过了几天，地日连线，赤纬，longitude处0点地面的基准法线和东西方向
	*/
	inline void SunMoveDay(const double &latitude)
	{
		//是哪一年
		thisyear = realdate.tm_year + 1900;

		//一年有几天
		ydays = getDaysinYear(thisyear);

		//一年度过的比例（相对于冬至）。非精确算法定为12/21；精确算法基于【上一年和本年冬至的精确时间】
#define TWOMIDWINTER_YEARPASSED
#ifndef TWOMIDWINTER_YEARPASSED
		if (nowdate.tm_mon == 11 && nowdate.tm_mday >= 21)
		yearpassed = nowdate.tm_mday - 21;
		else//TODO增加判断是否已过闰月
		yearpassed = nowdate.tm_yday + 11;
		//加上一天已度过的比例，（考虑到自转同时也在公转，更新基准）
		yearpassed += daypassed;
#else
		//TODO 加上精准冬至，转移到同一计算，根据年份是否变化更新
		lastmidwinter.tm_mon = 11;
		lastmidwinter.tm_mday = 22;
		lastmidwinter.tm_hour = 0;
		lastmidwinter.tm_min = 27;
		lastmidwinter.tm_sec = 53;
		lastmidwinter.tm_yday = 0;
		lastmidwinter.tm_isdst = 0;
		lastmidwinter.tm_year = thisyear - 1900 - 1;//2015  12-22 12:47:55
		nextmidwinter.tm_mon = 11;
		nextmidwinter.tm_mday = 22;
		nextmidwinter.tm_hour = 6;
		nextmidwinter.tm_min = 22;
		nextmidwinter.tm_sec = 38;
		nextmidwinter.tm_yday = 0;
		nextmidwinter.tm_isdst = 0;
		nextmidwinter.tm_year = thisyear - 1900;//2016  12-21 18:44:07
		if (basedate.tm_mon == nextmidwinter.tm_mon && basedate.tm_mday >= nextmidwinter.tm_mday)
		{
			lastmidwinter.tm_year++;
			nextmidwinter.tm_year++;
		}
		else
		{
			;
		}
		tyearstart = mktime(&lastmidwinter);
		tyearend = mktime(&nextmidwinter);
		//得到所给冬至时区的秒数
		nowtime += SECS_PER_HOUR*(basetimezone - devicetimezone);
		yearpassed = (nowtime - tyearstart) / (double)(tyearend - tyearstart)*ydays;
#endif
		//daypassed = getDaypassed(realdate);//TODO:精确值，每天日落时间有误差

		time_t secReal= mktime(&realdate);
		double baseDaypassed = lastmidwinter.tm_hour / 24.0 + lastmidwinter.tm_min / 24.0 / 60.0
			+ lastmidwinter.tm_sec / 24.0 / 3600.0;//冬至日已度过一天的比例
		daypassed = baseDaypassed + (double)(secReal - tyearstart) / SECS_PER_SOLARDAY
			- (double)SECS_PER_HOUR*(basetimezone - devicetimezone) / SECS_PER_SOLARDAY;//-后面弥补时区差
		daypassed = daypassed - (time_t)daypassed;

		//地日连线角度
		yearangle = yearpassed / ydays * 2 * MYPI;

		//地日连线向量
		yearvector = { (float)cos(yearangle), (float)sin(yearangle), 0.0f };

		//赤纬角
		declination_general = -cos(yearangle)*ECLIPTICOBLIQUITY_R;
		if (cos(yearangle) == 0)
		{
			if (yearangle < MYPI)
				declination_precise = MYPI / 2;
			else
				declination_precise = 3 * MYPI / 2;
			equatorangle = declination_precise;
		}
		else
		{
			equatorangle = -atan(tan(yearangle) / cos(ECLIPTICOBLIQUITY_R));
			double deccos = sin(yearangle)*sin(equatorangle) - cos(ECLIPTICOBLIQUITY_R)*cos(yearangle)*cos(equatorangle);
			declination_precise = acos(deccos);
		}
		if(cos(declination_precise)<0)
			declination_precise -= MYPI;

		//负赤纬角处0点地点的法向量，（等于日地射线，地球背面交于负赤纬处）
		backnormal = yearvector;

#define PRECISEDIRECTIONEW
#ifndef PRECISEDIRECTIONEW

		double directionyearangle;			//用于计算‘负赤纬角处0点地点的东西方向向量’z分量的辅助角度
		directionyearangle = yearpassed - ydays / 4.0;
		if (directionyearangle < 0)
			directionyearangle += ydays;
		//负赤纬角处0点地点的东西方向向量，(等于longitude处东西方向向量)
		double decd = -cos(directionyearangle / ydays * 2 * MYPI)*ECLIPTICOBLIQUITY_R;
		basedirectionEW = { (float)(sin(yearangle)*cos(decd)), (float)-(cos(yearangle)*cos(decd))
			, (float)sin(decd) };
#else
		if (yearangle > MYPI / 2 && yearangle < 3 * MYPI / 2)
			equatorangle = MYPI - equatorangle;
		else if (yearangle < MYPI / 2)
			equatorangle = -equatorangle;//TODO取负号，如果不取负号，旋转方向反
		else
			equatorangle = 2 * MYPI - equatorangle;
		basedirectionEW = AxisRotate(earthaxis, D3DXVECTOR3(0.0f, -1.0f, 0.0f), equatorangle);
#endif // !PRECISEDIRECTIONEW

		//longitude处0点地点的法向量
		basenormal = AxisRotate(basedirectionEW, backnormal, latitude + declination_precise);
	}

	/*
	计算：longitude处地面实时法线和东西方向向量，确定太阳位置的水平、垂直方位角
	*/
	inline void SunMove(const double &latitude)
	{
		SunMoveDay(latitude);//无记忆性重新计算日地位置参数

		//longitude指定纬度的地点，实时法线和东西方向向量
		groundnormal = AxisRotate(earthaxis, basenormal, daypassed * 2 * MYPI);
		grounddirectionEW = AxisRotate(earthaxis, basedirectionEW, daypassed * 2 * MYPI);
		//实时南北方向向量
		D3DXVECTOR3 grounddirectionSN;
		D3DXVec3Cross(&grounddirectionSN, &grounddirectionEW, &groundnormal);
		//太阳向量在地面上投影
		sunproj = -VecProj(yearvector, groundnormal);

		//计算太阳实时方位角
		if (D3DXVec3Length(&sunproj) == 0.0)//阳光直射TODO
		{
			hangle = 0.0f;
			vangle = MYPI / 2;
		}
		else
		{
			//单位化
			D3DXVec3Normalize(&sunproj, &sunproj);
			D3DXVec3Normalize(&grounddirectionSN, &grounddirectionSN);
			D3DXVec3Normalize(&grounddirectionEW, &grounddirectionEW);

			blockangleEW = acos(
				min((sunproj.x*grounddirectionEW.x + sunproj.y*grounddirectionEW.y + sunproj.z*grounddirectionEW.z), 1.0)
			);
			/*blockangleSN = acos(
				D3DXVec3Dot(&sunproj, &grounddirectionSN)
				/ D3DXVec3Length(&sunproj) 
				/ D3DXVec3Length(&grounddirectionSN)
			); */
			blockangleSN = acos(
				min((sunproj.x*grounddirectionSN.x + sunproj.y*grounddirectionSN.y + sunproj.z*grounddirectionSN.z), 1.0)
			);
			//实时水平太阳方位角
			if (blockangleEW > MYPI / 2 && blockangleSN > MYPI / 2)
				hangle = blockangleEW + MYPI;
			else if (blockangleEW > MYPI / 2)
				hangle = MYPI / 2 - blockangleSN;
			else if (blockangleSN > MYPI / 2)
				hangle = blockangleSN + MYPI / 2;
			else
				hangle = blockangleSN + MYPI / 2;
			//实时垂直太阳方位角
			D3DXVECTOR3 antiyearvector = -yearvector;
			vangle = asin(D3DXVec3Dot(&groundnormal, &antiyearvector) / D3DXVec3Length(&groundnormal) / D3DXVec3Length(&yearvector));
			if (vangle < 0)
			{
				if (blockangleEW <= MYPI / 2)
					vangle = MYPI - vangle;
				else
					vangle = 2 * MYPI + vangle;
			}
			else
			{
				if (blockangleEW <= MYPI / 2)
					vangle = MYPI - vangle;
				else
					;
			}
		}

		if (anti)//太阳转向
		{
			//过极点时如果3D世界视角也偏转，取消注释
			/*if (hangle < MYPI)
				hangle += MYPI;
			else
				hangle -= MYPI;*/
		}


		//光源位置更新
		RefreshLight();//更新光照指数
	}
	inline void setPos()
	{
		double sunh = 0; 	//太阳的水平分量
		if (vangle > MYPI && vangle < 2 * MYPI)//太阳在地平线下
		{
			sundown = true;//太阳已下山

			sunh = SUNDIST;//控制太阳发光光源在最远端，不影响太阳模型方位

			if (light0.Type == D3DLIGHT_DIRECTIONAL)
				light0.Direction.z = 0;
			else
				light0.Position.z = 0;//高度控制在地平线
									  /*light0.Diffuse.r = sunlightD.r*abs(cos(vangle));
									  light0.Diffuse.g = sunlightD.g*abs(cos(vangle));
									  light0.Diffuse.b = sunlightD.b*abs(cos(vangle)); */
		}
		else
		{
			sundown = false;

			sunh = SUNDIST*abs(cos(vangle));

			if (light0.Type == D3DLIGHT_DIRECTIONAL)
				light0.Direction.z = (float)(SUNDIST*sin(vangle));
			else
				light0.Position.z = (float)(-SUNDIST*sin(vangle));
			//light0.Diffuse = sunlightD;
		}

		if (light0.Type == D3DLIGHT_DIRECTIONAL)
		{
			light0.Direction.x = -(float)(sunh*cos(hangle));
			light0.Direction.y = -(float)(sunh*sin(hangle));

			light0.Diffuse.r = sunlightD_D.r*diffuseindex;
			light0.Diffuse.g = sunlightD_D.g*diffuseindex;
			light0.Diffuse.b = sunlightD_D.b*diffuseindex;
			light0.Ambient.r = sunlightA.r*ambientindex;
			light0.Ambient.g = sunlightA.g*ambientindex;
			light0.Ambient.b = sunlightA.b*ambientindex;
		}
		else
		{
			light0.Position.x = (float)(sunh*cos(hangle));//太阳光不需要 - viewpos_d.x TODO:其他光源
			light0.Position.y = (float)(sunh*sin(hangle));//太阳光不需要 - viewpos_d.y

			light0.Diffuse.r = sunlightD.r*diffuseindex;
			light0.Diffuse.g = sunlightD.g*diffuseindex;
			light0.Diffuse.b = sunlightD.b*diffuseindex;
			light0.Ambient.r = sunlightA.r*ambientindex;
			light0.Ambient.g = sunlightA.g*ambientindex;
			light0.Ambient.b = sunlightA.b*ambientindex;
		}


		//侧面辅助阳光更新SIDESUNANGLE
		light0s.Diffuse = light0.Diffuse;
		light0n.Diffuse = light0.Diffuse;
		if (light0.Type == D3DLIGHT_DIRECTIONAL)
		{
			light0s.Direction.x = -(float)(sunh*cos(hangle + SIDESUNANGLE));
			light0s.Direction.y = -(float)(sunh*sin(hangle + SIDESUNANGLE));
			light0s.Direction.z = light0.Direction.z + SIDESUN_UP;
			light0n.Direction.x = -(float)(sunh*cos(hangle - SIDESUNANGLE));
			light0n.Direction.y = -(float)(sunh*sin(hangle - SIDESUNANGLE));
			light0n.Direction.z = light0s.Direction.z;
		}
		else
		{
			light0s.Position.x = (float)(sunh*cos(hangle + SIDESUNANGLE));
			light0s.Position.y = (float)(sunh*sin(hangle + SIDESUNANGLE));
			light0s.Position.z = light0.Position.z - SIDESUN_UP;
			light0n.Position.x = (float)(sunh*cos(hangle - SIDESUNANGLE));
			light0n.Position.y = (float)(sunh*sin(hangle - SIDESUNANGLE));
			light0n.Position.z = light0s.Position.z;
		}

		//重设光源 TODO :是否需要
		device->SetLight(0, &light0);
		device->SetLight(1, &light0s);
		device->SetLight(2, &light0n);
		device->LightEnable(0, TRUE);
		device->LightEnable(1, TRUE);
		device->LightEnable(2, TRUE);

		//阴影强度（透明度）
		shadowmtrl.Diffuse.a = (float)(0.05 + MAXSHADOWINTENSITY*abs(sin(vangle)));

		//太阳矩阵
		sunLight = D3DXVECTOR4(
			(float)(SUNDIST*abs(cos(vangle))*cos(hangle))
			, (float)(SUNDIST*abs(cos(vangle))*sin(hangle))
			, (float)(-SUNDIST*sin(vangle))
			, 1);//设置全局太阳光源位置（用于画阴影，与实际发光的光源不同）
		D3DXMatrixIdentity(&sunTranslation);
		D3DXMatrixTranslation(&sunTranslation, sunLight.x, sunLight.y, sunLight.z);
		//阴影矩阵
		D3DXMatrixShadow(
			&matShadow,
			&sunLight,
			&groundPlane);//设置阴影矩阵
		shadowchanged = true;//阴影改变标志
	}
	//设置当前位置真实时间和提供冬至日时区的时间
	inline void SetTime(const double &longitude)
	{
		realtime = nowtime + (time_t)(SECS_PER_SOLARDAY*longitude / 2 / MYPI);
		gmtime_s(&realdate, &realtime);//真实时间

		basetime = nowtime + SECS_PER_HOUR*basetimezone;
		gmtime_s(&basedate, &basetime);//提供冬至日时区的时间
	}
	
	inline bool Render()
	{
		if (!device)
			return false;

		if (!skyback)
			return false;

		//画天空
		//device->SetTransform(D3DTS_WORLD, inblockTranslation);
		device->SetTransform(D3DTS_WORLD, &matworld);//经纬坐标策略，不使用正方区块
		device->SetRenderState(D3DRS_LIGHTING, FALSE);//临时关闭光照
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);//临时关闭雾

		//device->SetMaterial(&material0);
		skyback->DrawSubset(0);

		if (!sun)
			return false;
		//画太阳
		//device->SetTransform(D3DTS_WORLD, &matSun);
		device->SetTransform(D3DTS_WORLD, &sunTranslation);//经纬坐标策略，不使用正方区块
		sun->DrawSubset(0);

		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState(D3DRS_FOGENABLE, TRUE);

		return true;
	}
};