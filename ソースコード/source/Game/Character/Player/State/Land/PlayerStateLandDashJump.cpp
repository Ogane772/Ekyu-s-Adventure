#include "PlayerStateLandDashJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandDashJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::DASH_JUMP);
        m_Owner->StartDashJump();
    }
}

EPlayerStateType CPlayerStateLandDashJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    const bool isJumpUp = m_Owner->IsJumpUp();

    m_Owner->CheckUpWallHit();

    m_Owner->FrontInputMove(EPlayerSpeedParameterType::DASH_JUMP, DeltaTime, !isJumpUp);
    m_Owner->UpdateJumpFall(DeltaTime);

    if (m_Owner->IsWallHit())
    {
        return EPlayerStateType::COMMON_WALL_HIT;
    }

    if (isJumpUp)
    {
        return EPlayerStateType::LAND_DASH_JUMP;
    }

    //—Ž‰º’†‚©‚Â’n–Ê‚É•t‚¯‚Î’…’n‚Æ”»’è
    if (m_Owner->CheckGround() != ELandedState::AIR)
    {
        m_Owner->OnLanding();
        return EPlayerStateType::LAND_IDLE;
    }

    return EPlayerStateType::LAND_DASH_JUMP;
}