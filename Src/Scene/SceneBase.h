#pragma once

/// @brief シーンの基底クラス
class SceneBase
{
public:

	/// @brief コンストラクタ
	SceneBase(void);

	/// @brief デストラクタ
	virtual ~SceneBase(void) = 0;

	/// @brief リソースロード開始
	virtual void Load(void) = 0;

	/// @brief ロード完了
	virtual void EndLoad(void) = 0;

	/// @brief 初期化
	virtual void Initialize(void) = 0;

	/// @brief 更新処理
	virtual void Update(void) = 0;

	/// @brief 描画処理
	virtual void Draw(void) = 0;

	/// @brief 解放処理
	virtual void Release(void) = 0;

	/// @brief ロード中か
	/// @return ロード中ならtrue
	bool IsLoading(void) const;

	/// @brief デバッグモードフラグの取得 
	/// @return デバッグモードフラグ
	bool GetDebugMode(void) const{return isDebugMode_;}

protected:
	// デバッグモードかどうか
	bool isDebugMode_; 

	/// @brief Gui用の更新処理
	virtual void UpdateGui(void) = 0;

private:
	// ロード中かどうか
	bool isLoading_; 
};