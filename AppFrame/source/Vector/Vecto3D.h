#pragma once
#include <string>

// 3次元ベクトルクラス
class Vector3D
{
public:
	Vector3D() { _x = _y = _z = 0; }
	Vector3D(float x, float y, float z) { _x = x; _y = y; _z = z; }
	~Vector3D() {}

	Vector3D	operator+=(const Vector3D& right);			// +=演算子のオーバーロード
	Vector3D	operator-=(const Vector3D& right);			// -=演算子のオーバーロード
	Vector3D	operator*=(const Vector3D& right);			// *=演算子のオーバーロード
	Vector3D	operator*=(double right);					// スカラー倍の*=演算子のオーバーロード
	Vector3D	operator/=(const Vector3D& right);			// /=演算子のオーバーロード
	Vector3D	operator/=(double right);					// スカラー倍の/=演算子のオーバーロード
	bool 		operator==(const Vector3D& right) const;	// ==演算子のオーバーロード
	bool 		operator!=(const Vector3D& right) const;	// !=演算子のオーバーロード

	static double Dot(Vector3D& v1, Vector3D& v2);		// 内積
	static Vector3D Cross(Vector3D& v1, Vector3D& v2);	// 外積
	static double Length(Vector3D& v);					// 長さ
	static double LengthSq(Vector3D& v, Vector3D& v2);	// 長さの2点間
	static Vector3D Normalize(Vector3D& v);				// 正規化

	float _x, _y, _z;	// x,y,z成分
};

const Vector3D operator+(const Vector3D& left, const Vector3D& right);
const Vector3D operator-(const Vector3D& left, const Vector3D& right);
const Vector3D operator*(const Vector3D& left, const Vector3D& right);
const Vector3D operator*(const Vector3D& left, double right);
const Vector3D operator*(double left, const Vector3D& right);
const Vector3D operator/(const Vector3D& left, const Vector3D right);
const Vector3D operator/(const Vector3D& left, double right);
const Vector3D operator/(double left, const Vector3D& right);



