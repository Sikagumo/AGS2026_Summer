#include "InputManager.h"
#include <DxLib.h>

InputManager* InputManager::instance_ = nullptr;

void InputManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new InputManager();
	}

	instance_->Init();
}

InputManager::InputManager(void):
	input_(nullptr)
{
	triggerMap_.clear();
	funcNewMap_.clear();
	funcTrgDownMap_.clear();
	funcTrgUpMap_.clear();
}

void InputManager::Init(void)
{
	// 入力判定処理の生成と初期化
	input_ = std::make_unique<Input>();
	input_->Init();

	// 入力状況に応じたトリガーの登録
	using BTN   = Input::JOYPAD_BTN;
	using STICK = Input::JOYPAD_STICK;
	using MOUSE = Input::MOUSE;

	// プレイヤー操作
	RegisterTrigger(TYPE::PLAYER_MOVE_UP, { KEY_INPUT_W, KEY_INPUT_UP }, { }, { STICK::L_STICK_UP });
	RegisterTrigger(TYPE::PLAYER_MOVE_DOWN, { KEY_INPUT_S, KEY_INPUT_DOWN }, { }, { STICK::L_STICK_DOWN });
	RegisterTrigger(TYPE::PLAYER_MOVE_LEFT, { KEY_INPUT_A, KEY_INPUT_LEFT }, { }, { STICK::L_STICK_LEFT });
	RegisterTrigger(TYPE::PLAYER_MOVE_RIGHT, { KEY_INPUT_D, KEY_INPUT_RIGHT }, { }, { STICK::L_STICK_RIGHT });
	RegisterTrigger(TYPE::PLAYER_CHANGE, { KEY_INPUT_LSHIFT, KEY_INPUT_RSHIFT }, { BTN::RB_RIGHT, BTN::RB_BOTTOM }, { });

	// カメラ操作
	RegisterTrigger(TYPE::CAMERA_MOVE_LEFT,  { KEY_INPUT_J }, { }, { });
	RegisterTrigger(TYPE::CAMERA_MOVE_RIGHT, { KEY_INPUT_L }, { }, { });
	RegisterTrigger(TYPE::CAMERA_MOVE_UP,    { KEY_INPUT_I }, { }, { });
	RegisterTrigger(TYPE::CAMERA_MOVE_DOWN,  { KEY_INPUT_K }, { }, { });

	// メニュー操作
	RegisterTrigger(TYPE::SELECT_LEFT,  { KEY_INPUT_A, KEY_INPUT_LEFT },  { BTN::L_BUTTON }, { STICK::L_STICK_LEFT });
	RegisterTrigger(TYPE::SELECT_RIGHT, { KEY_INPUT_D, KEY_INPUT_RIGHT }, { BTN::R_BUTTON}, { STICK::L_STICK_RIGHT });
	RegisterTrigger(TYPE::SELECT_UP,    { KEY_INPUT_W, KEY_INPUT_UP },    { BTN::L_BUTTON }, { STICK::L_STICK_UP });
	RegisterTrigger(TYPE::SELECT_DOWN,  { KEY_INPUT_S, KEY_INPUT_DOWN },  { BTN::R_BUTTON }, { STICK::L_STICK_DOWN });
	RegisterTrigger(TYPE::SELECT_DECISION, { KEY_INPUT_SPACE, KEY_INPUT_RETURN  }, { BTN::RB_RIGHT, BTN::START, BTN::RB_BOTTOM }, { STICK::MAX }, MOUSE::CLICK_LEFT);
	RegisterTrigger(TYPE::SELECT_CANCEL,   { KEY_INPUT_BACK },                     { BTN::RB_RIGHT }, { });
	RegisterTrigger(TYPE::PAUSE,           { KEY_INPUT_ESCAPE, KEY_INPUT_BACK },   { BTN::SELECT }, { });

	// ゲーム状態遷移
	RegisterTrigger(TYPE::GAME_STATE_CHANGE, { KEY_INPUT_SPACE }, { BTN::RB_RIGHT }, { });

	// デバッグ系
	RegisterTrigger(TYPE::DEBUG_MODE, { KEY_INPUT_TAB }, { }, { });
	RegisterTrigger(TYPE::DEBUG_SCENE_CHANGE, { KEY_INPUT_RSHIFT }, { }, { });
	RegisterTrigger(TYPE::DEBUG_POINT_SET, { }, { }, { }, MOUSE::CLICK_LEFT);
	RegisterTrigger(TYPE::DEBUG_POINT_DELETE, { }, { }, { }, MOUSE::CLICK_RIGHT);


	// カーソル非表示
	ShowCursor(MOUSE_VISIBLE);
}

