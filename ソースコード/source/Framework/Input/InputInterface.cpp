#include "InputInterface.h"
#include "Input.h"
#include "GamePad.h"
#include <algorithm>

const float STICK_ANGLE_MAX = 1.0f;//ゲームパッドのスティック入力の最大値

CInputInterface::CInputInterface()
{
	m_Input = std::make_unique<CInput>();
	m_GamePad = std::make_unique<CGamePad>();
	m_VibrationParameters =
	{
		std::pair<EVibrationType, VibrationParameter>(EVibrationType::LOW,       VibrationParameter(0.15f, 30000)),
		std::pair<EVibrationType, VibrationParameter>(EVibrationType::MIDDLE,      VibrationParameter(0.3f, 30000)),
		std::pair<EVibrationType, VibrationParameter>(EVibrationType::HIGH,       VibrationParameter(1.2f, 30000)),
		std::pair<EVibrationType, VibrationParameter>(EVibrationType::EXTRA_LARGE, VibrationParameter(1.6f, 15000)),
	};
}

CInputInterface::~CInputInterface()
{
	m_Input.reset();
	m_GamePad.reset();
}

void CInputInterface::Update(float DeltaTime)
{
	if(m_Input)
	{
		m_Input->Update();
	}
	if(m_GamePad)
	{
		m_GamePad->Update(DeltaTime);
	}
}

Vector2 CInputInterface::GetLeftStick() const
{
	if(!m_Input || !m_GamePad)
	{
		return Vector2::Zero();
	}
	Vector2 leftStick = m_GamePad->GetLeftStick();

	//ゲームパッドの入力後、キーボードの移動入力を加算する
	if (m_Input->GetKeyPress('W'))
	{
		leftStick.y += STICK_ANGLE_MAX;
	}
	if (m_Input->GetKeyPress('S'))
	{
		leftStick.y -= STICK_ANGLE_MAX;
	}
	if (m_Input->GetKeyPress('A'))
	{
		leftStick.x -= STICK_ANGLE_MAX;
	}
	if (m_Input->GetKeyPress('D'))
	{
		leftStick.x += STICK_ANGLE_MAX;
	}
	//斜め移動を考慮した移動量に変換
	if (leftStick.Length() > 1.0f)
	{
		leftStick.Normalized();
	}
	leftStick.x = std::clamp(leftStick.x, -STICK_ANGLE_MAX, STICK_ANGLE_MAX);
	leftStick.y = std::clamp(leftStick.y, -STICK_ANGLE_MAX, STICK_ANGLE_MAX);

	return leftStick;
}

Vector2 CInputInterface::GetRightStick() const
{
	if (!m_Input || !m_GamePad)
	{
		return Vector2::Zero();
	}
	Vector2 rightStick = m_GamePad->GetRightStick();

	//ゲームパッドの入力後、キーボードの移動入力を加算する
	if (m_Input->GetKeyPress(VK_UP))
	{
		rightStick.y += STICK_ANGLE_MAX;
	}
	if (m_Input->GetKeyPress(VK_DOWN))
	{
		rightStick.y -= STICK_ANGLE_MAX;
	}
	if (m_Input->GetKeyPress(VK_LEFT))
	{
		rightStick.x -= STICK_ANGLE_MAX;
	}
	if (m_Input->GetKeyPress(VK_RIGHT))
	{
		rightStick.x += STICK_ANGLE_MAX;
	}
	rightStick.x = std::clamp(rightStick.x, -STICK_ANGLE_MAX, STICK_ANGLE_MAX);
	rightStick.y = std::clamp(rightStick.y, -STICK_ANGLE_MAX, STICK_ANGLE_MAX);
	//斜め移動を考慮した移動量に変換
	if (rightStick.Length() > 1.0f)
	{
		rightStick.Normalized();
	}
	return rightStick;
}

bool CInputInterface::IsJumpTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('O') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_A);
}

bool CInputInterface::IsJumpPress() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyPress('O') || m_GamePad->IsButtonPress(XINPUT_GAMEPAD_A);
}

bool CInputInterface::IsDiveJumpTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('K') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_X);
}

bool CInputInterface::IsDiveJumpPress() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyPress('K') || m_GamePad->IsButtonPress(XINPUT_GAMEPAD_X);
}

