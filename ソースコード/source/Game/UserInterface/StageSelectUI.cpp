#include "StageSelectUI.h"
#include "../Object/StageSelectFrame/StageSelectFrame.h"
#include "../../Framework/TextureLoader/TextureTypes.h"
#include "../../Framework/Scene/Scene.h"
#include "../../Framework/Fade/Fade.h"
#include "../../Framework/Input/InputInterface.h"
#include "../../Framework/Sound/Sound.h"
#include "../../Framework/SaveAndLoad/SaveAndLoad.h"
#include "../../Framework/Renderer/Renderer.h"
#include "../../Framework/Polygon/2DPolygon/2DPolygon.h"
#include "../../Framework/Scene/SceneTypes.h"

void CStageSelectUI::Init()
{
	CSaveAndLoad* save = CSaveAndLoad::GetInstance();
	if (!save)
	{
		return;
	}
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	scene.lock()->GetGameObjects<CStageSelectFrame>(ESceneObjectType::NOMESH_OBJECT, m_FrameList);
	m_MaxSelectNumber = (int)m_FrameList.size() - 1;
	save->PlayerLoad();

	//ステージのスターとドングリ数を取得
	for (CStageSelectFrame* frame : m_FrameList)
	{
		if (!frame)
		{
			continue;
		}
		const ESceneType type = frame->GetSceneType();
		m_StageInfos.emplace(type, frame->GetStageInfo());
		m_StageInfos[type].GetStageAcorn = save->GetAcornCount(type);
		m_StageInfos[type].GetStageStar = save->GetStarCount(type);
	}
	//表示するステージ名の画像と位置を設定
	m_StageImageList.emplace(ESceneType::TUTORIAL, std::make_unique<C2DPolygon>(ETextureType::STAGESELECT_CHUTORIAL));
	m_StageImagePositionList.emplace(ESceneType::TUTORIAL, Vector2(462.0f, 440.0f));
	m_StageImageList.emplace(ESceneType::FIRST, std::make_unique<C2DPolygon>(ETextureType::STAGESELECT_FIRSTSTAGE));
	m_StageImagePositionList.emplace(ESceneType::FIRST, Vector2(473.0f, 445.0f));
	m_StageImageList.emplace(ESceneType::SECOND, std::make_unique<C2DPolygon>(ETextureType::STAGESELECT_SECONDSTAGE));
	m_StageImagePositionList.emplace(ESceneType::SECOND, Vector2(532.0f, 445.0f));

	//他のUI画像を読み込み
	m_StageSelect = std::make_unique<C2DPolygon>(ETextureType::STAGESELECT);
	m_StageSelectButton = std::make_unique<C2DPolygon>(ETextureType::STAGESELECT_BUTTON);
	m_BG = std::make_unique<C2DPolygon>(ETextureType::BLACK);
	m_Slash = std::make_unique<C2DPolygon>(ETextureType::STAGESELECT_SLASH);
	m_Star = std::make_unique<C2DPolygon>(ETextureType::UI_STAR);
	m_Acorn = std::make_unique<C2DPolygon>(ETextureType::UI_ACORN);
	m_Counter = std::make_unique<C2DPolygon>(ETextureType::UI_NUMBER);

}

void CStageSelectUI::Update(float DeltaTime)
{
	if (m_IsStageSelect)
	{
		return;
	}
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			return;
		}
	}
	
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}

	//入力によるステージ選択
	if (instance->IsLeftSelectTrigger())
	{
		m_SelectNumber--;
		if (m_SelectNumber < 0)
		{
			m_SelectNumber = m_MaxSelectNumber;
		}
		PlaySoundIndex(ESoundIndex::SELECT_FRAME);
	}
	else if (instance->IsRightSelectTrigger())
	{
		m_SelectNumber++;
		if (m_SelectNumber > m_MaxSelectNumber)
		{
			m_SelectNumber = 0;
		}
		PlaySoundIndex(ESoundIndex::SELECT_FRAME);
	}
	else if (instance->IsDecisionTrigger())
	{
		m_IsStageSelect = true;
		PlaySoundIndex(ESoundIndex::SELECT_STAGE);
		StopBGM();
	}
}

void CStageSelectUI::Draw()
{
	if (IsEditMode())
	{
		return;
	}
	if (CFade* fade = CFade::GetInstance())
	{
		if (fade->IsFade())
		{
			return;
		}
	}
	CRenderer::SetDepthEnable(false);
	StageSelectUi();
	CRenderer::SetDepthEnable(true);
}

ESceneType CStageSelectUI::GetSelectSceneType() const
{
	return m_FrameList[m_SelectNumber]->GetSceneType();
}

void CStageSelectUI::StageSelectUi()
{
	CRenderer::Set2DShader();
	CRenderer::SetWorldViewProjection2D();
	if (m_StageSelect)
	{
		m_StageSelect->Draw(SpriteParameter(Vector2(380.0f, 15.0f)));
	}
	SelectButtonDraw();
	SelectStageInfoDraw();
}

