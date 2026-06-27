#include <windows.h>
#include "GamePad.h"
#include <algorithm>
namespace
{
	const float STICK_DEADZONE = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE / 32767.0f;
	constexpr SHORT LT_RT_DEAD_ZONE = 30;
	const DWORD PLAYER_INDEX = 0;//パッド情報を取得するプレイヤー番号
};

void CGamePad::Update(float DeltaTime)
{
	m_PrevInputState = m_InputState;
	const DWORD result = XInputGetState(PLAYER_INDEX, &m_InputState);

	if (result != ERROR_SUCCESS)
	{
		return;
	}
	UpdateVibration(DeltaTime);
}

bool CGamePad::IsButtonTrigger(const int Key) const
{
	return ((m_InputState.Gamepad.wButtons & Key) && !(m_PrevInputState.Gamepad.wButtons & Key));
}

bool CGamePad::IsButtonPress(const int Key) const
{
	return m_InputState.Gamepad.wButtons & Key;
}

bool CGamePad::LTButtonTrigger() const
{
	if (m_PrevInputState.Gamepad.bLeftTrigger < LT_RT_DEAD_ZONE && m_InputState.Gamepad.bLeftTrigger >= LT_RT_DEAD_ZONE)
	{
		return true;
	}
	return false;
}

bool CGamePad::LTButtonPress() const
{
	if (m_InputState.Gamepad.bLeftTrigger >= LT_RT_DEAD_ZONE)
	{
		return true;
	}
	return false;
}

bool CGamePad::RTButtonTrigger() const
{
	if (m_PrevInputState.Gamepad.bRightTrigger < LT_RT_DEAD_ZONE && m_InputState.Gamepad.bRightTrigger >= LT_RT_DEAD_ZONE)
	{
		return true;
	}
	return false;
}

bool CGamePad::RTButtonPress() const
{
	if (m_InputState.Gamepad.bRightTrigger >= LT_RT_DEAD_ZONE)
	{
		return true;
	}
	return false;
}

bool CGamePad::LStickYTriggerCheckUp() const
{
	return m_PrevInputState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && m_InputState.Gamepad.sThumbLY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool CGamePad::LStickYTriggerCheckDown() const
{
	return m_PrevInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && m_InputState.Gamepad.sThumbLY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool CGamePad::LStickXTriggerCheckLeft() const
{
	return m_PrevInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && m_InputState.Gamepad.sThumbLX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

bool CGamePad::LStickXTriggerCheckRight() const
{
	return m_PrevInputState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && m_InputState.Gamepad.sThumbLX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
}

void CGamePad::PlayVibration(const float Time, const int Power)
{
	if (m_IsVibration)
	{
		return;
	}

	m_IsVibration = true;
	m_VibrationTime = Time;
	m_VibrationCountTime = 0.0f;
	m_VibPower = Power;
}

void CGamePad::UpdateVibration(float DeltaTime)
{
	if (!m_IsVibration)
	{
		return;
	}
	
	if (m_VibrationCountTime < m_VibrationTime)
	{
		XINPUT_VIBRATION vibration;                
		vibration.wLeftMotorSpeed = m_VibPower;
		vibration.wRightMotorSpeed = m_VibPower;

		XInputSetState(PLAYER_INDEX, &vibration);// ジョイスティックにバイブレータデータを送信。
		m_VibrationCountTime += DeltaTime;
	}
	else
	{
		m_IsVibration = false;
		m_VibrationCountTime = 0.0f;
		m_VibPower = 0;
		XINPUT_VIBRATION vibration{};
		XInputSetState(PLAYER_INDEX, &vibration);
	}
}

Vector2 CGamePad::ClampStick(const SHORT InputVectorX, const SHORT InputVectorY) const
{
	Vector2 returnStick;
	returnStick.x = NormalizeAxis(InputVectorX);
	returnStick.y = NormalizeAxis(InputVectorY);

	const float length = returnStick.Length();
	if (length < STICK_DEADZONE)
	{
		returnStick = Vector2::Zero();
	}
	else
	{
		// デッドゾーンを考慮した値に変換
		const float scaled = (length - STICK_DEADZONE) / (1.0f - STICK_DEADZONE);
		const Vector2 lengthVector = returnStick / length;
		returnStick = lengthVector * scaled;
	}
	return returnStick;
}

float CGamePad::NormalizeAxis(const SHORT InputVector) const
{
	const float axis = (InputVector < 0) ? InputVector / 32768.0f : InputVector / 32767.0f;
	return std::clamp(axis, -1.0f, 1.0f);
}

Vector2 CGamePad::GetLeftStick()
{
	const Vector2 leftStick = ClampStick(m_InputState.Gamepad.sThumbLX, m_InputState.Gamepad.sThumbLY);
	return leftStick;
}

Vector2 CGamePad::GetRightStick()
{
	const Vector2 rightStick = ClampStick(m_InputState.Gamepad.sThumbRX, m_InputState.Gamepad.sThumbRY);
	return rightStick;
}