#pragma once
//ImGuiクラス このクラスではデバッグメニュー関連の処理を行う
#include "../GameObject/SceneObject.h"
#include "../ImGui/DebugImGuiEditor.h"

enum class ESceneType;
class CMouseCursor;

class CImGui : public CSceneObject
{
private:
	CImGuiEditor m_Editor;
	bool isPerspective = false;
	bool m_IsImGuiDraw = false;
	CMouseCursor* m_MouseCursor = nullptr;

	void CommonWindow();
	void PlayerWindow();
	void ObjectWindow();
	void ShadowLightWindow();
	void FogWindow();
	void CollisionWindow();
	void ObjectListWindow();
	//今のシーンの三角形ポリゴンの数
	int GetSceneMeshTriangleNum() const;
	//今プレイヤーと当たり判定を行っている三角形ポリゴンの数
	int GetPlayerNowMeshTriangleNum() const;
	//今プレイヤー以外と当たり判定を行っている三角形ポリゴンの数
	int GetObjectNowMeshTriangleNum() const;

public:
	virtual void Init() override;
	virtual void UnInit() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	void ResizeWindow();
	
	void SetSaveFile(const ESceneType Type);
	//マウスカーソルがImGuIウィンドウにいたらtrue
	bool WantCaptureMouse() const;
};