void InputManager::Update(void)
{
	input_->Update();
}

void InputManager::Release(void)
{
	triggerMap_.clear();
	funcNewMap_.clear();
	funcTrgDownMap_.clear();
	funcTrgUpMap_.clear();
	input_->Release();
}
void InputManager::Destroy(void)
{
	instance_->Release();

	delete instance_;
}


bool InputManager::IsNew(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto& func : funcNewMap_[type])
	{
		if (func(type, padNo))
		{
			return true;
		}
	}
	return false;
}

bool InputManager::IsTrgDown(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto& func : funcTrgDownMap_[type])
	{
		if (func(type, padNo))
		{
			return true;
		}
	}
	return false;
}

bool InputManager::IsTrgUp(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto& func : funcTrgUpMap_[type])
	{
		if (func(type, padNo))
		{
			return true;
		}
	}
	return false;
}

void InputManager::SetMousePos(const Vector2& pos)
{
	input_->SetMousePos(pos);
}

void InputManager::SetCursorMode(Input::CURSOR_MODE _mode)
{
	input_->SetCursorMode(_mode);
}

Vector2 InputManager::GetMousePos(void) const
{
	return input_->GetMousePos();
}
Vector2 InputManager::GetMouseMove(void) const
{
	return input_->GetMousePosDistance();
}

Vector2 InputManager::GetKnockLStickSize(Input::JOYPAD_NO _num) const
{
	auto padInfo = input_->GetJPadInputState(_num);

	return Vector2(padInfo.AKeyLX, padInfo.AKeyLY);
}
Vector2 InputManager::GetKnockRStickSize(Input::JOYPAD_NO _num) const
{
	auto padInfo = input_->GetJPadInputState(_num);
	return Vector2(padInfo.AKeyRX, padInfo.AKeyRY);
}

VECTOR InputManager::GetDirXY_LStick(Input::JOYPAD_NO _num, float _threshold) const
{
	VECTOR ret = {};

	// 指定のコントローラ未割当時、ゼロを返す
	if (GetJoypadNum() <= static_cast<int>(_num)) { return ret; }


	auto padInfo = input_->GetJPadInputState(_num);

	float dirX = static_cast<float>(padInfo.AKeyLX);
	float dirY = static_cast<float>(padInfo.AKeyLY);

	// 平方根により、おおよその最大値が1.0となる
	float len = sqrtf((dirX * dirX) + (dirY * dirY));
	if (len < _threshold)
	{
		return ret;
	}

	// デッドゾーン境界から再スケーリング(可変デッドゾーン)
	float scale = (len - _threshold) / (1.0f - _threshold);
	dirX = (dirX / len) * scale;
	dirY = (dirY / len) * scale;

	ret = VNorm(VGet(dirX, dirY, 0.0f));

	return ret;
}

VECTOR InputManager::GetDirXY_RStick(Input::JOYPAD_NO _num, float _threshold) const
{
	VECTOR ret = {};

	// 指定のコントローラ未割当時、ゼロを返す
	if (GetJoypadNum() <= static_cast<int>(_num)) { return ret; }


	auto padInfo = input_->GetJPadInputState(_num);

	float dirX = static_cast<float>(padInfo.AKeyRX);
	float dirY = static_cast<float>(padInfo.AKeyRY);

	// 平方根により、おおよその最大値が1.0となる
	float len = sqrtf((dirX * dirX) + (dirY * dirY));
	if (len < _threshold)
	{
		return ret;
	}

	// デッドゾーン境界から再スケーリング(可変デッドゾーン)
	float scale = (len - _threshold) / (1.0f - _threshold);
	dirX = (dirX / len) * scale;
	dirY = (dirY / len) * scale;

	ret = VNorm(VGet(dirX, dirY, 0.0f));

	return ret;
}

