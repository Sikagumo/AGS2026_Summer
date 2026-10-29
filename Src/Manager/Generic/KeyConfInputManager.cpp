#include "KeyConfInputManager.h"
#include <cmath>
#include <cstdio>
#include <array>
#include "../../Utility/UtilityMath.h"
#include "../../Application.h"

namespace
{
	// 判定および計算用の定数
	constexpr float MOUSE_VELOCITY_THRESHOLD = 1.0f; // マウス移動量の無視しきい値
	constexpr int TRIGGER_THRESHOLD = 128;           // XInputトリガーのしきい値
	constexpr int STICK_THRESHOLD = 16000;           // XInputスティックのしきい値

	// ファイルおよびシグネチャ関連定数
	constexpr size_t SIGNATURE_LENGTH = 4;
	const char* const FILE_PATH_KEY_CONFIG = "DatData/keyConfig.dat";
	const char* const FILE_PATH_SENSITIVITY = "DatData/sensitivity.dat";
	const char* const SIGNATURE_KEY_CONFIG = "kcnf";
	const char* const SIGNATURE_SENSITIVITY = "sens";
}

KeyConfInputManager* KeyConfInputManager::instance_ = nullptr;

void KeyConfInputManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new KeyConfInputManager();
	}
}

KeyConfInputManager& KeyConfInputManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		CreateInstance();
	}
	return *instance_;
}

void KeyConfInputManager::DestroyInstance(void)
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

KeyConfInputManager::KeyConfInputManager(void)
	: mousePosition_(UtilityMath::VECTOR2_ZERO)
	, inputTable_()
	, currentInputState_()
	, previousInputState_()
	, stickInfo_{}
	, rightStickSensitivity_{}
	, mouseSensitivity_{}
	, mouseWheelVolume_(0)
	, usePadNumber_(DX_INPUT_PAD1)
{
	InitializeInputTable();
	LoadInputTable();
	LoadSensitivitySettings();

	for (const auto& pair : inputTable_)
	{
		currentInputState_[pair.first] = false;
		previousInputState_[pair.first] = false;
	}
}

void KeyConfInputManager::InitializeInputTable(void)
{
	inputTable_.clear();

	inputTable_["UP"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_W},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_UP},
		{INPUT_TYPE::JOYPAD, PAD_INPUT_UP},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_UP)}
	};

	inputTable_["DOWN"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_S},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_DOWN},
		{INPUT_TYPE::JOYPAD, PAD_INPUT_DOWN},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_DOWN)}
	};

	inputTable_["LEFT"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_A},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_LEFT},
	  {INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_LEFT)}
	};

	inputTable_["RIGHT"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_D},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_RIGHT},
	  {INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_RIGHT)}
	};

	inputTable_["JUMP"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_SPACE},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_RETURN},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_A}
	};

	inputTable_["DODGE"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_LSHIFT},
		{INPUT_TYPE::JOYPAD, PAD_INPUT_C}
	};

	inputTable_["ATTACK_NORMAL"] =
	{
		{INPUT_TYPE::MOUSE, MOUSE_INPUT_LEFT},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::RIGHT_TRIGGER)}
	};

	inputTable_["ATTACK_SPECIAL"] =
	{
		{INPUT_TYPE::MOUSE, MOUSE_INPUT_RIGHT},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_TRIGGER)}
	};

	inputTable_["OK"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_SPACE},
	  {INPUT_TYPE::MOUSE, MOUSE_INPUT_LEFT},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_A}
	};

	inputTable_["CANCEL"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_ESCAPE},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_DELETE},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_B}
	};

	inputTable_["DEBUG_CANCEL"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_BACK}
	};

	inputTable_["PAUSE"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_ESCAPE},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_8}
	};

	inputTable_["LOCK_ON"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_E},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::RIGHT_STICK_PUSH)}
	};

	inputTable_["TARGET_CHANGE_LEFT"] =
	{
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_SHOULDER)}
	};

	inputTable_["TARGET_CHANGE_RIGHT"] =
	{
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::RIGHT_SHOULDER)}
	};

	inputTable_["CAMERA_UP"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_UP}
	};

	inputTable_["CAMERA_DOWN"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_DOWN}
	};

	inputTable_["CAMERA_LEFT"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_LEFT}
	};

	inputTable_["CAMERA_RIGHT"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_RIGHT}
	};

	inputTable_["APPLY_DEBUG"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_BACK},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_END}
	};

	inputTable_["UNAPPLY_DEBUG"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_RSHIFT},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_HOME}
	};
}

