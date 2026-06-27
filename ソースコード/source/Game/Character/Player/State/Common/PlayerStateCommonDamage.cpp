#include "PlayerStateCommonDamage.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateCommonDamage::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::DAMAGE);
        m_Owner->ResetSpeed();
    }
}

EPlayerStateType CPlayerStateCommonDamage::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    if (!m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::DAMAGE))
    {
        return EPlayerStateType::COMMON_DAMAGE;
    }

    m_Owner->StartInvincible();

    const ELandedState landed = m_Owner->CheckGround();
    if (landed == ELandedState::AIR)
    {
        //ƒvƒŒƒCƒ„[Ž€–SŽž‚ÍŽ€–S‚Å‚ ‚é‚±‚Æ‚ð•ª‚©‚è‚â‚·‚­‚·‚é‚½‚ß—Ž‰º‚ð•Ï‚¦‚é
        if (m_Owner->IsDeath())
        {
            return EPlayerStateType::COMMON_WALL_HIT;
        }
        return EPlayerStateType::AIR_FALL;
    }
    else
    {
        if (m_Owner->IsDeath())
        {
            return EPlayerStateType::COMMON_DEATH;
        }
        return EPlayerStateType::LAND_IDLE;
    }

    return EPlayerStateType::COMMON_DAMAGE;
}

