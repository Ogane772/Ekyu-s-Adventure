#include "Character.h"
#include <algorithm>
#include "../../Framework/Camera/Camera.h"
#include "../../Framework/Time/Time.h"
#include "../../Framework/HitStop/HitStop.h"

void CCharacter::Move(const SpeedParameter& SpeedParam, const float DeltaTime, const bool IsForcedBrake /*= false*/)
{
	const float movePower = SpeedParam.MoveDirection.LengthSq();
	Vector3 moveDirection = SpeedParam.MoveDirection;
	const float targetSpeed = SpeedParam.MaxSpeed * movePower;
	float applyRate = SpeedParam.Acceleration;

	bool isDeceleration = IsForcedBrake;
	//現在の速度が最高速度を超えている場合は減速
	if (m_CurrentSpeed > SpeedParam.MaxSpeed || isDeceleration)
	{
		applyRate = SpeedParam.Deceleration;
		isDeceleration = true;
	}
	const float alpha = std::clamp(applyRate * DeltaTime, 0.0f, 1.0f);
	if (isDeceleration)
	{
		m_CurrentSpeed = std::lerp(m_CurrentSpeed, SpeedParam.MinSpeed, alpha);
	}
	else
	{
		m_CurrentSpeed = std::lerp(m_CurrentSpeed, targetSpeed, alpha);
	}
	m_CurrentSpeed = (std::max)(m_CurrentSpeed, SpeedParam.MinSpeed);

	const float timeScale = GetTimeScale();

	//スティックを半倒しされた際にも入力方向を1として扱いたいため丸める
	moveDirection.Normalize();
	m_Position.z += moveDirection.z * m_CurrentSpeed * timeScale;
	m_Position.x += moveDirection.x * m_CurrentSpeed * timeScale;

	if (SpeedParam.RotationSpeed == 0.0f || moveDirection.LengthSq() == 0.0f)
	{
		return;
	}
	DirectionRotation(SpeedParam.RotationSpeed, DeltaTime, moveDirection);
	UpdateVector();
}

float CCharacter::GetAtan2Rotation(const Vector2& Direction) const
{
	const float rotation = atan2(Direction.x, Direction.y);
	return XMConvertToDegrees(rotation);
}

void CCharacter::DirectionRotation(const float RotationSpeed, float DeltaTime, const Vector3& Direction)
{
	const float goalRotationY = GetAtan2Rotation(Vector2(Direction.x, Direction.z));
	TargetRotation(RotationSpeed, DeltaTime, Vector3(m_Rotation.x, goalRotationY, m_Rotation.z));
}

void CCharacter::SetRotationYaw(const float Yaw)
{
	m_Rotation.y = Yaw;
	UpdateVector();
}

bool CCharacter::IsDeath() const
{
	return m_HP.GetCurrentHP() == 0;
}

void CCharacter::PlayCameraShake(const ECameraShakeType Type)
{
	if (CCamera* camera = GetCamera())
	{
		camera->PlayCameraShake(Type);
	}
}

void CCharacter::PlayHitStop(const EHitStopType Type)
{
	if (CHitStop* hitStop = CHitStop::GetInstance())
	{
		hitStop->Start(Type);
	}
}

float CCharacter::GetTimeScale() const
{
	float timeScale = 1.0f;
	if (CTime* time = CTime::GetInstance())
	{
		timeScale = time->GetTimeScale();
	}
	return timeScale;
}