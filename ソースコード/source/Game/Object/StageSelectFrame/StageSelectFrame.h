#pragma once
//ステージセレクトマップに使用する。ステージを表示する窓枠クラス
#include <vector>
#include <memory>
#include "../../../Framework/GameObject/GameObject.h"
#include "../../../Framework/Model/StaticMesh.h"
class CCommonObject;
enum class ESceneType;

//ステージ情報構造体
struct StageInfo
{
	//そのステージのスターとドングリ数
	int StageStar = 0;
	int StageAcorn = 0;
	//そのステージで獲得したスターとドングリ数
	int GetStageStar = 0;
	int GetStageAcorn = 0;
};

class CStageSelectFrame : public CGameObject
{
private:
	//UI側でカーソルがあっていたら真
	bool m_IsSelect = false;
	StageInfo m_StageInfo = StageInfo();
	std::unique_ptr<CStaticMesh> m_Model;
	ESceneType m_SceneType = ESceneType();
	float m_CountTime = 0;

public:
	CStageSelectFrame(const EStaticMeshType Model, const Vector3 SetPos, const Vector3 SetScale, const ESceneType Type, const int star, const int acorn);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	StageInfo GetStageInfo() const { return m_StageInfo; }
	bool IsSelect() const { return m_IsSelect; }
	void SetSelect(const bool Select)
	{
		if (m_IsSelect != Select)
		{
			m_CountTime = 0.0f;
		}
		m_IsSelect = Select; 
	}
	void SetSceneType(const ESceneType Type) { m_SceneType = Type; }
	ESceneType GetSceneType() const { return m_SceneType; }
};
