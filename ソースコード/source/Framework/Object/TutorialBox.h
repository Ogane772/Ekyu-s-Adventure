#pragma once
#include "../GameObject/GameObject.h"
#include <memory>
//プレイヤーが触れたとき、チュートリアル番号に応じたチュートリアルを表示するボックス
class CBox;
class CTutorialBox : public CGameObject
{
private:
	std::unique_ptr<CBox> m_Box;
	int m_TutorialNumber = 0;

public:
	CTutorialBox(const Vector3& SetPos, const Vector3& Max, const Vector3& Min);
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	virtual void SingleCollisonDraw();
	virtual CBox* GetCollisionBox() const override;
	virtual void SetPosition(const Vector3& SetPos)override;
	int* GetTutorialNumber() { return &m_TutorialNumber; }
	void SetTutorialNumber(const int Number){ m_TutorialNumber = Number; }
};
