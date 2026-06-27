#include "PauseUI.h"
#include "../../Framework/Polygon/2DPolygon/2DPolygon.h"
#include "../../Framework/TextureLoader/TextureTypes.h"
#include "../../Framework/Scene/SceneManager.h"
#include "../../Framework/Scene/SceneTypes.h"
#include "../../Framework/Camera/CameraSetting.h"
#include "../../Framework/Fade/Fade.h"
#include "../../Framework/Input/InputInterface.h"
#include "../../Game/Character/Player/Player.h"
#include "../../Framework/Renderer/Renderer.h"
#include "../../Framework/System/SystemContext.h"

void CPauseUI::Init()
{
	m_StateMachine.AddState<CPauseUI>(
		EPauseState::NONE,
		this
	);
	m_StateMachine.AddState<CPauseUI>(
		EPauseState::OPEN,
		this,
		&CPauseUI::StartOpen,
		&CPauseUI::UpdateOpen,
		nullptr,
		&CPauseUI::DrawOpen
	);
	m_StateMachine.AddState<CPauseUI>(
		EPauseState::OPTION,
		this,
		&CPauseUI::StartOption,
		&CPauseUI::UpdateOption,
		nullptr,
		&CPauseUI::DrawOption
	);
	m_StateMachine.AddState<CPauseUI>(
		EPauseState::CHECK_TITLE,
		this,
		&CPauseUI::StartCheckTitle,
		&CPauseUI::UpdateCheckTitle,
		nullptr,
		&CPauseUI::DrawCheckTitle
	);
	m_StateMachine.SetInitialState(EPauseState::NONE);

	m_SelectNumber = 0;

	m_BG = std::make_unique<C2DPolygon>(ETextureType::BLACK);
	m_PauseText = std::make_unique<C2DPolygon>(ETextureType::UI_PAUSE);
	m_GameBackText = std::make_unique<C2DPolygon>(ETextureType::UI_GAME_BACK);
	m_OptionText = std::make_unique<C2DPolygon>(ETextureType::UI_OPTION);
	m_OptionRedText = std::make_unique<C2DPolygon>(ETextureType::UI_OPTION_RED);
	m_TitleBackText = std::make_unique<C2DPolygon>(ETextureType::UI_TITLE_BACK);
	m_TitleBackRedText = std::make_unique<C2DPolygon>(ETextureType::UI_TITLE_BACK_RED	);
	m_CameraUpAndDown = std::make_unique<C2DPolygon>(ETextureType::UI_CAMERA_UPDOWN);
	m_CameraLeftAndRight = std::make_unique<C2DPolygon>(ETextureType::UI_CAMERA_LEFTRIGHT);
	m_CameraSpeed = std::make_unique<C2DPolygon>(ETextureType::UI_CAMERA_SPEED);
	m_CameraReverse = std::make_unique<C2DPolygon>(ETextureType::UI_REVERSE);
	m_CameraHigh = std::make_unique<C2DPolygon>(ETextureType::UI_HIGH);
	m_CameraBack = std::make_unique<C2DPolygon>(ETextureType::UI_BACK);
	m_CameraSlow = std::make_unique<C2DPolygon>(ETextureType::UI_SLOW);
	m_CameraNormal = std::make_unique<C2DPolygon>(ETextureType::UI_USUALLY);
	m_CameraSpeedNormal = std::make_unique<C2DPolygon>(ETextureType::UI_NORMAL);
	m_CameraArrow = std::make_unique<C2DPolygon>(ETextureType::UI_ARROW);
	m_Yes = std::make_unique<C2DPolygon>(ETextureType::UI_YES);
	m_No = std::make_unique<C2DPolygon>(ETextureType::UI_NO);
}

