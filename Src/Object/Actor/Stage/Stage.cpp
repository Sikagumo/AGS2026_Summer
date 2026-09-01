#include "Stage.h"
#include "../../Actor/ActorBase.h"
#include "../../../Manager/Generic/ResourceManager.h"
#include "../../../Object/Collision/CollisionController.h"
#include "../../../Utility/UtilityMath.h"
#include "../../Collider/ColliderModel.h"
#include "../../Collider/ColliderCapsule.h"
#include "../../../Shader/ShaderController.h"

constexpr float TREE_SCALE = 1.25f;
constexpr float TREE_POS_Y = -25.0f;


Stage::Stage(void)
	: viewStageTexHandle_(-1)
{
}

void Stage::Load(void)
{
	skyDome_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_SKYDOME);

	// ステージモデル
	transform_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE_COLLISION);
	viewTrans_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_STAGE);

	treePosModel_.modelId = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::MODEL_TREE_POSITION);

	
	int frameFront = MV1SearchFrame(treePosModel_.modelId, POS_FRAME_NAME_FRONT.c_str());
	int maxFront = MV1GetFrameChildNum(treePosModel_.modelId, frameFront);

	for (int i = 0; i < maxFront; i++)
	{
		std::shared_ptr<Tree> tree = std::make_shared<Tree>();
		tree->Load();

		treesFront_.emplace_back(tree);
	}


	int frameBack = MV1SearchFrame(treePosModel_.modelId, POS_FRAME_NAME_BACK.c_str());
	int maxBack = MV1GetFrameChildNum(treePosModel_.modelId, frameBack);
	for (int i = 0; i < maxBack; i++)
	{
		Transform tree = Transform();
		tree.modelId = ResourceManager::GetInstance().
			LoadModelDuplicate(ResourceManager::SRC::MODEL_TREE);

		treesBack_.emplace_back(tree);
	}


	viewStageTexHandle_ = MV1GetTextureGraphHandle(viewTrans_.modelId, 0);
}

void Stage::InitTransform(void)
{
	// スカイドーム初期化
	constexpr float SKYDOME_SCALE = 150.0f;
	skyDome_.InitTransform(SKYDOME_SCALE,
		Quaternion::Identity(), Quaternion::Identity());

	// 描画用ステージ
	constexpr float SCALE = 0.425f;
	constexpr VECTOR LOCAL_POS = { 0.0f, 10.0f, 0.0f };
	VECTOR localPos = LOCAL_POS;
	viewTrans_.InitTransform(SCALE,
		Quaternion::Identity(), Quaternion::Identity(),
		localPos);

	// 当たり判定用ステージ
	constexpr float COLLISION_POS_Y = -117.5f;
	localPos.y += COLLISION_POS_Y;
	transform_.InitTransform(SCALE,
		Quaternion::Identity(), Quaternion::Identity(),
		localPos);


	treePosModel_.InitTransform(SCALE,
		Quaternion::Identity(), Quaternion::Identity(),
		localPos);


	// 背景の木
	int frameBack = MV1SearchFrame(treePosModel_.modelId, POS_FRAME_NAME_BACK.c_str());
	int maxBack = MV1GetFrameChildNum(treePosModel_.modelId, frameBack);
	for (int i = 0; i < maxBack; i++)
	{
		float rot = static_cast<float>(360 - GetRand(360 * 2));
		VECTOR pos = MV1GetFramePosition(treePosModel_.modelId, (frameBack + (i + 1)));
		pos.y = TREE_POS_Y;

		treesBack_.at(i).InitTransform(TREE_SCALE
			, Quaternion::Identity(), Quaternion::AngleAxis(UtilityMath::Deg2RadF(rot), UtilityMath::AXIS_Y)
			, pos);
	}
}

void Stage::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ割り当て
	ColliderModel* colModel = new ColliderModel(ColliderBase::TAG::STAGE, &transform_);
	ownColliders_[static_cast<int>(ColliderBase::TAG::STAGE)].push_back(colModel);

	colModel->SetTriger(true);

	for (const auto& excludeName : EXCLUDE_STAGE_NAMES)
	{
		colModel->AddExcludeFrameIds(excludeName);
	}


	ColliderModel* wallCollider = new ColliderModel(ColliderBase::TAG::WALL, &transform_);

	// 壁のコライダ割り当て
	ownColliders_[static_cast<int>(ColliderBase::TAG::WALL)].push_back(wallCollider);
	wallCollider->SetTriger(false);

	for (const auto& excludeName : EXCLUDE_WALL_NAMES)
	{
		wallCollider->AddExcludeFrameIds(excludeName);
	}

	CollisionController::GetInstance().RegisterActor(this);
}


