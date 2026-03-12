#include "GameObjectBase.h"

GameObjectBase::GameObjectBase()
{
	// 位置関連初期化
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vOldPos = VGet(0.0f, 0.0f, 0.0f);
	_vDir = VGet(0.0f, 0.0f, 0.0f);
	_vScale = VGet(1.0f, 1.0f, 1.0f);
	_vMove = VGet(0.0f, 0.0f, 0.0f);
}

GameObjectBase::~GameObjectBase()
{

}

bool GameObjectBase::Initialize()
{
	return true;
}

bool GameObjectBase::Terminate()
{
	// animManagerのリソース解放
	_animManager.Release();

	return true;
}

bool GameObjectBase::Process()
{
	// アニメーション更新
	_animManager.Update();

	// ターゲットのY軸回転角度を計算して回転に反映
	//CalcTargetY();

	return true;
}

bool GameObjectBase::Render()
{
	// モデルの描画処理
	int modelHandle = _animManager.GetModelHandle();
	if (modelHandle != -1)
	{
		// 回転行列を作成
		VECTOR vRot = { 0, 0, 0 };
		//vRot.y = CalcTargetY();// モデル標準でどちらを向いているかで式が変わる
		vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);// モデル標準でどちらを向いているかで式が変わる
		MATRIX mRotY = MGetRotY(vRot.y);

		// 位置移動行列を作成
		MATRIX mTrans = MGetTranslate(_vPos);

		// スケール行列を作成
		MATRIX mScale = MGetScale(_vScale);

		// 回転 -> スケール -> 移動 の順に行列を掛け合わせる
		MATRIX m = MGetIdent();
		m = MMult(m, mRotY);
		m = MMult(m, mScale);
		m = MMult(m, mTrans);

		// モデルに行列をセット
		MV1SetMatrix(modelHandle, m);

		// animManager経由で描画
		_animManager.Render();
	}

	return true;
}

float GameObjectBase::CalcTargetY()
{
	// 回転処理
	float _rotationY = atan2f(_vDir.x * -1.0f, _vDir.z * -1.0f); // 現在のY軸回転角度

	// 目標角度と現在角度の差分を計算
	float angleDiff = _rotationY;

	// 角度差を -π～π の範囲に正規化
	while(angleDiff > DX_PI_F)
	{
		angleDiff -= DX_TWO_PI_F;
	}
	while(angleDiff < -DX_PI_F)
	{
		angleDiff += DX_TWO_PI_F;
	}

	// 20度をラジアンに変換（度→ラジアン）
	float rotationStep = DEG2RAD(_vMove.y);

	if(fabsf(angleDiff) > rotationStep)
	{
		// 差分が大きい場合は20度ずつ回転
		if(angleDiff > 0.0f)
		{
			_rotationY += rotationStep;
		}
		else
		{
			_rotationY -= rotationStep;
		}
	}
	else
	{
		// 差分が小さい場合は目標角度にぴったり合わせる
		_rotationY = _rotationY;
	}

	// 角度を -π～π の範囲に正規化
	while(_rotationY > DX_PI_F)
	{
		_rotationY -= DX_TWO_PI_F;
	}
	while(_rotationY < -DX_PI_F)
	{
		_rotationY += DX_TWO_PI_F;
	}

	return _rotationY;
}