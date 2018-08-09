#pragma once
//#include <d3d9.h>
#include <d3dx9tex.h>
//#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#define MYPI					3.14159265358979323
#define EQUATORRADIUS			6378140.0
#define POLARRADIUS				6356755.0
#define AVGRADIUS				6371004.0
#define DATELINE				MYPI
#define TIMEZONERADIAN			(MYPI / 12)

//»Æ³à½»½Ç
#define ECLIPTICOBLIQUITY		23.433333
#define ECLIPTICOBLIQUITY_R		0.408988

//¾­Î³¶È
#define LONGITUDE_SHANGHAI		(121.8*MYPI / 180.0)
#define LATITUDE_SHANGHAI		(31.2*MYPI / 180.0)



inline void NormalizeLongitudeLatitude(double &longitude, double &latitude)
{
	//(-180, 180]
	if (longitude > MYPI)
	{
		for (; longitude > MYPI;)
			longitude -= 2 * MYPI;
	}
	else if (longitude <= -MYPI)
	{
		for (; longitude <= -MYPI;)
			longitude += 2 * MYPI;
	}



	//(-180, 180]
	if (latitude > MYPI)
	{
		for (; latitude >= MYPI;)
			latitude -= 2 * MYPI;
	}
	else if (latitude <= -MYPI)
	{
		for (; latitude <= -MYPI;)
			latitude += 2 * MYPI;
	}
	//[-90, 90]
	if (!(latitude <= MYPI / 2 && latitude >= -MYPI / 2))
	{
		if (latitude > MYPI / 2)
		{
			latitude = MYPI - latitude;
		}
		else if (latitude < -MYPI / 2)
		{
			latitude = -MYPI - latitude;
		}
	}
}