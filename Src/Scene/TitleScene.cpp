#include "TitleScene.h"
#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/SkyDome/SkyDome.h"


TitleScene::TitleScene(void)
	:
	SceneBase()
	
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	
	// スカイドーム
	skyDome_ = new SkyDome(empty_);
	skyDome_->Init();

	// 定点カメラ
	sceneMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& input = InputManager::GetInstance();
	if (input.IsTrgDown(InputManager::TYPE::SELECT_DECISION,Input::JOYPAD_NO::PAD1)
		|| input.IsTrgDown(InputManager::TYPE::SELECT_DECISION, Input::JOYPAD_NO::PAD2))
	{
		sceneMng_.ChangeScene(SceneManager::SCENE_ID::MATCH);
	}
	
	

	skyDome_->Update();
}

void TitleScene::Draw(void)
{
	skyDome_->Draw();


}

void TitleScene::Release(void)
{
	skyDome_->Release();
	delete skyDome_;

	
}
