#include "PlayerStateMachine.h"
#include "PlayerStateBase.h"
#include "PlayrStateTypeAndAnimationNumber.h"
#include "Land/PlayerStateLandIdle.h"
#include "Land/PlayerStateLandMove.h"
#include "Land/PlayerStateLandNormalJump.h"
#include "Land/PlayerStateLandTailAttack.h"
#include "Land/PlayerStateLandTurn.h"
#include "Land/PlayerStateLandTurnJump.h"
#include "Land/PlayerStateLandDashJump.h"
#include "Land/PlayerStateLandCrouchingIdle.h"
#include "Land/PlayerStateLandCrouchingMove.h"
#include "Land/PlayerStateLandCrouchingSlide.h"
#include "Land/PlayerStateLandCrouchingJUMP.h"
#include "Land/PlayerStateLandSlopeSlip.h"
#include "Air/PlayerStateAirFall.h"
#include "Air/PlayerStateAirCliff.h"
#include "Air/PlayerStateAirSpinJump.h"
#include "Air/PlayerStateAirWallSlip.h"
#include "Air/PlayerStateAirWallJump.h"
#include "Air/PlayerStateAirDiveJump.h"
#include "Air/PlayerStateAirHipDrop.h"
#include "Common/PlayerStateCommonWallHit.h"
#include "Common/PlayerStateCommonCongratulation.h"
#include "Common/PlayerStateCommonDamage.h"
#include "Common/PlayerStateCommonDeath.h"
#include "../Player.h"
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

CPlayerStateMachine::CPlayerStateMachine(CPlayer* InOwner)
	: CStateMachine(InOwner)
{
	m_StateList.emplace(EPlayerStateType::LAND_IDLE, std::make_shared<CPlayerStateLandIdle>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_MOVE, std::make_shared<CPlayerStateLandMove>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_TURN, std::make_shared<CPlayerStateLandTurn>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_TURN_JUMP, std::make_shared<CPlayerStateLandTurnJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_DASH_JUMP, std::make_shared<CPlayerStateLandDashJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_NORMAL_JUMP, std::make_shared<CPlayerStateLandNormalJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_TAIL_ATTACK, std::make_shared<CPlayerStateLandTailAttack>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_CROUCHING_IDLE, std::make_shared<CPlayerStateLandCrouchingIdle>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_CROUCHING_MOVE, std::make_shared<CPlayerStateLandCrouchingMove>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_CROUCHING_SLIDE, std::make_shared<CPlayerStateLandCrouchingSlide>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_CROUCHING_JUMP, std::make_shared<CPlayerStateLandCrouchingJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::LAND_SLOPE_SLIP, std::make_shared<CPlayerStateLandSlopeSlip>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_FALL, std::make_shared<CPlayerStateAirFall>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_CLIFF, std::make_shared<CPlayerStateAirCliff>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_SPIN_JUMP, std::make_shared<CPlayerStateAirSpinJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_WALL_SLIP, std::make_shared<CPlayerStateAirWallSlip>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_WALL_JUMP, std::make_shared<CPlayerStateAirWallJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_DIVE_JUMP, std::make_shared<CPlayerStateAirDiveJump>(m_Owner));
	m_StateList.emplace(EPlayerStateType::AIR_HIPDROP, std::make_shared<CPlayerStateAirHipDrop>(m_Owner));
	m_StateList.emplace(EPlayerStateType::COMMON_WALL_HIT, std::make_shared<CPlayerStateCommonWallHit>(m_Owner));
	m_StateList.emplace(EPlayerStateType::COMMON_CONGRATULATION, std::make_shared<CPlayerStateCommonCongratulation>(m_Owner));
	m_StateList.emplace(EPlayerStateType::COMMON_DAMAGE, std::make_shared<CPlayerStateCommonDamage>(m_Owner));
	m_StateList.emplace(EPlayerStateType::COMMON_DEATH, std::make_shared<CPlayerStateCommonDeath>(m_Owner));
	m_CurrentStateType = m_PrevStateType = EPlayerStateType::MAX;
}