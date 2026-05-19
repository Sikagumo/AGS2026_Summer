#pragma once

#include "../SceneBase.h"

class SceneLobby : public SceneBase
{
public:

	SceneLobby(void);

	~SceneLobby(void);

	void Load(void) override;

	void Initialize(void) override;

	void Update(void) override;


private:
};

