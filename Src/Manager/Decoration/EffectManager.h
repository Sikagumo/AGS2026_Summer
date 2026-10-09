#pragma once
#include <DxLib.h>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "../../Utility/UtilityMath.h"

class EffectManager
{
public:
	/// @brief 個別エフェクトの識別子
	enum class EFFECT
	{
		NONE = -1,

		EFFECT_WAVE,
		EFFECT_LANDING,
		EFFECT_MG,
		EFFECT_BOSS_HIT,
		EFFECT_LASER,
		EFFECT_MISSILE,

		EFFECT_PLAYER_BULLET,
		EFFECT_PLAYER_BLAST,
		EFFECT_PLAYER_RECOVERY,
		EFFECT_PLAYER_POISON,

	};
	/// @brief エフェクトのリソースデータ構造体
	/// @brief エフェクト発生時のパラメータ設定構造体
	struct EFFECT_DATA
	{
		int Data = 0;                             // エフェクトのリソースID・種類識別データ
		VECTOR pos = UtilityMath::VECTOR_ZERO;    // エフェクトの発生位置（3D座標）
		VECTOR rot = UtilityMath::VECTOR_ZERO;    // エフェクトの回転角（オイラー角）
		VECTOR scl = UtilityMath::VECTOR_ONE;     // エフェクトの拡大率（スケール）
		float speed = 0.0f;                       // エフェクトの再生速度
	};

	/// @brief 現在再生中のエフェクト管理用構造体
	struct PLAYING_EFFECT
	{
		EFFECT effectId;                          // 再生中のエフェクト識別ID（列挙型）
		int playHandle;                           // Effekseer等の再生ハンドルID
		const void* owner;                        // エフェクトの所有者アクター（追従対象等のポインタ）
		int tag;                                  // 分類・識別用のタグ用ID
	};


	/// @brief インスタンスを明示的に生成
	/// @param void 
	static void CreateInstance(void);

	/// @brief インスタンス取得
	/// @return EffectControllerインスタンスの参照
	static EffectManager& GetInstance(void);

	/// @brief 初期化処理
	/// @param void 
	void Initialize(void);

	/// @brief 3Dエフェクトの再生
	/// @param _effect 再生するエフェクトのID
	/// @param _pos 再生させる座標
	/// @param _rot 再生させる角度
	/// @param _scl 再生させる大きさ
	/// @param _speed 再生速度
	/// @param _owner 生成者の識別用（this)
	/// @param _tag 複数生成時用のタグ何もなければ１
	void Play(const EFFECT _effect, const VECTOR _pos, const VECTOR _rot, const VECTOR _scl, float _speed, const void* _owner, int _tag = 1);

	/// @brief エフェクトが再生中か確認
	/// @param _effect 対象のエフェクトID
	/// @return 再生中ならtrue
	bool IsPlaying(EFFECT _effect, const void* _owner, int _tag = 1);

	/// @brief インスタンスの破棄
	/// @param void 
	void  DestroyInstance(void);

	/// @brief ポジション更新
	/// @param _owner 生成者の識別用（this)
	/// @param _tag 複数生成時用のタグ何もなければ１
	/// @param _pos 変更座標
	void UpdatePos(const EFFECT _effect, const void* _owner, const VECTOR _pos, int _tag = 1);

	/// @brief 角度更新
	/// @param _owner 生成者の識別用（this)
	/// @param _tag 複数生成時用のタグ何もなければ１
	/// @param _rot 変更角度
	void UpdateRot(const EFFECT _effect, const void* _owner, const VECTOR _rot, int _tag = 1);

	/// @brief サイズ更新
	/// @param _owner 生成者の識別用（this)
	/// @param _tag 複数生成時用のタグ何もなければ１
	/// @param _scl 変更サイズ
	void UpdateScl(const EFFECT _effect, const void* _owner, const VECTOR _scl, int _tag = 1);

	/// @brief 指定したエフェクトIDの再生をすべて強制停止する
	void Stop(EFFECT _effect, const void* _owner, int _tag = 1);

	/// @brief 全エフェクトの時間更新処理
	void Update(void);

	/// @brief 全エフェクトの画面描画処理
	void Draw(void);

private:


	static EffectManager* instance_;					// シングルトンインスタンス
	std::unordered_map<EFFECT, EFFECT_DATA> effect_;	// サウンドハンドルの管理マップ
	std::vector<PLAYING_EFFECT> playingList_;			// 再生中のエフェクトリスト

	/// @brief コンストラクタ
	EffectManager(void) = default;
	/// @brief デストラクタ
	~EffectManager(void) = default;

	// コピーコンストラクタ対策
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator=(EffectManager&&) = delete;
};

