#include "GameScene.h"
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Object/Actor/ActorBase.h"
#include "../Object/Stage/Stage.h"
#include "../Object/SkyDome/SkyDome.h"
#include "../Object/Player/Player.h"
#include "../Object/Enemy/EnemyManager.h"
#include "../Object/Collider/ColliderBase.h"
#include "../Manager/Camera.h"


GameScene::GameScene(void)
	: stage_(nullptr),
	  skyDome_(nullptr),
	  player_(nullptr),
	  enemyMng_(nullptr),
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

	player_ = new Player();
	player_->Init();

	// ステージモデルのコライダーをプレイヤーに登録
	player_->AddHitCollider(stageCollider);


	enemyMng_ = new EnemyManager(player_);
	enemyMng_->Init();

	// 当たり判定登録
	enemyMng_->AddHitCollider(stageCollider);
	enemyMng_->AddHitCollider(player_->GetOwnCollider(static_cast<int>(CharaBase::COLLIDER_TYPE::CAPSULE)));
		
	skyDome_ = new SkyDome(player_->GetTransform());
	skyDome_->Init();

	Camera* camera = sceneMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());
	camera->Init();
	camera->AddHitCollider(stageCollider);
}

void GameScene::Update(void)
{

	// シーン遷移
	InputManager& input = InputManager::GetInstance();
	if (input.KeyIsTrgDown(KEY_INPUT_RETURN))
	{
		sceneMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}


	skyDome_->Update();

	player_->Update();

	enemyMng_->Update();

	Camera* camera = sceneMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());
	camera->Update();
}

void GameScene::Draw(void)
{
	skyDome_->Draw();

	stage_->Draw();

	player_->Draw();

	enemyMng_->Draw();
}

void GameScene::Release(void)
{
	skyDome_->Release();
	delete skyDome_;

	stage_->Release();
	delete stage_;

	enemyMng_->Release();
	delete enemyMng_;

	player_->Release();
	delete player_;
}
