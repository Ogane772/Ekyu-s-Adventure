#pragma once
//タイトルマップ
#include "../Scene/Scene.h"
class CDemoPlayer;
class C2DPolygon;

class CTitleMap : public CScene
{
private:
	CDemoPlayer* m_TitleDemoPlayer = nullptr;
	C2DPolygon* m_TitlePolygon = nullptr;
	
public:
	virtual void Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex) override;
	virtual void UnInit() override;
	virtual ESceneType Update(float DeltaTime) override;
	virtual void Draw() override;
};