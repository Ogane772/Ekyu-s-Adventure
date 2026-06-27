#include "MainGameUI.h"
#include "../Character/Player/Player.h"
#include "../../Framework/Polygon/2DPolygon/2DPolygon.h"
#include "../../Framework/TextureLoader/TextureTypes.h"
#include "../../Framework/Scene/SceneManager.h"
#include "../../Framework/Fade/Fade.h"
#include "../../Framework/Renderer/Renderer.h"
#include "../../Framework/System/SystemContext.h"

namespace
{
	constexpr int JUMP_TUTORIAL = 1;
	constexpr int SPIN_JUMP_TUTORIAL = 2;
	constexpr int ATTACK_TUTORIAL = 3;
	constexpr int WALL_JUMP_TUTORIAL = 4;
}

void CMainGameUI::Init()
{
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return;
	}
	m_Player = GetPlayer();

	m_Life = std::make_unique<C2DPolygon>(ETextureType::UI_HEART);
	m_DamageLife = std::make_unique<C2DPolygon>(ETextureType::UI_DAMAGE_HEART);
	m_StarIcon = std::make_unique<C2DPolygon>(ETextureType::UI_STAR);
	m_GetStar = std::make_unique<C2DPolygon>(ETextureType::UI_GET_STAR);
	m_StageClear = std::make_unique<C2DPolygon>(ETextureType::UI_STAGE_CLEAR);
	m_Black = std::make_unique<C2DPolygon>(ETextureType::BLACK);
	
	m_ItemCounter = std::make_unique<C2DPolygon>(ETextureType::UI_NUMBER);
	m_AcornIcon = std::make_unique<C2DPolygon>(ETextureType::UI_ACORN);
	m_AcornCross = std::make_unique<C2DPolygon>(ETextureType::UI_CROSS);
	m_StarCross = std::make_unique<C2DPolygon>(ETextureType::UI_CROSS);
	m_BackButton = std::make_unique<C2DPolygon>(ETextureType::UI_BACK_BUTTON);
	m_NextText = std::make_unique<C2DPolygon>(ETextureType::UI_NEXT);
	m_GamepadButton = std::make_unique<C2DPolygon>(ETextureType::UI_GAMEPAD_BUTTON);
	m_JumpTips = std::make_unique<C2DPolygon>(ETextureType::UI_JUMP);
	m_CameraResetTips = std::make_unique<C2DPolygon>(ETextureType::UI_CAMERA_RESET);
	m_CrouchingTips = std::make_unique<C2DPolygon>(ETextureType::UI_CROUCHING);
	m_DiveTips = std::make_unique<C2DPolygon>(ETextureType::UI_DIVE);
	m_SpinTips = std::make_unique<C2DPolygon>(ETextureType::UI_SPIN);
	m_TailAttackTips = std::make_unique<C2DPolygon>(ETextureType::UI_TAIL_ATTACK);
	m_DashJumpTips = std::make_unique<C2DPolygon>(ETextureType::UI_DASH_JUMP);
	m_WallJumpTips = std::make_unique<C2DPolygon>(ETextureType::UI_WALL_JUMP);
	m_HighJumpTips = std::make_unique<C2DPolygon>(ETextureType::UI_HIGH_JUMP);
	m_TurnJumpTips = std::make_unique<C2DPolygon>(ETextureType::UI_TURN_JUMP);
	m_HipDropTips = std::make_unique<C2DPolygon>(ETextureType::UI_HIPDROP);
	m_LB_Button = std::make_unique<C2DPolygon>(ETextureType::UI_LB_BUTTON);
	m_RB_Button = std::make_unique<C2DPolygon>(ETextureType::UI_RB_BUTTON);
	m_RT_Button = std::make_unique<C2DPolygon>(ETextureType::UI_RT_BUTTON);
	m_LT_Button = std::make_unique<C2DPolygon>(ETextureType::UI_LT_BUTTON);
	//チュートリアルマップの場合は追加でチュートリアル画像をロード
	if (manager->IsTutorialMap())
	{
		m_TutorialJump = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL1);
		m_TutorialSpin = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL2);
		m_TutorialHipDrop = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL3);
		m_TutorialTailAttack = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL4);
		m_TutorialAir = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL5);
		m_TutorialGround = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL6);
		m_TutorialWallKickTips = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL7);
		m_TutorialWallKick = std::make_unique<C2DPolygon>(ETextureType::TUTORIAL8);
	}
}

