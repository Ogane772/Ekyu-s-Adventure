#pragma once
//ステージクリアに必要なスターを表示するドアクラス
#include <vector>
#include "../../../Framework/GameObject/GameObject.h"
#include "../../../Framework/Time/TimeScheduler.h"
class CGameObject;
class CCommonObject;
class CStageClearDoor;

class CStarDoor : public CGameObject
{
private:
	//ドアが開くスターの数
	int m_OpenStarCount = 0;
	bool m_IsUpdateDoorAnimation = false;
	CCommonObject* m_StarOpenDoor = nullptr;
	CStageClearDoor* m_StageClearDoor = nullptr;
	CTimeScheduler m_DoorAnimationTimeScheduler;

public:
	CStarDoor(const Vector3& SetPos,const Vector3& SetRot, const int OpenStarCount);
	virtual void Update(float DeltaTime) override;
	//ドアが開くか確認　空いたらtrue
	bool IsEndStarDoorAnimation() const;
	void StartStarDoorAnimation();
	bool IsStartStarDoorAnimation(const int GetStarCount) const;
	virtual void SetPosition(const Vector3& SetPos)override;
	virtual void SetRotation(const Vector3& SetRot)override;
	virtual void ChangeTexture(const ETextureType Type, const int Index)override;
	int* GetOpenStar();
	virtual void SetDestroyEdit()override;
	void SetStarTexture();
	CCommonObject* GetStarOpenDoor() const { return m_StarOpenDoor; }
};
