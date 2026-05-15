#pragma once
#include <vector>


class PActionController
{
public:

	enum class ATTACK_STATE
	{
		NONE = -1,
		ACTION,
		END,
	};

	PActionController(void);
	~PActionController(void) = default;

	void SetMotion(float _timeActive, float _timeEnd, float _timeAtk, float timeInput = 0.0f);

private:

	struct ActionParam
	{
		// 有効時間
		const float timeActive_;

		// 終了時間
		const float timeEnd_;


		// 攻撃までのタイミング
		const float timeAtk_;


		// 次の入力までの有効時間
		const float timeInput_;
	};

	float curTimeAction_;

	float curTimeAtk_;

	float curTimeInput_;

	std::vector<ActionParam> actions_;
};