void CMainGameUI::Draw()
{
	if (IsEditMode())
	{
		return;
	}
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			return;
		}
	}
	if (IsEvent())
	{
		return;
	}
	CRenderer::SetDepthEnable(false);
	CRenderer::Set2DShader();
	CRenderer::SetWorldViewProjection2D();
	PlayerUi();
	TutorialUi();
	CRenderer::SetDepthEnable(true);
}

void CMainGameUI::PlayerUi()
{
	if (!m_Player)
	{
		return;
	}

	GamepadGuideUi();

	for (int i = 0; i < m_Player->GetMaxHp(); i++)
	{
		if (i < m_Player->GetHP().GetCurrentHP())
		{
			if (m_Life)
			{
				m_Life->Draw(SpriteParameter(Vector2((45.0f * i) + 45.0f, 10.0f), Vector2(0.4f, 0.4f)));
			}
		}
		else
		{
			if (m_DamageLife)
			{
				m_DamageLife->Draw(SpriteParameter(Vector2((45.0f * i) + 45.0f, 10.0f), Vector2(0.4f, 0.4f)));
			}
		}
	}
	
	//取得ドングリ数表示
	if (m_AcornIcon)
	{
		m_AcornIcon->Draw(SpriteParameter(Vector2(700.0f, 10.0f), Vector2(0.5f, 0.5f)));
	}
	if (m_AcornCross)
	{
		m_AcornCross->Draw(SpriteParameter(Vector2(750.0f, 25.0f), Vector2(0.5f, 0.5f)));
	}
	const int acornCount = m_Player->GetMyAcornCount();
	int acornDigit = 1;
	if (acornCount >= 10 && acornCount < 100)
	{
		acornDigit = 2;
	}
	else if (acornCount >= 100)
	{
		acornDigit = 3;
	}
	if (m_ItemCounter)
	{
		m_ItemCounter->ScoreDraw(Vector2(780, 16), m_Player->GetMyAcornCount(), acornDigit);
	}

	//取得スター数表示
	if (m_StarIcon)
	{
		m_StarIcon->Draw(SpriteParameter(Vector2(900.0f, 10.0f), Vector2(0.5f, 0.5f)));
	}
	if (m_StarCross)
	{
		m_StarCross->Draw(SpriteParameter(Vector2(950.0f, 25.0f), Vector2(0.5f, 0.5f)));
	}
	if (m_ItemCounter)
	{
		m_ItemCounter->ScoreDraw(Vector2(980, 16), m_Player->GetMyStarCount(), 1);
	}
	//次へ進むボタン表示
	if (m_Player->IsEnableClearButton())
	{
		if (m_Player->IsDamageState() || m_Player->IsDeath())
		{
			return;
		}
		if (m_Black)
		{
			m_Black->Draw(SpriteParameter(Vector2(512.0f, 500.0f), Vector2(0.14f, 0.13f), 0.0f, 0.4f));
		}
		if (m_BackButton)
		{
			m_BackButton->Draw(SpriteParameter(Vector2(498.0f, 506.0f), Vector2(0.8f, 0.8f)));
		}
		if (m_NextText)
		{
			m_NextText->Draw(SpriteParameter(Vector2(525.0f, 553.0f), Vector2(0.8f, 0.8f)));
		}
	}

	if (!m_Player->IsCongratulationState())
	{
		return;
	}
	//GetStar表示
	if (!m_Player->IsEnableGetStar())
	{
		return;
	}
	//ステージクリアとして扱うスターなら専用UI表示
	if (m_Player->IsGetStageClearStar())
	{
		if (m_StageClear)
		{
			m_StageClear->Draw(SpriteParameter(Vector2(315.0f, 80.0f), Vector2(1.2f, 1.2f)));
		}
	}
	else
	{
		if (m_GetStar)
		{
			m_GetStar->Draw(SpriteParameter(Vector2(425.0f, 90.0f), Vector2(1.2f, 1.2f)));
		}
	}
}

