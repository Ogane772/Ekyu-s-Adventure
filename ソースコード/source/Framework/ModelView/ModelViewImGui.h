#pragma once
//モデルビューモードのImGuiクラス　モデルの位置、大きさやアニメーションの設定を行える
#include "../GameObject/SceneObject.h"

class CModelViewImGui : public CSceneObject
{
private:
	bool m_IsImGuiDraw = true;

	void ModelWindowDraw();
	void LightWindowDraw();
	void ShadowLightWindow();
public:
	virtual void Init() override;
	virtual void UnInit() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
};