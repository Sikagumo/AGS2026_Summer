#pragma once
#include <DxLib.h>
#include "../../Common/Quaternion.h"

/// <summary>
/// モデル制御の基本情報
/// 大きさ：VECTOR基準
/// 回転　：Quaternion基準
/// 位置　：VECTOR基準
/// </summary>
class Transform
{

public:

	// モデルのハンドルID
	int modelId;

	// 大きさ
	VECTOR scl;
	// 回転
	VECTOR rot;
	// 位置
	VECTOR pos;
	VECTOR localPos;
	VECTOR prePos;

	// 行列
	MATRIX matScl;
	MATRIX matRot;
	MATRIX matPos;

	// 回転
	Quaternion quaRot;

	// ローカル回転
	Quaternion quaRotLocal;


	// コンストラクタ
	Transform(void);

	// デストラクタ
	~Transform(void) = default;

	// モデル制御の基本情報更新
	void Update(void);
	
	void DrawModelDir(void);

	// 解放
	void Release(void);

	// モデルのハンドルIDを設定
	void SetModel(int modelHId);

	/// @brief 数値初期
	void InitTransform(const VECTOR& _scl,const Quaternion& _rot, const Quaternion& _rotLocal, const VECTOR& _pos);
	void InitTransform(float _scl,const Quaternion& _rot, const Quaternion& _rotLocal, const VECTOR& _pos);
	void InitTransform(float _scl,const Quaternion& _rot, const Quaternion& _rotLocal);
	void InitTransform(void);

	/// @brief 移動処理
	/// @param _movePow 移動量
	void Translate(const VECTOR& _movePow);
	void Translate(const VECTOR& _dir, float _movePow);

	/// @brief 回転処理
	/// @param _axis 回転方向
	/// @param _pow 速度
	void Rotate(const VECTOR& _axis, float _pow);
	void Rotate(const Quaternion& _rot);


	// 前方方向を取得
	VECTOR GetForward(void) const;

	// 後方方向を取得
	VECTOR GetBack(void) const;

	// 右方向を取得
	VECTOR GetRight(void) const;

	// 左方向を取得
	VECTOR GetLeft(void) const;

	// 上方向を取得
	VECTOR GetUp(void) const;

	// 下方向を取得
	VECTOR GetDown(void) const;

	// 対象方向を取得
	VECTOR GetDir(const VECTOR& dir) const;

	void GetScale(float _scale) { scl = VGet(_scale, _scale, _scale); };
};