void KeyConfInputManager::Update(void)
{
	previousInputState_ = currentInputState_;

	// キーボードの生データ
	std::array<char, 256> keyState{};
	GetHitKeyStateAll(keyState.data());

	int mouseState = GetMouseInput();
	int padState = GetJoypadInputState(usePadNumber_);

	GetMousePoint(&mousePosition_.x, &mousePosition_.y);

	mouseWheelVolume_ = GetMouseWheelRotVol();

	// XInputの状態
	XINPUT_STATE xInput{};
	if (GetJoypadXInputState(usePadNumber_, &xInput) == 0)
	{
		stickInfo_.leftStickX = xInput.ThumbLX;
		stickInfo_.leftStickY = xInput.ThumbLY;
		stickInfo_.rightStickX = xInput.ThumbRX;
		stickInfo_.rightStickY = xInput.ThumbRY;
	}
	else
	{
		stickInfo_.leftStickX = 0;
		stickInfo_.leftStickY = 0;
		stickInfo_.rightStickX = 0;
		stickInfo_.rightStickY = 0;
	}

	for (const auto& pair : inputTable_)
	{
		const auto& eventName = pair.first;
		bool isHit = false;

		if (eventName == "APPLY_DEBUG" || eventName == "UNAPPLY_DEBUG")
		{
			isHit = true;
			for (const auto& inputInfo : pair.second)
			{
				if (!CheckKeyboardInput(inputInfo, keyState))
				{
					isHit = false;
					break;
				}
			}
		}
		else
		{
			for (const auto& inputInfo : pair.second)
			{
				switch (inputInfo.type)
				{
				case INPUT_TYPE::KEY_BOARD:
					if (CheckKeyboardInput(inputInfo, keyState))
					{
						isHit = true;
					}
					break;

				case INPUT_TYPE::MOUSE:
					if ((mouseState & inputInfo.id) != 0)
					{
						isHit = true;
					}
					break;

				case INPUT_TYPE::JOYPAD:
					if ((padState & inputInfo.id) != 0)
					{
						isHit = true;
					}
					break;

				case INPUT_TYPE::XINPUT_ANALOG:
					if (CheckXInputAnalog(inputInfo, xInput))
					{
						isHit = true;
					}
					break;

				default:
					break;
				}
			}
		}
		currentInputState_[eventName] = isHit;
	}
}

bool KeyConfInputManager::IsPressed(const std::string& _name) const
{
	auto input = currentInputState_.find(_name);
	return (input != currentInputState_.end()) && input->second;
}

bool KeyConfInputManager::IsTriggerUp(const std::string& _name) const
{
	auto currentInput = currentInputState_.find(_name);
	auto previousInput = previousInputState_.find(_name);

	if (currentInput == currentInputState_.end())
	{
		return false;
	}

	return !(currentInput->second) && previousInput->second;
}

bool KeyConfInputManager::IsTriggerDown(const std::string& _name) const
{
	auto currentInput = currentInputState_.find(_name);
	auto previousInput = previousInputState_.find(_name);

	if (currentInput == currentInputState_.cend())
	{
		return false;
	}

	return currentInput->second && !(previousInput->second);
}

Vector2F KeyConfInputManager::GetMouseVelocityAndFixCenter(void)
{
	int centerX = Application::SCREEN_HALF_X;
	int centerY = Application::SCREEN_HALF_Y;

	int currentX = 0; // 現在のマウスX
	int currentY = 0; // 現在のマウスY
	GetMousePoint(&currentX, &currentY);

	float diffX = static_cast<float>(currentX - centerX); // Xの移動量
	float diffY = static_cast<float>(currentY - centerY); // Yの移動量

	if (std::abs(diffX) <= MOUSE_VELOCITY_THRESHOLD)
	{
		diffX = 0.0f;
	}

	if (std::abs(diffY) <= MOUSE_VELOCITY_THRESHOLD)
	{
		diffY = 0.0f;
	}

	SetMousePoint(centerX, centerY);
	SetMouseDispFlag(false);

	return Vector2F(diffX, diffY);
}

void KeyConfInputManager::SetMouseSensitivity(const MouseSensitivity& _sensitivity)
{
	mouseSensitivity_ = _sensitivity;
}

void KeyConfInputManager::CalculateNormalizedStick(int _rawX, int _rawY, float _deadZone,
	float& _outNormalX, float& _outNormalY) const
{
	float normalX = static_cast<float>(_rawX) / XINPUT_VAL_MAX;
	float normalY = static_cast<float>(_rawY) / XINPUT_VAL_MAX;
	float length = sqrtf(normalX * normalX + normalY * normalY);

	if (length < _deadZone)
	{
		_outNormalX = 0.0f;
		_outNormalY = 0.0f;
		return;
	}

	float scale = (length - _deadZone) / (1.0f - _deadZone);

	_outNormalX = (normalX / length) * scale;
	_outNormalY = (normalY / length) * scale;
}

