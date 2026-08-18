#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>

class AnimationController;

class PActionController
{
public:

	enum class PACTION_STATE
	{
		NONE = -1,
		ACTION,
		END,
	};

	/// @brief コンストラクタ
	/// @param _isRapidFire 連射か否か
	PActionController(std::unique_ptr<AnimationController>& _anim, bool _isRapidFire);
	~PActionController(void) = default;

	void Update(void);

	void DrawDebug(void);


	/// @brief 行動割り当て処理
	/// @param _actionNum 登録番号
	/// @param _timeActive 行動有効時間
	/// @param _timeActiveFunc 行動までの時間
	/// @param _timeEnd 行動終了時間
	/// @param _actionProc 行動
	/// @param _timeStop 停止する時間
	/// @param _timeStopActive 停止までの時間
	/// @param _timeInput 次の入力までの時間
	void SetAction(int _actionNum, float _timeActive, float _timeActiveFunc
				   , float _timeEnd, std::function<void(void)> _actionProc, float _timeStop = 0.0f, float _timeStopActive = 0.0f, float _timeInput = 0.0f);

	/// @brief 登録した行動を開始する
	/// @param _actionName 登録した行動名
	void Active(int _actionNum);

	/// @brief 入力可能か否か
	bool IsActiveInput(void)const;

	/// @brief 行動中か否か
	bool IsActiveAction(void)const { return (curActionNum_ != -1); };

	/// @brief 現在の行動番号を取得
	int GetCurActionNum(void)const { return curActionNum_; }

	/// @brief 行動が終わっているか否か
	bool IsEndActionActive(void)const { return (curTimeActionActive_ <= 0.0f); };

	PACTION_STATE GetActionState(void)const { return actionState_; };


private:

	struct ActionParam
	{
		// 有効時間
		float timeActive = 0.0f;

		// 終了時間
		float timeEnd = 0.0f;

		// 行動までの時間
		float timeActionActive = 0.0f;

		// 次の入力までの有効時間
		float timeInput = 0.0f;

		// 停止する時間
		float timeStop = 0.0f;

		// 停止までの時間
		float timeStopActive = 0.0f;

		std::function<void(void)> actionProcess;
		
	};
	std::map<const int, const ActionParam> actions_;

	std::unique_ptr<AnimationController>& animation_;

	PACTION_STATE actionState_;


	// 現在実行中の行動名
	int curActionNum_;

	float curTimeAction_;

	// 行動するまでの時間
	float curTimeActionActive_;

	float curTimeInput_;

	// 停止するまでの時間
	float curTimeStopActive_;

	// 連射か否か
	bool isRapidFire_ = false;


	void Update_Action(void);
	void Update_End(void);
};

