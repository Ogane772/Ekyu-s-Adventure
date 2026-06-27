#include "ModelViewMap.h"
#include "../Shadow/ShadowMap.h"
#include "../Renderer/Renderer.h"
#include "../ModelView/ModelViewImGui.h"
#include "../ModelView/ViewModel.h"
#include "../Model/StaticMeshType.h"
#include "../Object/CommonObject.h"
#include "../Fade/Fade.h"
#include "../Camera/Camera.h"
#include "../Scene/SceneTypes.h"

void CModelViewMap::Init(const std::string& LoadSaveFile, const ESoundIndex PlayBGMIndex)
{
	if (CFade* fade = CFade::GetInstance())
	{
		fade->Start(false, 1.4f, 1.4f);
	}
	m_Camera = CreateAddGameObject<CCamera>(ESceneObjectType::CAMERA);
	if (!m_Camera)
	{
		return;
	}
	{
		CCommonObject *common = new CCommonObject();
		if (!common)
		{
			return;
		}
		common->Init(EStaticMeshType::TEST_STAGE, Vector3(0.0f, 0.0f, 0.0f), Vector3(20.0f, 20.0f, 20.0f), Vector3(0.0f, 0.0f, 0.0f));
		common->SetNoCullingCheck(true);
		AddGameObject(ESceneObjectType::MESH_OBJECT, common);
	}
	m_ViewModel = CreateAddGameObject<CViewModel>(ESceneObjectType::ENEMY);
	
	CreateAddGameObject<CModelViewImGui>(ESceneObjectType::NOMESH_OBJECT);
}

ESceneType CModelViewMap::Update(float DeltaTime)
{
	CScene::Update(DeltaTime);
	CShadowMap::Update();
	return ESceneType::MODEL_VIEW;
}

void CModelViewMap::Draw()
{
	CScene::ShadowDraw();
	CScene::Draw();
	CRenderer::SetShader();
}
