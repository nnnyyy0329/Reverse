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