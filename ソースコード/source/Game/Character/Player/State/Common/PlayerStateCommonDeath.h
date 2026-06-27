#pragma once
#include "../PlayerStateBase.h"
#include "../../../../../Framework/State/FunctionStateMachine.h"
enum class EPlayerStateType : int;

class CPlayerStateCommonDeath : public CPlayerStateBase
{
public:
	CPlayerStateCommonDeath(CPlayer* InOwner) : CPlayerStateBase(InOwner) {};

	virtual void Enter() override;

	virtual EPlayerStateType Update(const float DeltaTime) override;

private:
	float m_CountDeathTime = 0.0f;
	//アニメーションに合わせたSEの再生タイミング。左から再生したかのフラグ、再生時間
	std::vector<std::pair<bool, float>> PlaySoundTiming = 
	{ 
		{false, 0.16f }, 
		{false, 0.58f }, 
		{false, 1.25f }
	};
};