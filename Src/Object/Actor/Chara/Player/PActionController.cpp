#include "PActionController.h"
#include "../../../../Manager/Generic/SceneManager.h"
#include <DxLib.h>

PActionController::PActionController(bool _isRapidFire)
	: actionState_(PACTION_STATE::NONE)
	, curTimeAction_(0.0f), curTimeInput_(0.0f)
	, curActionNum_(-1)
	, isRapidFire_(_isRapidFire)
{
}

void PActionController::SetAction(int _actionNum, float _timeActive, float _timeEnd
	, float _timeActionActive, std::function<void(void)> _actionProc, float _timeInput)
{
	if (_actionNum < 0)
	{
		OutputDebugString("\n行動番号は０以上の値で登録してください\n");
		return;
	}

	if (actions_.find(_actionNum) != actions_.end())
	{
		OutputDebugString("\n同一名義の行動が既に登録されています。\n");
		return;
	}

	ActionParam param = ActionParam();
	param.timeActive = _timeActive;
	param.timeEnd = _timeEnd;
	param.timeInput = _timeInput;
	param.timeActionActive = _timeActionActive;
	param.actionProcess = _actionProc;
	
	actions_.emplace(_actionNum, param);
}

void PActionController::Active(int _actionNum)
{
	if (actions_.find(_actionNum) == actions_.end())
	{
		OutputDebugString("\n有効にしようとした行動が見つかりませんでした。\n");
		return;
	}

#ifdef _DEBUG
	if (actionState_ != PACTION_STATE::NONE)
	{
		OutputDebugString("\n行動中でしたが、行動を上書きしました。\n");
	}
#endif


	actionState_ = PACTION_STATE::ACTION;

	// 行動の情報を割り当て
	curActionNum_ = _actionNum;
	curTimeAction_ = actions_[_actionNum].timeActive;
	curTimeActionActive_ = actions_[_actionNum].timeActionActive;
	curTimeInput_  = actions_[_actionNum].timeInput;
}

void PActionController::Update(void)
{
	// 行動名が未割当時、処理終了
	if (curActionNum_ == -1 || actionState_ == PACTION_STATE::NONE) { return; }

	curTimeAction_ -= SceneManager::GetInstance().GetDeltaTime();

	if (actionState_ == PACTION_STATE::ACTION)
	{
		Update_Action();
	}
	else if (actionState_ == PACTION_STATE::END)
	{
		Update_End();
	}

}
void PActionController::Update_Action(void)
{
	curTimeInput_ = ((curTimeInput_ > 0) ? curTimeInput_ - SceneManager::GetInstance().GetDeltaTime() : 0.0f);


	if (curTimeActionActive_ > 0.0f)
	{
		curTimeActionActive_ -= SceneManager::GetInstance().GetDeltaTime();

		// 一度だけ行動を実行
		if (curTimeActionActive_ <= 0.0f)
		{
			curTimeAction_ = 0.0f;
			actions_[curActionNum_].actionProcess();
		}
	}

	
	if (curTimeAction_ < 0.0f)
	{
		actionState_ = PACTION_STATE::END;
		curTimeAction_ = actions_[curActionNum_].timeEnd;
	}
}
void PActionController::Update_End(void)
{
	if (curTimeAction_ < 0.0f)
	{
		actionState_ = PACTION_STATE::NONE;
		curActionNum_ = -1;
	}
}

void PActionController::DrawDebug(void)
{
#ifdef _DEBUG
	if (actionState_ == PACTION_STATE::NONE) { return; }

	DrawFormatString(0, 16, 0xffff00, "PAction(type:%s, active: %.1f, input:%.1f, num:%d)"
		, (actionState_ == PACTION_STATE::ACTION) ? "ACTION" : "END"
		, curTimeAction_, curTimeInput_, curActionNum_);
#endif
}