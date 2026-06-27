#include "PlayerStateAirFall.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateAirFall::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::FALL);
        m_Owner->ResetJumpPower();
    }
}

EPlayerStateType CPlayerStateAirFall::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    const bool isInputMove = m_Owner->IsInputMove();

    m_Owner->InputMove(EPlayerSpeedParameterType::AIR_MOVE, DeltaTime);
    m_Owner->FallMove(DeltaTime);

    if (m_Owner->IsDiveJumpTrigger())
    {
        return EPlayerStateType::AIR_DIVE_JUMP;
    }
    if (m_Owner->IsHipDropTrigger())
    {
        return EPlayerStateType::AIR_HIPDROP;
    }
    if (m_Owner->IsCliffHit())
    {
        return EPlayerStateType::AIR_CLIFF;
    }
    if (m_Owner->CheckWallSlip())
    {
        return EPlayerStateType::AIR_WALL_SLIP;
    }
    if (m_Owner->IsSpinJumpTrigger())
    {
        return EPlayerStateType::AIR_SPIN_JUMP;
    }

    if (m_Owner->CheckGround() != ELandedState::AIR)
    {
        m_Owner->OnLanding();
        if (m_Owner->IsSlopeSlip())
        {
            return EPlayerStateType::LAND_SLOPE_SLIP;
        }
        if (isInputMove)
        {
            return EPlayerStateType::LAND_MOVE;
        }
        else
        {
            return EPlayerStateType::LAND_IDLE;
        }
    }

    return EPlayerStateType::AIR_FALL;
}
