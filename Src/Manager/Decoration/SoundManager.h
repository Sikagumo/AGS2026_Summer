#pragma once
#include <DxLib.h>
#include <unordered_map>
#include <mutex>

/// @brief サウンド管理クラス（シングルトン）
class SoundManager
{
public:
	/// @brief サウンドの分類
	enum class TYPE
	{
		NONE,
		BGM,
		SE
	};

	/// @brief 個別サウンドの識別子
	enum class SOUND
	{
		NONE = -1,
		//BGM
		BGM_TITLE,
		BGM_GAME,

		//SE
		SE_DAMAGE_PLAYER, // プレイヤー被ダメージ処理
		SE_BOSS_LANDING,//ボスの着地
		SE_MG_FIRE,
		SE_ROAD,
	};

	/// @brief サウンドのリソースデータ構造体
	struct SOUND_DATA
	{
		int data;		// DxLibのサウンドハンドル
		TYPE type;		// BGMかSEか
		int playMode;	// 再生モード（ループか単発か）
		bool is3D;		//3Dの音声ハンドルか
		VECTOR playPos;	//音の発生源の位置
		float radius;	//音の届く距離
	};

	/// @brief インスタンスを明示的に生成
	/// @param void 
	static void CreateInstance(void);

	/// @brief インスタンス取得
	/// @return SoundManagerインスタンスの参照
	static SoundManager& GetInstance(void);

	/// @brief 初期化処理
	/// @param void 
	void Initialize(void);

	/// @brief サウンドリソースの登録
	/// @param type サウンドの種類
	/// @param sound 識別用ID
	/// @param _data DxLibサウンドハンドル
	void Add(const TYPE type, const SOUND sound, const int _data);

	/// @brief サウンドの再生
	/// @param _sound 再生するサウンドID
	void Play(const SOUND _sound);

	///	@brief 3Dサウンドの再生
	/// @param _sound 再生するサウンドID
	/// @param _playPos 音の発生源
	/// @param _listenPos 音の聞き取り手
	/// @param　_radius 音の聞こえる範囲
	void Play3D(const SOUND _sound, const VECTOR _playPos, const VECTOR _listenPos, const float _radius);

	/// @brief 再生中の3Dサウンドの発生源位置を更新する
	/// @param _sound 対象のサウンドID
	/// @param _newPos 新しい発生源の座標
	void Set3DPosition(const SOUND _sound, const VECTOR _newPos);

	///	@brief　3Dサウンドの更新による音の変化
	/// @param _listenPos 音の聞き取り手
	void Update3D(const VECTOR _listenPos);

	/// @brief 指定したサウンドの停止
	/// @param _sound 停止するサウンドID
	void Stop(const SOUND _sound);

	/// @brief 全てのBGMを停止
	/// @param void 
	void StopAllBGM(void);

	/// @brief 全リソースの解放
	/// @param void 
	void Release(void);

	/// @brief 音量の個別調整
	/// @param _sound 対象のサウンドID
	/// @param _percent 音量（0-100）
	void AdjustVolume(const SOUND _sound, const int _percent);

	/// @brief サウンドが再生中か確認
	/// @param sound 対象のサウンドID
	/// @return 再生中ならtrue
	bool IsPlaying(SOUND sound);

	/// @brief インスタンスの破棄
	/// @param void 
	void  DestroyInstance(void);

	/// @brief BGMのマスターボリューム設定
	/// @param volume 音量（0-100）
	void SetMasterVolumeBGM(int volume);

	/// @brief SEのマスターボリューム設定
	/// @param volume 音量（0-100）
	void SetMasterVolumeSE(int volume);

	/// @brief BGMのマスターボリューム取得
	/// @return 現在の音量設定（0-100）
	int GetMasterVolumeBGM(void) const { return masterVolumeBGM_; }

	/// @brief SEのマスターボリューム取得
	/// @return 現在の音量設定（0-100）
	int GetMasterVolumeSE(void) const { return masterVolumeSE_; }

	/// @brief マスターボリュームを現在読み込まれている全てのサウンドに適用
	/// @param void 
	void ApplyMasterVolumes(void);

private:
	// 定数関連
	static constexpr int MAX_PERCENT = 100;       // パーセントの最大値
	static constexpr int MIN_PERCENT = 0;         // パーセントの最小値
	static constexpr int DX_MAX_VOLUME = 255;     // DxLibの最大音量設定値
	static constexpr int DEFAULT_BGM_VOLUME = 70; // BGMの初期音量
	static constexpr int DEFAULT_SE_VOLUME = 80;  // SEの初期音量

	static SoundManager* instance_; // シングルトンインスタンス
	static std::recursive_mutex g_soundMutex; // スレッドセーフ用ミューテックス

	// メンバ変数
	std::unordered_map<SOUND, SOUND_DATA> sounds_; // サウンドハンドルの管理マップ
	int masterVolumeBGM_;                          // BGMの主音量（0-100）
	int masterVolumeSE_;                           // SEの主音量（0-100）

	/// @brief コンストラクタ
	/// @param void 
	SoundManager(void) = default;

	/// @brief デストラクタ
	/// @param void 
	~SoundManager(void) = default;

	// コピーコンストラクタ対策
	SoundManager(const SoundManager&)			 = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&)			= delete;
	SoundManager& operator=(SoundManager&&) = delete;

	/// @brief 音量パーセントをDxLib用の値(0-255)に変換するヘルパー
	/// @param percent 変換前のパーセント値
	/// @return DxLib形式の音量値
	int ToDxVolume(int percent) const { return percent * DX_MAX_VOLUME / MAX_PERCENT; }
};