#pragma once
#include "../Scene/Scene.h"
class CViewModel;

class CModelViewMap : public CScene
{
private:
	CViewModel* m_ViewModel;

public:
	virtual void Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex);
	virtual ESceneType Update(float DeltaTime);
	virtual void Draw();
	CViewModel* GetViewModel() const { return m_ViewModel; }
};