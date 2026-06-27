#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateAirSpinJump : public CPlayerStateBase
{
public:
	CPlayerStateAirSpinJump(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

	virtual void Exit() override;

private:
	float m_CountSpinJumpTime = 0.0f;
};
