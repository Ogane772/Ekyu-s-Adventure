#include "StageSelectFrame.h"
#include "../../../Framework/Scene/Scene.h"
#include "../../UserInterface/StageSelectUI.h"

namespace
{
	constexpr float SELECT_FRAME_Z = 52.4f;//カーソルがあっているときに前進する位置
	constexpr float SELECT_ROTATION_SPEED = 2.2f;//カーソルがあっているときの回転速度
	constexpr float SELECT_ROTATION_LIMIT = 8.0f;//カーソルがあっているときの回転値の上限
	constexpr float SELECT_LERP_SPEED = 0.2f;//カーソルがあっているときの補間速度
	constexpr float UNSELECTED_LERP_SPEED = 0.1f;//カーソルがあっていない時の補間速度
};

CStageSelectFrame::CStageSelectFrame(const EStaticMeshType Model, const Vector3 SetPos, const Vector3 SetScale, const ESceneType Type, const int star, const int acorn)
{
	m_SceneType = Type;
	m_Position = SetPos;
	m_Rotation = Vector3::Zero();
	m_DefaultPosition = m_Position;
	m_Scale = SetScale;
	m_StageInfo.StageStar = star;
	m_StageInfo.StageAcorn = acorn;
	m_Model = std::make_unique<CStaticMesh>(Model);
	if (m_Model)
	{
		m_Model->SetEnableLight(false);
	}
}

void CStageSelectFrame::Update(float DeltaTime)
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	CStageSelectUI* ui = scene.lock()->GetGameObject<CStageSelectUI>(ESceneObjectType::UI);
	//ステージが選択された際は回転値をリセット
	if (ui && ui->IsStageSelect())
	{
		m_Rotation = Vector3::Zero();
		return;
	}
	//ステージ選択項目があっている際は前に進むかつ左右に回転する
	if (m_IsSelect)
	{
		const float alpha = std::clamp(m_CountTime / SELECT_LERP_SPEED, 0.0f, 1.0f);
		m_Position.z = std::lerp(m_DefaultPosition.z, SELECT_FRAME_Z, alpha);
		m_Rotation.z = sinf(m_CountTime * SELECT_ROTATION_SPEED) * SELECT_ROTATION_LIMIT;
	}
	else
	{
		//非選択時は位置と回転をデフォルトに戻す
		const float alpha = std::clamp(m_CountTime / UNSELECTED_LERP_SPEED, 0.0f, 1.0f);
		m_Position.z = std::lerp(m_Position.z, m_DefaultPosition.z, alpha);
		m_Rotation.z = std::lerp(m_Rotation.z, 0.0f, alpha);
	}
	m_CountTime += DeltaTime;
	UpdateWorldMatrix();
}

void CStageSelectFrame::Draw()
{
	if (m_Model)
	{
		m_Model->Draw(m_World);
	}
}

