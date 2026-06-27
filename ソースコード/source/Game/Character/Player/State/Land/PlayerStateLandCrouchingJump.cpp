#include "PlayerStateLandCrouchingJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandCrouchingJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::CROUCHING_JUMP);
        m_Owner->StartCrouchingJump();
    }
}

EPlayerStateType CPlayerStateLandCrouchingJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->CheckUpWallHit();

    m_Owner->BackJumpMove(EPlayerSpeedParameterType::CROUCHING_JUMP, DeltaTime);

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
        return EPlayerStateType::LAND_CROUCHING_JUMP;
    }

    if (m_Owner->IsCliffHit())
    {
        return EPlayerStateType::AIR_CLIFF;
    }

    if (m_Owner->CheckWallSlip())
    {
        m_Owner->ReverseRotationYaw();
        return EPlayerStateType::AIR_WALL_SLIP;
    }

    //—Ž‰º’†‚©‚Â’n–Ê‚É•t‚¯‚Î’…’n‚Æ”»’è
    if (m_Owner->CheckGround() != ELandedState::AIR)
    {
        m_Owner->OnLanding();
        return EPlayerStateType::LAND_IDLE;
    }

    return EPlayerStateType::LAND_CROUCHING_JUMP;
}

void CPlayerStateLandCrouchingJump::Exit()
{
    if (m_Owner)
    {
        m_Owner->SetNotBlendAnimation(true);
    }
}
