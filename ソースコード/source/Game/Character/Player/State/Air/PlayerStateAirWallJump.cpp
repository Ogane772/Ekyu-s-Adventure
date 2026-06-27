#include "PlayerStateAirWallJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateAirWallJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::JUMP_START);
        m_Owner->StartWallJump();
    }
}

EPlayerStateType CPlayerStateAirWallJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    const bool isJumpUp = m_Owner->IsJumpUp();

    m_Owner->CheckUpWallHit();

    m_Owner->FrontInputMove(EPlayerSpeedParameterType::WALL_JUMP, DeltaTime, !isJumpUp);
    m_Owner->UpdateJumpFall(DeltaTime);

    if (m_Owner->IsSpinJumpTrigger())
    {
        return EPlayerStateType::AIR_SPIN_JUMP;
    }

    if (m_Owner->IsDiveJumpTrigger())
    {
        return EPlayerStateType::AIR_DIVE_JUMP;
    }
    if (m_Owner->IsHipDropTrigger())
    {
        return EPlayerStateType::AIR_HIPDROP;
    }
    if (m_Owner->CheckWallSlip())
    {
        return EPlayerStateType::AIR_WALL_SLIP;
    }
    if (m_Owner->IsCliffHit())
    {
        return EPlayerStateType::AIR_CLIFF;
    }

    //—Ž‰º’†‚©‚Â’n–Ê‚É•t‚¯‚Î’…’n‚Æ”»’è
    if (!isJumpUp)
    {
        if (m_Owner->CheckGround() != ELandedState::AIR)
        {
            m_Owner->OnLanding();
            return EPlayerStateType::LAND_IDLE;
        }
    }

    return EPlayerStateType::AIR_WALL_JUMP;
}