#include "PlayerStateLandCrouchingMove.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandCrouchingMove::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CROUCHING_WALK);
    }
    m_CountMoveSETime = 0.0f;
}

EPlayerStateType CPlayerStateLandCrouchingMove::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->InputMove(EPlayerSpeedParameterType::CROUCHING_MOVE, DeltaTime);

    m_Owner->UpdateLanding(DeltaTime);

    m_Owner->CheckMoveSE(m_CountMoveSETime);
    m_CountMoveSETime += DeltaTime;

    if (m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    if (m_Owner->IsSlopeSlip())
    {
        if (m_Owner->SlopeSlipMove(true, DeltaTime))
        {
            return EPlayerStateType::LAND_SLOPE_SLIP;
        }
    }

    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::LAND_CROUCHING_JUMP;
    }
    if (m_Owner->IsTailAttackTrigger())
    {
        return EPlayerStateType::LAND_TAIL_ATTACK;
    }

    if (!m_Owner->IsCrouchingPress())
    {
        return EPlayerStateType::LAND_MOVE;
    }

    if (!m_Owner->IsInputMove())
    {
        return EPlayerStateType::LAND_CROUCHING_IDLE;
    }

    return EPlayerStateType::LAND_CROUCHING_MOVE;
}
