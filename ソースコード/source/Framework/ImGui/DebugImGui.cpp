#include "DebugImGui.h"
#include "ImGuiObjectIcon.h"
#include "../ImGui/Common/imgui.h"
#include "../ImGui/Common/imgui_impl_dx11.h"
#include "../ImGui/Common/imgui_impl_win32.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Renderer/Renderer.h"
#include "../Camera/Camera.h"
#include "../Input/InputInterface.h"
#include "../TextureLoader/TextureTypes.h"
#include "../Shadow/ShadowMap.h"
#include "../Light/DirectionalLight.h"
#include "../Mouse/MouseCursor.h"
#include "../Object/CommonObject.h"
#include "../Collision/CollisionTag.h"
#include "../System/SystemContext.h"
#include "../../Game/Character/Player/Player.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"
#include "../../Game/Character/Enemy/Enemy.h"

void CImGui::Init()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	m_MouseCursor = GetMouseCursor();
	//ImGuiセットアップ	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	
	ImGui_ImplWin32_Init(system->GetWindow());
	ImGui_ImplDX11_Init(CRenderer::GetDevice(), CRenderer::GetDeviceContext());
	ImGui::StyleColorsDark();

	m_Editor.Init();
}

void CImGui::UnInit()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (system)
	{
		system->SetIsDebugCollisionRender(false);
		system->SetEditMode(false);
	}
	m_Editor.UnInit();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CImGui::Update(float DeltaTime)
{
	//ImGui表示キー
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}

	if (instance->IsImGuiDrawTrigger())
	{
		m_IsImGuiDraw = !m_IsImGuiDraw;
	}
}

