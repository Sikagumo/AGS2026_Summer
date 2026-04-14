#include "Input.h"
#include "../Application.h"
#include "../Utility/UtilityCommon.h"
#include "../Utility/AsoUtility.h"
#include <DxLib.h>
#include <algorithm>


Input::Input(void):
	mouseInput_(-1),
	cursorMode_(CURSOR_MODE::NONE)
{
}


void Input::Init(void)
{
	Input::MouseInfo info;

	// マウス
	info = Input::MouseInfo();
	for (int i = 0; i < static_cast<int>(MOUSE::MAX); i++)
	{
		mouseInfos_.emplace(static_cast<MOUSE>(i), info);
	}

	//スティック
	Input::StickInfo stickInfo;
	stickInfo = Input::StickInfo();
	for (int padNo = 0; padNo <= static_cast<int>(JOYPAD_NO::PAD4); padNo++)
	{
		std::vector<StickInfo>sticks;
		for (int i = 0; i < static_cast<int>(JOYPAD_STICK::MAX); i++)
		{
			stickInfo.key = static_cast<JOYPAD_STICK>(i);
			sticks.push_back(stickInfo);
		}
		stickInfos_.emplace(static_cast<JOYPAD_NO>(padNo), sticks);
	}
}

void Input::Update(void)
{
	// キーボード検知
	for (auto& [type, keyInfo] : keyInfos_)
	{
		keyInfo.keyOld = keyInfo.keyNew;
		keyInfo.keyNew = CheckHitKey(keyInfo.key);
		keyInfo.keyTrgDown = (keyInfo.keyNew && !keyInfo.keyOld);
		keyInfo.keyTrgUp   = (!keyInfo.keyNew && keyInfo.keyOld);
	}

	// マウス
	UpdateMouse();


	// パッド情報
	SetJPadInState(JOYPAD_NO::KEY_PAD1);
	SetJPadInState(JOYPAD_NO::PAD1);
	SetJPadInState(JOYPAD_NO::PAD2);
	SetJPadInState(JOYPAD_NO::PAD3);
	SetJPadInState(JOYPAD_NO::PAD4);

	// スティック検知
	for (auto& [type, stickInfo] : stickInfos_)
	{
		for (auto& stick : stickInfo)
		{
			int overSize = PadStickOverSize(type, stick.key);
			stick.keyOld = stick.keyNew;
			stick.keyNew = (overSize > STICK_THRESHOLD);
			stick.keyTrgDown = (!stick.keyOld && stick.keyNew);
			stick.keyTrgUp   = (stick.keyOld && !stick.keyNew);
		}
	}
}
void Input::UpdateMouse(void)
{
	// マウス検知
	mouseInput_ = GetMouseInput();

	mousePrePos_ = mousePos_;
	GetMousePoint(&mousePos_.x, &mousePos_.y);

	const Vector2 SCREEN_SIZE = { Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y };

	// 
	bool isWrap = false;

	if (cursorMode_ == CURSOR_MODE::CONFINED_SIDE)
	{
		Vector2 cursorPos = mousePos_;

		// マウス座標が画面外に出たとき、反対に移動
		bool isWrap = UtilityCommon::WrapValue(cursorPos, SCREEN_SIZE);
		if (isWrap) { SetMousePos(cursorPos); }
	}
	else if (cursorMode_ == CURSOR_MODE::CONFINED)
	{
		// マウス座標を制限
		mousePos_.x = std::clamp(mousePos_.x, AsoUtility::VECTOR2_ZERO.x, SCREEN_SIZE.x);
		mousePos_.y = std::clamp(mousePos_.y, AsoUtility::VECTOR2_ZERO.y, SCREEN_SIZE.y);
		if (isWrap) { SetMousePos(mousePos_); }
	}

	wheelRot_ = GetMouseWheelRotVol();

	for (auto& [type, mouseInfo] : mouseInfos_)
	{
		mouseInfo.keyOld = mouseInfo.keyNew;

		switch (type)
		{
		case MOUSE::CLICK_RIGHT:
			mouseInfo.keyNew = (mouseInput_ & MOUSE_INPUT_RIGHT) != 0;
			break;

		case MOUSE::CLICK_LEFT:
			mouseInfo.keyNew = (mouseInput_ & MOUSE_INPUT_LEFT) != 0;
			break;

		case MOUSE::CLICK_MIDDLE:
			mouseInfo.keyNew = (mouseInput_ & MOUSE_INPUT_MIDDLE) != 0;
			break;

		case MOUSE::MOVE_LEFT:
			mouseInfo.keyNew = (mousePos_.x < mousePrePos_.x);
			break;

		case MOUSE::MOVE_RIGHT:
			mouseInfo.keyNew = (mousePos_.x > mousePrePos_.x);
			break;

		case MOUSE::MOVE_UP:
			mouseInfo.keyNew = (mousePos_.y < mousePrePos_.y);
			break;

		case MOUSE::MOVE_DOWN:
			mouseInfo.keyNew = (mousePos_.y > mousePrePos_.y);
			break;

		case MOUSE::WHEEL_FRONT:
			mouseInfo.keyNew = (wheelRot_ > 0);
			break;

		case MOUSE::WHEEL_BACK:
			mouseInfo.keyNew = (wheelRot_ < 0);
			break;

		case MOUSE::MAX:
		default:
			break;
		}
		mouseInfo.keyTrgDown = (mouseInfo.keyNew && !mouseInfo.keyOld);
		mouseInfo.keyTrgUp = (!mouseInfo.keyNew && mouseInfo.keyOld);
	}
}

