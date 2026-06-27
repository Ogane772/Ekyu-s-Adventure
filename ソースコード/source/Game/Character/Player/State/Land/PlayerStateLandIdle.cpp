#include "PlayerStateLandIdle.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

namespace
{
    constexpr int PINCH_HP = 2;
};

void CPlayerStateLandIdle::Enter()
{
    if (!m_Owner)
    {
        return;
    }
    if (m_Owner->GetHP().GetCurrentHP() <= PINCH_HP)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::IDLE_PINCH);
    }
    else
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::IDLE);
    }
    m_Owner->ResetSpeed();
}

EPlayerStateType CPlayerStateLandIdle::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    if(m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    m_Owner->UpdateLanding(DeltaTime);
    if (m_Owner->IsSlopeSlip())
    {
        return EPlayerStateType::LAND_SLOPE_SLIP;
    }
    if (m_Owner->IsCrouchingPress())
    {
        return EPlayerStateType::LAND_CROUCHING_IDLE;
    }
    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::LAND_NORMAL_JUMP;
    }
    if (m_Owner->IsTailAttackTrigger())
    {
        return EPlayerStateType::LAND_TAIL_ATTACK;
    }

    if(m_Owner->IsInputMove())
    {
        return EPlayerStateType::LAND_MOVE;
    }

    return EPlayerStateType::LAND_IDLE;
}