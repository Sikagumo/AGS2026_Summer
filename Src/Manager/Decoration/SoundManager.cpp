#include "SoundManager.h"


#include <algorithm>

// シングルトンインスタンスの初期化
SoundManager* SoundManager::instance_ = nullptr;

// 排他制御用ミューテックスの定義
std::recursive_mutex SoundManager::g_soundMutex;

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
}

SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

void SoundManager::Initialize(void)
{
	// 初期音量の設定
	masterVolumeBGM_ = DEFAULT_BGM_VOLUME;
	masterVolumeSE_ = DEFAULT_SE_VOLUME;
}

void SoundManager::Add(const TYPE type, const SOUND sound, const int _data)
{
	// マップへの追加をスレッドセーフにする
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	if (sounds_.find(sound) != sounds_.end()) { return; }

	// BGMならループ再生、SEならバックグラウンド再生（単発）を設定
	int mode = (type == TYPE::BGM) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	sounds_.emplace(sound, SOUND_DATA{ _data, type, mode });

	// 登録した瞬間のマスターボリュームをハンドルに即座に反映させる
	int targetVol = (type == TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
	ChangeVolumeSoundMem(ToDxVolume(targetVol), _data);
}

void SoundManager::Play(const SOUND _sound)
{
	if (instance_ == nullptr) return;
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);

	auto it = sounds_.find(_sound);
	if (it != sounds_.end() && it->second.data > 0)
	{
		// 登録時に設定した再生モードで再生開始
		PlaySoundMem(it->second.data, it->second.playMode);
	}
}

void SoundManager::Play3D(const SOUND _sound, const VECTOR _playPos, const VECTOR _listenPos, const float _radius)
{
	if (instance_ == nullptr) return;
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	auto it = sounds_.find(_sound);
	if (it != sounds_.end() && it->second.data > 0)
	{
		it->second.is3D = true;
		it->second.playPos = _playPos;
		it->second.radius = _radius;
		int volume = 0;
		VECTOR soundDiff = VSub(_playPos, _listenPos);
		float soundDistance = VSize(soundDiff);
		if (soundDistance <= _radius)
		{
			float volumeRatio = 1.0f - (soundDistance / _radius);
			volume = static_cast<int>(static_cast<float>(masterVolumeSE_)* volumeRatio);
		}
		AdjustVolume(_sound, volume);
		PlaySoundMem(it->second.data, it->second.playMode);
	}

}

void SoundManager::Update3D(const VECTOR _listenPos)
{
	if (instance_ == nullptr)return;
	std::lock_guard<std::recursive_mutex>lock(g_soundMutex);

	for (auto& pair : sounds_)
	{
		if (pair.second.is3D && CheckSoundMem(pair.second.data) == 1)
		{
			VECTOR  soundDiff = VSub(pair.second.playPos, _listenPos);
			float soundDistance = VSize(soundDiff);

			int volume = 0;
			if (soundDistance <= pair.second.radius)
			{
				float volumeRatio = 1.0f - (soundDistance/pair.second.radius);
				volume = static_cast<int>(static_cast<float>(masterVolumeSE_) * volumeRatio);
			}
			else
			{
				volume = 0;
			}
			AdjustVolume(pair.first, volume);
		}
	}

}

void SoundManager::Stop(const SOUND _sound)
{
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	auto it = sounds_.find(_sound);
	if (it != sounds_.end())
	{
		StopSoundMem(it->second.data);
	}
}

void SoundManager::StopAllBGM(void)
{
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	for (auto& pair : sounds_)
	{
		// 管理マップの中からBGMタイプのものだけを抽出して停止
		if (pair.second.type == TYPE::BGM)
		{
			StopSoundMem(pair.second.data);
		}
	}
}

void SoundManager::Set3DPosition(const SOUND _sound, const VECTOR _newPos)
{
	if (instance_ == nullptr) return;
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);

	auto it = sounds_.find(_sound);
	if (it != sounds_.end() && it->second.is3D)
	{
		it->second.playPos = _newPos;
	}
}

void SoundManager::Release(void)
{
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	for (auto& pair : sounds_)
	{
		// DXライブラリ側からメモリを解放
		DeleteSoundMem(pair.second.data);
	}
	sounds_.clear();
}

void SoundManager::AdjustVolume(const SOUND sound, const int percent)
{
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	auto it = sounds_.find(sound);
	if (it != sounds_.end())
	{
		// 個別の音量設定を適用（0〜255に変換）
		ChangeVolumeSoundMem(ToDxVolume(percent), it->second.data);
	}
}

bool SoundManager::IsPlaying(SOUND sound)
{
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	auto it = sounds_.find(sound);
	if (it == sounds_.end()) { return false; }

	// DxLib仕様：1なら再生中、0なら停止中
	return CheckSoundMem(it->second.data) == 1;
}

void SoundManager::DestroyInstance(void)
{
	// 全音声を削除してからインスタンスを破棄
	Release();
	delete instance_;
	instance_ = nullptr;
}

void SoundManager::SetMasterVolumeBGM(int volume)
{
	// 0〜100の範囲内に制限して保存し、全体の音量を更新
	masterVolumeBGM_ = std::clamp(volume, MIN_PERCENT, MAX_PERCENT);
	ApplyMasterVolumes();
}

void SoundManager::SetMasterVolumeSE(int volume)
{
	masterVolumeSE_ = std::clamp(volume, MIN_PERCENT, MAX_PERCENT);
	ApplyMasterVolumes();
}

void SoundManager::ApplyMasterVolumes(void)
{
	std::lock_guard<std::recursive_mutex> lock(g_soundMutex);
	for (auto& pair : sounds_)
	{
		// 種類に応じて現在のマスターボリュームを再計算して一括適用
		int targetPercent = (pair.second.type == TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
		ChangeVolumeSoundMem(ToDxVolume(targetPercent), pair.second.data);
	}
}