#pragma once
//キャラクターの親クラス
#include "../../Framework/GameObject/GameObject.h"
#include "../../Framework/Status/HP.h"
#include "../../Framework/Model/SkeletalMesh.h"
#include "AnimationData.h"
#include <unordered_map>
enum class ECameraShakeType;
enum class EHitStopType;

class CCharacter : public CGameObject
{
protected:
	struct SpeedParameter
	{
		float MinSpeed = 0.0f;//最低速度
		float MaxSpeed = 0.0f;//最高速度
		float Acceleration = 0.0f;//加速度
		float Deceleration = 0.0f;//減速度
		float RotationSpeed = 0.0f;//回転速度
		Vector3 MoveDirection = Vector3::Zero();//移動方向

		SpeedParameter() {};
		SpeedParameter(const float InMinSpeed, const float InMaxSpeed, const float InAcceleration, const float InDeceleration, const float InRotationSpeed)
			: MinSpeed(InMinSpeed), MaxSpeed(InMaxSpeed), Acceleration(InAcceleration), Deceleration(InDeceleration), RotationSpeed(InRotationSpeed)
		{};
	};

	float m_CurrentSpeed = 0.0f;
	HP m_HP;
	std::unique_ptr<CSkeletalMesh> m_SkeletalMesh;
	int m_CurrentAnimationNumber = -1;
	int m_PrevAnimationNumber = -1;
	//真の場合、アニメーション切り替え時に補間設定が入っていても無視する(一部アニメーションの場合、意図しない補間が入るため)
	bool m_IsNotBlendAnimation = false;

	void Move(const SpeedParameter& SpeedParam, const float DeltaTime, const bool IsForcedBrake = false);
	//引数で渡したX.Zのベクトルの方にY軸を回転させる
	float GetAtan2Rotation(const Vector2& Direction) const;
	void DirectionRotation(const float RotationSpeed, float DeltaTime, const Vector3& Direction);

	template<typename T>
	void PlayAnimation(std::unordered_map<T, AnimationData>& AnimationDataList, const T AnimationNumber, const bool InIsBlend = false)
	{
		if (!m_SkeletalMesh)
		{
			return;
		}
		if (!AnimationDataList.contains(AnimationNumber))
		{
			return;
		}
		if (m_CurrentAnimationNumber == static_cast<int>(AnimationNumber))
		{
			return;
		}
		const AnimationData& data = AnimationDataList[AnimationNumber];
		m_PrevAnimationNumber = m_CurrentAnimationNumber;
		const bool isBlend = InIsBlend && !m_IsNotBlendAnimation;
		m_CurrentAnimationNumber = m_SkeletalMesh->SetAnimationChange(data.IsLoop, static_cast<int>(AnimationNumber), isBlend, data.AnimationSpeed);
		m_IsNotBlendAnimation = false;
	}

	template<typename T>
	bool IsAnimationEnd(const T AnimationNumber) const
	{
		if (!m_SkeletalMesh)
		{
			return false;
		}
		if (m_CurrentAnimationNumber != static_cast<int>(AnimationNumber))
		{
			return false;
		}
		return m_SkeletalMesh->IsEnd();
	}

public:
	void SetRotationYaw(const float Yaw);
	virtual void Damage(const int Damage) {};
	void ResetSpeed() { m_CurrentSpeed = 0.0f; }
	bool IsZeroSpeed() const { return m_CurrentSpeed <= 0.01f; }
	const HP& GetHP() const { return m_HP; }
	bool IsDeath() const;
	void SetNotBlendAnimation(const bool IsNot) { m_IsNotBlendAnimation = IsNot; }
	void PlayCameraShake(const ECameraShakeType Type);
	void PlayHitStop(const EHitStopType Type);
	float GetTimeScale() const;
};
