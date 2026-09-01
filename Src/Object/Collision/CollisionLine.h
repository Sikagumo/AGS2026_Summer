#pragma once

class ColliderBase;
struct CollisionInfo;

/// @brief 線分の衝突判定を管理するクラス
class CollisionLine
{
public:
	/// @brief 線分 vs モデル（地形）の衝突判定
	/// @param _lineCol 判定対象（線分/レイ）
	/// @param _modelCol 判定対象（3Dモデル/メッシュ）
	/// @param _outInfo 衝突時の詳細データを格納する構造体
	/// @return 衝突している場合はtrue
	static bool CheckLineVsModel(const ColliderBase* _lineCol, const ColliderBase* _modelCol,
		CollisionInfo& _outInfo);
};