#include "PlayerStateLandMove.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

namespace
{
    constexpr float NOT_TURN_TIME = 0.2f;//即時ターンの場合、待機から即ターンに移行してしまうため、猶予を設ける
    constexpr float CHANGE_IDLE_STATE_TIME = 0.1f;//	//ターンを入力しようとした際に一瞬スティックがニュートラルを経由すると待機に戻ってしまうため待機に戻るまでの猶予時間
};

void CPlayerStateLandMove::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::WALK);
    }
    m_CountMoveTime = 0.0f;
    m_CountNotInputMoveTime = 0.0f;
    m_CountMoveSETime = 0.0f;
}

EPlayerStateType CPlayerStateLandMove::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
        return EPlayerStateType::MAX;
    }

    if (m_Owner->CheckGround() == ELandedState::AIR)
    {
        return EPlayerStateType::AIR_FALL;
    }

    bool isTurnInput = false;

    if (m_CountMoveTime > NOT_TURN_TIME)
    {
        isTurnInput = m_Owner->IsTurnInput();
    }

    //ターンの場合は、LandMoveで回転が行われるとターン開始位置が今の位置よりズレるためLandMoveを行わない
    const bool isNotMove = isTurnInput;
    if (!isNotMove)
    {
        m_Owner->InputMove(EPlayerSpeedParameterType::LAND_MOVE, DeltaTime);
        m_Owner->CheckLandMoveAnimation();
    }

    m_Owner->UpdateLanding(DeltaTime);
    if (m_Owner->IsSlopeSlip())
    {
        if (m_Owner->SlopeSlipMove(true, DeltaTime))
        {
            return EPlayerStateType::LAND_SLOPE_SLIP;
        }
    }
    if (m_Owner->IsCrouchingPress())
    {
        return EPlayerStateType::LAND_CROUCHING_SLIDE;
    }
    if (isTurnInput)
    {
        return EPlayerStateType::LAND_TURN;
    }
    if (m_Owner->IsJumpTrigger())
    {
        return EPlayerStateType::LAND_NORMAL_JUMP;
    }
    if (m_Owner->IsTailAttackTrigger())
    {
        return EPlayerStateType::LAND_TAIL_ATTACK;
    }

    if (m_Owner->IsInputMove())
    {
        m_CountNotInputMoveTime = 0.0f;
        m_Owner->CheckMoveSE(m_CountMoveSETime);
    }
    else
    {
        m_CountNotInputMoveTime += DeltaTime;
    }

    if (m_CountNotInputMoveTime > CHANGE_IDLE_STATE_TIME)
    {
        return EPlayerStateType::LAND_IDLE;
    }

    m_CountMoveTime += DeltaTime;
    m_CountMoveSETime += DeltaTime;

    return EPlayerStateType::LAND_MOVE;
}