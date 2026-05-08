#include "GameScene.h"
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Object/Actor/ActorBase.h"
#include "../Object/Stage/Stage.h"
#include "../Object/SkyDome/SkyDome.h"

#include "../Object/Collider/ColliderBase.h"
#include "../Camera/Camera.h"


GameScene::GameScene(void)
	: stage_(nullptr),
	  skyDome_(nullptr),
	  SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Initialize(void)
{
	stage_ = new Stage();
	stage_->Initialize();
	const ColliderBase* stageCollider = stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	
	
	skyDome_->Initialize();

	Camera* camera = sceneMng_.GetCamera();
	camera->Initialize();
	camera->AddHitCollider(stageCollider);
}

void GameScene::Update(void)
{

	// ƒV[ƒ“‘JˆÚ
	
	if (inputMng_.IsTrgDown(KEY_INPUT_SPACE) || inputMng_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RB_BOTTOM)
		|| inputMng_.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD2, InputManager::JOYPAD_BTN::RB_BOTTOM))
	{
		sceneMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}


	skyDome_->Update();

	

	Camera* camera = sceneMng_.GetCamera();
	camera->Update();
}

void GameScene::Draw(void)
{
	skyDome_->Draw();

	stage_->Draw();

	DrawFormatString(10, 10, 0xffffff, "Game SCENE");
}

void GameScene::Release(void)
{
	skyDome_->Release();
	delete skyDome_;

	stage_->Release();
	delete stage_;

	
}
