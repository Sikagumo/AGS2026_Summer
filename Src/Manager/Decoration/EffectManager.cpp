#include "EffekseerForDXLib.h"
#include "../../Application.h"
#include "../Generic/ResourceManager.h"
#include "../../Utility/UtilityMath.h"
#include "EffectManager.h"
#include <algorithm>

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

void EffectManager::Initialize(void)
{
    effect_.clear();
    playingList_.clear();

    EFFECT_DATA wave = EFFECT_DATA();
    wave.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_WAVE);
    effect_[EFFECT::EFFECT_WAVE] = wave;

    EFFECT_DATA loading = EFFECT_DATA();
    loading.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_LANDING);
    effect_[EFFECT::EFFECT_LANDING] = loading;

    EFFECT_DATA mg = EFFECT_DATA();
    mg.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_MG);
    effect_[EFFECT::EFFECT_MG] = mg;

    EFFECT_DATA bossHit = EFFECT_DATA();
    bossHit.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_BOSS_HIT);
    effect_[EFFECT::EFFECT_BOSS_HIT] = bossHit;

    EFFECT_DATA laser = EFFECT_DATA();
    laser.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_LASER);
    effect_[EFFECT::EFFECT_LASER] = laser;

    EFFECT_DATA missile = EFFECT_DATA();
    missile.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_MISSILE);
    effect_[EFFECT::EFFECT_MISSILE] = missile;

    EFFECT_DATA pBlast = EFFECT_DATA();
    pBlast.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_PLAYER_BLAST);
    effect_[EFFECT::EFFECT_PLAYER_BLAST] = pBlast;

    EFFECT_DATA pRecovery = EFFECT_DATA();
    pRecovery.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_PLAYER_RECOVERY);
    effect_[EFFECT::EFFECT_PLAYER_RECOVERY] = pRecovery;

    EFFECT_DATA pPoison = EFFECT_DATA();
    pPoison.Data = ResourceManager::GetInstance().LoadHandleId(ResourceManager::SRC::EFFECT_PLAYER_POISON);
    effect_[EFFECT::EFFECT_PLAYER_POISON] = pPoison;

}

void EffectManager::Play(const EFFECT _effect, const VECTOR _pos, const VECTOR _rot, const VECTOR _scl, float _speed, const void* _owner, int _tag)
{
    auto it = effect_.find(_effect);
    if (it == effect_.end())
    {
        return;
    }

    // 引数のパラメータを保持
    it->second.pos = _pos;
    it->second.rot = _rot;
    it->second.scl = _scl;
    it->second.speed = _speed;

    int playHandle = PlayEffekseer3DEffect(it->second.Data);

    // 再生成功時、各種パラメータを設定する
    if (playHandle != -1)
    {
        SetPosPlayingEffekseer3DEffect(playHandle, it->second.pos.x, it->second.pos.y, it->second.pos.z);
       
        SetRotationPlayingEffekseer3DEffect(playHandle, UtilityMath::Deg2RadD(it->second.rot.x), UtilityMath::Deg2RadD(it->second.rot.y), UtilityMath::Deg2RadD(it->second.rot.z));
       
        SetScalePlayingEffekseer3DEffect(playHandle, it->second.scl.x, it->second.scl.y, it->second.scl.z);
        SetSpeedPlayingEffekseer3DEffect(playHandle, it->second.speed);

        PLAYING_EFFECT activeEffect;
        activeEffect.effectId = _effect;
        activeEffect.owner = _owner;
        activeEffect.tag = _tag;
        activeEffect.playHandle = playHandle;
        playingList_.push_back(activeEffect);
    }
}

bool EffectManager::IsPlaying(EFFECT _effect, void* _owner, int _tag)
{
    for (const auto& active : playingList_)
    {
        if (active.owner == _owner && active.tag == _tag)
        {
            if (active.effectId == _effect)
            {
                if (IsEffekseer3DEffectPlaying(active.playHandle) == 0)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void EffectManager::Stop(EFFECT _effect, void* _owner, int _tag)
{
    for (const auto& active : playingList_)
    {
        if (active.owner == _owner && active.tag == _tag)
        {
            if (active.effectId == _effect)
            {
                StopEffekseer3DEffect(active.playHandle);
            }
        }
    }
}

void EffectManager::UpdatePos(const EFFECT _effect, const void* _owner, const VECTOR _pos, int _tag)
{
    for (const auto& active : playingList_)
    {
        if (active.owner == _owner && active.tag == _tag)
        {
            if (active.effectId == _effect)
            {
                SetPosPlayingEffekseer3DEffect(active.playHandle, _pos.x, _pos.y, _pos.z);
            }
        }
    }
}

void EffectManager::UpdateRot(const EFFECT _effect, const void* _owner, const VECTOR _rot, int _tag)
{
   
    for (const auto& active : playingList_)
    {
        if (active.owner == _owner && active.tag == _tag)
        {
            if (active.effectId == _effect)
            {
                SetRotationPlayingEffekseer3DEffect(active.playHandle, UtilityMath::Deg2RadD(_rot.x), UtilityMath::Deg2RadD(_rot.y), UtilityMath::Deg2RadD(_rot.z));
            }
        }
    }
}

void EffectManager::UpdateScl(const EFFECT _effect, const void* _owner, const VECTOR _scl, int _tag)
{
    for (const auto& active : playingList_)
    {
        if (active.owner == _owner && active.tag == _tag)
        {
            if (active.effectId == _effect)
            {
                SetScalePlayingEffekseer3DEffect(active.playHandle, _scl.x, _scl.y, _scl.z);
            }
        }
    }
}

// =========================================================================

void EffectManager::Update(void)
{
    UpdateEffekseer3D();

    
    playingList_.erase(
        std::remove_if(playingList_.begin(), playingList_.end(), [](const PLAYING_EFFECT& active) {
            return IsEffekseer3DEffectPlaying(active.playHandle) != 0;
            }),
        playingList_.end()
    );
}

void EffectManager::Draw(void)
{
    DrawEffekseer3D();
}