void CStageSelectUI::SelectButtonDraw()
{
	if (m_BG)
	{
		m_BG->Draw(SpriteParameter(Vector2(965.0f, 450.0f), Vector2(0.15f, 0.15f), 0.0f, 0.4f));
	}
	if (m_StageSelectButton)
	{
		m_StageSelectButton->Draw(SpriteParameter(Vector2(960.0f, 450.0f)));
	}
}

void CStageSelectUI::SelectStageInfoDraw()
{
	if (m_BG)
	{
		m_BG->Draw(SpriteParameter(Vector2(462.0f, 440.0f), Vector2(0.2f, 0.163f), 0.0f, 0.4f));
	}

	const ESceneType sceneType = GetSelectSceneType();
	SelectStageStarDraw(sceneType);
	SelectStageAcornDraw(sceneType);
	SetSelectStageSelect(sceneType);

	if (m_StageImageList[sceneType])
	{
		m_StageImageList[sceneType]->Draw(SpriteParameter(m_StageImagePositionList[sceneType]));
	}
}

void CStageSelectUI::SelectStageStarDraw(const ESceneType Type)
{
	if (m_Star)
	{
		m_Star->Draw(SpriteParameter(Vector2(532.0f, 490.0f), Vector2(0.6f, 0.6f)));
	}
	if (m_Counter)
	{
		m_Counter->ScoreDraw(Vector2(610.0f, 502.0f), m_StageInfos[Type].GetStageStar, 1);
	}
	if (m_Slash)
	{
		m_Slash->Draw(SpriteParameter(Vector2(635.0f, 497.0f), Vector2(0.5f, 0.5f)));
	}
	if (m_Counter)
	{
		m_Counter->ScoreDraw(Vector2(680.0f, 502.0f), m_StageInfos[Type].StageStar, 1);
	}
}

void CStageSelectUI::SelectStageAcornDraw(const ESceneType Type)
{
	if (!m_Counter || !m_Slash || !m_Acorn)
	{
		return;
	}

	m_Acorn->Draw(SpriteParameter(Vector2(540.0f, 546.0f), Vector2(0.6f, 0.6f)));

	if (m_StageInfos[Type].GetStageAcorn < 10)
	{
		//桁数1
		m_Counter->ScoreDraw(Vector2(610.0f, 558.0f), m_StageInfos[Type].GetStageAcorn, 1);
		m_Slash->Draw(SpriteParameter(Vector2(635.0f, 553.0f), Vector2(0.5f, 0.5f)));
		SelectStageAcornMax(Type);
	}
	else if (m_StageInfos[Type].GetStageAcorn < 100)
	{
		//桁数2
		m_Counter->ScoreDraw(Vector2(600.0f, 558.0f), m_StageInfos[Type].GetStageAcorn, 2);
		m_Slash->Draw(SpriteParameter(Vector2(655.0f, 553.0f), Vector2(0.5f, 0.5f)));
		SelectStageAcornMax(Type);
	}
	else if (m_StageInfos[Type].GetStageAcorn >= 100)
	{
		//桁数3
		m_Counter->ScoreDraw(Vector2(575.0f, 558.0f), m_StageInfos[Type].GetStageAcorn, 3);
		m_Slash->Draw(SpriteParameter(Vector2(671.0f, 553.0f), Vector2(0.5f, 0.5f)));
		SelectStageAcornMax(Type);
	}
}

void CStageSelectUI::SelectStageAcornMax(const ESceneType Type)
{
	if (!m_Counter)
	{
		return;
	}

	if (m_StageInfos[Type].StageAcorn < 10)
	{
		//桁数1
		m_Counter->ScoreDraw(Vector2(680.0f, 558.0f), m_StageInfos[Type].StageAcorn, 1);
	}
	else if (m_StageInfos[Type].StageAcorn < 100)
	{
		//桁数2
		m_Counter->ScoreDraw(Vector2(680.0f, 558.0f), m_StageInfos[Type].StageAcorn, 2);
	}
	else if (m_StageInfos[Type].StageAcorn >= 100)
	{
		if (m_StageInfos[Type].GetStageAcorn > 99)
		{
			//桁数3
			m_Counter->ScoreDraw(Vector2(700.0f, 558.0f), m_StageInfos[Type].StageAcorn, 3);
		}
		else
		{
			//桁数3
			m_Counter->ScoreDraw(Vector2(680.0f, 558.0f), m_StageInfos[Type].StageAcorn, 3);
		}
	}
}

void CStageSelectUI::SetSelectStageSelect(const ESceneType Type)
{
	for (CStageSelectFrame* frame : m_FrameList)
	{
		if (!frame)
		{
			continue;
		}

		if (frame->GetSceneType() == Type)
		{
			frame->SetSelect(true);
		}
		else
		{
			frame->SetSelect(false);
		}
	}
}