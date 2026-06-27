#include "PlayerStateLandTurnJump.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateLandTurnJump::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::MOVE_CHANGE_JUMP);
        m_Owner->StartTurnJump();
    }
}

EPlayerStateType CPlayerStateLandTurnJump::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    m_Owner->CheckUpWallHit();

    m_Owner->BackJumpMove(EPlayerSpeedParameterType::TURN_JUMP, DeltaTime);

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
        return EPlayerStateType::LAND_TURN_JUMP;
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
        return EPlayerStateType::LAND_IDLE;
    }

    return EPlayerStateType::LAND_TURN_JUMP;
}

void CPlayerStateLandTurnJump::Exit()
{
    if (m_Owner)
    {
        //ƒ‚[ƒVƒ‡ƒ“‘¤‚Å”½“]‚µ‚Ä‚¢‚é‚½‚ß‰ñ“]’l‚ð‡‚í‚¹‚é‚½‚ß”½“]‚·‚é
        m_Owner->ReverseRotationYaw();
        m_Owner->SetNotBlendAnimation(true);
    }
}
