#pragma once

#include "../Scene/SceneBase.h"

class StageBase : public SceneBase
{
public:

	//@brief コンストラクタ
	StageBase(void);

	virtual ~StageBase(void) override;

	virtual void Load(void) override;

	virtual void EndLoad(void) override;

	virtual void Initialize(void) override = 0;

	virtual void Update(void) override = 0;

	virtual void Draw(void) override = 0;

	virtual void Release(void) override = 0;

protected:

	float LimitMinX_;

	float LimitMaxX_;

	float LimitMinY_;

	float LimitMaxY_;

	virtual void DebugDraw(void) const;
};

