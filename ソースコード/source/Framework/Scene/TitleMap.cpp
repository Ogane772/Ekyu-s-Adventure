#include "TitleMap.h"
#include "../Scene/SceneTypes.h"
#include "../Video/DemoPlayer.h"
#include "../Fade/Fade.h"
#include "../Renderer/Renderer.h"
#include "../Input/InputInterface.h"
#include "../Polygon/2DPolygon/2DPolygon.h"
#include "../../Framework/Model/StaticMesh.h"
#include "../../Framework/System/SystemContext.h"

namespace
{
	constexpr WCHAR TITLE_DEMO_PATH[] = { L"asset/title/demo.mp4" };
};

void CTitleMap::Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex)
{
	m_TitlePolygon = new C2DPolygon();
	m_TitleDemoPlayer = new CDemoPlayer();
	if (m_TitleDemoPlayer)
	{
		m_TitleDemoPlayer->Play((LPCTSTR)TITLE_DEMO_PATH);
	}
	if (CFade* fade = CFade::GetInstance())
	{
		fade->Start(false, 1.4f, 1.4f);
	}
}

void CTitleMap::UnInit()
{
	if (m_TitleDemoPlayer)
	{
		m_TitleDemoPlayer->Dispose();
		delete m_TitleDemoPlayer;
	}
	delete m_TitlePolygon;
	CScene::UnInit();
	CStaticMesh::AllLoadModelDelete();
}

ESceneType CTitleMap::Update(float DeltaTime)
{
	if (m_TitleDemoPlayer)
	{
		m_TitleDemoPlayer->UpdateAndPresent();
	}
	if (CInputInterface* instance = CInputInterface::GetInstance())
	{
		if (instance->IsDecisionTrigger())
		{
			return ESceneType::STAGE_SELECT;
		}
	}
	return ESceneType::TITLE;
}

void CTitleMap::Draw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system || !m_TitleDemoPlayer || !m_TitlePolygon)
	{
		return;
	}
	CRenderer::BillBoardBlendSet();
	//タイトルは動画を再生している都合上ここでフェードを描画しないと反映されないため描画
	if (CFade* fade = CFade::GetInstance())
	{
		fade->Draw();
	}
	std::weak_ptr<ID3D11ShaderResourceView> demoTexture = m_TitleDemoPlayer->GetSResView();
	CRenderer::SetTitleShader();
	CRenderer::SetWorldViewProjection2D();;
	m_TitlePolygon->Draw(demoTexture, Vector2(system->GetCurrentScreenWidth(), system->GetCurrentScreenHeight()));
	CRenderer::BillBoardBlendEnd();

	//毎フレーム動画からテクスチャを取得するため描画済みの画像はリリース
	m_TitleDemoPlayer->ReleaseSResView();
}
