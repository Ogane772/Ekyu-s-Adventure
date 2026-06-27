#pragma once
#include "../../Framework/GameObject/SceneObject.h"

class C2DPolygon;

class CMainGameUI : public CSceneObject
{
private:
	std::unique_ptr<C2DPolygon> m_Life;
	std::unique_ptr<C2DPolygon> m_DamageLife;
	std::unique_ptr<C2DPolygon> m_ItemCounter = nullptr;
	std::unique_ptr<C2DPolygon> m_AcornIcon = nullptr;
	std::unique_ptr<C2DPolygon> m_StarIcon = nullptr;
	std::unique_ptr<C2DPolygon> m_AcornCross = nullptr;
	std::unique_ptr<C2DPolygon> m_StarCross = nullptr;
	std::unique_ptr<C2DPolygon> m_BackButton = nullptr;
	std::unique_ptr<C2DPolygon> m_NextText = nullptr;
	std::unique_ptr<C2DPolygon> m_GetStar = nullptr;
	std::unique_ptr<C2DPolygon> m_GamepadButton = nullptr;
	std::unique_ptr<C2DPolygon> m_JumpTips = nullptr;
	std::unique_ptr<C2DPolygon> m_CameraResetTips = nullptr;
	std::unique_ptr<C2DPolygon> m_CrouchingTips = nullptr;
	std::unique_ptr<C2DPolygon> m_DiveTips = nullptr;
	std::unique_ptr<C2DPolygon> m_SpinTips = nullptr;
	std::unique_ptr<C2DPolygon> m_TailAttackTips = nullptr;
	std::unique_ptr<C2DPolygon> m_DashJumpTips = nullptr;
	std::unique_ptr<C2DPolygon> m_WallJumpTips = nullptr;
	std::unique_ptr<C2DPolygon> m_HighJumpTips = nullptr;
	std::unique_ptr<C2DPolygon> m_TurnJumpTips = nullptr;
	std::unique_ptr<C2DPolygon> m_HipDropTips = nullptr;
	std::unique_ptr<C2DPolygon> m_LB_Button = nullptr;
	std::unique_ptr<C2DPolygon> m_RB_Button = nullptr;
	std::unique_ptr<C2DPolygon> m_RT_Button = nullptr;
	std::unique_ptr<C2DPolygon> m_LT_Button = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialJump = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialSpin = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialHipDrop = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialTailAttack = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialAir = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialGround = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialWallKickTips = nullptr;
	std::unique_ptr<C2DPolygon> m_TutorialWallKick = nullptr;
	std::unique_ptr<C2DPolygon> m_Black = nullptr;
	std::unique_ptr<C2DPolygon> m_StageClear = nullptr;
;
	int m_TutorialNumber = 0;
	CPlayer* m_Player = nullptr;
	void PlayerUi();
	void GamepadGuideUi();
	void TutorialUi();

public:
	virtual void Init() override;
	virtual void Draw() override;
	void SetTutorialNumber(const int Number) { m_TutorialNumber = Number; }
};