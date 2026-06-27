#pragma once
#include "../Character.h"
//ステージセレクトで表示するプレイヤー
struct AnimationData;
enum class EPlayerAnimationNumber;

class CStageSelectPlayer : public CCharacter
{
private:
	enum class EStageSelectPlayerSpeedType
	{
		STAND_UP,
		MOVE,
	};
	bool m_SelectMove = false;
	//アニメーションリスト
	std::unordered_map<EPlayerAnimationNumber, AnimationData> m_AnimationDataList;
	std::unordered_map<EStageSelectPlayerSpeedType, SpeedParameter> m_SpeedParameterList;

public:
	void Init(const Vector3& SetPos, const Vector3& SetRot);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	void SetSelectMove();
};
