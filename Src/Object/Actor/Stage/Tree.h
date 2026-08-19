#pragma once
#include "../../Actor/ActorBase.h"

class Tree : public ActorBase
{
public:

	Tree(void);

	~Tree(void) = default;

	void Load(void)override;
	void Update(void)override;

	bool GetIsActive(void)const { return isActive_; };
	void SetIsActive(bool _isActive) { isActive_ = _isActive; };

protected:

	void InitTransform(void)override;
	void InitCollider(void)override;
	void InitAnimation(void)override {};
	virtual void InitPost(void)override {};

private:

	bool isActive_;
};