void CImGui::Draw()
{
	if (!m_IsImGuiDraw || !m_MouseCursor)
	{
		return;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ウィンドウ作成
	ImGui::Begin("DebugMenu", nullptr, ImGuiWindowFlags_MenuBar);
	//タブ表示
	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Common"))
		{
			CommonWindow();
			ObjectWindow();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Player"))
		{
			PlayerWindow();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("ShadowLightAndFog"))
		{
			ShadowLightWindow();
			FogWindow();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Collison"))
		{
			CollisionWindow();
			ImGui::EndTabItem();
		}				
		if (ImGui::BeginTabItem("ObjectList"))
		{
			ObjectListWindow();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	
	m_Editor.Draw();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui::ResizeWindow()
{
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();
}

void CImGui::CommonWindow()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system || !m_MouseCursor)
	{
		return;
	}
	CPlayer* player = GetPlayer();
	std::weak_ptr<CScene> scene = GetScene();
	if (!player || scene.expired())
	{
		return;
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Spacing();
	if (ImGui::Checkbox("IsEditMode", system->IsEditModeAddr()))
	{
		if (!system->IsEditMode())
		{
			m_Editor.EditOff();
			CPlayerStart* start = scene.lock()->GetGameObject<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT);
			if (start)
			{
				if (!*player->IsDebugNoStart())
				{
					const Vector3 startPosition = start->GetPosition();
					player->SetPosition(startPosition);
					player->SetDefaultPosition(startPosition);
					player->SetRotationYaw(start->GetRotation().y);
				}
			}
			if (CCamera* camera = GetCamera())
			{
				camera->ResetPlayerCamera();
			}
		}
		
		m_MouseCursor->SetCreateNumber(-1);
	}

	ImGui::Spacing();
	if (ImGui::Checkbox("DebugCollision", system->IsDebugCollisionRenderAddr()))
	{
		if (system->IsDebugCollisionRender())
		{
			bool* single = m_MouseCursor->GetSingleCollisonDraw();
			*single = false;
		}
	}
	ImGui::Spacing();
	if (ImGui::Checkbox("DebugSingleCollision", m_MouseCursor->GetSingleCollisonDraw()))
	{
		if (*m_MouseCursor->GetSingleCollisonDraw())
		{
			system->SetIsDebugCollisionRender(false);
		}
	}
	ImGui::Spacing();
	static bool disableShadow = false;
	if (ImGui::Checkbox("DebugDisableShadow", &disableShadow))
	{
		std::vector<CCommonObject*> commons;
		scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, commons);
		for (CCommonObject* common : commons)
		{
			common->SetEditorDisableShadow(disableShadow);
		}
		std::vector<CEnemy*> enemys;
		scene.lock()->GetGameEnemyObjects(ESceneObjectType::ENEMY, enemys);
		for (CEnemy* enemy : enemys)
		{
			enemy->SetEditorDisableShadow(disableShadow);
		}
		player->SetEditorDisableShadow(disableShadow);
	}
	
	ImGui::Checkbox("DisableAnimationModel", system->IsDebugDisableAnimationModelRenderAddr());
	
	ImGui::Spacing();
	ImGui::Checkbox("DebugNoItemHit", player->IsDebugNoItemHit());
	ImGui::Spacing();
	ImGui::Checkbox("DebugNoStart", player->IsDebugNoStart());
	ImGui::Spacing();

	const Vector3 playerPosition = player->GetPosition();
	ImGui::Text("PlayerPosition:(X:%.3f,Y:%.3f,Z:%.3f )", playerPosition.x, playerPosition.y, playerPosition.z);
	if (CCamera* camera = GetCamera())
	{
		const Vector3 cameraPosition = camera->GetPosition();
		ImGui::Text("CameraPosition:(X:%.3f,Y:%.3f,Z:%.3f )", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	}
	const Vector3 mouseCursorPosition = m_MouseCursor->GetPosition();
	ImGui::Text("MouseCursorPosition:(X:%.3f,Y:%.3f,Z:%.3f )", mouseCursorPosition.x, mouseCursorPosition.y, mouseCursorPosition.z);
	ImGui::Spacing();
}

void CImGui::PlayerWindow()
{
	CPlayer* player = GetPlayer();
	if (!player)
	{
		return;
	}
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	const Vector3 playerPosition = player->GetPosition();
	const Vector3 playerRotation = player->GetRotation();
	float positionX = playerPosition.x;
	float positionY = playerPosition.y;
	float positionZ = playerPosition.z;
	float rotationX = playerRotation.x;
	float rotationY = playerRotation.y;
	float rotationZ = playerRotation.z;

	ImGui::Checkbox("DebugNoDamage", player->IsDebugNoDamage());
	ImGui::Spacing();
	if (ImGui::TreeNode("Player")) 
	{
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		//タブを表示　引数1タブの内部名後の引数タブ名+変数
		if (ImGui::TreeNode("Player Position", "Position:(X:%.3f,Y:%.3f,Z:%.3f )", playerPosition.x, playerPosition.y, playerPosition.z))
		{
			ImGui::SliderFloat("X", &positionX, -100.0f, 100.0f);
			ImGui::SliderFloat("Y", &positionY, -100.0f, 100.0f);
			ImGui::SliderFloat("Z", &positionZ, -100.0f, 100.0f);
			player->SetPosition(Vector3(positionX, positionY, positionZ));
			if (ImGui::Button("Position_Reset")) 
			{
				player->ResetDefaultPosition();
			}
			ImGui::TreePop();
		}
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

		if (ImGui::TreeNode("Player Rotation", "Rotation:(X:%.3f,Y:%.3f,Z:%.3f )", playerRotation.x, playerRotation.y, playerRotation.z))
		{
			ImGui::SliderFloat("RX", &rotationX, 0.0f, 360.0f);
			ImGui::SliderFloat("RY", &rotationY, 0.0f, 360.0f);
			ImGui::SliderFloat("RZ", &rotationZ, 0.0f, 360.0f);
			player->SetRotation(Vector3(rotationX, rotationY, rotationZ));
			if (ImGui::Button("Rotation_Reset")) 
			{
				player->SetRotation(Vector3::Zero());
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void CImGui::ObjectWindow()
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("GameObjectNum"))
	{
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
		const int max = static_cast<int>(ESceneObjectType::MAX);
		for (int i = 0; i < max; ++i)
		{
			ImGui::Text("GameObject[%d]:%d", i, scene.lock()->GetGameObjectCount(static_cast<ESceneObjectType>(i)));
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
}

void CImGui::ShadowLightWindow()
{
	const Vector3 lightRotation = CShadowMap::GetLightRotation();
	float rotationX = lightRotation.x;
	float rotationY = lightRotation.y;
	float rotationZ = lightRotation.z;
	ShaderShadowLightProjection projection;
	projection.View_Width = CShadowMap::GetLightProjection().View_Width;
	projection.View_Height = CShadowMap::GetLightProjection().View_Height;
	projection.FarZ = CShadowMap::GetLightProjection().FarZ;
	if (ImGui::TreeNode("Light"))
	{
		if (ImGui::TreeNode("Light Projection", "Projection:(Width%.2f,Height%.2f,Far%.2f)", projection.View_Width, projection.View_Height, projection.FarZ))
		{
			ImGui::SliderFloat("Width", &projection.View_Width, 1.0f, 700.0f);
			ImGui::SliderFloat("Height", &projection.View_Height, 1.0f, 700.0f);
			ImGui::SliderFloat("Far", &projection.FarZ, 1.0f, 700.0f);
			ImGui::InputFloat("ShadowView", CShadowMap::GetShadowView());
			CShadowMap::SetLightProjection(projection);
			if (ImGui::Button("projection_Reset"))
			{
				CShadowMap::LightProjectionReset();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Light Rotation", "Rotation:(X:%.3f,Y:%.3f,Z:%.3f )", rotationX, rotationY, rotationZ))
		{
			ImGui::SliderFloat("RX", &rotationX, -360.0f, 360.0f);
			ImGui::SliderFloat("RY", &rotationY,  -360.0f, 360.0f);
			ImGui::SliderFloat("RZ", &rotationZ,  -360.0f, 360.0f);
			CShadowMap::SetLightRotation(Vector3(rotationX, rotationY, rotationZ));
			if (ImGui::Button("Rotation_Reset")) 
			{
				CShadowMap::LightRotationReset();
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	CDirectionalLight* directionalLight = directionalLight->GetInstance();
	if(!directionalLight)
	{
		return;
	}

	LIGHT light = directionalLight->GetLight();
	ImGui::Text("DirectionalLight");
	ImGui::Spacing();
	if (ImGui::TreeNode("Direction", "Direction:(X:%.3f,Y:%.3f,Z:%.3f )", light.Direction.x, light.Direction.y, light.Direction.z))
	{
		ImGui::SliderFloat("DirX", &light.Direction.x, -300.0f, 300.0f);
		ImGui::SliderFloat("DirY", &light.Direction.y, -300.0f, 300.0f);
		ImGui::SliderFloat("DirZ", &light.Direction.z, -300.0f, 300.0f);
		directionalLight->SetDirection(light.Direction);
		if (ImGui::Button("Direction_Reset"))
		{
			light.Direction = directionalLight->GetDefaultLight().Direction;
			directionalLight->ResetDirection();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Diffuse", "Diffuse:(R:%.3f,G:%.3f,B:%.3f )", light.Diffuse.r, light.Diffuse.g, light.Diffuse.b))
	{
		ImGui::SliderFloat("DifR", &light.Diffuse.r, 0.0f, 100.0f);
		ImGui::SliderFloat("DifG", &light.Diffuse.g, 0.0f, 100.0f);
		ImGui::SliderFloat("DifB", &light.Diffuse.b, 0.0f, 100.0f);
		directionalLight->SetDiffuse(light.Diffuse);
		if (ImGui::Button("Diffuse_Reset"))
		{
			light.Diffuse = directionalLight->GetDefaultLight().Diffuse;
			directionalLight->ResetDiffuse();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Ambient", "Ambient:(R:%.3f,G:%.3f,B:%.3f )", light.Ambient.r, light.Ambient.g, light.Ambient.b))
	{
		ImGui::SliderFloat("AmbR", &light.Ambient.r, 0.0f, 100.0f);
		ImGui::SliderFloat("AmbG", &light.Ambient.g, 0.0f, 100.0f);
		ImGui::SliderFloat("AmbB", &light.Ambient.b, 0.0f, 100.0f);
		directionalLight->SetAmbient(light.Ambient);
		if (ImGui::Button("Ambient_Reset")) 
		{
			light.Ambient = directionalLight->GetDefaultLight().Ambient;
			directionalLight->ResetAmbient();
		}
		ImGui::TreePop();
	}
}

void CImGui::FogWindow()
{
	static bool isFog = true;
	FOG_CONSTANT& fog = CRenderer::GetFogConstant();
	if (fog.FogParam.x == 1.0f)
	{
		isFog = true;
	}
	else
	{
		isFog = false;
	}
	ImGui::Text("Fog");
	ImGui::Spacing();
	if (ImGui::TreeNode("FogParam", "Fog:(ON:%d,Near:%.3f,Far:%.3f )", isFog, fog.FogParam.y, fog.FogParam.z))
	{
		if (ImGui::Checkbox("IsFog", &isFog))
		{
			if (isFog)
			{
				fog.FogParam.x = 1.0f;
			}
			else
			{
				fog.FogParam.x = 0.0f;
			}
		}
		ImGui::SliderFloat("Near", &fog.FogParam.y, 0.0f, 100.0f);
		ImGui::SliderFloat("Far", &fog.FogParam.z, 0.0f, 300.0f);
		if (ImGui::Button("Fog_Reset"))
		{
			isFog = true;
			CRenderer::ResetFogConstant();
		}
		ImGui::TreePop();
	}
}

void CImGui::CollisionWindow()
{
	ImGui::Text("SceneMeshTriangleNum:%d", GetSceneMeshTriangleNum());
	ImGui::Spacing();
	ImGui::Text("PlayerMeshTriangleNum:%d", GetPlayerNowMeshTriangleNum());
	ImGui::Spacing();
	ImGui::Text("ObjectMeshTriangleNum:%d", GetObjectNowMeshTriangleNum());
	ImGui::Spacing();
}

void CImGui::ObjectListWindow()
{
	if (!m_MouseCursor)
	{
		return;
	}
	const std::vector<MapObject>& mapObject = m_MouseCursor->GetMapObjectList();
	if (mapObject.size() == 0)
	{
		ImGui::Text("NoObject");
		return;
	}

	int number = 0;
	ImGui::ListBoxHeader("MapList", static_cast<int>(mapObject.size()));
	for (const MapObject& item : mapObject)
	{
		if (!item.Object)
		{
			continue;
		}

		char num[99];
		sprintf_s(num, "%s:%d", item.Name.c_str(), item.ObjectNumber);
		const bool isSelectObject = m_MouseCursor->IsSelectObject();
		const int createNumber = m_MouseCursor->GetCreateNumber();
		const int originalNumber = item.Object->GetOriginalNumber();
		const bool isChangeColor = isSelectObject && createNumber == originalNumber;
		if (isChangeColor)
		{//選んでいるオブジェクトを黄色文字にする
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
		}	
		if (ImGui::Selectable(num))
		{
			if (createNumber != originalNumber)
			{
				m_MouseCursor->SetMouseObject(item.Object, item.SelectObjectType, item.MeshType);
			}
			else
			{
				ImGui::PopStyleColor(2);
			}
			break;
		}
		if (isChangeColor)
		{
			ImGui::PopStyleColor(2);
		}
		number++;
	}
	ImGui::ListBoxFooter();
}

void CImGui::SetSaveFile(const ESceneType Type)
{
	m_Editor.SetSaveFile(Type);
}

bool CImGui::WantCaptureMouse() const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse;
}

int CImGui::GetSceneMeshTriangleNum() const
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return 0;
	}
	std::vector<CCommonObject*> models;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, models);
	std::vector<CollisionMesh>* collision_mesh;

	int size = 0;
	for (CCommonObject* model : models)
	{
		collision_mesh = model->GetNormalModelMeshCollision();

		size += (int)collision_mesh->size();
	}
	return size;
}

int CImGui::GetPlayerNowMeshTriangleNum() const
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return 0;
	}
	std::vector<CCommonObject*> models;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, models);
	std::vector<CollisionMesh>* collision_mesh;

	int size = 0;
	for (CCommonObject* model : models)
	{
		if (!model->IsCulling() || model->GetPlayerCollisionTag() == ECollisionTagName::NO_LINE_HIT || model->GetPlayerCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
		{
			continue;
		}

		collision_mesh = model->GetNormalModelMeshCollision();

		size += static_cast<int>(collision_mesh->size());
	}
	return size;
}

int CImGui::GetObjectNowMeshTriangleNum() const
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return 0;
	}
	std::vector<CCommonObject*> models;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, models);
	std::vector<CollisionMesh>* collision_mesh;

	int size = 0;
	for (CCommonObject* model : models)
	{
		if (model->GetObjectCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
		{
			continue;
		}
		if (model->GetObjectCollisionTag() != ECollisionTagName::OBJECT_COLISION)
		{
			continue;
		}
		collision_mesh = model->GetNormalModelMeshCollision();

		size += (int)collision_mesh->size();
	}
	return size;
}