void KeyConfInputManager::ApplyRightStickSensitivity(int _rightStickX, int _rightStickY,
	float& _outNormalX, float& _outNormalY) const
{
	float normalX = 0.0f;
	float normalY = 0.0f;
	CalculateNormalizedStick(_rightStickX, _rightStickY, rightStickSensitivity_.deadZone,
		normalX, normalY);

	normalX *= rightStickSensitivity_.x;
	normalY *= rightStickSensitivity_.y;

	normalX = std::fmax(-1.0f, std::fmin(1.0f, normalX));
	normalY = std::fmax(-1.0f, std::fmin(1.0f, normalY));

	if (rightStickSensitivity_.invertX)
	{
		normalX = -normalX;
	}
	if (rightStickSensitivity_.invertY)
	{
		normalY = -normalY;
	}

	_outNormalX = normalX;
	_outNormalY = normalY;
}

bool KeyConfInputManager::CheckKeyboardInput(const InputInfo& _inputInfo,
	const std::array<char, 256>& _keyState) const
{
	return (_keyState[_inputInfo.id] != 0);
}

bool KeyConfInputManager::CheckXInputAnalog(const InputInfo& _inputInfo,
	const XINPUT_STATE& _xInputState) const
{
	switch (static_cast<XINPUT_ANALOG_ID>(_inputInfo.id))
	{
	case XINPUT_ANALOG_ID::LEFT_TRIGGER:
		return (_xInputState.LeftTrigger > TRIGGER_THRESHOLD);

	case XINPUT_ANALOG_ID::RIGHT_TRIGGER:
		return (_xInputState.RightTrigger > TRIGGER_THRESHOLD);

	case XINPUT_ANALOG_ID::LEFT_STICK_UP:
		return (_xInputState.ThumbLY > STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::LEFT_STICK_DOWN:
		return (_xInputState.ThumbLY < -STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::LEFT_STICK_LEFT:
		return (_xInputState.ThumbLX < -STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::LEFT_STICK_RIGHT:
		return (_xInputState.ThumbLX > STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::RIGHT_STICK_UP:
		return (_xInputState.ThumbRY > STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::RIGHT_STICK_DOWN:
		return (_xInputState.ThumbRY < -STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::RIGHT_STICK_LEFT:
		return (_xInputState.ThumbRX < -STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::RIGHT_STICK_RIGHT:
		return (_xInputState.ThumbRX > STICK_THRESHOLD);

	case XINPUT_ANALOG_ID::LEFT_SHOULDER:
		return (_xInputState.Buttons[XINPUT_BUTTON_LEFT_SHOULDER] != 0);

	case XINPUT_ANALOG_ID::RIGHT_SHOULDER:
		return (_xInputState.Buttons[XINPUT_BUTTON_RIGHT_SHOULDER] != 0);

	case XINPUT_ANALOG_ID::LEFT_STICK_PUSH:
		return (_xInputState.Buttons[XINPUT_BUTTON_LEFT_THUMB] != 0);

	case XINPUT_ANALOG_ID::RIGHT_STICK_PUSH:
		return (_xInputState.Buttons[XINPUT_BUTTON_RIGHT_THUMB] != 0);

	default:
		break;
	}

	return false;
}

VECTOR KeyConfInputManager::GetLeftStickDirection(void) const
{
	float normalX = 0.0f;
	float normalY = 0.0f;

	CalculateNormalizedStick(stickInfo_.leftStickX, stickInfo_.leftStickY,
		LEFT_STICK_DEAD_ZONE, normalX, normalY);

	if (normalX == 0.0f && normalY == 0.0f)
	{
		return UtilityMath::VECTOR_ZERO;
	}

	return UtilityMath::VNormalize(VGet(normalX, 0.0f, normalY));
}

Vector2F KeyConfInputManager::GetRightStick(void) const
{
	float processedX = 0.0f;
	float processedY = 0.0f;

	ApplyRightStickSensitivity(stickInfo_.rightStickX, stickInfo_.rightStickY, processedX, processedY);

	return Vector2F(processedX, processedY);
}

Vector2F KeyConfInputManager::GetLeftStickRaw(void) const
{
	float normalX = static_cast<float>(stickInfo_.leftStickX) / XINPUT_VAL_MAX;
	float normalY = static_cast<float>(stickInfo_.leftStickY) / XINPUT_VAL_MAX;

	float length = sqrtf(normalX * normalX + normalY * normalY);

	if (length < LEFT_STICK_DEAD_ZONE)
	{
		return UtilityMath::VECTOR2F_ZERO;
	}

	return Vector2F(normalX, normalY);
}

Vector2F KeyConfInputManager::GetRightStickRaw(void) const
{
	float normalX = static_cast<float>(stickInfo_.rightStickX) / XINPUT_VAL_MAX;
	float normalY = static_cast<float>(stickInfo_.rightStickY) / XINPUT_VAL_MAX;

	float length = sqrtf(normalX * normalX + normalY * normalY);

	if (length < rightStickSensitivity_.deadZone)
	{
		return UtilityMath::VECTOR2F_ZERO;
	}

	return Vector2F(normalX, normalY);
}

void KeyConfInputManager::SetRightStickSensitivity(const RightStickSensitivity& _sensitivity)
{
	rightStickSensitivity_ = _sensitivity;
}

void KeyConfInputManager::SaveInputTable(void)
{
	FILE* file = nullptr;
	fopen_s(&file, FILE_PATH_KEY_CONFIG, "wb");

	if (!file)
	{
		return;
	}

	// ヘッダー情報
	KeyConfigHeader header{};
	memcpy(header.signature, SIGNATURE_KEY_CONFIG, SIGNATURE_LENGTH);
	header.version = 1.0f;
	header.dataNum = static_cast<uint32_t>(inputTable_.size());
	fwrite(&header, sizeof(header), 1, file);

	for (const auto& pair : inputTable_)
	{
		// 名前の文字数
		uint8_t nameSize = static_cast<uint8_t>(pair.first.size());
		fwrite(&nameSize, sizeof(nameSize), 1, file);
		fwrite(pair.first.data(), nameSize, 1, file);

		// 割り当てられているキーの数
		uint8_t dataSize = static_cast<uint8_t>(pair.second.size());
		fwrite(&dataSize, sizeof(dataSize), 1, file);
		fwrite(pair.second.data(), sizeof(InputInfo), dataSize, file);
	}

	fclose(file);
}

void KeyConfInputManager::LoadInputTable(void)
{
	// ファイル読み込み
	auto handle = FileRead_open(FILE_PATH_KEY_CONFIG);

	if (handle == 0)
	{
		return;
	}

	// ヘッダー情報
	KeyConfigHeader header{};
	FileRead_read(&header, sizeof(header), handle);

	if (memcmp(header.signature, SIGNATURE_KEY_CONFIG, SIGNATURE_LENGTH) != 0)
	{
		FileRead_close(handle);
		return;
	}

	for (uint32_t i = 0; i < header.dataNum; ++i)
	{
		// 名前の文字数
		uint8_t nameSize = 0;
		FileRead_read(&nameSize, sizeof(nameSize), handle);

		// イベント名
		std::string eventName(nameSize, '\0');
		FileRead_read(eventName.data(), nameSize, handle);

		// 割り当てられているキーの数
		uint8_t dataSize = 0;
		FileRead_read(&dataSize, sizeof(dataSize), handle);

		inputTable_[eventName].resize(dataSize);
		FileRead_read(inputTable_[eventName].data(), sizeof(InputInfo) * dataSize, handle);
	}

	FileRead_close(handle);
}

void KeyConfInputManager::SaveSensitivitySettings(void) const
{
	FILE* file = nullptr;
	fopen_s(&file, FILE_PATH_SENSITIVITY, "wb");

	if (!file)
	{
		return;
	}

	// 感度設定用のヘッダー
	SensitivityHeader header{};
	memcpy(header.signature, SIGNATURE_SENSITIVITY, SIGNATURE_LENGTH);
	fwrite(&header, sizeof(header), 1, file);
	fwrite(&mouseSensitivity_, sizeof(mouseSensitivity_), 1, file);
	fwrite(&rightStickSensitivity_, sizeof(rightStickSensitivity_), 1, file);

	fclose(file);
}

void KeyConfInputManager::LoadSensitivitySettings(void)
{
	// ファイル読み込み
	auto handle = FileRead_open(FILE_PATH_SENSITIVITY);

	if (handle == 0)
	{
		return;
	}

	SensitivityHeader header{};
	FileRead_read(&header, sizeof(header), handle);

	if (memcmp(header.signature, SIGNATURE_SENSITIVITY, SIGNATURE_LENGTH) != 0)
	{
		FileRead_close(handle);
		return;
	}

	FileRead_read(&mouseSensitivity_, sizeof(mouseSensitivity_), handle);
	FileRead_read(&rightStickSensitivity_, sizeof(rightStickSensitivity_), handle);

	FileRead_close(handle);
}

void KeyConfInputManager::SetUsePadNumber(int _padNumber)
{
	usePadNumber_ = _padNumber;
}