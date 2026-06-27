#pragma once
#include "../../Framework/GameObject/SceneObject.h"
#include "../../Framework/Calculation/Vector.h"
#include <unordered_map>
#include <vector>
using namespace FrameWork;
class C2DPolygon;
class CStageSelectFrame;
enum class ESceneType;
struct StageInfo;

class CStageSelectUI : public CSceneObject
{
private:             
	std::unique_ptr<C2DPolygon> m_StageSelect;
	std::unique_ptr<C2DPolygon> m_StageSelectButton;
	std::unique_ptr<C2DPolygon> m_BG;
	std::unique_ptr<C2DPolygon> m_Slash;
	std::unique_ptr<C2DPolygon> m_Star;
	std::unique_ptr<C2DPolygon> m_Acorn;
	std::unique_ptr<C2DPolygon> m_Counter;
	std::unordered_map<ESceneType, StageInfo> m_StageInfos;
	std::unordered_map<ESceneType, std::unique_ptr<C2DPolygon>> m_StageImageList;//ステージに紐づく画像
	std::unordered_map<ESceneType, Vector2> m_StageImagePositionList;//ステージに紐づく画像の位置
	std::vector<CStageSelectFrame*> m_FrameList;
	int m_SelectNumber = 0;
	int m_MaxSelectNumber = 0;
	//ステージが選択されたか
	bool m_IsStageSelect = false;

	void StageSelectUi();
	void SelectButtonDraw();
	void SelectStageInfoDraw();
	void SelectStageStarDraw(const ESceneType Type);
	void SelectStageAcornDraw(const ESceneType Type);
	void SelectStageAcornMax(const ESceneType Type);
	//引数で渡したフレーム番号の選択フラグをtrueにする
	void SetSelectStageSelect(const ESceneType Type);

public:
	virtual void Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void Draw() override;
	bool IsStageSelect() const { return m_IsStageSelect; }
	ESceneType GetSelectSceneType() const;
};