void Input::Release(void)
{
	keyInfos_.clear();
	mouseInfos_.clear();
	stickInfos_.clear();
}


void Input::Add(int key)
{
	// 同一のキーが割り当て時は処理終了
	for (auto [type, keyInfo] : keyInfos_)
	{
		if (type == key) { return; }
	}

	Input::Info info = Input::Info();
	info.key = key;
	info.keyOld = false;
	info.keyNew = false;
	info.keyTrgDown = false;
	info.keyTrgUp = false;
	keyInfos_.emplace(key, info);
}

void Input::Clear(void)
{
	keyInfos_.clear();
}

bool Input::IsNew(int key) const
{
	return Find(key).keyNew;
}

bool Input::IsTrgDown(int key) const
{
	return Find(key).keyTrgDown;
}

bool Input::IsTrgUp(int key) const
{
	return Find(key).keyTrgUp;
}

void Input::SetMousePos(const Vector2& pos)
{
	mousePos_ = pos;
	mousePrePos_ = pos;
	SetMousePoint(static_cast<int>(mousePos_.x), static_cast<int>(mousePos_.y));
}

Vector2 Input::GetMousePosDistance(void) const
{
	return (mousePos_ - mousePrePos_);
}


bool Input::IsMouseNew(MOUSE mouse) const
{
	return FindMouse(mouse).keyNew;
}

bool Input::IsMouseTrgUp(MOUSE mouse) const
{
	return FindMouse(mouse).keyTrgUp;
}

bool Input::IsMouseTrgDown(MOUSE mouse) const
{
	return FindMouse(mouse).keyTrgDown;
}

const Input::Info& Input::Find(int key) const
{

	auto it = keyInfos_.find(key);
	if (it != keyInfos_.end())
	{
		return it->second;
	}

	return infoEmpty_;

}

const Input::MouseInfo& Input::FindMouse(MOUSE key) const
{
	auto it = mouseInfos_.find(key);
	if (it != mouseInfos_.end())
	{
		return it->second;
	}

	return mouseInfoEmpty_;
}

Input::JOYPAD_TYPE Input::GetJPadType(JOYPAD_NO no)
{
	return static_cast<Input::JOYPAD_TYPE>(GetJoypadType(static_cast<int>(no)));
}

DINPUT_JOYSTATE Input::GetJPadDInputState(JOYPAD_NO no)
{
	// コントローラ情報
	GetJoypadDirectInputState(static_cast<int>(no), &joyDInState_);
	return joyDInState_;
}

XINPUT_STATE Input::GetJPadXInputState(JOYPAD_NO no)
{
	// コントローラ情報
	GetJoypadXInputState(static_cast<int>(no), &joyXInState_);
	return joyXInState_;
}

