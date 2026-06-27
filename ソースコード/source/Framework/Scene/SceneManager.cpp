#include "SceneManager.h"
#include "../GameObject/GameObject.h"
#include "../Scene/SceneTypes.h"
#include "../Scene/Scene.h"
#include "../Scene/TitleMap.h"
#include "../Scene/FirstBossMap.h"
#include "../Scene/StageSelectMap.h"
#include "../Renderer/Renderer.h"
#include "../Polygon/2DPolygon/2DPolygon.h"
#include "../Camera/Camera.h"
#include "../ImGui/DebugImGui.h"
#include "../Scene/ModelViewMap.h"
#include "../Scene/MainGameMap.h"
#include "../Fade/Fade.h"
#include "../Sound/SoundTypes.h"
#include "../System/SystemContext.h"

void CSceneManager::Init()
{
	m_FadeOutStateMachine.AddState<CSceneManager>(
		EFadeOutState::NONE,
		this,
		nullptr,
		nullptr,
		nullptr
	);
	m_FadeOutStateMachine.AddState<CSceneManager>(
		EFadeOutState::FADE_OUT,
		this,
		&CSceneManager::StartFadeOut,
		&CSceneManager::UpdateFadeOut,
		&CSceneManager::EndFadeOut
	);
	m_InterruptScene = m_ChangeSceneType = ESceneType::NONE;
	//シーンに紐づくBGMを定義　NONE=シーン側で再生か、BGMを再生しない
	m_SceneBGMList =
	{
		{ ESceneType::TEST, ESoundIndex::NONE },
		{ ESceneType::TITLE, ESoundIndex::NONE },
		{ ESceneType::STAGE_SELECT, ESoundIndex::STAGESELECT_BGM },
		{ ESceneType::TUTORIAL, ESoundIndex::TUTORIAL_BGM },
		{ ESceneType::FIRST, ESoundIndex::FIRST_STAGE_BGM },
		{ ESceneType::SECOND, ESoundIndex::FIRST_STAGE_BGM },
		{ ESceneType::BOSS, ESoundIndex::BOSS_BGM },
		{ ESceneType::MODEL_VIEW, ESoundIndex::NONE },
	};
	CRenderer::Init();

	ChangeScene(ESceneType::TITLE);
}

CSceneManager::~CSceneManager()
{
	if (m_Scene)
	{
		m_Scene->UnInit();
		m_Scene.reset();
	}
	CRenderer::UnInit();
}

void CSceneManager::Update(float DeltaTime)
{
	//シーンから受け取るシーンタイプか外部から要求されたシーンタイプが現在と異なればシーン遷移
	//ただしリトライが要求された際は現在のシーンでシーンチェンジを行う
	ESceneType sceneType = m_Scene->Update(DeltaTime);
	if (m_ChangeSceneType == ESceneType::NONE)
	{
		if (m_Scene->IsRetry() || sceneType != m_CurrentSceneType || m_InterruptScene != ESceneType::NONE)
		{
			m_ChangeSceneType = sceneType;
			if (m_InterruptScene != ESceneType::NONE)
			{
				m_ChangeSceneType = m_InterruptScene;
			}
			m_FadeOutStateMachine.ChangeState(EFadeOutState::FADE_OUT);
		}
	}

	m_FadeOutStateMachine.Update(DeltaTime);

	if (CFade* fade = CFade::GetInstance())
	{
		fade->Update(DeltaTime);
	}
}

void CSceneManager::Draw()
{
	CRenderer::Begin();

	m_Scene->Draw();

	if (CFade* fade = CFade::GetInstance())
	{
		CRenderer::SetDepthEnable(false);
		fade->Draw();
		CRenderer::SetDepthEnable(true);
	}

	CRenderer::End();
}

void CSceneManager::ResizeWindow()
{
	std::vector<CImGui*> ims;
	m_Scene->GetGameObjects<CImGui>(ESceneObjectType::UI, ims);
	for (CImGui* im : ims)
	{
		im->ResizeWindow();
	}

	CRenderer::SetWorldViewProjection2D();
}

void CSceneManager::ChangeScene(const ESceneType Type)
{
	//今のシーンを削除
	if (m_Scene)
	{
		m_Scene->UnInit();
		m_Scene.reset();
	}
	//シーン先の初期化＆移行
	m_CurrentSceneType = Type;
	m_InterruptScene = m_ChangeSceneType = ESceneType::NONE;
	std::shared_ptr<CScene> scene = CreateScene(Type);
	m_Scene = std::move(scene);

	//シーンに紐づくセーブデータとBGMを取得
	std::string saveFileName = "";
	if (CSystemContext* system = CSystemContext::GetInstance())
	{
		saveFileName = system->GetLoadSaveFile(Type);
	}
	ESoundIndex bgmIndex = ESoundIndex::NONE;
	if (m_SceneBGMList.contains(Type))
	{
		bgmIndex = m_SceneBGMList[Type];
	}
	m_Scene->SetSceneType(Type);
	m_Scene->Init(saveFileName, bgmIndex);
}

std::shared_ptr<CScene> CSceneManager::CreateScene(const ESceneType Type)
{
	switch (Type)
	{
	case ESceneType::TITLE:
		return std::make_shared<CTitleMap>();
	case ESceneType::STAGE_SELECT:
		return std::make_shared<CStageSelectMap>();
	case ESceneType::TEST:
	case ESceneType::TUTORIAL:
	case ESceneType::FIRST:
	case ESceneType::SECOND:
		return std::make_shared<CMainGameMap>();
	case ESceneType::BOSS:
		return std::make_shared<CFirstBossMap>();
	case ESceneType::MODEL_VIEW:
		return std::make_shared<CModelViewMap>();
	}
	return nullptr;
}

bool CSceneManager::IsTutorialMap() const
{
	return m_CurrentSceneType == ESceneType::TUTORIAL;
}

CCamera* CSceneManager::GetCamera() const
{
	if (m_Scene)
	{
		return m_Scene->GetCamera();
	}
	return nullptr;
}

CPlayer* CSceneManager::GetPlayer() const
{
	if (m_Scene)
	{
		return m_Scene->GetPlayer();
	}
	return nullptr;
}

CMouseCursor* CSceneManager::GetMouseCursor() const
{
	if (m_Scene)
	{
		return m_Scene->GetMouseCursor();
	}
	return nullptr;
}

CEffectManager* CSceneManager::GetEffectManager() const
{
	if (m_Scene)
	{
		return m_Scene->GetEffectManager();
	}
	return nullptr;
}

void CSceneManager::StartFadeOut()
{
	if (!m_Scene)
	{
		return;
	}
	if (CFade* fade = CFade::GetInstance())
	{
		fade->Start(true, m_Scene->GetNormalFadeOutTime(), m_Scene->GetCircleFadeOutTime());
	}
}

void CSceneManager::UpdateFadeOut(float DeltaTime)
{
	CFade* fade = CFade::GetInstance();
	if (!fade)
	{
		m_FadeOutStateMachine.ChangeState(EFadeOutState::NONE);
	}
	if (!fade->IsFade())
	{
		m_FadeOutStateMachine.ChangeState(EFadeOutState::NONE);
	}
}

void CSceneManager::EndFadeOut()
{
	ChangeScene(m_ChangeSceneType);
}

