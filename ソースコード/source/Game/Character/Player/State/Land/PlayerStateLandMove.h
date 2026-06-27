#pragma once
#include "../PlayerStateBase.h"
enum class EPlayerStateType : int;

class CPlayerStateLandMove : public CPlayerStateBase
{
public:
	CPlayerStateLandMove(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};
	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

private:
	float m_CountMoveTime = 0.0f;
	float m_CountMoveSETime = 0.0f;
	float m_CountNotInputMoveTime = 0.0f;
};