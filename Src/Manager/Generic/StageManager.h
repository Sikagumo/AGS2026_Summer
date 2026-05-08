#pragma once

#include <memory>

class StageBase;

class StageManager
{
public:

	StageManager(void);

	~StageManager(void);

	void ChangeStage(std::shared_ptr<StageBase> nextStage);

	void Update(void);

	void Draw(void);

	void Release(void);

	bool IsLoading(void) const;

private:

	std::shared_ptr<StageBase> currentStage_;
};

