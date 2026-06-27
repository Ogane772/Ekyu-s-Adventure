#include "PlayerStateAirDiveJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateAirDiveJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::DIVE_JUMP);
        m_Owner->StartDiveJump();
    }
}

EPlayerStateType CPlayerStateAirDiveJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    m_Owner->CheckUpWallHit();
 
    m_Owner->DiveJumpMove(DeltaTime);

    if (m_Owner->IsCliffHit())
    {
        return EPlayerStateType::AIR_CLIFF;
    }
    if (m_Owner->IsWallHit())
    {
        return EPlayerStateType::COMMON_WALL_HIT;
    }

    //—Ž‰º’†‚©‚Â’n–Ê‚É•t‚¯‚Î’…’n‚Æ”»’è
    if (!m_Owner->IsJumpUp())
    {
        if (m_Owner->CheckGround() != ELandedState::AIR)
        {
            m_Owner->OnLanding();
            if (m_Owner->IsSlopeSlip())
            {
                return EPlayerStateType::LAND_SLOPE_SLIP;
            }
            if (m_Owner->IsInputMove())
            {
                return EPlayerStateType::LAND_MOVE;
            }
            return EPlayerStateType::LAND_IDLE;
        }
    }

    return EPlayerStateType::AIR_DIVE_JUMP;
}

void CPlayerStateAirDiveJump::Exit()
{
    if (m_Owner)
    {
        m_Owner->DisableAttackSphere();
    }
}