bool CInputInterface::IsSpinJumpTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('I') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_LEFT_SHOULDER);
}

bool CInputInterface::IsCameraResetTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('P') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER) || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_RIGHT_THUMB);
}

bool CInputInterface::IsInteractTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger(VK_RETURN) || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_BACK);
}

bool CInputInterface::IsDecisionTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger(VK_RETURN) || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_A);
}

bool CInputInterface::IsSwimmingTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('O') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_A);
}

bool CInputInterface::IsSwimmingUpPress() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyPress('O') || m_GamePad->IsButtonPress(XINPUT_GAMEPAD_A);
}

bool CInputInterface::IsPauseTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('B') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_START);
}

bool CInputInterface::IsLeftSelectTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('A') || m_GamePad->LStickXTriggerCheckLeft() || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_DPAD_LEFT);
}

bool CInputInterface::IsRightSelectTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('D') || m_GamePad->LStickXTriggerCheckRight() || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_DPAD_RIGHT);
}

bool CInputInterface::IsUpSelectTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('W') || m_GamePad->LStickYTriggerCheckUp() || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_DPAD_UP);
}

bool CInputInterface::IsDownSelectTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('S') || m_GamePad->LStickYTriggerCheckDown() || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_DPAD_DOWN);
}

bool CInputInterface::IsImGuiDrawTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('N');
}

bool CInputInterface::IsImGuiCaptureObjectTrigger() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyTrigger(VK_LBUTTON);
}

bool CInputInterface::IsImGuiCaptureObjectDeleteTrigger() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyTrigger(VK_DELETE);
}

bool CInputInterface::IsImGuiCaptureObjectRelease() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyRelease(VK_LBUTTON);
}

bool CInputInterface::IsImGuiSaveTrigger() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('Z');
}

bool CInputInterface::IsDebugCameraScreenClickTrigger() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyTrigger(VK_RBUTTON);
}

bool CInputInterface::IsDebugCameraRotationPress() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyPress(VK_RBUTTON);
}

bool CInputInterface::IsViewModelScreenClickTrigger() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyTrigger(VK_LBUTTON) || m_Input->GetKeyTrigger(VK_RBUTTON);
}

bool CInputInterface::IsViewModelCameraRotationPress() const
{
	if (!m_Input)
	{
		return false;
	}
	return m_Input->GetKeyPress(VK_LBUTTON) && m_Input->GetKeyPress(VK_LSHIFT);
}

float CInputInterface::GetEditCameraUpDown() const
{
	if (!m_Input || !m_GamePad)
	{
		return 0.0f;
	}

	float upDown = 0.0f;
	const float inputMax = 1.0f;

	if (m_GamePad->IsButtonPress(XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		upDown += inputMax;
	}
	else if (m_GamePad->IsButtonPress(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		upDown -= inputMax;
	}
	//ゲームパッドの入力後、キーボードの移動入力を加算する
	if (m_Input->GetKeyPress('Q'))
	{
		upDown += inputMax;
	}
	else if (m_Input->GetKeyPress('E'))
	{
		upDown -= inputMax;
	}
	upDown = std::clamp(upDown, -inputMax, inputMax);
	return upDown;
}

bool CInputInterface::IsCrouchingPress() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyPress('U') || m_GamePad->LTButtonPress() || m_GamePad->RTButtonPress();
}

bool CInputInterface::IsHipDropTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('U') || m_GamePad->LTButtonTrigger() || m_GamePad->RTButtonTrigger();
}

bool CInputInterface::IsTailAttackTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyTrigger('K') || m_GamePad->IsButtonTrigger(XINPUT_GAMEPAD_X);
}

bool CInputInterface::IsCliffUpTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyPress('W') || m_GamePad->GetLeftStick().y > 0.8f;
}

bool CInputInterface::IsCliffDownTrigger() const
{
	if (!m_Input || !m_GamePad)
	{
		return false;
	}
	return m_Input->GetKeyPress('S') || m_GamePad->GetLeftStick().y < -0.8f;
}

void CInputInterface::PlayVibration(const EVibrationType Type)
{
	if (!m_GamePad)
	{
		return;
	}
	const VibrationParameter& parameter = m_VibrationParameters[Type];
	m_GamePad->PlayVibration(parameter.Time, parameter.Power);
}