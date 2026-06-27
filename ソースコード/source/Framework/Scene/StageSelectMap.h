#pragma once
#include "../Scene/Scene.h"

class CStageSelectPlayer;
class CStageSelectUI;

class CStageSelectMap : public CScene
{
private:
	CStageSelectPlayer* m_TitlePlayer = nullptr;
	CImGui* m_ImGui = nullptr;
	CStageSelectUI* m_StageSelectUI = nullptr;
	bool m_IsStageSelect = false;
	float m_CountFadeStartTime = 0.0f;

public:
	virtual void Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex) override;
	virtual void UnInit() override;
	virtual ESceneType Update(float DeltaTime) override;
	virtual void Draw() override;
};
