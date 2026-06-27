#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateLandCrouchingSlide : public CPlayerStateBase
{
public:
	CPlayerStateLandCrouchingSlide(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};
	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

	virtual void Exit() override;
};