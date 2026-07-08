#include "MGBase.h"
#include "../../../../../../Manager/Decoration/SoundManager.h"
#include "../../../../../../Manager/Decoration/EffectManager.h"
#include "../../../../../../Utility/UtilityMath.h"
#include "../../../../../Collision/CollisionController.h"
#include "../../Bullet/Boss/BBulletMG.h"

MGBase::MGBase(void)
    : bulletDir_{ 0.0f, 0.0f, 0.0f }
    , bulletCount_(MAX_BULLET_COUNT)
    , muzzlePos_{ 0.0f, 0.0f, 0.0f }
    , muzzleCount_(0)
    , isAttack_(false)
{
}

MGBase::~MGBase(void)
{
}

void MGBase::SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos)
{
    bone_.id = _id;
    bone_.transform = _trans;
    bone_.playerPos = _playerPos;
    tag_ = _tag;
}

const VECTOR MGBase::GetPos(void) const
{
    VECTOR localRotPos = transform_.quaRot.PosAxis(localPos_);
    return VAdd(transform_.pos, localRotPos);
}

void MGBase::ChangeState(STATE _state)
{
    state_ = _state;

    int state = static_cast<int>(state_);

    // 各状態遷移の初期処理
    ChangeState(state);
}

void MGBase::UpdateCommon(void)
{
 
    // HPがなくなったら死亡処理（左右共通）
    if (hp_ <= 0 && isAlive_)
    {
        ChangeState(static_cast<int>(STATE::END));
    }
   
    for (std::shared_ptr<BBulletBase>  bullet : bullets_)
    {
        bullet->Update();
        
    }
    
        stateUpdate_();
  
}

void MGBase::LookPlayer(void)
{
    VECTOR moveDir = VSub(bone_.playerPos, transform_.pos);
    moveDir = VNorm(moveDir);

    float horizontalDistance = sqrtf(moveDir.z * moveDir.z + moveDir.x * moveDir.x);
    float targetAngleRad = atan2(moveDir.y, horizontalDistance);

    // 度数法に変換して制限をかける
    float targetAngleDeg = UtilityMath::Rad2DegF(targetAngleRad);
    targetAngleDeg = std::clamp(targetAngleDeg, LIMIT_MIN_ANGLE, LIMIT_MAX_ANGLE);

    // クオータニオンを作成してボスの回転と合成
    targetAngleRad = UtilityMath::Deg2RadF(targetAngleDeg);
    Quaternion quaRot = Quaternion::AngleAxis(-targetAngleRad, UtilityMath::AXIS_X);

    quaRot = Quaternion::Mult(quaRot, Quaternion::AngleAxis((targetAngleRad*look), UtilityMath::AXIS_Y));

    transform_.quaRot = Quaternion::Mult(bone_.transform.quaRot, quaRot);

    Quaternion bulletRot = transform_.quaRot;

    //発射向きのランダム化
    float randDirX = UtilityMath::RandRangeF(-5, 5);
    bulletRot = Quaternion::Mult(bulletRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(randDirX), UtilityMath::AXIS_X));
    float randDirY = UtilityMath::RandRangeF(-5, 5);
    bulletRot = Quaternion::Mult(bulletRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(randDirY), UtilityMath::AXIS_Y));


    //発射方向の計算
    VECTOR forward = VGet(0.0f, 0.0f, 1.0f);
    MATRIX rotationMatrix = Quaternion::ToMatrix(bulletRot);
    VECTOR bulletDir = VTransformSR(forward, rotationMatrix);
    bulletDir_ = VNorm(bulletDir);
}

void MGBase::ChangeState(int state)
{
    stateBase_ = state;
    // 各状態遷移の初期処理
    stateChanges_[stateBase_]();
}

void MGBase::ChangeStateIdle(void)
{
    stateUpdate_ = std::bind(&MGBase::UpdateIdle, this);
}

void MGBase::ChangeStateAttack(void)
{
    stateUpdate_ = std::bind(&MGBase::UpdateAttack, this);
    bulletCount_ = MAX_BULLET_COUNT;
    isAttack_ = true;
    

    VECTOR effectRot=transform_.quaRot.GetForward();

    EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_MG, effectPos_, effectRot, { 10.0f,10.0f,10.0f }, 1.0f,this);
}

void MGBase::ChangeStateEnd(void)
{
    stateUpdate_ = std::bind(&MGBase::UpdateEnd, this);
    isAlive_ = false;
    CollisionController::GetInstance().SetCollisionActive(this, tag_, false);
}

void MGBase::UpdateIdle(void)
{
    transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
    LookPlayer();
}

void MGBase::UpdateAttack(void)
{
    bulletCount_--;
    transform_.pos = MV1GetFramePosition(bone_.transform.modelId, bone_.id);
    LookPlayer();

    // ローカル座標を回転させてワールド座標へ変換
    VECTOR localRotPos = transform_.quaRot.PosAxis(muzzlePos_[1]);
    // 位置を加算して最終的なワールド座標にする
    effectPos_ = VAdd(transform_.pos, localRotPos);
    EffectManager::GetInstance().UpdatePos(EffectManager::EFFECT::EFFECT_MG, this, effectPos_);
    Quaternion effectquaRot = Quaternion::Mult(transform_.quaRot, Quaternion::AngleAxis(UtilityMath::Deg2RadF(-90.0f), UtilityMath::AXIS_X));

    VECTOR effectRot = effectquaRot.ToEuler();
    effectRot.x = UtilityMath::Rad2DegF(effectRot.x);
    effectRot.y = UtilityMath::Rad2DegF(effectRot.y);
    effectRot.z = UtilityMath::Rad2DegF(effectRot.z);
    EffectManager::GetInstance().UpdateRot(EffectManager::EFFECT::EFFECT_MG, this, effectRot);

    if (EffectManager::GetInstance().IsPlaying(EffectManager::EFFECT::EFFECT_MG) == false)
    {
        EffectManager::GetInstance().Play(EffectManager::EFFECT::EFFECT_MG, effectPos_, effectRot, { 10.0f,10.0f,10.0f }, 1.0f, this);
    }
    

    SoundManager::GetInstance().Set3DPosition(SoundManager::SOUND::SE_MG_FIRE, transform_.pos);
    if (bulletCount_ >= 0) {
        CreateBullets();
    }
    if (bulletCount_ <= 0) {
        isAttack_ = false;
        SoundManager::GetInstance().Stop(SoundManager::SOUND::SE_MG_FIRE);
        ChangeState(STATE::IDLE);
    }
}

void MGBase::UpdateEnd(void)
{
}

void MGBase::CreateBullets(void)
{
    muzzleCount_++;
    if (muzzleCount_ > MUZZLE_MAX_COUNT - 1) {
        muzzleCount_ = 0;
    }
    std::shared_ptr<BBulletBase> bullet = GetValidBullet();
    // ローカル座標を回転させてワールド座標へ変換
    VECTOR localRotPos = transform_.quaRot.PosAxis(muzzlePos_[muzzleCount_]);

    // 位置を加算して最終的なワールド座標にする
    VECTOR bulletpos = VAdd(transform_.pos, localRotPos);

    bullet->CreateBullets(bulletpos, bulletDir_, 3.0f);
    bullet->Init();
    bullet->SetTransform(transform_);
}

std::shared_ptr<BBulletBase> MGBase::GetValidBullet(void)
{
    
    for (auto& bullet : bullets_) {
        if (!bullet->GetIsAlive()) return bullet;
    }
    std::shared_ptr<BBulletBase> bullet = std::make_shared<BBulletMG>();
    bullets_.emplace_back(bullet);
    bullet->Load();
    return bullet;
}