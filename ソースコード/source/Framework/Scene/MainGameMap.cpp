#include "MainGameMap.h"
#include "../Shadow/ShadowMap.h"
#include "../Renderer/Renderer.h"
#include "../Input/InputInterface.h"
#include "../Fade/Fade.h"
#include "../GameObject/GameObject.h"
#include "../ImGui/DebugImGui.h"
#include "../Camera/Camera.h"
#include "../SaveAndLoad/SaveAndLoad.h"
#include "../Model/StaticMesh.h"
#include "../Mouse/MouseCursor.h"
#include "../Scene/SceneTypes.h"
#include "../../Game/UserInterface/MainGameUI.h"
#include "../../Game/UserInterface/PauseUI.h"
#include "../../Game/Effect/EffectManager.h"
#include "../../Game/Character/Player/Player.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"

void CMainGameMap::Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex)
{
	m_Camera = CreateAddGameObject<CCamera>(ESceneObjectType::CAMERA);
	m_EffectManager = CreateAddGameObject<CEffectManager>(ESceneObjectType::BILLBOARD);
	CShadowMap::SetLightRotation(Vector3(79.385f, -9.231f, 31.385f));
	
	m_Player = CreateAddGameObject<CPlayer>(ESceneObjectType::PLAYER);
	if (!m_Player)
	{
		return;
	}
	m_Camera->SetTarget(m_Player);

	m_MouseCursor = CreateAddGameObject<CMouseCursor>(ESceneObjectType::UI);

	if (CSaveAndLoad* save = CSaveAndLoad::GetInstance())
	{
		save->Load(LoadSaveFile.c_str());
	}

	CPlayerStart* start = GetGameObject<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT);
	if (start)
	{
		m_Player->SetPosition(Vector3(start->GetPosition()));
		m_Player->SetDefaultPosition(Vector3(start->GetPosition()));
		m_Player->SetRotationYaw(start->GetRotation().y);
	}
	else
	{
		m_Player->SetPosition(Vector3(-1.538f, 3.236f, -0.513f));
		m_Player->SetDefaultPosition(Vector3(-1.538f, 3.236f, -0.513f));
	}
	
	CreateAddGameObject<CMainGameUI>(ESceneObjectType::UI);
	CreateAddGameObject<CPauseUI>(ESceneObjectType::UI);

	m_ImGui = CreateAddGameObject<CImGui>(ESceneObjectType::UI);
	if (m_ImGui)
	{
		m_ImGui->SetSaveFile(m_SceneType);
	}
	if (m_Camera)
	{
		m_Camera->ChangeState(ECameraState::CAMERA_RESET);
	}

	if (CFade* fade = CFade::GetInstance())
	{
		fade->Start(false, 1.5f, 1.5f);
	}

	PlayBGM(PlayBGMIndex);
}

void CMainGameMap::UnInit()
{
	CScene::UnInit();
	if (!m_IsRetry)
	{
		CStaticMesh::AllLoadModelDelete();
	}
}

ESceneType CMainGameMap::Update(float DeltaTime)
{
	CScene::Update(DeltaTime);

	CShadowMap::Update();
	
	if (!m_Player)
	{
		return m_SceneType;
	}
	if (!m_IsStageClearChangeScene && m_Player->IsEnableClearButton())
	{
		CInputInterface* instance = CInputInterface::GetInstance();
		if (!instance)
		{
			return m_SceneType;
		}
		
		if (instance->IsInteractTrigger())
		{
			m_IsStageClearChangeScene = true;
			if (m_SceneType == ESceneType::TUTORIAL)
			{
				if (CSaveAndLoad* save = CSaveAndLoad::GetInstance())
				{
					save->SetMyStarAndAcorn(ESceneType::TUTORIAL, m_Player->GetMyStarCount(), m_Player->GetMyAcornCount());
					save->PlayerSave();
				}
				return ESceneType::STAGE_SELECT;
			}
			if (CSaveAndLoad* save = CSaveAndLoad::GetInstance())
			{
				save->SetTemporaryItemCount(m_Player->GetMyStarCount(), m_Player->GetMyAcornCount());
				save->SetStartMainGameScene(m_SceneType);
			}
			return ESceneType::BOSS;
		}
	}

	return m_SceneType;
}

void CMainGameMap::Draw()
{
	CScene::ShadowDraw();
	CScene::Draw();
	CRenderer::SetShader();
}
