#pragma once
//チュートリアル、1面、2面といったゲーム部分のマップクラス
#include "../Scene/Scene.h"
class CImGui;

class CMainGameMap : public CScene
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