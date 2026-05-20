#pragma once

class SceneBase
{
public:

	//コンストラクタ
	SceneBase(void) = default;

	//デストラクタ
	virtual ~SceneBase(void) = 0;

	//初期化
	virtual void Initialize(void) = 0;

	//更新処理
	virtual void Update(void) = 0;

	virtual void UpdateCollision(void) {};

	//描画処理
	virtual void Draw(void) = 0;

	//解放処理
	virtual void Release(void) = 0;

	//リソースロード開始
	virtual void Load(void) = 0;

	//ロード完了
	virtual void EndLoad(void) = 0;

	//ロード中か
	bool IsLoading(void) const;

private:
	//ロード中かどうか
	bool isLoading_;
};
