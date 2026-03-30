#pragma once
#include "appframe.h"
#include "ICameraAddon.h"

class CameraBase;

// カメラの引き、寄りを制御するアドオン
class CameraDollyAddon : public ICameraAddon
{
public:
	CameraDollyAddon();
	virtual ~CameraDollyAddon() = default;

	void Process() override;
	void Apply(CameraBase* camera) override;
	bool IsFinished() override { return !_bIsActive; }

	// 前後移動を開始
	// どれくらい移動するか(+で引き、-で寄り)、演出の全体時間、開始、終了のイージング時間
	void StartDolly(float maxDollyDist, float duration, float transitionTime);

	// 強制終了
	void Stop();

	// 滑らかに元の位置に戻る
	void CancelSmoothly();

	// trueなら、水平方向のみにドリーを適用
	void SetUseHorrizontalOnly(bool only) { _bUseHorrizontalOnly = only; }

private:
	float CalculateCurrentOffset();// 現在時間からドリー量を計算

	float _fMaxDollyDist;// 最大移動距離
	float _fCurrentOffset;// 現在のオフセット距離
	float _fDuration;// 演出全体時間
	float _fCurrentTime;// 経過時間
	float _fTransitionTime;// イージング時間
	bool _bIsActive;// 有効フラグ
	bool _bUseHorrizontalOnly;// 水平方向のみ適用するか
};

