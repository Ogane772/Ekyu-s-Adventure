#include "PlayerStateLandTailAttack.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

namespace
{
    constexpr float TAIL_ATTACK_MOVE_TIME = 0.45f;
};

void CPlayerStateLandTailAttack::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::TAIL_ATTACK, false);
        m_Owner->StartTailAttack();
        m_CountTalkAttackTime = 0.0f;
    }
}

EPlayerStateType CPlayerStateLandTailAttack::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    if (m_CountTalkAttackTime < TAIL_ATTACK_MOVE_TIME)
    {
        m_Owner->FrontMove(EPlayerSpeedParameterType::TAIL_ATTACK, DeltaTime);
    }
    m_Owner->UpdateTailAttackCollision(m_CountTalkAttackTime);
    m_CountTalkAttackTime += DeltaTime;

    m_Owner->UpdateLanding(DeltaTime);

    if (m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    if (m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::TAIL_ATTACK))
    {
        return EPlayerStateType::LAND_IDLE;

    }
    return EPlayerStateType::LAND_TAIL_ATTACK;
}

void CPlayerStateLandTailAttack::Exit()
{
    if (m_Owner)
    {
        m_Owner->DisableAttackSphere();
        m_Owner->SetNotBlendAnimation(true);
    }
}