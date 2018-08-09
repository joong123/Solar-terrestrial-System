#include"stdafx.h"
#include"mytype.h"

floatlonglong::floatlonglong()
{
	longint = 0;
	decimal = 0;
}

floatlonglong::floatlonglong(long long li, float fd)
{
	longint = li;
	decimal = fd;
}

floatlonglong & floatlonglong::operator+=(const floatlonglong & fll2)
{
	longint += fll2.longint;
	decimal += fll2.decimal;
	if (decimal >= 1.0f)
	{
		longint++;
		decimal -= 1.0f;
	}
	else if (decimal <= -1.0f)
	{
		longint--;
		decimal += 1.0f;
	}
	return *this;
}

floatlonglong & floatlonglong::operator-=(const floatlonglong & fll2)
{
	longint -= fll2.longint;
	decimal -= fll2.decimal;
	if (decimal >= 1.0f)
	{
		longint++;
		decimal -= 1.0f;
	}
	else if (decimal <= -1.0f)
	{
		longint--;
		decimal += 1.0f;
	}
	return *this;
}

DoubleVec3::DoubleVec3()
{
	x = y = z = 0.0;
}

DoubleVec3::DoubleVec3(double a, double b, double c)
{
	x = a;
	y = b;
	z = c;
}

DoubleVec3 & DoubleVec3::operator+=(const DoubleVec3 & dv3)
{
	x += dv3.x;
	y += dv3.y;
	z += dv3.z;

	return *this;
}

DoubleVec3 & DoubleVec3::operator-=(const DoubleVec3 & dv3)
{
	x -= dv3.x;
	y -= dv3.y;
	z -= dv3.z;

	return *this;
}

DoubleVec3 & DoubleVec3::operator*=(double db)
{
	x *= db;
	y *= db;
	z *= db;

	return *this;
}

DoubleVec3 & DoubleVec3::operator/=(double db)
{
	double a = 1.0 / db;
	x *= a;
	y *= a;
	z *= a;

	return *this;
}

DoubleVec3 DoubleVec3::operator+() const
{
	return *this;
}

DoubleVec3 DoubleVec3::operator-() const
{
	return DoubleVec3(-x, -y, -z);
}

DoubleVec3 DoubleVec3::operator+(const DoubleVec3 & dv3) const
{
	return DoubleVec3(x + dv3.x, y + dv3.y, z + dv3.z);
}

DoubleVec3 DoubleVec3::operator-(const DoubleVec3 & dv3) const
{
	return DoubleVec3(x - dv3.x, y - dv3.y, z - dv3.z);
}

DoubleVec3 DoubleVec3::operator*(double db) const
{
	return DoubleVec3(x*db, y*db, z*db);
}

DoubleVec3 DoubleVec3::operator/(double db) const
{
	double a = 1.0 / db;
	return DoubleVec3(x*a, y*a, z*a);
}

DoubleVec3 operator*(double db, const DoubleVec3 &dv3)
{
	return DoubleVec3(dv3.x*db, dv3.y*db, dv3.z*db);
}
