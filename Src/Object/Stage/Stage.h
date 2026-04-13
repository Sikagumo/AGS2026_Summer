#pragma once
#include "../Actor/ActorBase.h"
#include <string>
#include <vector>

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

	void Update(void)override;

	void Draw(void)override;

protected:

	void InitLoad(void)override;

	void InitTransform(void)override;

	void InitCollider(void)override;

	void InitAnimation(void)override;

	void InitPost(void)override;


private:

	// 除外フレーム名称
	const std::vector<std::string> EXCLUDE_FRAME_NAMES = {
		"Mush", "Grass",
	};

	// 対象フレーム
	const std::vector<std::string> TARGET_FRAME_NAMES = {
		"Ground",
	};
};