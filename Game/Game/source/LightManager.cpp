#include "LightManager.h"

LightManager::LightManager()
	: _eCurrentLightType(LIGHT_TYPE::DIRECTIONAL)
	, _bIsLightEnabled(true)
	, _vDirectionalDir(VGet(-1.0f, -1.0f, -1.0f))
	, _vPointPos(VGet(0.0f, 0.0f, 0.0f))
	, _fPointRange(1000.0f)
	, _fPointAtten0(0.0f)
	, _fPointAtten1(0.005f)
	, _fPointAtten2(0.0f)
	, _vSpotPos(VGet(0.0f, 0.0f, 0.0f))
	, _vSpotDir(VGet(0.0f, -1.0f, 0.0f))
	, _fSpotOutAngle(DX_PI_F / 4.0f)
	, _fSpotInAngle(DX_PI_F / 6.0f)
	, _fSpotRange(1000.0f)
	, _fSpotAtten0(0.0f)
	, _fSpotAtten1(0.005f)
	, _fSpotAtten2(0.0f)
{
	// ライトの色を初期化
	_ambientColor = GetColorF(0.5f, 0.5f, 0.5f, 0.f);
	_diffuseColor = GetColorF(1.0f, 1.0f, 1.0f, 0.0f);
	_specularColor = GetColorF(1.0f, 1.0f, 1.0f, 0.0f);
}

LightManager::~LightManager()
{
}

void LightManager::Initialize()
{
	// ライティングを有効化
	SetUseLighting(TRUE);
	_bIsLightEnabled = true;

	// デフォルトで平行光源を設定
	SetLightType(LIGHT_TYPE::DIRECTIONAL);

	// アンビエントライトの設定
	SetGlobalAmbientLight(_ambientColor);
}

int LightManager::CreateLightHandle()
{
	// 平行光源ハンドルを作成
	int lightHandle = CreateDirLightHandle(VGet(0.0f, -1.0f, 0.0f));

	return lightHandle;
}

int LightManager::CreatePointLight(VECTOR vPos, float fRange, COLOR_F color)
{
	// ポイントライトハンドルを作成
	int lightHandle = CreatePointLightHandle(vPos, fRange, 0.0f, 0.005f, 0.0f);

	if (lightHandle == -1) { return -1; }// 作成失敗

	// ライトの色を設定
	SetLightDifColor(color);

	// ライトを有効化
	SetLightEnable(true);

	return lightHandle;
}

int LightManager::CreateSpotLight(VECTOR vPos, VECTOR vDir, float fOutAngle, float fInAngle, float fRange, COLOR_F color)
{
	// スポットライトハンドルを作成
	int lightHandle = CreateSpotLightHandle(vPos, vDir, fOutAngle, fInAngle, fRange, 0.0f, 0.005f, 0.0f);

	if (lightHandle == -1) { return -1; }// 作成失敗

	// ライトの色を設定
	SetLightDifColor(color);

	// ライトを有効化
	SetLightEnable(true);

	return lightHandle;
}

void LightManager::SetLightType(LIGHT_TYPE eType)
{
	// 現在のライトタイプを保存
	_eCurrentLightType = eType;

	// ライトタイプに応じて設定
	switch (eType)
	{
		case LIGHT_TYPE::DIRECTIONAL:
			// 平行光源
			ChangeLightTypeDir(_vDirectionalDir);
			break;
		case LIGHT_TYPE::POINT:
			// ポイントライト
			ChangeLightTypePoint(_vPointPos, _fPointRange, _fPointAtten0, _fPointAtten1, _fPointAtten2);
			break;
		case LIGHT_TYPE::SPOT:
			// スポットライト
			ChangeLightTypeSpot(_vSpotPos, _vSpotDir, _fSpotOutAngle, _fSpotInAngle, _fSpotRange, _fSpotAtten0, _fSpotAtten1, _fSpotAtten2);
			break;
	}
}

void LightManager::SetLightEnable(bool bEnable)
{
	// ライティングの有効、無効を設定
	SetUseLighting(bEnable ? true : false);

	// 有効フラグを更新
	_bIsLightEnabled = bEnable;
}

void LightManager::SetDirectionalLightDir(VECTOR vDir)
{
	// 方向ベクトルを保存
	_vDirectionalDir = vDir;

	// 平行光源が選択されている場合のみ適用
	if (_eCurrentLightType == LIGHT_TYPE::DIRECTIONAL)
	{
		ChangeLightTypeDir(_vDirectionalDir);
	}
}

void LightManager::SetAmbientLight(COLOR_F color)
{
	// アンビエントライトの色を保存
	_ambientColor = color;

	// アンビエントライトの色を設定
	SetGlobalAmbientLight(_ambientColor);
}

void LightManager::UpdateSpotLight()
{
	// 保存されているパラメータを使用してスポットライトを設定
	ChangeLightTypeSpot(
		_vPointPos,
		_vSpotDir,
		_fSpotOutAngle,
		_fSpotInAngle,
		_fPointRange,
		_fSpotAtten0,
		_fSpotAtten1,
		_fPointAtten2
	);
}