void Input::SetJPadInState(JOYPAD_NO jpNo)
{

	int no = static_cast<int>(jpNo);
	auto stateNew = GetJPadInputState(jpNo);
	auto& stateNow = padInfos_[no];

	int max = static_cast<int>(JOYPAD_BTN::MAX);
	for (int i = 0; i < max; i++)
	{

		stateNow.ButtonsOld[i] = stateNow.ButtonsNew[i];
		stateNow.ButtonsNew[i] = stateNew.ButtonsNew[i];

		stateNow.IsOld[i]   = stateNow.IsNew[i];
		//stateNow.IsNew[i] = (stateNow.ButtonsNew[i] == 128 || stateNow.ButtonsNew[i] == 255);
		stateNow.IsNew[i]   = stateNow.ButtonsNew[i] > 0;

		stateNow.IsTrgDown[i] = (stateNow.IsNew[i] && !stateNow.IsOld[i]);
		stateNow.IsTrgUp[i]   = (!stateNow.IsNew[i] && stateNow.IsOld[i]);


		stateNow.AKeyLX = stateNew.AKeyLX;
		stateNow.AKeyLY = stateNew.AKeyLY;
		stateNow.AKeyRX = stateNew.AKeyRX;
		stateNow.AKeyRY = stateNew.AKeyRY;

	}

}

