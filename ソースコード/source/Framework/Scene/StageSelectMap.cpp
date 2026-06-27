#include "StageSelectMap.h"
#include "../Shadow/ShadowMap.h"
#include "../Mouse/MouseCursor.h"
#include "../SaveAndLoad/SaveAndLoad.h"
#include "../Camera/Camera.h"
#include "../Renderer/Renderer.h"
#include "../Fade/Fade.h"
#include "../ImGui/DebugImGui.h"
#include "../Sound/SoundTypes.h"
#include "../Scene/SceneTypes.h"
#include "../../Game/UserInterface/StageSelectUI.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"
#include "../../Game/Character/Player/StageSelectPlayer.h"
#include "../../Game/Object/StageSelectFrame/StageSelectFrame.h"

namespace
{
	constexpr float FADE_START_TIME = 0.7f;
};

void CStageSelectMap::Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex)
{
	m_NormalFadeOutTime = 1.4f;
	m_CircleFadeOutTime = 1.4f;
	m_Camera = CreateAddGameObject<CCamera>(ESceneObjectType::CAMERA);
	CShadowMap::SetLightRotation(Vector3(79.385f, -9.231f, 31.385f));


	//各種ステージのフレームを作成
	if(CStageSelectFrame* frame = new CStageSelectFrame(EStaticMeshType::STAGESELECT_TUTOLIAL_STAGE_FRAME, Vector3(-2.109f, 2.68f, 54.84f), Vector3(1.193f, 1.193f, 1.193f), ESceneType::TUTORIAL, 1, 0))
	{
		AddGameObject(ESceneObjectType::NOMESH_OBJECT, frame);
		frame->SetOriginalNumber(-1);
	}
	if(CStageSelectFrame* frame = new CStageSelectFrame(EStaticMeshType::STAGESELECT_FIRST_STAGE_FRAME, Vector3(5.014f, 2.1f, 54.84f), Vector3(1.0f, 1.0f, 1.0f), ESceneType::FIRST, 8, 100))
	{
		AddGameObject(ESceneObjectType::NOMESH_OBJECT, frame);
		frame->SetOriginalNumber(-1);
	}
	if(CStageSelectFrame* frame = new CStageSelectFrame(EStaticMeshType::STAGESELECT_SECOND_STAGE_FRAME, Vector3(12.622f, 2.55f, 54.84f), Vector3(1.0f, 1.0f, 1.0f), ESceneType::SECOND, 7, 100))
	{
		AddGameObject(ESceneObjectType::NOMESH_OBJECT, frame);
		frame->SetOriginalNumber(-1);
	}

	m_MouseCursor = CreateAddGameObject<CMouseCursor>(ESceneObjectType::UI);

	if (CSaveAndLoad* save = CSaveAndLoad::GetInstance())
	{
		save->Load("SelectMAP.txt");
	}

	m_TitlePlayer = new CStageSelectPlayer();
	if (m_TitlePlayer)
	{
		m_TitlePlayer->Init(Vector3(-1.5f, 2.15f, 47.0f), Vector3(0.0f, 122.0f, 0.0f));
	}
	AddGameObject(ESceneObjectType::PLAYER, m_TitlePlayer);

	m_StageSelectUI = CreateAddGameObject<CStageSelectUI>(ESceneObjectType::UI);
	m_ImGui = CreateAddGameObject<CImGui>(ESceneObjectType::UI);
	if (m_ImGui)
	{
		m_ImGui->SetSaveFile(ESceneType::STAGE_SELECT);
	}

	if (m_Camera)
	{
		m_Camera->ChangeState(ECameraState::STAGE_SELECT);
	}

	if (CFade* fade = CFade::GetInstance())
	{
		fade->Start(false, 1.5f, 1.5f);
	}
	PlayBGM(ESoundIndex::STAGESELECT_BGM);

}

void CStageSelectMap::UnInit()
{
	CScene::UnInit();
	CStaticMesh::AllLoadModelDelete();
}

ESceneType CStageSelectMap::Update(float DeltaTime)
{
	if (!m_TitlePlayer || !m_StageSelectUI)
	{
		return ESceneType::STAGE_SELECT;
	}
	CScene::Update(DeltaTime);
	CShadowMap::Update();

	//ステージが選択されたらプレイヤーの移動開始
	if (!m_IsStageSelect && m_StageSelectUI->IsStageSelect())
	{
		m_IsStageSelect = true;
		m_TitlePlayer->SetSelectMove();
	}

	//プレイヤーの椅子立ち上がり→移動に合わせてフェードを開始する
	if (m_IsStageSelect)
	{
		m_CountFadeStartTime += DeltaTime;
		if (m_CountFadeStartTime > FADE_START_TIME)
		{
			return m_StageSelectUI->GetSelectSceneType();
		}
	}
	return ESceneType::STAGE_SELECT;
}

void CStageSelectMap::Draw()
{
	CScene::ShadowDraw();
	CScene::Draw();
	CRenderer::SetShader();
}
