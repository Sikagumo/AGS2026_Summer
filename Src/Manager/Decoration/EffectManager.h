#pragma once
#include <DxLib.h>
#include <unordered_map>
#include <vector>
#include <mutex>

class EffectManager
{
public:
	/// @brief 個別エフェクトの識別子
	enum class EFFECT
	{
		NONE,
		EFFECT_WAVE,
		EFFECT_LANDING,
		EFFECT_MG,
		EFFECT_BOSS_HIT,
		EFFECT_LASER,

	};
	/// @brief エフェクトのリソースデータ構造体
	struct EFFECT_DATA
	{
		int Data;
		VECTOR pos;
		VECTOR rot;
		VECTOR scl;
		float speed;
	};
	struct PLAYING_EFFECT
	{
		EFFECT effectId;
		int playHandle;
		const void* owner; 
		int tag;           
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

	/// @brief Effectリソースの登録
	/// @param effect 識別用ID
	/// @param _data DxLibエフェクトハンドル
	void Add(const EFFECT _effect, const int _data);


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
	bool IsPlaying(EFFECT _effect);

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
	void Stop(EFFECT _effect);

	/// @brief 全エフェクトの時間更新処理
	void Update(void);

	/// @brief 全エフェクトの画面描画処理
	void Draw(void);

private:

	static EffectManager* instance_; // シングルトンインスタンス

	std::unordered_map<EFFECT, EFFECT_DATA> effect_; // サウンドハンドルの管理マップ
	std::vector<PLAYING_EFFECT> playingList_;


	EffectManager(void) = default;
	~EffectManager(void) = default;

	// コピーコンストラクタ対策
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator=(EffectManager&&) = delete;
};

