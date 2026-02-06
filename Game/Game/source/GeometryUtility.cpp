#include "GeometryUtility.h"

namespace GeometryUtility
{
	// 対象が前方にあるかどうかを判定する
	bool IsFacing(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& targetPos, float dotThreshold)
	{
		// 対象への方向ベクトルを取得
		VECTOR toTarget = VSub(targetPos, fromPos);
		toTarget.y = 0.0f; // Y軸を無視

		// ベクトルの長さがほぼ0の場合は判定不可
		if(VSize(toTarget) < 0.001f) return false;

		// 正規化
		toTarget = VNorm(toTarget);

		// 前方ベクトル
		VECTOR forwardDir = fromDir;
		forwardDir.y = 0.0f;
		forwardDir = VNorm(forwardDir);

		// 内積を計算して判定
		float dot = VDot(forwardDir, toTarget);
		return dot > dotThreshold;
	}

	// 攻撃が前方からかどうかを判定する
	bool IsAttackFromFront(const VECTOR& targetPos, const VECTOR& targetDir, const VECTOR& attackDir, float dotThreshold)
	{
		// 攻撃方向ベクトルを正規化
		VECTOR normAttackDir = attackDir;
		normAttackDir.y = 0.0f;

		// ベクトルの長さがほぼ0の場合は判定不可
		if(VSize(normAttackDir) < 0.001f) return false;

		// 正規化
		normAttackDir = VNorm(normAttackDir);

		// ターゲットの前方ベクトル
		VECTOR forwardDir = targetDir;
		forwardDir.y = 0.0f;
		forwardDir = VNorm(forwardDir);

		// 攻撃方向を反転させてターゲット方向への攻撃として計算
		VECTOR attackToTarget = VScale(normAttackDir, -1.0f);
		float dot = VDot(forwardDir, attackToTarget);

		// 閾値と比較して前方からの攻撃か判定
		return dot > dotThreshold;
	}

	// 対象の内積の値を取得
	float GetDotToTarget(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& targetPos)
	{
		// 対象への方向ベクトルを取得
		VECTOR toTarget = VSub(targetPos, fromPos);
		toTarget.y = 0.0f; // Y軸を無視

		// ベクトルの長さがほぼ0の場合は0を返す
		if(VSize(toTarget) < 0.001f) return 0.0f;

		// 正規化
		toTarget = VNorm(toTarget);

		// 前方ベクトル
		VECTOR forwardDir = fromDir;
		forwardDir.y = 0.0f;
		forwardDir = VNorm(forwardDir);

		// 内積を計算して返す
		return VDot(forwardDir, toTarget);
	}

	// 攻撃方向からの内積の値を取得
	float GetDotFromAttack(const VECTOR& targetDir, const VECTOR& attackDir)
	{
		// 攻撃方向ベクトルを正規化
		VECTOR normAttackDir = attackDir;
		normAttackDir.y = 0.0f;

		// ベクトルの長さがほぼ0の場合は0を返す
		if(VSize(normAttackDir) < 0.001f) return 0.0f;

		// 正規化
		normAttackDir = VNorm(normAttackDir);

		// ターゲットの前方ベクトル
		VECTOR forwardDir = targetDir;
		forwardDir.y = 0.0f;
		forwardDir = VNorm(forwardDir);

		// 攻撃方向を反転させてターゲット方向への攻撃として計算
		VECTOR attackToTarget = VScale(normAttackDir, -1.0f);

		// 内積を計算して返す
		return VDot(forwardDir, attackToTarget);
	}

	// 2つのベクトル間の角度を取得（ラジアン）
	float GetAngleRad(const VECTOR& vec1, const VECTOR& vec2)
	{
		// 正規化
		VECTOR v1 = VNorm(vec1);
		VECTOR v2 = VNorm(vec2);

		// 内積を計算
		float dot = VDot(v1, v2);

		// 内積の範囲を[-1, 1]にクランプ
		if(dot > 1.0f) dot = 1.0f;
		if(dot < -1.0f) dot = -1.0f;

		// 角度を計算して返す
		return acosf(dot);
	}

	// 2つのベクトル間の角度を取得（度数法）
	float GetAngleDeg(const VECTOR& vec1, const VECTOR& vec2)
	{
		// 角度をラジアンで取得し、度数法に変換して返す
		return GetAngleRad(vec1, vec2) * RADIAN_TO_DEGREE;
	}
}