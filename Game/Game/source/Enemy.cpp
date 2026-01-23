#include "Enemy.h"
#include "BulletManager.h"
#include "AttackBase.h"
#include "AttackManager.h"
#include "StateCommon.h"

namespace {
	constexpr auto COLLISION_RADIUS = 30.0f;// 敵の当たり判定半径
	constexpr auto COLLISION_HEIGHT = 100.0f;// 敵の当たり判定高さ
}

Enemy::Enemy() : _vHomePos(VGet(0.0f, 0.0f, 0.0f)), _bCanRemove(false)
{
	// モデル読み込み
	_iHandle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, 0, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	_vDir = VGet(0.0f, 0.0f, 1.0f);// 前方を向いておく

	// 腰位置の設定(マップモデルとの判定用)
	_colSubY = 40.0f;

	// 当たり判定用(カプセル)
	_fCollisionR = COLLISION_RADIUS;
	_fCollisionHeight = COLLISION_HEIGHT;
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_vCollisionTop = VGet(0.0f, _fCollisionHeight, 0.0f);

	_lifeBarHandle = ResourceServer::GetInstance()->GetHandle("LifeBar");
	_lifeBarFrameHandle = ResourceServer::GetInstance()->GetHandle("LifeBarFrame");

	SetCharaType(CHARA_TYPE::ENEMY);// キャラタイプを設定
}

Enemy::~Enemy() 
{
}

bool Enemy::Initialize() 
{
	_vOldPos = _vPos;
	_vHomePos = _vPos;// 初期位置を保存
	_fLife = _enemyParam.fMaxLife;// 体力に最大値をセット
	_damageCnt = 0;
	_bIsExist = true;
	_bCanRemove = false;

	return true;
}

bool Enemy::Terminate() 
{
	return true;
}

bool Enemy::Process() 
{
	_fPlayTime += 0.5f;// 再生時間を進める
	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if (_fPlayTime >= _fTotalTime) {
		_fPlayTime = 0.0f;
	}

	_vOldPos = _vPos;// 前フレームの位置を保存

	_vMove = VGet(0.0f, 0.0f, 0.0f);// 毎フレーム移動量リセット

	// 状態更新
	if (_currentState) {
		auto nextState = _currentState->Update(this);
		if (nextState) {
			ChangeState(nextState);
		}
	}

	_vPos = VAdd(_vPos, _vMove);// 位置更新

	// カプセルに座標を対応させる
	_vCollisionBottom = VAdd(_vPos, VGet(0, _fCollisionR, 0));// 半径分ずらして中心位置に
	_vCollisionTop = VAdd(_vPos, VGet(0, _fCollisionHeight - _fCollisionR, 0));// 高さ分ずらす

	return true;
}

bool Enemy::Render() 
{
	// 再生時間をセットする
	MV1SetAttachAnimTime(_iHandle, _iAttachIndex, _fPlayTime);

	// モデルを描画する
	VECTOR vRot = {0, 0, 0};
	vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);// モデル標準でどちらを向いているかで式が変わる
	MATRIX mRotY = MGetRotY(vRot.y);// Y軸回転行列を作成

	// 位置移動行列を作成
	MATRIX mTrans = MGetTranslate(_vPos);

	// スケール行列を作成
	MATRIX mScale = MGetScale(_vScale);

	// 回転 -> 移動 -> スケール の順に行列を掛け合わせる
	MATRIX m = MGetIdent();// 単位行列を取得
	m = MMult(m, mRotY);
	m = MMult(m, mScale);
	m = MMult(m, mTrans);

	// モデルに行列をセット
	MV1SetMatrix(_iHandle, m);

	// モデルの描画
	MV1DrawModel(_iHandle);

	DrawLifeBar();

	return true;
}

