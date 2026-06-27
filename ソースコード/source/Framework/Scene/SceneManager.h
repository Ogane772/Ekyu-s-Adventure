#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "../Singleton/Singleton.h"
#include "../State/FunctionStateMachine.h"

class CScene;
class CCamera;
class CPlayer;
class CMouseCursor;
class CEffectManager;
enum class ESceneType;
enum class ESoundIndex;

class CSceneManager : public CSingleton<CSceneManager>
{
private:
	enum class EFadeOutState
	{
		NONE,
		FADE_OUT,
	};

	std::shared_ptr<CScene> m_Scene;
	ESceneType m_CurrentSceneType;
	ESceneType m_ChangeSceneType;
	ESceneType m_InterruptScene;//ポーズやEditor等のシーンクラス以外から要求されたシーン

	std::unordered_map<ESceneType, ESoundIndex> m_SceneBGMList;//シーンタイプに応じたステージのBGMリスト
	CFunctionStateMachine<EFadeOutState> m_FadeOutStateMachine;

	void StartFadeOut();
	void UpdateFadeOut(float DeltaTime);
	void EndFadeOut();

public:
	void Init();
	~CSceneManager();
	void Update(float DeltaTime);
	void Draw();
	void ResizeWindow();
	std::weak_ptr<CScene> GetScene() const { return m_Scene; }

	void ChangeScene(const ESceneType Type);

	std::shared_ptr<CScene> CreateScene(const ESceneType Type);

	void SetInterruptScene(ESceneType Type) { m_InterruptScene = Type; }

	void RetryScene() { m_InterruptScene = m_CurrentSceneType; }

	bool IsTutorialMap() const;

	CCamera* GetCamera() const;
	CPlayer* GetPlayer() const;
	CMouseCursor* GetMouseCursor() const;
	CEffectManager* GetEffectManager() const;
};