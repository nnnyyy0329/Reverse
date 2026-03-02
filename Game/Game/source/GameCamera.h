#pragma once
#include "appframe.h"

class PlayerBase;

class GameCamera
{
public:
	GameCamera();
	virtual ~GameCamera();
	
	void Process(InputManager* input, bool isInput);
	void Render();
	void DebugRender();
	void SetUp();// カメラ設定
	void UpdateCamera();// カメラの更新処理
	void UpdateCameraPos();// カメラ位置の更新
	void ControlCamera(float lx, float ly, float rx, float ry, float analogMin);// カメラ操作処理
	
	// ゲッター
	VECTOR GetVPos() const { return _vPos; }// カメラ位置を取得
	VECTOR GetVTarget() const { return _vTarget; }// 注視点を取得
	float GetCameraAngleH() const { return _angleH; }// カメラの水平角度を取得
	bool GetIsCharNearCamera() const { return _bIsCharNearCamera; }// キャラ近接フラグを取得
	void SetVPos(const VECTOR& pos) { _vPos = pos; }// カメラ位置を設定
	void SetIsCharNearCamera(bool b) { _bIsCharNearCamera = b; }// キャラ近接フラグを設定
	
	// ターゲットを設定する関数
	void SetTarget(std::shared_ptr<PlayerBase> target);
	
protected:
	VECTOR _vPos;
	VECTOR _vTarget;// 注視点
	float _nearClip;// 描画する手前の距離
	float _farClip;// 描画する奥の距離
	VECTOR _posOffset;// カメラ位置のオフセット
	VECTOR _targetOffset;// 注視点のオフセット
	
	// ターゲットとなるゲームオブジェクト
	std::shared_ptr<PlayerBase> _targetObject;
	
	float _distance;// 注視点からカメラまでの距離
	float _angleH;// 水平方向の角度(ラジアン)
	float _angleV;// 垂直方向の角度(ラジアン)
	float _angleVMin;// 仰角上限（上方向：-20度）
	float _angleVMax;// 仰角下限（下方向：50度）
	
	// フェードアウト
	bool _bIsCharNearCamera;// キャラとの距離が1m以下のフェードアウトフラグ
	
	// オートリセンター
	float _fNoRightStickTime;// 右スティック未入力の経過フレーム数
	float _fAutoCenterSpeed;// オートリセンター補間速度
	float _fAutoCenterDeadZone;// 補間のデッドゾーン（急旋回時の揺れ防止）
};