#include "GeometryUtility.h"

// 汎用計算関数
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

	// 扇形内にいるかチェック
	bool IsInSector(const VECTOR& targetPos, const SectorData& sectorData)
	{
		// 距離チェック
		VECTOR toTarget = VSub(targetPos, sectorData.center);	// ターゲットへの向き
		toTarget.y = 0.0f;										// Y軸はむしする
		float distance = VSize(toTarget);						// ベクトルのサイズ取得

		if(distance > sectorData.range){ return false; }	// 扇形範囲内に入っていないならスキップ
		if(distance < 0.001f){ return true; }				// ほぼ中心に入っているならtrueにする

		// 角度チェック
		VECTOR normDir = VNorm(sectorData.direction);	// 扇形情報の向きを正規化
		VECTOR normToTarget = VNorm(toTarget);			// ターゲットへの向きを正規化

		float dot = VDot(normDir, normToTarget);			// 扇形の向きベクトルとターゲットへの向きベクトル間の内積取得
		float angle = acosf(fmax(-1.0f, fmin(1.0f, dot)));	// クランプ

		// 扇形内に入っているなら
		return angle <= sectorData.angle / 2.0f;
	}

	// 扇形描画
	void DrawSector(const SectorData& sectorData, int division, int fillColor, int lineColor)
	{
		// 基準角度計算
		VECTOR normDir = VNorm(sectorData.direction);
		float baseAngle = atan2f(normDir.x, normDir.z);

		float halfAngle = sectorData.angle / 2.0f;	// 半分の角度
		float startAngle = baseAngle - halfAngle;	// 開始角度
		float endAngle = baseAngle + halfAngle;		// 終了角度

		// 中心点
		VECTOR center = VAdd(sectorData.center, VGet(0, sectorData.heightOffset, 0));

		// 三角形で扇形を描画
		for(int i = 0; i < division; ++i)
		{
			float angle1 = startAngle + (endAngle - startAngle) * i / division;
			float angle2 = startAngle + (endAngle - startAngle) * (i + 1) / division;

			VECTOR p1 = VAdd(center, VGet(sinf(angle1) * sectorData.range, 0, cosf(angle1) * sectorData.range));
			VECTOR p2 = VAdd(center, VGet(sinf(angle2) * sectorData.range, 0, cosf(angle2) * sectorData.range));

			// 塗りつぶし
			DrawTriangle3D(center, p1, p2, fillColor, TRUE);

			// 線
			DrawLine3D(p1, p2, lineColor);
		}

		// 境界線
		VECTOR startPoint = VAdd(center, VGet(sinf(startAngle) * sectorData.range, 0, cosf(startAngle) * sectorData.range));
		VECTOR endPoint = VAdd(center, VGet(sinf(endAngle) * sectorData.range, 0, cosf(endAngle) * sectorData.range));

		DrawLine3D(center, startPoint, lineColor);
		DrawLine3D(center, endPoint, lineColor);
	}
}