void CMainGameUI::GamepadGuideUi()
{
	if (!m_Player)
	{
		return;
	}
	if (CSystemContext* system = CSystemContext::GetInstance())
	{
		if (system->IsPause())
		{
			return;
		}
	}
	if (m_Player->IsDeath() || m_Player->IsCongratulationState())
	{
		return;
	}

	//ボタン表示
	if (m_GamepadButton)
	{
		m_GamepadButton->Draw(SpriteParameter(Vector2(1060.0f, 120.0f), Vector2(1.2f, 1.2f)));
	}
	if (m_LB_Button)
	{
		m_LB_Button->Draw(SpriteParameter(Vector2(1020.0f, 85.0f), Vector2(0.8f, 0.77f)));
	}
	if (m_RB_Button)
	{
		m_RB_Button->Draw(SpriteParameter(Vector2(1150.0f, 85.0f), Vector2(0.78f, 0.8f)));
	}
	if (m_LT_Button)
	{
		m_LT_Button->Draw(SpriteParameter(Vector2(1030.0f, 20.0f), Vector2(0.7f, 0.7f)));
	}
	if (m_RT_Button)
	{
		m_RT_Button->Draw(SpriteParameter(Vector2(1150.0f, 23.0f), Vector2(0.6f, 0.65f)));
	}

	if (m_CameraResetTips)
	{
		m_CameraResetTips->Draw(SpriteParameter(Vector2(1110.0f, 105.0f), Vector2(0.3f, 0.3f)));
	}

	//プレイヤーのアクションに応じた現在行えるアクションをボタン位置に表示
	if (m_Player->IsNormalJumpableState())
	{
		if (m_JumpTips)
		{
			m_JumpTips->Draw(SpriteParameter(Vector2(1060.0f, 220.0f), Vector2(0.4f, 0.4f)));
		}
	}
	if (m_Player->IsSpinJumpableState())
	{
		if (m_SpinTips)
		{
			m_SpinTips->Draw(SpriteParameter(Vector2(1000.0f, 105.0f), Vector2(0.35f, 0.35f)));
		}
	}
	if (m_Player->IsDiveJumpableState())
	{
		if (m_DiveTips)
		{
			m_DiveTips->Draw(SpriteParameter(Vector2(1010.0f, 170.0f), Vector2(0.35f, 0.35f)));
		}
	}
	if (m_Player->IsHipDropPossibleState())
	{
		if (m_HipDropTips)
		{
			m_HipDropTips->Draw(SpriteParameter(Vector2(1015.0f, 35.0f), Vector2(0.38f, 0.38f)));
		}
	}
	if (m_Player->IsHighJumpableState() || m_Player->IsCrouchingJumpableState())
	{
		if (m_HighJumpTips)
		{
			m_HighJumpTips->Draw(SpriteParameter(Vector2(1060.0f, 220.0f), Vector2(0.35f, 0.35f)));
		}
	}
	if (m_Player->IsWallJumpableState())
	{
		if (m_WallJumpTips)
		{
			m_WallJumpTips->Draw(SpriteParameter(Vector2(1030.0f, 220.0f), Vector2(0.38f, 0.38f)));
		}
	}
	if (m_Player->IsTailAttackPossibleState())
	{
		if (m_TailAttackTips)
		{
			m_TailAttackTips->Draw(SpriteParameter(Vector2(1000.0f, 180.0f), Vector2(0.28f, 0.28f)));
		}
	}
	if (m_Player->IsCrouchingPossibleState())
	{
		if (m_CrouchingTips)
		{
			m_CrouchingTips->Draw(SpriteParameter(Vector2(1055.0f, 35.0f), Vector2(0.35f, 0.35f)));
		}
	}
	if (m_Player->IsTurnJumpableState())
	{
		if (m_TurnJumpTips)
		{
			m_TurnJumpTips->Draw(SpriteParameter(Vector2(1020.0f, 220.0f), Vector2(0.38f, 0.38f)));
		}
	}
	if (m_Player->IsDashJumpableState())
	{
		if (m_DashJumpTips)
		{
			m_DashJumpTips->Draw(SpriteParameter(Vector2(1040.0f, 220.0f), Vector2(0.35f, 0.35f)));
		}
	}
}

