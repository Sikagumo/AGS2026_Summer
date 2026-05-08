#include"SceneBase.h"

//デストラクタ
SceneBase::~SceneBase(void)
{
	isLoading_ = false;
}

//初期化
void SceneBase::Initialize(void)
{
	isLoading_ = false;
}

//更新処理
void SceneBase::Update(void)
{
}

//描画処理
void SceneBase::Draw(void)
{
}

//解放処理
void SceneBase::Release(void)
{
}

// ロード開始
void SceneBase::Load(void)
{
	isLoading_ = true;
}

// ロード完了
void SceneBase::EndLoad(void)
{
	isLoading_ = false;
}

// ロード中か
bool SceneBase::IsLoading(void) const
{
	return isLoading_;
}
