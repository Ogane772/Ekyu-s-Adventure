#include "PlayerStateLandNormalJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandNormalJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->SetJumpAnimationAndJumpPower();
    }
}

EPlayerStateType CPlayerStateLandNormalJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->CheckUpWallHit();

    if (m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::FINAL_JUMP_START))
    {
        m_Owner->ChangeFinalJumpLoop();
    }

    m_Owner->UpdateJumpFall(DeltaTime);

    m_Owner->InputMove(EPlayerSpeedParameterType::AIR_MOVE, DeltaTime);

    m_Owner->JumpSlipMove(DeltaTime);

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

    if (m_Owner->IsJumpUp())
    {
        return EPlayerStateType::LAND_NORMAL_JUMP;
    }

    if (m_Owner->IsCliffHit())
    {
        return EPlayerStateType::AIR_CLIFF;
    }

    if (m_Owner->CheckWallSlip())
    {
        return EPlayerStateType::AIR_WALL_SLIP;
    }

    //—Ž‰º’†‚©‚Â’n–Ê‚É•t‚¯‚Î’…’n‚Æ”»’è
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
        else
        {
            return EPlayerStateType::LAND_IDLE;
        }
    }

    return EPlayerStateType::LAND_NORMAL_JUMP;
}

void CPlayerStateLandNormalJump::Exit()
{
    if (m_Owner)
    {
        m_Owner->DestroyFinalJumpSE();
    }
}
