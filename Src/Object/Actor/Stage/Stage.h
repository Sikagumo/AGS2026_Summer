#pragma once
#include <string>
#include <vector>
#include "../../Actor/ActorBase.h"
#include "../../../Shader/ShaderParameters.h"
#include "./Tree.h"

class Stage : public ActorBase
{
public:

	// 衝突判定種別
	enum class COLLIDER_TYPE
	{
		MODEL = 0,
		MAX,
	};

	Stage(void);

	~Stage(void)override = default;

	void Load(void)override;

	void Update(void)override;

	void Draw(void)override;

	VECTOR GetPos(void) { return transform_.pos; }


protected:

	void InitTransform(void)override;

	void InitCollider(void)override;

	void InitAnimation(void)override;

	void InitPost(void)override;


private:

	const std::string POS_FRAME_NAME_FRONT = "tree_In";
	const std::string POS_FRAME_NAME_BACK  = "tree_Out";

	Transform viewTrans_;
	int viewStageTexHandle_;

	Transform skyDome_;
	TextureScaleShaderParams texScaleParams_;

	// 木のモデル
	Transform treePosModel_;
	std::vector<std::shared_ptr<Tree>> treesFront_;
	std::vector<Transform> treesBack_;

	// 壁の除外フレーム名称
	const std::vector<std::string> EXCLUDE_WALL_NAMES =
	{
		"Collision_Ground",
	};

	// ステージの除外フレーム名称
	const std::vector<std::string> EXCLUDE_STAGE_NAMES = 
	{
		"Collision_Wall",
	};
};