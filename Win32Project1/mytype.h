#pragma once

struct floatlonglong {
	long long longint;
	float decimal;

public:
	floatlonglong();
	floatlonglong(long long li, float fd);

	// assignment operators
	floatlonglong& operator += (const floatlonglong& fll2);
	floatlonglong& operator -= (const floatlonglong& fll2);
	floatlonglong& operator *= (float fl);
	floatlonglong& operator /= (float fl);
};

struct DoubleVec3 {
	double x;
	double y;
	double z;
public:
	DoubleVec3();
	DoubleVec3(double a, double b, double c);
	
	// assignment operators
	DoubleVec3& operator += (const DoubleVec3& dv3);
	DoubleVec3& operator -= (const DoubleVec3& dv3);
	DoubleVec3& operator *= (double db);
	DoubleVec3& operator /= (double db);

	// unary operators
	DoubleVec3 operator + () const;
	DoubleVec3 operator - () const;

	// binary operators
	DoubleVec3 operator + (const DoubleVec3& dv3) const;
	DoubleVec3 operator - (const DoubleVec3& dv3) const;
	DoubleVec3 operator * (double db) const;
	DoubleVec3 operator / (double db) const;

	friend DoubleVec3 operator * (double, const struct DoubleVec3&);
};