void Enemy::DebugRender() 
{
	// カプセルの当たり判定を描画
	DrawCapsule3D(
		_vCollisionBottom,_vCollisionTop,_fCollisionR,16,
		GetColor(255, 0, 0),GetColor(255, 255, 255),FALSE
	);

	// 移動範囲を描画
	{
		unsigned int color = GetColor(0, 0, 255);// 青
		DrawCircle3D(_vHomePos, _enemyParam.fMoveRadius, color, 16);
		DrawLine3D(_vPos, _vHomePos, color);// 現在位置から初期位置への線
	}

	// 索敵範囲を描画
	{
		auto fVisionRange = _enemyParam.fVisionRange;// 索敵距離
		auto fVisionAngle = _enemyParam.fVisionAngle;// 索敵角度(半分)
		unsigned int color = GetColor(0, 255, 0);// 緑
		int segments = 16;// 扇形の分割数
		DrawFan3D(_vPos, _vDir, fVisionRange, fVisionAngle, color, segments);
	}

	// 追跡中の各範囲の描画
	{
		if (_currentState && _currentState->IsChasing()) {
			// 攻撃可能範囲を描画
			unsigned int attackColor = GetColor(255, 0, 0);// 赤
			DrawCircle3D(_vPos, _enemyParam.fAttackRange, attackColor, 16);

			// 追跡限界範囲を描画
			unsigned int chaseColor = GetColor(255, 255, 0);// 黄
			DrawCircle3D(_vPos, _enemyParam.fChaseLimitRange, chaseColor, 16);
		}
	}

	// デバッグ文字列の描画
	{
		// 敵を箱型と定義して、カメラに映っているか判定
		// 中心から半径と高さ分の範囲
		VECTOR vMin = VAdd(_vPos, VGet(-_fCollisionR, 0.0f, -_fCollisionR));
		VECTOR vMax = VAdd(_vPos, VGet(_fCollisionR, _fCollisionHeight, _fCollisionR));

		// CheckCameraViewClip_Box：カメラの視界外ならTRUEを返す
		if (CheckCameraViewClip_Box(vMin, vMax) == FALSE) {// 映っている時だけ
			VECTOR vScreenPos = ConvWorldPosToScreenPos(VAdd(_vPos, VGet(0, 150.0f, 0)));// 画面座標に変換(少し上に調整)
			int x = static_cast<int>(vScreenPos.x);
			int y = static_cast<int>(vScreenPos.y);
			int size = 16;// 改行用
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 255, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
			DrawFormatString(x, y, GetColor(255, 255, 0), "  life   = %5.2f / %5.2f", _fLife, _enemyParam.fMaxLife); y += size;

			// 状態名
			// ステートから名前を取得
			const char* stateName = "None";
			if (_currentState) {
				stateName = _currentState->GetName();
			}
			DrawFormatString(x, y, GetColor(255, 255, 0), "  state  = %s", stateName); y += size;
		}
	}
}

void Enemy::DrawLifeBar()
{
	if (_lifeBarHandle == -1 || _lifeBarFrameHandle == -1) return;

	// 座標と距離の変換
	// 敵の頭上の座標
	VECTOR vHeadPos = VAdd(_vPos, VGet(0.0f, 100.0f, 0.0f));

	// カメラ座標を取得
	VECTOR vCamPos = GetCameraPosition();

	// カメラから敵までの距離
	auto dist = VSize(VSub(vHeadPos, vCamPos));

	// 画面上の座標に変換
	VECTOR vPos2D = ConvWorldPosToScreenPos(vHeadPos);

	// カメラ範囲外チェック
	if (vPos2D.z < 0.0f || vPos2D.z > 1.0f) return;

	// 遠近感のスケール計算
	// ライフバーを3D空間上でどれくらいの幅にするか
	auto worldBarWidth = 80.0f;

	// カメラの右方向ベクトルを取得
	MATRIX mViewMat = GetCameraViewMatrix();// ビュー行列を取得
	VECTOR vCamRight = VGet(mViewMat.m[0][0], mViewMat.m[1][0], mViewMat.m[2][0]);

	// 中心座標から、右に幅の半分だけずらした3D座標を計算
	VECTOR vRightEdgePos = VAdd(
		vHeadPos,
		VScale(vCamRight, worldBarWidth / 2.0f)
	);

	// ずらした点を画面座標に変換
	VECTOR vRightEdgePos2D = ConvWorldPosToScreenPos(vRightEdgePos);

	// 画面上のピクセル幅を計算
	auto dx = vRightEdgePos2D.x - vPos2D.x;
	auto dy = vRightEdgePos2D.y - vPos2D.y;
	auto halfWidth2D = sqrtf(dx * dx + dy * dy);

	// 最終的な描画幅
	int drawW = static_cast<int>(halfWidth2D * 2.0f);

	// 高さは画像の比率に合わせて計算
	int originW, originH;
	GetGraphSize(_lifeBarHandle, &originW, &originH);
	auto aspectScale = static_cast<float>(drawW) / static_cast<float>(originW);
	int drawH = static_cast<int>(originH * aspectScale);

	// 描画基準位置
	// 中心座標から幅の半分を引いて左上座標を求める
	int screenX = static_cast<int>(vPos2D.x) - (drawW / 2);
	int screenY = static_cast<int>(vPos2D.y) - (drawH / 2);

	// 描画
	// ライフの割合
	auto lifeRatio = _fLife / _enemyParam.fMaxLife;
	if (lifeRatio < 0.0f) lifeRatio = 0.0f;
	if (lifeRatio > 1.0f) lifeRatio = 1.0f;

	// 枠
	DrawExtendGraph(
		screenX, screenY,
		screenX + drawW, screenY + drawH,
		_lifeBarFrameHandle,
		TRUE
	);

	// 中身
	if (lifeRatio > 0.0f) {// ライフがあるとき
		// 画面上で表示する幅
		int currentDrawW = static_cast<int>(drawW * lifeRatio);
		// 元画像から切り取る幅
		int cutW = static_cast<int>(originW * lifeRatio);

		DrawRectExtendGraph(
			screenX, screenY,
			screenX + currentDrawW, screenY + drawH,
			0, 0,
			cutW, originH,
			_lifeBarHandle,
			TRUE
		);
	}
}