void CPauseUI::Update(float DeltaTime)
{
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			return;
		}
	}
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	//ポーズボタンによるポーズ開閉
	if (instance->IsPauseTrigger())
	{
		CPlayer* player = GetPlayer();
		if (!player)
		{
			return;
		}
		//ポーズの入力は非イベント中かスター取得中以外とする
		if (!system->IsEvent() && !player->IsCongratulationState())
		{
			system->SetPause(!system->IsPause());
			if (system->IsPause())
			{
				m_StateMachine.ChangeState(EPauseState::OPEN);
			}
			else
			{
				m_StateMachine.ChangeState(EPauseState::NONE);
			}
		}
	}
	if (!system->IsPause())
	{
		return;
	}

	m_StateMachine.Update(DeltaTime);
}

void CPauseUI::Draw()
{
	if (CSystemContext* system = CSystemContext::GetInstance())
	{
		if (!system->IsPause())
		{
			return;
		}
	}
	
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			return;
		}
	}
	CRenderer::SetDepthEnable(false);
	if (m_BG)
	{
		m_BG->Draw(0.51f);
	}
	m_StateMachine.Draw();
	CRenderer::SetDepthEnable(true);
}

void CPauseUI::StartOpen()
{
	m_SelectNumber = static_cast<int>(EOpenState::GAME_BACK);
}

void CPauseUI::UpdateOpen(float DeltaTime)
{
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	if (instance->IsDecisionTrigger())
	{
		switch (static_cast<EOpenState>(m_SelectNumber))
		{
		case EOpenState::GAME_BACK:
		{
			CSystemContext* system = CSystemContext::GetInstance();
			if (!system)
			{
				return;
			}
			system->SetPause(false);
			m_StateMachine.ChangeState(EPauseState::NONE);
		}
		break;
		case EOpenState::OPTION:
			m_StateMachine.ChangeState(EPauseState::OPTION);
			break;
		case EOpenState::TITLE_BACK:
			m_StateMachine.ChangeState(EPauseState::CHECK_TITLE);
			break;
		}
	}
	if (instance->IsUpSelectTrigger())
	{
		m_SelectNumber--;
		if (m_SelectNumber < 0)
		{
			m_SelectNumber = static_cast<int>(EOpenState::TITLE_BACK);
		}
	}
	if (instance->IsDownSelectTrigger())
	{
		m_SelectNumber++;
		if (m_SelectNumber > static_cast<int>(EOpenState::TITLE_BACK))
		{
			m_SelectNumber = static_cast<int>(EOpenState::GAME_BACK);
		}
	}
}

void CPauseUI::DrawOpen()
{
	//選択番号に応じたスケール値を取得
	auto getScale = [=, this](const EOpenState State)->Vector2
	{
		if (m_SelectNumber == static_cast<int>(State))
		{
			return Vector2(0.75f, 0.75f);
		}
		return Vector2(0.5f, 0.5f);
	};
	if (m_PauseText)
	{
		m_PauseText->Draw(SpriteParameter(Vector2(480.0f, 50.0f), Vector2(1.0f, 1.0f)));
	}
	if (m_GameBackText)
	{
		m_GameBackText->Draw(SpriteParameter(Vector2(430.0f, 260.0f), getScale(EOpenState::GAME_BACK)));
	}
	if (m_OptionText)
	{
		m_OptionText->Draw(SpriteParameter(Vector2(510.0f, 390.0f), getScale(EOpenState::OPTION)));
	}
	if (m_TitleBackText)
	{
		m_TitleBackText->Draw(SpriteParameter(Vector2(430.0f, 520.0f), getScale(EOpenState::TITLE_BACK)));
	}
}

void CPauseUI::StartOption()
{
	m_SelectNumber = static_cast<int>(EOptionState::CAMERA_LEFT_AND_RIGHT);
}

void CPauseUI::UpdateOption(float DeltaTime)
{
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	if (instance->IsDecisionTrigger())
	{
		if (m_SelectNumber == static_cast<int>(EOptionState::BACK))
		{
			m_StateMachine.ChangeState(EPauseState::OPEN);
		}
	}
	if (instance->IsUpSelectTrigger())
	{
		m_SelectNumber--;
		if (m_SelectNumber < 0)
		{
			m_SelectNumber = static_cast<int>(EOptionState::BACK);
		}
	}
	if (instance->IsDownSelectTrigger())
	{
		m_SelectNumber++;
		if (m_SelectNumber > static_cast<int>(EOptionState::BACK))
		{
			m_SelectNumber = static_cast<int>(EOptionState::CAMERA_LEFT_AND_RIGHT);
		}
	}
	if (instance->IsLeftSelectTrigger())
	{
		InputLeftOrRightOption(false);
	}
	if (instance->IsRightSelectTrigger())
	{
		InputLeftOrRightOption(true);
	}
}

