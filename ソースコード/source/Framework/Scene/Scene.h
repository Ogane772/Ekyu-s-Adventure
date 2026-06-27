#pragma once
#include <typeinfo>
#include <vector>
#include <string>

class CEnemy;
class CMobEnemy;
class CHitBlock;
class CCamera;
class CPlayer;
class CMouseCursor;
class CEffectManager;
class CImGui;
class CSceneObject;
enum class ESoundIndex;
enum class ESceneType;
//ゲームオブジェクト格納番号
enum class ESceneObjectType
{
	PLAYER,
	ENEMY,
	MESH_OBJECT,
	NOMESH_OBJECT,
	BILLBOARD,
	SOUND,
	UI,
	CAMERA,
	MAX
};
class CScene
{
protected:
	ESceneType m_SceneType = ESceneType();
	std::vector<CSceneObject*> m_SceneObject[static_cast<int>(ESceneObjectType::MAX)];
	CCamera* m_Camera = nullptr;
	CPlayer* m_Player = nullptr;
	CMouseCursor* m_MouseCursor = nullptr;
	CEffectManager* m_EffectManager = nullptr;
	bool m_IsRetry = false;
	//フェードアウト用変数。シーン遷移時のフェード時間はSceneManager側で管理するため時間のみ定義
	float m_NormalFadeOutTime = 1.0f;
	float m_CircleFadeOutTime = 1.0f;

public:
	CScene() {};
	virtual ~CScene() {};
	virtual void Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex) = 0;

	//指定したクラスを作成及びリストに追加する
	template <typename T>
	T* CreateAddGameObject(const ESceneObjectType Type)
	{
		T* object = new T();
		if (!object)
		{
			return nullptr;
		}
		object->Init();
		m_SceneObject[static_cast<int>(Type)].emplace_back(object);

		return object;
	}

	//指定したクラスをリストに追加する
	template <typename T>
	T* AddGameObject(const ESceneObjectType Type, T* Object)
	{
		m_SceneObject[static_cast<int>(Type)].emplace_back(Object);
		return Object;
	}

	virtual void UnInit();

	virtual ESceneType Update(float DeltaTime);

	virtual void Draw();

	void ShadowDraw();

	//最初に一致した指定したオブジェクトクラスを取得
	template <typename T>
	T* GetGameObject(const ESceneObjectType Type)
	{
		for (CSceneObject* object : m_SceneObject[static_cast<int>(Type)])
		{//typeid = 引数で渡した変数の型を調べる
			if (typeid(*object) == typeid(T))
			{
				return (T*)object;
			}
		}
		return nullptr;
	}

	//指定したオブジェクトクラスのリストを取得
	template <typename T>
	void GetGameObjects(const ESceneObjectType Type, std::vector<T*>& OutList)
	{
		for (CSceneObject* object : m_SceneObject[static_cast<int>(Type)])
		{
			if (typeid(*object) == typeid(T))
			{
				OutList.emplace_back((T*)object);
			}
		}
	}

	//エネミーオブジェクトの取得
	void GetGameEnemyObjects(const ESceneObjectType Type, std::vector<CEnemy*>& OutList);

	//雑魚エネミーオブジェクトの取得
	void GetGameMobEnemyObjects(const ESceneObjectType Type, std::vector<CMobEnemy*>& OutList);

	//ヒットブロックオブジェクトの取得
	void GetHitBlockObjects(const ESceneObjectType Type, std::vector<CHitBlock*>& OutList);

	int GetGameObjectCount(const ESceneObjectType Type) const
	{
		return static_cast<int>(m_SceneObject[static_cast<int>(Type)].size());
	}

	void PlayBGM(const ESoundIndex SoundIndex);
	bool IsRetry() const { return m_IsRetry; }
	void SetSceneType(const ESceneType Type) { m_SceneType = Type; }

	float GetNormalFadeOutTime() const { return m_NormalFadeOutTime; }
	float GetCircleFadeOutTime() const { return m_CircleFadeOutTime; }

	CCamera* GetCamera() const { return m_Camera; }
	CPlayer* GetPlayer() const { return m_Player; }
	CMouseCursor* GetMouseCursor() const { return m_MouseCursor; }
	CEffectManager* GetEffectManager() const { return m_EffectManager; }
};