#include "SoundManager.h"

#include <DxLib.h>
#include <algorithm>

// シングルトンインスタンスの初期化
SoundManager* SoundManager::instance_ = nullptr;

// 排他制御用ミューテックスの定義
std::mutex SoundManager::g_soundMutex;

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
	std::lock_guard<std::mutex> lock(g_soundMutex);
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
	std::lock_guard<std::mutex> lock(g_soundMutex);

	auto it = sounds_.find(_sound);
	if (it != sounds_.end() && it->second.data > 0)
	{
		// 登録時に設定した再生モードで再生開始
		PlaySoundMem(it->second.data, it->second.playMode);
	}
}

void SoundManager::Stop(const SOUND _sound)
{
	std::lock_guard<std::mutex> lock(g_soundMutex);
	auto it = sounds_.find(_sound);
	if (it != sounds_.end())
	{
		StopSoundMem(it->second.data);
	}
}

void SoundManager::StopAllBGM(void)
{
	std::lock_guard<std::mutex> lock(g_soundMutex);
	for (auto& pair : sounds_)
	{
		// 管理マップの中からBGMタイプのものだけを抽出して停止
		if (pair.second.type == TYPE::BGM)
		{
			StopSoundMem(pair.second.data);
		}
	}
}

void SoundManager::Release(void)
{
	std::lock_guard<std::mutex> lock(g_soundMutex);
	for (auto& pair : sounds_)
	{
		// DXライブラリ側からメモリを解放
		DeleteSoundMem(pair.second.data);
	}
	sounds_.clear();
}

void SoundManager::AdjustVolume(const SOUND sound, const int percent)
{
	std::lock_guard<std::mutex> lock(g_soundMutex);
	auto it = sounds_.find(sound);
	if (it != sounds_.end())
	{
		// 個別の音量設定を適用（0〜255に変換）
		ChangeVolumeSoundMem(ToDxVolume(percent), it->second.data);
	}
}

bool SoundManager::IsPlaying(SOUND sound)
{
	std::lock_guard<std::mutex> lock(g_soundMutex);
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
	std::lock_guard<std::mutex> lock(g_soundMutex);
	for (auto& pair : sounds_)
	{
		// 種類に応じて現在のマスターボリュームを再計算して一括適用
		int targetPercent = (pair.second.type == TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
		ChangeVolumeSoundMem(ToDxVolume(targetPercent), pair.second.data);
	}
}