void CPauseUI::DrawOption()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	auto getScale = [=, this](const EOptionState State)->Vector2
	{
		if (m_SelectNumber == static_cast<int>(State))
		{
			return Vector2(0.65f, 0.65f);
		}
		return Vector2(0.4f, 0.4f);
	};

	if (m_OptionRedText)
	{
		m_OptionRedText->Draw(SpriteParameter(Vector2(430.0f, 50.0f), Vector2(0.9f, 0.9f)));
	}
	if (m_CameraLeftAndRight)
	{
		m_CameraLeftAndRight->Draw(SpriteParameter(Vector2(200.0f, 200.0f), getScale(EOptionState::CAMERA_LEFT_AND_RIGHT)));
	}
	if (m_CameraUpAndDown)
	{
		m_CameraUpAndDown->Draw(SpriteParameter(Vector2(200.0f, 350.0f), getScale(EOptionState::CAMERA_UP_AND_DOWN)));
	}
	if (m_CameraSpeed)
	{
		m_CameraSpeed->Draw(SpriteParameter(Vector2(200.0f, 470.0f), getScale(EOptionState::CAMERA_SPEED)));
	}
	if (m_CameraBack)
	{
		m_CameraBack->Draw(SpriteParameter(Vector2(200.0f, 550.0f), getScale(EOptionState::BACK)));
	}

	switch (static_cast<EOptionState>(m_SelectNumber))
	{
	case EOptionState::CAMERA_LEFT_AND_RIGHT:
	{
		if (!m_CameraNormal || !m_CameraReverse || !m_CameraArrow)
		{
			return;
		}
		ECameraRotationSettingType type = system->GetCameraRotationLeftAndRightType();
		if (type == ECameraRotationSettingType::NORMAL)
		{
			m_CameraNormal->Draw(SpriteParameter(Vector2(890.0f, 200.0f), Vector2(0.65f, 0.65f)));
		}
		else
		{
			m_CameraReverse->Draw(SpriteParameter(Vector2(890.0f, 210.0f), Vector2(0.65f, 0.65f)));
		}
		m_CameraArrow->Draw(SpriteParameter(Vector2(825.0f, 210.0f), Vector2(0.4f, 0.65f)));
		m_CameraArrow->Draw(SpriteParameter(Vector2(1125.0f, 300.0f), Vector2(0.4f, 0.65f), 180.0f));
		break;
	}
	case EOptionState::CAMERA_UP_AND_DOWN:
	{
		if (!m_CameraNormal || !m_CameraReverse || !m_CameraArrow)
		{
			return;
		}
		ECameraRotationSettingType type = system->GetCameraRotationUpAndDownType();
		if (type == ECameraRotationSettingType::NORMAL)
		{
			m_CameraNormal->Draw(SpriteParameter(Vector2(890.0f, 345.0f), Vector2(0.65f, 0.65f)));
		}
		else
		{
			m_CameraReverse->Draw(SpriteParameter(Vector2(890.0f, 355.0f), Vector2(0.65f, 0.65f)));
		}
		m_CameraArrow->Draw(SpriteParameter(Vector2(825.0f, 355.0f), Vector2(0.4f, 0.65f)));
		m_CameraArrow->Draw(SpriteParameter(Vector2(1125.0f, 445.0f), Vector2(0.4f, 0.65f), 180.0f));
		break;
	}
	case EOptionState::CAMERA_SPEED:
	{
		if (!m_CameraSpeedNormal || !m_CameraHigh || !m_CameraSlow || !m_CameraArrow)
		{
			return;
		}
		ECameraSpeedSettingType type = system->GetCameraSpeedType();
		if (type == ECameraSpeedSettingType::NORMAL)
		{
			m_CameraSpeedNormal->Draw(SpriteParameter(Vector2(900.0f, 480.0f), Vector2(0.55f, 0.55f)));
		}
		else if (type == ECameraSpeedSettingType::HIGH)
		{
			m_CameraHigh->Draw(SpriteParameter(Vector2(900.0f, 480.0f), Vector2(0.55f, 0.55f)));
		}
		else if (type == ECameraSpeedSettingType::SLOW)
		{
			m_CameraSlow->Draw(SpriteParameter(Vector2(900.0f, 480.0f), Vector2(0.55f, 0.55f)));
		}
		m_CameraArrow->Draw(SpriteParameter(Vector2(825.0f, 480.0f), Vector2(0.4f, 0.65f)));
		m_CameraArrow->Draw(SpriteParameter(Vector2(1125.0f, 570.0f), Vector2(0.4f, 0.65f), 180.0f));
		break;
	}
	}
}

