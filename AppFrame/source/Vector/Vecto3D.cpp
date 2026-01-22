#include "Vecto3D.h"

// ベクトル足し算
const Vector3D operator+(const Vector3D& left, const Vector3D& right)
{
	return Vector3D(left) += right;
}

// ベクトル引き算
const Vector3D operator-(const Vector3D& left, const Vector3D& right)
{
	return Vector3D(left) -= right;
}

// ベクトル掛け算
const Vector3D operator*(const Vector3D& left, const Vector3D& right)
{
	return Vector3D(left) *= right;
}

// スカラー倍
const Vector3D operator*(const Vector3D& left, double right)
{
	return Vector3D(left) *= right;
}

// スカラー倍
const Vector3D operator*(double left, const Vector3D& right)
{
	return Vector3D(right) *= left;
}

// ベクトル割り算
const Vector3D operator/(const Vector3D& left, const Vector3D right)
{
	return Vector3D(left) /= right;
}

// スカラー割り算
const Vector3D operator/(const Vector3D& left, double right)
{
	return Vector3D(left) /= right;
}

// スカラー割り算
const Vector3D operator/(double left, const Vector3D& right)
{
	return Vector3D(right) /= left;
}

// +=演算子のオーバーロード
Vector3D	Vector3D::operator+=(const Vector3D& right)
{
	Vector3D tmp;
	tmp._x = _x + right._x;	// ベクトル足し算
	tmp._y = _y + right._y;	// ベクトル足し算
	tmp._z = _z + right._z;	// ベクトル足し算
	return tmp;
}

// -=演算子のオーバーロード
Vector3D	Vector3D::operator-=(const Vector3D& right)
{
	Vector3D tmp;
	tmp._x = _x - right._x;	// ベクトル引き算
	tmp._y = _y - right._y;	// ベクトル引き算
	tmp._z = _z - right._z;	// ベクトル引き算
	return tmp;
}

// *=演算子のオーバーロード
Vector3D	Vector3D::operator*=(const Vector3D& right)
{
	Vector3D tmp;
	tmp._x = _x * right._x;	// ベクトル掛け算
	tmp._y = _y * right._y;	// ベクトル掛け算
	tmp._z = _z * right._z;	// ベクトル掛け算
	return tmp;
}

// スカラー倍の*=演算子のオーバーロード
Vector3D	Vector3D::operator*=(double right)
{
	Vector3D tmp;
	tmp._x = _x * right;	// スカラー倍
	tmp._y = _y * right;	// スカラー倍
	tmp._z = _z * right;	// スカラー倍
	return tmp;
}

// /=演算子のオーバーロード
Vector3D	Vector3D::operator/=(const Vector3D& right)
{
	Vector3D tmp;
	tmp._x = _x / right._x;	// ベクトル割り算
	tmp._y = _y / right._y;	// ベクトル割り算
	tmp._z = _z / right._z;	// ベクトル割り算
	return tmp;
}

// スカラー倍の/=演算子のオーバーロード
Vector3D	Vector3D::operator/=(double right)
{
	Vector3D tmp;
	tmp._x = _x / right;	// スカラー割り算
	tmp._y = _y / right;	// スカラー割り算
	tmp._z = _z / right;	// スカラー割り算
	return tmp;
}

// ==演算子のオーバーロード
bool	Vector3D::operator==(const Vector3D& right) const
{
	// 全ての要素が等しいかチェック
	return (_x == right._x && _y == right._y && _z == right._z);
}

// !=演算子のオーバーロード
bool	Vector3D::operator!=(const Vector3D& right) const
{
	// ==演算子を利用して判定
	return !((*this) == right);
}

// 内積
double	Vector3D::Dot(Vector3D& v1, Vector3D& v2)
{
	// ベクトルを要素ごとに2乗して足し合わせる
	return v1._x * v2._x + v1._y * v2._y + v1._z * v2._z;
}

// 外積
Vector3D	Vector3D::Cross(Vector3D& v1, Vector3D& v2)
{
	// y * z 、 z * x 、 x * y の組み合わせで計算
	// z * x 、 x * y 、 y * z の組み合わせで計算
	return Vector3D
	(
		v1._y * v2._z - v1._z * v2._y,	// x成分
		v1._z * v2._x - v1._x * v2._z,	// y成分
		v1._x * v2._y - v1._y * v2._x	// z成分
	);
}

// 長さ
double	Vector3D::Length(Vector3D& v)
{
	// ベクトルを要素ごとに2乗して足し合わせて平方根を取る
	return sqrt(v._x * v._x + v._y * v._y + v._z * v._z);
}

// 長さの2点間
double	Vector3D::LengthSq(Vector3D& v1, Vector3D& v2)
{
	// 2点間の差を要素ごとに2乗して足し合わせて平方根を取る
	return sqrt
	(
		(v2._x - v1._x) * (v2._x - v1._x) +	// x成分の差の2乗
		(v2._y - v1._y) * (v2._y - v1._y) +	// y成分の差の2乗
		(v2._z - v1._z) * (v2._z - v1._z)	// z成分の差の2乗
	);
}

// 正規化
Vector3D	Vector3D::Normalize(Vector3D& v)
{
	double len = Length(v);									// ベクトルの長さを取得
	return Vector3D(v._x / len, v._y / len, v._z / len);	// 各要素を長さで割る
}