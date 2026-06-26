#include "EffekseerForDXLib.h"
#include "EffectManager.h"

// 静的メンバ変数の実体定義
EffectManager* EffectManager::instance_ = nullptr;

void EffectManager::CreateInstance(void)
{
    if (instance_ == nullptr)
    {
        instance_ = new EffectManager();
    }
}

EffectManager& EffectManager::GetInstance(void)
{
   
    return *instance_;
}

void EffectManager::DestroyInstance(void)
{
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}

void EffectManager::Update(void)
{
    UpdateEffekseer3D();
}

void EffectManager::Draw(void)
{
    DrawEffekseer3D();
}

void EffectManager::Initialize(void)
{
    effect_.clear();

}

void EffectManager::Add(const EFFECT _effect, const int _data)
{
    EFFECT_DATA newData;
    newData.Data = _data;
    newData.pos = VGet(0.0f, 0.0f, 0.0f);
    newData.rad = VGet(0.0f, 0.0f, 0.0f);
    newData.scl = VGet(1.0f, 1.0f, 1.0f);
    newData.speed = 1.0f;

    effect_[_effect] = newData;
}

void EffectManager::Play(const EFFECT _effect, const VECTOR _pos, const VECTOR _rot, const VECTOR _scl, float _speed)
{
    auto it = effect_.find(_effect);
    if (it == effect_.end())
    {
        return;
    }

    // 引数のパラメータを保持
    it->second.pos = _pos;
    it->second.rad = _rot;
    it->second.scl = _scl;
    it->second.speed = _speed;

   
    int playHandle = PlayEffekseer3DEffect(it->second.Data);

    // 再生成功時各種パラメータを設定する
    if (playHandle != -1)
    {
        SetPosPlayingEffekseer3DEffect(playHandle, _pos.x, _pos.y, _pos.z);
        SetRotationPlayingEffekseer3DEffect(playHandle, _rot.x, _rot.y, _rot.z);
        SetScalePlayingEffekseer3DEffect(playHandle, _scl.x, _scl.y, _scl.z);
        SetSpeedPlayingEffekseer3DEffect(playHandle, _speed);
    }
}

bool EffectManager::IsPlaying(EFFECT _effect)
{
    auto it = effect_.find(_effect);
    if (it == effect_.end())
    {
        return false;
    }

   
    // 戻り値は「0:再生中、-1:再生終了」なので、0のときが true になります
    if (IsEffekseer3DEffectPlaying(it->second.Data) == 0)
    {
        return true;
    }

    return false;
}