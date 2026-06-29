#pragma once
//ボス戦に使用するシーン。オブジェクト数が少ないため処理上でオブジェクトの配置を行う
#include "../Scene/Scene.h"
class CImGui;

class CFirstBossMap : public CScene
{
private:
	CImGui* m_ImGui = nullptr;
	bool m_IsStageClearChangeScene = false;//ステージクリアによるシーン遷移なら真

public:
	virtual void Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex) override;
	virtual void UnInit() override;
	virtual ESceneType Update(float DeltaTime) override;
	virtual void Draw() override;
};