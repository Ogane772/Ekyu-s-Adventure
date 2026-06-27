#include "StageSelectPlayer.h"
#include "../../../Framework/Model/SkeletalMesh.h"
#include "State/PlayrStateTypeAndAnimationNumber.h"

namespace
{
	constexpr float STAND_UP_HEIGHT_SPEED = 2.5f;//立ち上がる際の高さの速度
	constexpr float STAND_UP_HEIGHT_TIME = 0.2f;//立ち上がる際の高さを移動する時間
};

void CStageSelectPlayer::Init(const Vector3& SetPos, const Vector3& SetRot)
{
	m_AnimationDataList =
	{
		{ EPlayerAnimationNumber::SIT,			   AnimationData(0.5f, true)  },
		{ EPlayerAnimationNumber::WALL_HIT_END,	   AnimationData(0.4f, false)  },
		{ EPlayerAnimationNumber::RUN,			   AnimationData(0.4f,  true)  },
	};
	m_SpeedParameterList =
	{
		{ EStageSelectPlayerSpeedType::STAND_UP, SpeedParameter(0.0f, 0.13f, 2.0f, 0.0f, 0.0f) },
		{ EStageSelectPlayerSpeedType::MOVE, SpeedParameter(0.0f, 0.24f, 0.0f, 5.0f, 0.0f) },
	};
	m_IsNoCullingCheck = true;
	m_Position = SetPos;
	m_Scale = Vector3(8.0f, 8.0f, 8.0f);
	m_Rotation = SetRot;
	m_SkeletalMesh = std::make_unique<CSkeletalMesh>(ESkeletalMeshType::PLAYER);
	PlayAnimation<EPlayerAnimationNumber>(m_AnimationDataList, EPlayerAnimationNumber::SIT, true);
	CreateVector();
	UpdateVector();
}

void CStageSelectPlayer::Update(float DeltaTime)
{
	if (!m_SkeletalMesh)
	{
		return;
	}
	m_SkeletalMesh->Update(DeltaTime);

	if (!m_SelectMove)
	{
		UpdateWorldMatrix();
		return;
	}
	
	//WALL_HIT_END=椅子から立ち上がるアニメーション。椅子から立ち上がる→前に向かって移動を行う
	if (m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::WALL_HIT_END))
	{
		SpeedParameter& speedParam = m_SpeedParameterList[EStageSelectPlayerSpeedType::STAND_UP];
		speedParam.MoveDirection = m_Front;
		Move(speedParam, DeltaTime, false);
		if (m_FrameTime > STAND_UP_HEIGHT_TIME)
		{
			m_Position.y -= STAND_UP_HEIGHT_SPEED * DeltaTime;
		}
		m_FrameTime += DeltaTime;
	}
	if (IsAnimationEnd(EPlayerAnimationNumber::WALL_HIT_END))
	{
		PlayAnimation<EPlayerAnimationNumber>(m_AnimationDataList, EPlayerAnimationNumber::RUN, true);
	}

	if (m_CurrentAnimationNumber == static_cast<int>(EPlayerAnimationNumber::RUN))
	{
		SpeedParameter& speedParam = m_SpeedParameterList[EStageSelectPlayerSpeedType::MOVE];
		speedParam.MoveDirection = m_Front;
		Move(speedParam, DeltaTime, false);
	}

	UpdateWorldMatrix();
}

void CStageSelectPlayer::Draw()
{
	if (m_SkeletalMesh)
	{
		m_SkeletalMesh->Draw(m_World);
	}
}

void CStageSelectPlayer::SetSelectMove()
{
	PlayAnimation<EPlayerAnimationNumber>(m_AnimationDataList, EPlayerAnimationNumber::WALL_HIT_END, true);
	m_SelectMove = true;
}