void Enemy::DrawCircle3D(VECTOR vCenter, float fRadius, unsigned int color, int segments) 
{
	// 1区画当たりの角度(ラジアン)
	auto step = DX_TWO_PI_F / static_cast<float>(segments);

	// 最初の点を計算
	VECTOR vPrevPos;
	vPrevPos.x = vCenter.x + sinf(0.0f) * fRadius;
	vPrevPos.y = vCenter.y;// 高さは中心と同じ
	vPrevPos.z = vCenter.z + cosf(0.0f) * fRadius;

	// 分割数分ループして点を計算し、線を描画していく
	for (int i = 0; i <= segments; i++) {
		auto angle = step * static_cast<float>(i);// 現在の角度を計算

		// 次の点を計算
		VECTOR vNextPos;
		vNextPos.x = vCenter.x + sinf(angle) * fRadius;
		vNextPos.y = vCenter.y;// 高さは中心と同じ
		vNextPos.z = vCenter.z + cosf(angle) * fRadius;

		// 前の点と次の点を結ぶ線を描画
		DrawLine3D(vPrevPos, vNextPos, color);

		vPrevPos = vNextPos;// 次のループのために点を進める
	}
}

void Enemy::DrawFan3D(VECTOR vCenter, VECTOR vDir, float fRadius, float fHalfAngleDeg, unsigned int color, int segments)
{
	// 角度計算
	auto halfAngleRad = fHalfAngleDeg * DEGREE_TO_RADIAN;// 半分の角度(ラジアン)
	auto currentDirAngle = atan2f(vDir.x, vDir.z);// 向きベクトルから現在の角度を計算
	auto startAngle = currentDirAngle - halfAngleRad;// 扇形の左端
	auto totalAngle = halfAngleRad * 2.0f;// 扇形の全体の角度

	// 左端と右端の境界線を描画
	// 左端の座標計算
	VECTOR vLeftEdge = VAdd(
		vCenter,
		VGet(sinf(startAngle) * fRadius, 0.0f, cosf(startAngle) * fRadius)// 
	);
	// 右端の座標計算(start + total)
	auto endAngle = startAngle + totalAngle;
	VECTOR vRightEdge = VAdd(
		vCenter,
		VGet(sinf(endAngle) * fRadius, 0.0f, cosf(endAngle) * fRadius)
	);

	// 中心から端への線
	DrawLine3D(vCenter, vLeftEdge, color);
	DrawLine3D(vCenter, vRightEdge, color);

	// 扇形の弧(外周)を描画
	VECTOR vPrevPoint = vLeftEdge;// 左端から開始
	for (int i = 1; i <= segments; ++i) {
		// 現在の割合(0.0 ~ 1.0)
		auto ratio = static_cast<float>(i) / static_cast<float>(segments);

		// 現在の角度
		auto angle = startAngle + (totalAngle * ratio);

		// 次の点を計算
		VECTOR vNextPoint = VAdd(
			vCenter,
			VGet(sinf(angle) * fRadius, 0.0f, cosf(angle) * fRadius)
		);

		// 前の点と次の点を結ぶ線を描画
		DrawLine3D(vPrevPoint, vNextPoint, color);

		// 次のループのために点を進める
		vPrevPoint = vNextPoint;
	}

}



void Enemy::ChangeState(std::shared_ptr<EnemyState> newState) 
{
	if (_currentState) {
		_currentState->Exit(this);
	}
	_currentState = newState;
	if (_currentState) {
		_currentState->Enter(this);
	}
}

void Enemy::SetEnemyParam(const EnemyParam& param) 
{
	_enemyParam = param;

	// 視界のcos値を計算して設定
	auto rad = _enemyParam.fVisionAngle * DEGREE_TO_RADIAN;
	_enemyParam.fVisionCos = cosf(rad);
}