void InputManager::RegisterTrigger(const TYPE type, const std::vector<int> keys,
								   const std::vector<Input::JOYPAD_BTN> padButtons, const std::vector<Input::JOYPAD_STICK> padSticks,
								   const Input::MOUSE mouse)
{
	// トリガーの情報を設定
	TriggerInfo info = { keys, padButtons, padSticks, mouse };

	// 情報の格納
	triggerMap_[type] = info;

	// 各種処理の配列
	std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> newFuncs;
	std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> isTrgDownFuncs;
	std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> isTrgUpFuncs;

	// キーが登録されている場合
	if (!info.keys.empty())
	{
		for (auto key : keys)
		{
			input_->Add(key);
		}

		newFuncs.push_back([this](TYPE t, Input::JOYPAD_NO) { return IsNewKey(t); });
		isTrgDownFuncs.push_back([this](TYPE t, Input::JOYPAD_NO) { return IsTrgDownKey(t); });
		isTrgUpFuncs.push_back([this](TYPE t, Input::JOYPAD_NO) { return IsTrgUpKey(t); });

	}
	// パッドのボタンが登録されている場合
	if (!info.padButtons.empty())
	{
		newFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsNewPadButton(t, padNo); });
		isTrgDownFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsTrgDownPadButton(t, padNo); });
		isTrgUpFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsTrgUpPadButton(t, padNo); });
	}
	// パッドのスティックが登録されている場合
	if (!info.padSticks.empty())
	{
		newFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsNewPadStick(t, padNo); });
		isTrgDownFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsTrgDownPadStick(t, padNo); });
		isTrgUpFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsTrgUpPadStick(t, padNo); });
	}
	// マウスの登録がされている場合
	if (info.mouse != Input::MOUSE::MAX && info.mouse != Input::MOUSE::NONE)
	{
		newFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsNewMouse(t); });
		isTrgDownFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsTrgDownMouse(t); });
		isTrgUpFuncs.push_back([this](TYPE t, Input::JOYPAD_NO padNo) { return IsTrgUpMouse(t); });
	}

	// 処理の登録
	RegisterTriggerFunction(type, newFuncs, isTrgDownFuncs, isTrgUpFuncs);
}

void InputManager::RegisterTriggerFunction(const TYPE type, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> newFuncs, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> trgDownFuncs, std::vector<std::function<bool(TYPE, Input::JOYPAD_NO)>> trgUpFuncs)
{
	funcNewMap_.emplace(type, newFuncs);
	funcTrgDownMap_.emplace(type, trgDownFuncs);
	funcTrgUpMap_.emplace(type, trgUpFuncs);
}


bool InputManager::IsNewKey(const TYPE type)
{
	for (int key : triggerMap_[type].keys)
	{
		if (input_->IsNew(key))
		{
			return true;
		}
	}
	return false;
}
bool InputManager::IsTrgDownKey(const TYPE type)
{
	for (int key : triggerMap_[type].keys)
	{
		if (input_->IsTrgDown(key))
		{
			return true;
		}
	}
	return false;
}
bool InputManager::IsTrgUpKey(const TYPE type)
{
	for (int key : triggerMap_[type].keys)
	{
		if (input_->IsTrgUp(key))
		{
			return true;
		}
	}
	return false;
}


bool InputManager::IsNewPadButton(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto button : triggerMap_[type].padButtons)
	{
		if (input_->IsPadBtnNew(padNo, button))
		{
			return true;
		}
	}
	return false;
}
bool InputManager::IsTrgDownPadButton(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto button : triggerMap_[type].padButtons)
	{
		if (input_->IsPadBtnTrgDown(padNo, button))
		{
			return true;
		}
	}
	return false;
}
bool InputManager::IsTrgUpPadButton(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto button : triggerMap_[type].padButtons)
	{
		if (input_->IsPadBtnTrgUp(padNo, button))
		{
			return true;
		}
	}
	return false;
}


bool InputManager::IsNewPadStick(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto stick : triggerMap_[type].padSticks)
	{
		if (input_->IsStickNew(padNo, stick))
		{
			return true;
		}
	}
	return false;
}
bool InputManager::IsTrgDownPadStick(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto stick : triggerMap_[type].padSticks)
	{
		if (input_->IsStickDown(padNo, stick))
		{
			return true;
		}
	}
	return false;
}
bool InputManager::IsTrgUpPadStick(const TYPE type, const Input::JOYPAD_NO padNo)
{
	for (auto stick : triggerMap_[type].padSticks)
	{
		if (input_->IsStickUp(padNo, stick))
		{
			return true;
		}
	}
	return false;
}


bool InputManager::IsNewMouse(const TYPE type)
{
	return input_->IsMouseNew(triggerMap_[type].mouse);
}
bool InputManager::IsTrgDownMouse(const TYPE type)
{
	return input_->IsMouseTrgDown(triggerMap_[type].mouse);
}
bool InputManager::IsTrgUpMouse(const TYPE type)
{
	return input_->IsMouseTrgUp(triggerMap_[type].mouse);
}