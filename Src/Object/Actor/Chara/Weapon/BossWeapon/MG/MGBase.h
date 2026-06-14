#pragma once
#include "../../WeaponBase.h"
#include <algorithm>
#include <vector>

class BBulletBase;

class MGBase:public WeaponBase
{
public:

	MGBase(void);
	virtual ~MGBase(void);
    // WeaponBaseからのオーバーライド
    void SetBone(int _id, Transform _trans, ColliderBase::TAG _tag, VECTOR _playerPos) override;
    const VECTOR GetPos(void) const override;
    void SetDamage(int _damage) override { hp_ -= _damage; }
    bool GetIsAlive(void) override { return isAlive_; }
    int GetHp(void) override { return hp_; }
    // 状態遷移
    void ChangeState(STATE _state)override;

protected:

    // 回転制限の定数（度数法）
    static constexpr float LIMIT_MIN_ANGLE = -30.0f;
    static constexpr float LIMIT_MAX_ANGLE = 20.0f;
    static constexpr int MAX_BULLET_COUNT = 200;
    static constexpr int MUZZLE_MAX_COUNT = 6;

    static constexpr float CAPSULE_RADIUS = 20.0f;

    // 子クラス(MGL/MGR)のUpdateProcessから呼ばれる共通の更新関数
    void UpdateCommon(void);

    /// プレイヤーを狙う（回転制限付き）
    void LookPlayer(void) ;

    // 弾を発射する共通の仕組み（中身は現在のMGL/MGRのものと同じ）
    virtual void CreateBullets(void);
    std::shared_ptr<BBulletBase> GetValidBullet(void);

    // 左右で共通して使う変数メンバ
    VECTOR bulletDir_;
    int bulletCount_;
    VECTOR muzzlePos_[MUZZLE_MAX_COUNT];
    int muzzleCount_;
    std::vector<std::shared_ptr<BBulletBase>> bullets_;
    int look;

    // 状態（ステート）管理用の関数ポインタと関数群
    std::function<void(void)> stateUpdate_;
    void ChangeState(int state) override;
    void ChangeStateIdle(void) override;
    void ChangeStateAttack(void) override;
    void ChangeStateEnd(void) override;

    void UpdateAttack(void) override;
    void UpdateIdle(void) override;
    void UpdateEnd(void) override;

    // 左右で変わるパラメータ（子クラスの初期化時に設定してもらう）
    VECTOR localPos_ = { 0.0f, 0.0f, 0.0f };

  

};

