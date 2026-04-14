#include "GameScene.h"
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Object/Actor/ActorBase.h"
#include "../Object/Stage/Stage.h"
#include "../Object/SkyDome/SkyDome.h"

#include "../Object/Collider/ColliderBase.h"
#include "../Manager/Camera.h"


GameScene::GameScene(void)
	: stage_(nullptr),
	  skyDome_(nullptr),
	  SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	stage_ = new Stage();
	stage_->Init();
	const ColliderBase* stageCollider = stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	
	skyDome_ = new SkyDome(player_->GetTransform());
	skyDome_->Init();

	Camera* camera = sceneMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());
	camera->Init();
	camera->AddHitCollider(stageCollider);
}

void GameScene::Update(void)
{

	// ƒV[ƒ“‘JˆÚ
	InputManager& input = InputManager::GetInstance();
	if (input.IsTrgDown(InputManager::TYPE::SELECT_DECISION, Input::JOYPAD_NO::PAD1)
		|| input.IsTrgDown(InputManager::TYPE::SELECT_DECISION, Input::JOYPAD_NO::PAD2))
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

	
}

void GameScene::Release(void)
{
	skyDome_->Release();
	delete skyDome_;

	stage_->Release();
	delete stage_;

	
}