void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
	float STAGE_SCALE = 50.0f;
	
	texScaleParams_.scaleX = STAGE_SCALE;
	texScaleParams_.scaleY = STAGE_SCALE;
	
	for (auto& tree : treesFront_)
	{
		tree->Init();
	}

	// 木の位置を割り当て
	int frameFront = MV1SearchFrame(treePosModel_.modelId, POS_FRAME_NAME_FRONT.c_str());
	int maxFront = MV1GetFrameChildNum(treePosModel_.modelId, frameFront);
	for (int i = 0; i < maxFront; i++)
	{
		// ランダムで角度を変更
		float rot = static_cast<float>(360 - GetRand(360 * 2));
		VECTOR pos = MV1GetFramePosition(treePosModel_.modelId, (frameFront + (i + 1)));
		pos.y = TREE_POS_Y;

		treesFront_.at(i)->GetTransform().InitTransform(TREE_SCALE
			, Quaternion::Identity(), Quaternion::AngleAxis(UtilityMath::Deg2RadF(rot), UtilityMath::AXIS_Y)
			, pos);
	}

}

void Stage::Update(void)
{
	// スカイドーム
	constexpr float SKYDOME_ROT_SPEED = 0.0005f;
	skyDome_.Rotate(UtilityMath::AXIS_Y, SKYDOME_ROT_SPEED);


	// カメラの現在位置を取得
	VECTOR cameraPos = GetCameraPosition();

	// カメラから「遠い順（降順）」にソート
	std::sort(treesFront_.begin(), treesFront_.end(),
		[&cameraPos](const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) {
			// 非アクティブなものは後ろへ追いやる
			if (!a->GetIsActive()) { return false; }
			if (!b->GetIsActive()) { return true; }

			VECTOR posA = a->GetTransform().pos;
			VECTOR posB = b->GetTransform().pos;

			// 平方根(sqrt)計算を避けるため、距離の2乗で比較
			float distSqA = (posA.x - cameraPos.x) * (posA.x - cameraPos.x) +
				(posA.y - cameraPos.y) * (posA.y - cameraPos.y) +
				(posA.z - cameraPos.z) * (posA.z - cameraPos.z);

			float distSqB = (posB.x - cameraPos.x) * (posB.x - cameraPos.x) +
				(posB.y - cameraPos.y) * (posB.y - cameraPos.y) +
				(posB.z - cameraPos.z) * (posB.z - cameraPos.z);

			// 遠いもの(距離が大きいもの)を先頭にする
			return distSqA > distSqB;
		});
	// カメラから「遠い順（降順）」にソート
	std::sort(treesBack_.begin(), treesBack_.end(),
		[&cameraPos](const Transform& a, const Transform& b) {
			VECTOR posA = a.pos;
			VECTOR posB = b.pos;

			// 平方根(sqrt)計算を避けるため、距離の2乗で比較
			float distSqA = (posA.x - cameraPos.x) * (posA.x - cameraPos.x) +
				(posA.y - cameraPos.y) * (posA.y - cameraPos.y) +
				(posA.z - cameraPos.z) * (posA.z - cameraPos.z);

			float distSqB = (posB.x - cameraPos.x) * (posB.x - cameraPos.x) +
				(posB.y - cameraPos.y) * (posB.y - cameraPos.y) +
				(posB.z - cameraPos.z) * (posB.z - cameraPos.z);

			// 遠いもの(距離が大きいもの)を先頭にする
			return distSqA > distSqB;
		});

	for (auto& frontTree : treesFront_)
	{
		if (!frontTree->GetIsActive()) { continue; }

		frontTree->Update();
	}

	for (auto& backTree : treesBack_)
	{
		backTree.Update();
	}
}

void Stage::Draw(void)
{
	// スカイドーム
	MV1DrawModel(skyDome_.modelId);


	// ステージ描画
	ShaderController::GetInstance().Draw3D(
		ResourceManager::SRC::VS_TEX_SCALE,
		ResourceManager::SRC::PS_TEX_SCALE,
		viewTrans_.modelId,
		texScaleParams_,
		texScaleParams_,
		0,
		viewStageTexHandle_,
		false
	);

#ifdef _DEBUG
	// 自分が持っているすべてのコライダーを描画する
	for (const auto& [tagId, colliderList] : ownColliders_)
	{
		for (auto* collider : colliderList)
		{
			if (collider != nullptr)
			{
				//collider->Draw();
			}
		}
	}

	DrawFormatString(10, 120, 0xffffff, "ステージの座標：%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
#endif
}
void Stage::DrawTree(void)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// 場外の木
	for (auto& treeBack : treesBack_)
	{
		MV1DrawModel(treeBack.modelId);
	}

	// 当たり判定ありの木
	for (auto& treeFront : treesFront_)
	{
		if (!treeFront->GetIsActive()) { continue; }
		
		MV1DrawModel(treeFront->GetTransform().modelId);
	}

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
