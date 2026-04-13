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
	SceneBase(),
	imgTitle_(-1),
	imgPushSpace_(-1)
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	// タイトル画像
	imgTitle_ = resMng_.GetHandleId(ResourceManager::SRC::IMG_TITLE);
	imgPushSpace_ = resMng_.GetHandleId(ResourceManager::SRC::IMG_PUSH_SPACE);
	imgShadow_ = resMng_.GetHandleId(ResourceManager::SRC::IMG_SHADOW); 

	bigPlant_.SetModel(resMng_.GetHandleId(ResourceManager::SRC::MODEL_PLANET_PITFALL));
	bigPlant_.InitTransform();
	bigPlant_.Update();

	spherePlant_.SetModel(resMng_.GetHandleId(ResourceManager::SRC::MODEL_PLANET_SPHERE));
	spherePlant_.InitTransform(SPHERE_SCALE,
							   Quaternion::Euler(SPHERE_ROT.x, SPHERE_ROT.y, SPHERE_ROT.z),
							   Quaternion::Identity(),
							   SPHERE_POS);
	spherePlant_.Update();

	// プレイヤー
	player_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::MODEL_PLAYER));
	player_.InitTransform(PLAYER_SCALE,
						  Quaternion::Euler(PLAYER_ROT.x, PLAYER_ROT.y, PLAYER_ROT.z),
						  Quaternion::Euler(PLAYER_ROT_LOCAL.x, PLAYER_ROT_LOCAL.y, PLAYER_ROT_LOCAL.z),
						  PLAYER_POS);
	player_.Update();
	playerAnim_ = new AnimationController(player_.modelId);
	playerAnim_->AddExternal(0, 30.0f, resMng_.GetHandleId(ResourceManager::SRC::ANIM_PLAYER_RUN));
	playerAnim_->Play(0);

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
	if (input.KeyIsTrgDown(KEY_INPUT_SPACE) ||
		input.PadIsBtnTrgDown(InputManager::PAD_NO::PAD1, InputManager::PAD_BTN::START))
	{
		sceneMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}
	
	spherePlant_.Rotate(AsoUtility::AXIS_Y, -1.0f);
	playerAnim_->Update();

	skyDome_->Update();
}

void TitleScene::Draw(void)
{
	skyDome_->Draw();

	MV1DrawModel(player_.modelId);
	MV1DrawModel(spherePlant_.modelId);
	MV1DrawModel(bigPlant_.modelId);
	float scale = 50.0f;

	DrawRotaGraph(IMG_TITLE_X, IMG_TITLE_Y, 1.0, 0.0, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_X, IMG_PUSH_Y, 1.0, 0.0, imgPushSpace_, true);
}

void TitleScene::Release(void)
{
	skyDome_->Release();
	delete skyDome_;

	playerAnim_->Release();
	delete playerAnim_;
}
