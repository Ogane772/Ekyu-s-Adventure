#include "PlayerStateCommonDeath.h"
#include "../PlayrStateTypeAndAnimationNumber.h"
#include "../../Player.h"

void CPlayerStateCommonDeath::Enter()
{
    if (m_Owner)
    {
        m_Owner->PlayAnimationPlayer(EPlayerAnimationNumber::DEATH);
        m_Owner->StartDeath();
    }
    m_CountDeathTime = 0.0f;
}

EPlayerStateType CPlayerStateCommonDeath::Update(const float DeltaTime)
{
    if (!m_Owner)
    {
		return EPlayerStateType::MAX;
    }

    for (std::pair<bool, float>& timing : PlaySoundTiming)
    {
        if (timing.first)
        {
            continue;
        }
        if (m_CountDeathTime > timing.second)
        {
            m_Owner->PlayDeathAnimeSE();
            timing.first = true;
            break;
        }
    }

    if (m_Owner->IsAnimationEndPlayer(EPlayerAnimationNumber::DEATH))
    {
        m_Owner->EndDeath();
    }

    m_CountDeathTime += DeltaTime;

    return EPlayerStateType::COMMON_DEATH;
}