void  CMainGameUI::TutorialUi()
{
	if (m_TutorialNumber == 0)
	{
		return;
	}

	if (!m_Black || !m_TutorialAir)
	{
		return;
	}

	switch (m_TutorialNumber)
	{
	case JUMP_TUTORIAL:
		m_Black->Draw(SpriteParameter(Vector2(512.0f, 500.0f), Vector2(0.14f, 0.103f), 0.0f, 0.4f));
		if (m_TutorialJump)
		{
			m_TutorialJump->Draw(SpriteParameter(Vector2(542.0f, 500.0f), Vector2(0.6f, 0.6f)));
		}
		break;
	case SPIN_JUMP_TUTORIAL:
		m_Black->Draw(SpriteParameter(Vector2(512.0f, 500.0f), Vector2(0.14f, 0.13f), 0.0f, 0.4f));
		m_TutorialAir->Draw(SpriteParameter(Vector2(512.0f, 503.0f), Vector2(0.6f, 0.6f)));
		if (m_TutorialSpin)
		{
			m_TutorialSpin->Draw(SpriteParameter(Vector2(537.0f, 535.0f), Vector2(0.6f, 0.6f)));
		}
		break;
	case ATTACK_TUTORIAL:
		m_Black->Draw(SpriteParameter(Vector2(512.0f, 500 - 7.0f), Vector2(0.14f, 0.15f), 0.0f, 0.4f));
		m_TutorialAir->Draw(SpriteParameter(Vector2(512.0f, 503.0f - 7.0f), Vector2(0.6f, 0.6f)));
		if (m_TutorialHipDrop)
		{
			m_TutorialHipDrop->Draw(SpriteParameter(Vector2(512.0f, 535.0f - 7.0f), Vector2(0.6f, 0.6f)));
		}
		if (m_TutorialGround)
		{
			m_TutorialGround->Draw(SpriteParameter(Vector2(512.0f, 575.0f - 7.0f), Vector2(0.6f, 0.6f)));
		}
		if (m_TutorialTailAttack)
		{
			m_TutorialTailAttack->Draw(SpriteParameter(Vector2(512.0f, 607.0f - 7.0f), Vector2(0.6f, 0.6f)));
		}
		break;
	case WALL_JUMP_TUTORIAL:
		m_Black->Draw(SpriteParameter(Vector2(477.0f, 500 - 7.0f), Vector2(0.179f, 0.125f), 0.0f, 0.4f));
		m_TutorialAir->Draw(SpriteParameter(Vector2(512.0f, 503.0f - 7.0f), Vector2(0.6f, 0.6f)));
		if (m_TutorialWallKickTips)
		{
			m_TutorialWallKickTips->Draw(SpriteParameter(Vector2(512.0f, 535.0f - 7.0f), Vector2(0.6f, 0.6f)));
		}
		if (m_TutorialWallKick)
		{
			m_TutorialWallKick->Draw(SpriteParameter(Vector2(582.0f, 575.0f - 7.0f), Vector2(0.6f, 0.6f)));
		}
		break;
	}
}
