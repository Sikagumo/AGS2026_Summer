#pragma once

#include <functional>

/// @brief ローディング画面を制御するクラス
class Loading
{
public:

	/// @brief インスタンスを生成する
	static void CreateInstance(void);

	/// @brief インスタンスを取得する
	/// @return Loading* インスタンスのポインタ
	static Loading* GetInstance(void);

	/// @brief インスタンスを破棄する
	static void DestroyInstance(void);

	/// @brief 初期化する
	void Initialize(void);

	/// @brief 更新する
	void Update(void);

	/// @brief 描画する
	void Draw(void);

	/// @brief 非同期ロードを開始する
	/// @param _loadFunc ロード中に実行する関数オブジェクト
	void StartAsyncLoad(std::function<void()> _loadFunc);

	/// @brief ロード完了処理を行う
	void EndAsyncLoad(void);

	/// @brief ロード中か確認する
	/// @return bool ロード中ならtrue
	bool IsLoading(void) const;

	/// @brief 進捗率を取得する
	/// @return int 進捗率
	int GetProgress(void) const;

	/// @brief 進捗率を設定する
	/// @param _progress 設定する進捗率
	void SetProgress(float _progress);

private:

	// 定数関連
	static constexpr float MAX_PROGRESS = 100.0f; // 進捗率の最大値
	static constexpr float MIN_PROGRESS = 0.0f;   // 進捗率の最小値

	// シングルトン関連
	static Loading* instance_;

	// 状態関連
	bool isLoading_; // ロード中フラグ
	float progress_; // 進捗率

	// 画像関連
	int imageHandle_;

	/// @brief コンストラクタ
	Loading(void);

	/// @brief デストラクタ
	~Loading(void);

	/// @brief コピーコンストラクタを禁止する
	Loading(const Loading&) = delete;

	/// @brief 代入演算子を禁止する
	Loading& operator=(const Loading&) = delete;

	/// @brief ムーブコンストラクタを禁止する
	Loading(Loading&&) = delete;

	/// @brief ムーブ代入演算子を禁止する
	Loading& operator=(Loading&&) = delete;
};