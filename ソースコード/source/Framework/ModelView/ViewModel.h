#pragma once

#include "../GameObject/GameObject.h"
#include <assimp/scene.h>
#include <string>
class CStaticMesh;
class CSkeletalMesh;

class CViewModel : public CGameObject
{
private:
	int m_AnimationNumber = 0;
	bool m_IsSkeletalMeshDraw = false;
	CStaticMesh* m_StaticMesh = nullptr;
	CSkeletalMesh* m_SkeletalMesh = nullptr;
	POINT m_Pt;
	std::string m_FileName;

public:
	virtual void Init() override;
	virtual void UnInit() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	virtual void ShadowDraw() override;
	void LoadModel(const std::string& FileName);
	int GetAnimationNumber() const { return m_AnimationNumber; }

	float GetAnimationSpeed() const;
	aiAnimation* GetAiAnimation() const;
	const aiScene* GetAiScene() const;
	float GetAnimeFrame() const;
	void SetAnimeSpeed(const float Speed);
	void SetAnimationNumber(const UINT Number);
	void SetAnimeLoop(const bool IsLoop);
	void ResetAnimeFrame();
	bool IsSkeletalMeshDraw() const { return m_IsSkeletalMeshDraw; }
	const std::string& GetFileName() const { return m_FileName; }
};