bool Enemy::IsTargetVisible(std::shared_ptr<CharaBase> target)
{
	if (!target) return false;

	// 距離チェック
	VECTOR vToTarget = VSub(target->GetPos(), _vPos);
	auto dist = VSize(vToTarget);// ターゲットまでの距離
	if (dist > _enemyParam.fVisionRange) return false;// 索敵距離外

	// 角度チェック
	VECTOR vDirToTarget = VNorm(vToTarget);
	auto dot = VDot(_vDir, vDirToTarget);
	// dotがlimitCos未満なら視界外
	if (dot < _enemyParam.fVisionCos) return false;

	// 障害物チェック

	return true;// 視界内にいる
}

void Enemy::SpawnBullet(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime) {
	auto bulletManager = _bulletManager.lock();// マネージャーが有効か確認
	if (bulletManager) {
		// タイプを設定して、発射リクエストをする
		bulletManager->Shoot(vStartPos, vDir, fRadius, fSpeed, lifeTime, _eCharaType);
	}
}

void Enemy::StartAttack(const EnemyAttackSettings& settings)
{

	if (!_attackCollision) {// 攻撃コリジョンがなければ作成
		_attackCollision = std::make_shared<AttackBase>();
		_attackCollision->Initialize();
	}

	// 位置更新
	UpdateAttackTransform(settings);// 初期座標をセット

	// ステートごとの設定を反映
	_attackCollision->SetCapsuleAttackData(
		settings.vTopOffset,// top
		settings.vBottomOffset,// bottom
		settings.fRadius,// 半径
		settings.fDelay,// 発生遅延
		settings.fDuration,// 持続時間
		settings.fRecovery,// 後隙
		settings.fDamage,// ダメージ
		false// ヒットフラグ
	);

	// 攻撃開始
	_attackCollision->ProcessStartAttack();

	// AttackManagerに登録
	AttackManager::GetInstance()->RegisterAttack(_attackCollision, ATTACK_OWNER_TYPE::ENEMY, settings.ownerId);
}

void Enemy::UpdateAttackTransform(const EnemyAttackSettings& settings) 
{
	if (!_attackCollision) return;

	// 敵の向きベクトルをラジアンに変換
	auto yaw = atan2f(_vDir.x, _vDir.z);

	MATRIX mRotY = MGetRotY(yaw);// Y軸回転行列を作成

	// ローカルオフセットを回転行列で変換
	VECTOR vTopOffsetRot = VTransform(settings.vTopOffset, mRotY);
	VECTOR vBottomOffsetRot = VTransform(settings.vBottomOffset, mRotY);

	// ワールド座標を計算
	VECTOR vTopPos = VAdd(_vPos, vTopOffsetRot);
	VECTOR vBottomPos = VAdd(_vPos, vBottomOffsetRot);

	// 攻撃コリジョンの座標だけ更新
	auto col = _attackCollision->GetAttackCollision();
	_attackCollision->SetCapsuleAttackData(
		vTopPos,// ワールド座標
		vBottomPos,
		col.attackColR,
		col.attackDelay,
		col.attackDuration,
		col.recovery,
		col.damage,
		col.isHit
	);
}

void Enemy::StopAttack() 
{
	if (!_attackCollision) return;
	// 攻撃停止
	_attackCollision->ProcessStopAttack();
}

void Enemy::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType)
{
	// 体力を減らす
	if(_fLife <= 0.0f) return;// 体力が0なら無効
	_fLife -= fDamage;

	// 変身前プレイヤーからの攻撃なら最低1は残す
	if(eType == ATTACK_OWNER_TYPE::SURFACE_PLAYER){
		if(_fLife <= 1.0f){
			_fLife = 1.0f;
			// スタンステートへ遷移
			ChangeState(std::make_shared<Common::Stun>());
			return;
		}
	}

	// 死亡判定
	if (IsDead()) {
		// 死亡時
		_fLife = 0.0f;// マイナス防止

		// 死亡ステートへ遷移
		ChangeState(std::make_shared<Common::Dead>());
		return;
	}

	// 生存時
	// スタンかダウン中ならステートの変更はしない
	if (_currentState) {
		bool isStun = (std::dynamic_pointer_cast<Common::Stun>(_currentState) != nullptr);
		bool isDown = (std::dynamic_pointer_cast<Common::Down>(_currentState) != nullptr);

		if (isStun || isDown) {
			return;
		}
	}

	// 通常ダメージ
	_damageCnt++;// ダメージ回数をカウントアップ
	ChangeState(std::make_shared<Common::Damage>());// ダメージステートへ遷移
}

bool Enemy::IsDead() const
{
	return _fLife <= 0.0f;// 体力が0以下なら死亡
}

std::shared_ptr<EnemyState> Enemy::GetRecoveryState() const
{
	if (_recoveryHandler) {// ハンドラが設定されていれば実行
		return _recoveryHandler(const_cast<Enemy*>(this));
	}

	return nullptr;
}