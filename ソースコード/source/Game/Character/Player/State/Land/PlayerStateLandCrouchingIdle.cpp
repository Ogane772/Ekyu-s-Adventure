#include "PlayerStateLandCrouchingIdle.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandCrouchingIdle::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CROUCHING);
    }
}

EPlayerStateType CPlayerStateLandCrouchingIdle::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->UpdateLanding(DeltaTime);

    if (m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    if (m_Owner->IsSlopeSlip())
    {
        return EPlayerStateType::LAND_SLOPE_SLIP;
    }

    if (!m_Owner->IsCrouchingPress())
    {
        return EPlayerStateType::LAND_IDLE;
    }

    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::LAND_CROUCHING_JUMP;
    }
    if (m_Owner->IsTailAttackTrigger())
    {
        return EPlayerStateType::LAND_TAIL_ATTACK;
    }

    if (m_Owner->IsInputMove())
    {
        return EPlayerStateType::LAND_CROUCHING_MOVE;
    }

    return EPlayerStateType::LAND_CROUCHING_IDLE;
}