void CPauseUI::StartCheckTitle()
{
	//はい　いいえ　と並んでおり、初期カーソル位置を（いいえ=1）として扱う
	m_SelectNumber = static_cast<int>(ECheckTitleState::BACK);
}

void CPauseUI::UpdateCheckTitle(float DeltaTime)
{
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	if (instance->IsDecisionTrigger())
	{
		if (m_SelectNumber == static_cast<int>(ECheckTitleState::RETURN_TITLE))
		{
			if (CSceneManager* manager = CSceneManager::GetInstance())
			{
				manager->SetInterruptScene(ESceneType::TITLE);
			}
			if (CSystemContext* system = CSystemContext::GetInstance())
			{
				system->SetPause(false);
			}
			m_StateMachine.ChangeState(EPauseState::NONE);
		}
		else if (m_SelectNumber == static_cast<int>(ECheckTitleState::BACK))
		{
			m_StateMachine.ChangeState(EPauseState::OPEN);
		}
	}
	if (instance->IsLeftSelectTrigger())
	{
		m_SelectNumber--;
		if (m_SelectNumber < 0)
		{
			m_SelectNumber = static_cast<int>(ECheckTitleState::BACK);
		}
	}
	if (instance->IsRightSelectTrigger())
	{
		m_SelectNumber++;
		if (m_SelectNumber > static_cast<int>(ECheckTitleState::BACK))
		{
			m_SelectNumber = static_cast<int>(ECheckTitleState::RETURN_TITLE);
		}
	}
}

void CPauseUI::DrawCheckTitle()
{
	auto getScale = [=, this](const ECheckTitleState State)->Vector2
	{
		if (m_SelectNumber == static_cast<int>(State))
		{
			return Vector2(0.75f, 0.75f);
		}
		return Vector2(0.5f, 0.5f);
	};
	if (m_TitleBackRedText)
	{
		m_TitleBackRedText->Draw(SpriteParameter(Vector2(280.0f, 80.0f)));
	}
	if (m_Yes)
	{
		m_Yes->Draw(SpriteParameter(Vector2(380.0f, 360.0f), getScale(ECheckTitleState::RETURN_TITLE)));
	}
	if (m_No)
	{
		m_No->Draw(SpriteParameter(Vector2(620.0f, 360.0f), getScale(ECheckTitleState::BACK)));
	}
}

void CPauseUI::InputLeftOrRightOption(const bool IsRightInput)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	switch (static_cast<EOptionState>(m_SelectNumber))
	{
	case EOptionState::CAMERA_LEFT_AND_RIGHT:
		system->ChangeCameraRotationLeftAndRightType();
		break;
	case EOptionState::CAMERA_UP_AND_DOWN:
		system->ChangeCameraRotationUpAndDownType();
		break;
	case EOptionState::CAMERA_SPEED:
		if (IsRightInput)
		{
			system->NextCameraSpeedType();
		}
		else
		{
			system->PrevCameraSpeedType();
		}
		break;
	}
}
