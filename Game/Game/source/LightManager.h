#pragma once
#include "appframe.h"

class LightManager
{
public:

	// ライトの種類
	enum class LIGHT_TYPE
	{
		DIRECTIONAL,// 平行光源
		POINT,// ポイントライト
		SPOT// スポットライト
	};

	LightManager();
	virtual ~LightManager();

	// ライトの初期設定を行う
	void Initialize();

	// ライトハンドルを生成する
	// ハンドルを保存、初期パラメータ設定
	// 戻り値:ライトハンドル
	int CreateLightHandle();

	// 指定座標にポイントライトを生成
	// ライトの位置、影響範囲、ライトの色
	// 戻り値:ライトハンドル
	int CreatePointLight(VECTOR vPos, float fRange, COLOR_F color);

	// 指定座標にスポットライトを生成
	// ライトの位置、方向、外側の角度(ラジアン)、内側の角度(ラジアン)、影響範囲、ライトの色
	// 戻り値:ライトハンドル
	int CreateSpotLight(VECTOR vPos, VECTOR vDir, float fOutAngle, float fInAngle, float fRange, COLOR_F color);

	// デフォルトライトの種類を設定
	// 現在のライトタイプを保存、ライトタイプを変更
	// 設定するライトの種類
	void SetLightType(LIGHT_TYPE eType);

	// デフォルトライトの有効、無効を設定
	// 有効フラグ
	void SetLightEnable(bool bEnable);

	// 平行光源の方向を設定
	// ライトの方向ベクトル
	void SetDirectionalLightDir(VECTOR vDir);

	// アンビエントライトの色を設定
	// 環境光の色を保存、SetGlobalAmbientLight
	// グローバルアンビエントライトカラー
	void SetAmbientLight(COLOR_F color);

	// 現在のライトタイプを取得
	LIGHT_TYPE GetLightType() { return _eCurrentLightType; }

	// ライトの有効状態を取得
	bool IsLightEnable() { return _bIsLightEnabled; }

private:

	// スポットライトを内部パラメータで更新
	void UpdateSpotLight();

	// デフォルトライトの状態
	LIGHT_TYPE _eCurrentLightType;// 現在のライトタイプ
	bool _bIsLightEnabled;// ライト有効フラグ

	// 平行光源パラメータ
	VECTOR _vDirectionalDir;// 方向

	// ポイントライトパラメータ
	VECTOR _vPointPos;// 位置
	float _fPointRange;// 範囲
	float _fPointAtten0;// 距離減衰パラメータ
	float _fPointAtten1;
	float _fPointAtten2;

	// スポットライトパラメータ
	VECTOR _vSpotPos;// 位置
	VECTOR _vSpotDir;// 方向
	float _fSpotOutAngle;// 外側の角度
	float _fSpotInAngle;// 内側の角度
	float _fSpotRange;// 範囲
	float _fSpotAtten0;// 距離減衰パラメータ
	float _fSpotAtten1;
	float _fSpotAtten2;

	// ライトの色
	COLOR_F _ambientColor;// アンビエントライト色
	COLOR_F _diffuseColor;// ディフューズ色
	COLOR_F _specularColor;// スペキュラ色
};

