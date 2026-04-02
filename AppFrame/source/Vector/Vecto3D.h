/*****************************************
* file   Vector3D.h
* brief  3次元ベクトルクラス
* author 成田 悠真
* date   2026/1/22
******************************************/

#pragma once
#include <string>

/// @brief 3次元ベクトルクラス
class Vector3D
{
public:

	/// @brief デフォルトコンストラクタ。全ての成分を0に初期化
	Vector3D() { _x = _y = _z = 0; }

	// @brief コンストラクタ。x,y,z成分を指定して初期化
	Vector3D(float x, float y, float z) { _x = x; _y = y; _z = z; }

	/// @brief コピーコンストラクタ
	~Vector3D() {}

	/// @brief ベクトル同士の加算を行う+=演算子のオーバーロード
	///
	/// @return 加算後のベクトルを返す
	Vector3D	operator+=(const Vector3D& right);	
	
	/// @brief ベクトル同士の減算を行う-=演算子のオーバーロード
	///
	/// @return 減算後のベクトルを返す
	Vector3D	operator-=(const Vector3D& right);			
	
	/// @brief ベクトル同士の要素ごとの乗算を行う*=演算子のオーバーロード
	///
	/// @return 乗算後のベクトルを返す
	Vector3D	operator*=(const Vector3D& right);		

	/// @brief ベクトル同士の要素ごとのスカラー倍の*=演算子のオーバーロード
	///
	/// @return 乗算後のベクトルを返す
	Vector3D	operator*=(double right);				

	/// @brief ベクトル同士の要素ごとの除算を行う/=演算子のオーバーロード
	///
	/// @return 除算後のベクトルを返す
	Vector3D	operator/=(const Vector3D& right);		

	/// @brief ベクトル同士の要素ごとのスカラー倍の/=演算子のオーバーロード
	///
	/// @return 除算後のベクトルを返す
	Vector3D	operator/=(double right);				

	/// @brief ベクトル同士を比較する==演算子のオーバーロード
	///
	/// @return ベクトルが等しい場合はtrue、そうでない場合はfalseを返す
	bool 		operator==(const Vector3D& right) const;

	/// @brief ベクトル同士を比較する!=演算子のオーバーロード
	///
	/// @return ベクトルが等しくない場合はtrue、そうでない場合はfalseを返す
	bool 		operator!=(const Vector3D& right) const;	

	/// @brief ベクトル同士の内積を計算する静的メソッド
	///
	/// @return 内積の値を返す
	static double Dot(Vector3D& v1, Vector3D& v2);		
	
	/// @brief ベクトル同士の外積を計算する静的メソッド
	///
	/// @return 外積のベクトルを返す
	static Vector3D Cross(Vector3D& v1, Vector3D& v2);

	/// @brief ベクトルの長さを計算する静的メソッド
	///
	/// @return ベクトルの長さを返す
	static double Length(Vector3D& v);					
	
	/// @brief ベクトルの長さの二乗を計算する静的メソッド
	///
	/// @return ベクトルの長さの二乗を返す
	static double LengthSq(Vector3D& v, Vector3D& v2);	
	
	/// @brief ベクトルを正規化する静的メソッド
	///
	/// @return 正規化されたベクトルを返す
	static Vector3D Normalize(Vector3D& v);				


	/* メンバ変数 */

	// x,y,z成分
	float _x, _y, _z;	
};

/* 非メンバ関数 */

const Vector3D operator+(const Vector3D& left, const Vector3D& right);
const Vector3D operator-(const Vector3D& left, const Vector3D& right);
const Vector3D operator*(const Vector3D& left, const Vector3D& right);
const Vector3D operator*(const Vector3D& left, double right);
const Vector3D operator*(double left, const Vector3D& right);
const Vector3D operator/(const Vector3D& left, const Vector3D right);
const Vector3D operator/(const Vector3D& left, double right);
const Vector3D operator/(double left, const Vector3D& right);