Input::JOYPAD_IN_STATE Input::GetJPadInputState(JOYPAD_NO no)
{

	JOYPAD_IN_STATE ret = JOYPAD_IN_STATE();

	auto type = GetJPadType(no);

	switch (type)
	{
	case Input::JOYPAD_TYPE::OTHER:
		break;

	case Input::JOYPAD_TYPE::XBOX_360:
	{
	}
	break;

	case Input::JOYPAD_TYPE::XBOX_ONE:
	{
		auto d = GetJPadDInputState(no);
		auto x = GetJPadXInputState(no);

		int idx;

		//   Y
		// X   B
		//   A


		// Y
		idx = static_cast<int>(JOYPAD_BTN::RB_TOP);
		ret.ButtonsNew[idx] = d.Buttons[3];

		// X
		idx = static_cast<int>(JOYPAD_BTN::RB_LEFT);
		ret.ButtonsNew[idx] = d.Buttons[2];

		// B
		idx = static_cast<int>(JOYPAD_BTN::RB_RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[1];

		// A
		idx = static_cast<int>(JOYPAD_BTN::RB_BOTTOM);
		ret.ButtonsNew[idx] = d.Buttons[0];

		// R_BUTTON
		idx = static_cast<int>(JOYPAD_BTN::R_BUTTON);
		ret.ButtonsNew[idx] = d.Buttons[5];

		// L_BUTTON
		idx = static_cast<int>(JOYPAD_BTN::L_BUTTON);
		ret.ButtonsNew[idx] = d.Buttons[4];

		// START
		idx = static_cast<int>(JOYPAD_BTN::START);
		ret.ButtonsNew[idx] = d.Buttons[7];

		// SELECT
		idx = static_cast<int>(JOYPAD_BTN::SELECT);
		ret.ButtonsNew[idx] = d.Buttons[6];

		// R_TRIGGER
		idx = static_cast<int>(JOYPAD_BTN::R_TRIGGER);
		ret.ButtonsNew[idx] = x.RightTrigger;

		// L_TRIGGER	
		idx = static_cast<int>(JOYPAD_BTN::L_TRIGGER);
		ret.ButtonsNew[idx] = x.LeftTrigger;

		// →
		idx = static_cast<int>(JOYPAD_BTN::CROSS_RIGHT);
		ret.ButtonsNew[idx] = x.Buttons[3];

		// ←
		idx = static_cast<int>(JOYPAD_BTN::CROSS_LEFT);
		ret.ButtonsNew[idx] = x.Buttons[2];

		// ↓
		idx = static_cast<int>(JOYPAD_BTN::CROSS_DOWN);
		ret.ButtonsNew[idx] = x.Buttons[1];

		// ↑
		idx = static_cast<int>(JOYPAD_BTN::CROSS_TOP);
		ret.ButtonsNew[idx] = x.Buttons[0];

		// 左スティック押し込み
		idx = static_cast<int>(JOYPAD_BTN::L_STICK);
		ret.ButtonsNew[idx] = d.Buttons[8];

		// 右スティック押し込み
		idx = static_cast<int>(JOYPAD_BTN::R_STICK);
		ret.ButtonsNew[idx] = d.Buttons[9];


		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;

		// 右スティック
		ret.AKeyRX = d.Rx;
		ret.AKeyRY = d.Ry;

	}
	break;

	case Input::JOYPAD_TYPE::DUAL_SHOCK_4:
		break;

	case Input::JOYPAD_TYPE::DUAL_SENSE:
	{

		auto d = GetJPadDInputState(no);
		int idx;

		//   △
		// □  〇
		//   ×

		idx = static_cast<int>(JOYPAD_BTN::RB_TOP);
		ret.ButtonsNew[idx] = d.Buttons[3];// Y

		idx = static_cast<int>(JOYPAD_BTN::RB_LEFT);
		ret.ButtonsNew[idx] = d.Buttons[2];// X

		idx = static_cast<int>(JOYPAD_BTN::RB_RIGHT);
		ret.ButtonsNew[idx] = d.Buttons[1];// B

		idx = static_cast<int>(JOYPAD_BTN::RB_BOTTOM);
		ret.ButtonsNew[idx] = d.Buttons[0];// A

		// 左スティック
		ret.AKeyLX = d.X;
		ret.AKeyLY = d.Y;

		// 右スティック
		ret.AKeyRX = d.Z;
		ret.AKeyRY = d.Rz;

	}
	break;

	case Input::JOYPAD_TYPE::SWITCH_JOY_CON_L:
	case Input::JOYPAD_TYPE::SWITCH_JOY_CON_R:
	case Input::JOYPAD_TYPE::SWITCH_PRO_CTRL:
	case Input::JOYPAD_TYPE::MAX:
		break;
	}

	return ret;

}

int Input::PadStickOverSize(const JOYPAD_NO no, const JOYPAD_STICK stick)
{
	int ret = 0;
	auto padInfo = GetJPadInputState(no);
	switch (stick)
	{
	case JOYPAD_STICK::L_STICK_UP:
		ret = ((padInfo.AKeyLY < 0) ? padInfo.AKeyLY : 0);
		break;

	case JOYPAD_STICK::L_STICK_DOWN:
		ret = ((padInfo.AKeyLY > 0) ? padInfo.AKeyLY : 0);
		break;

	case JOYPAD_STICK::L_STICK_LEFT:
		ret = ((padInfo.AKeyLX < 0) ? padInfo.AKeyLX : 0);
		break;

	case JOYPAD_STICK::L_STICK_RIGHT:
		ret = ((padInfo.AKeyLX > 0) ? padInfo.AKeyLX : 0);
		break;

	case JOYPAD_STICK::R_STICK_UP:
		ret = ((padInfo.AKeyRY < 0) ? padInfo.AKeyRY : 0);
		break;

	case JOYPAD_STICK::R_STICK_DOWN:
		ret = ((padInfo.AKeyRY > 0) ? padInfo.AKeyRY : 0);
		break;

	case JOYPAD_STICK::R_STICK_LEFT:
		ret = ((padInfo.AKeyRX < 0) ? padInfo.AKeyRX : 0);
		break;

	case JOYPAD_STICK::R_STICK_RIGHT:
		ret = ((padInfo.AKeyRX > 0) ? padInfo.AKeyRX : 0);
		break;

	case JOYPAD_STICK::MAX:
	default:
		break;
	}
	return abs(ret);
}

bool Input::IsPadBtnNew(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsNew[static_cast<int>(btn)];
}

bool Input::IsPadBtnTrgDown(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsTrgDown[static_cast<int>(btn)];
}

bool Input::IsPadBtnTrgUp(JOYPAD_NO no, JOYPAD_BTN btn) const
{
	return padInfos_[static_cast<int>(no)].IsTrgUp[static_cast<int>(btn)];
}

bool Input::IsStickNew(JOYPAD_NO no, JOYPAD_STICK stick) const
{
	for (auto& stickInfo : stickInfos_)
	{
		if (stickInfo.first != no)
		{
			continue;
		}
		for (auto& stickI : stickInfo.second)
		{
			if (stickI.key != stick)
			{
				continue;
			}
			return stickI.keyNew;
		}
	}
	return false;
}

bool Input::IsStickDown(JOYPAD_NO no, JOYPAD_STICK stick) const
{
	for (auto& stickInfo : stickInfos_)
	{
		if (stickInfo.first != no) { continue; }

		for (auto& stickI : stickInfo.second)
		{
			if (stickI.key != stick)
			{
				continue;
			}
			return stickI.keyTrgDown;
		}
	}
	return false;
}

bool Input::IsStickUp(JOYPAD_NO no, JOYPAD_STICK stick) const
{
	for (auto& stickInfo : stickInfos_)
	{
		if (stickInfo.first != no) { continue; }

		for (auto& stickI : stickInfo.second)
		{
			if (stickI.key != stick)
			{
				continue;
			}
			return stickI.keyTrgUp;
		}
	}
	return false;
}