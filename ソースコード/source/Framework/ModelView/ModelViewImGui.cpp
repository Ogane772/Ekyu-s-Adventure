#include "ModelViewImGui.h"
#include "../Renderer/Renderer.h"
#include "../Input/InputInterface.h"
#include "../Camera/Camera.h"
#include "../ImGui/Common/imgui_impl_dx11.h"
#include "../ImGui/Common/imgui_impl_win32.h"
#include "../ImGui/Common/imgui.h"
#include "../Light/DirectionalLight.h"
#include "../Shadow/ShadowMap.h"
#include "../ModelView/ViewModel.h"
#include "../Scene/ModelViewMap.h"
#include "../System/SystemContext.h"

void CModelViewImGui::Init()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(system->GetWindow());
	ImGui_ImplDX11_Init(CRenderer::GetDevice(), CRenderer::GetDeviceContext());
	ImGui::StyleColorsDark();
}

void CModelViewImGui::UnInit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CModelViewImGui::Update(float DeltaTime)
{
	//ImGui表示キー
	CInputInterface* instance = CInputInterface::GetInstance();
	if(!instance)
	{
		return;
	}
	if (instance->IsImGuiDrawTrigger())
	{
		m_IsImGuiDraw = !m_IsImGuiDraw;
	}
}

void CModelViewImGui::Draw()
{
	if (!m_IsImGuiDraw)
	{
		return;
	}
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("ModelView", nullptr, ImGuiWindowFlags_MenuBar);
	//タブ表示
	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Model"))
		{
			ModelWindowDraw();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Light"))
		{
			LightWindowDraw();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("ShadowLight"))
		{
			ShadowLightWindow();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CModelViewImGui::ModelWindowDraw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}

	CModelViewMap* modelViewScene = dynamic_cast<CModelViewMap*>(scene.lock().get());
	if (!modelViewScene)
	{
		return;
	}
	CGameObject* camera = modelViewScene->GetCamera();
	CViewModel* viewModel = modelViewScene->GetViewModel();
	if (!camera || !viewModel)
	{
		return;
	}
	const std::string& filename = viewModel->GetFileName();
	//テストウィンドウ作成
	ImGui::Text("FPSaverage %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Spacing();
	std::string fix = filename.substr(filename.find_last_of('\\') + 1);
	ImGui::Text("FileName:%s", fix.c_str());
	ImGui::Spacing();
	ImGui::LabelText("", "Camera Pos:(X%.2f,Y%.2f,Z%.2f)", camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
	ImGui::Checkbox("DebugCollision", system->IsDebugCollisionRenderAddr());
	ImGui::Spacing();
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	const Vector3 modelPosition = viewModel->GetPosition();
	const Vector3 modelScale = viewModel->GetScale();
	const Vector3 modelRotation = viewModel->GetRotation();

	float sliderModelPositionX = modelPosition.x;
	float sliderModelPositionY = modelPosition.y;
	float sliderModelPositionZ = modelPosition.z;
	float sliderModelScaleX = modelScale.x;
	float sliderModelRotationX = modelRotation.x;
	float sliderModelRotationY = modelRotation.y;
	float sliderModelRotationZ = modelRotation.z;

	if (viewModel->IsSkeletalMeshDraw())
	{
		if (ImGui::TreeNode("Animation"))
		{
			// アニメーション名以外の邪魔な文字列を切り取る
			std::string tmpName = viewModel->GetAiAnimation()->mName.C_Str();
			const std::string fixName = tmpName.substr(tmpName.find_last_of('|') + 1);
			float animeSpeed = viewModel->GetAnimationSpeed();
			ImGui::Text("NowAnime:%d", viewModel->GetAnimationNumber());
			ImGui::Text("AnimeName:%s", fixName.c_str());
			ImGui::Spacing();
			ImGui::Spacing();

			//アニメーション番号設定
			if (ImGui::Button("AddAnimeNumber")) 
			{
				const int addNumber = viewModel->GetAnimationNumber() + 1;
				viewModel->SetAnimationNumber(addNumber);
			}
			ImGui::SameLine();
			if (ImGui::Button("MinusAnimeNumber")) 
			{
				const int minusNumber = viewModel->GetAnimationNumber() - 1;
				viewModel->SetAnimationNumber(minusNumber);
			}

			//アニメーション速度設定
			ImGui::Text("AnimeSpeed:");
			ImGui::SliderFloat("", &animeSpeed, 0.0f, 3.0f);
			viewModel->SetAnimeSpeed(animeSpeed);
			ImGui::Spacing();

			//アニメーションの経過フレーム表示
			ImGui::Text("AnimeFrame:%f", viewModel->GetAnimeFrame());
			ImGui::Spacing();

			//アニメーションループ設定
			static bool isLoop = false;
			if (ImGui::Checkbox("IsLoop", &isLoop))
			{
				viewModel->ResetAnimeFrame();
			}
			viewModel->SetAnimeLoop(isLoop);
			ImGui::Text("AnimeList:");

			ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
			for (unsigned int i = 0; i < viewModel->GetAiScene()->mNumAnimations; i++)
			{//fbx内のアニメーション番号とアニメーション名を表示
				std::string tmp = viewModel->GetAiScene()->mAnimations[i]->mName.C_Str();
				std::string animationName = tmp.substr(tmp.find_last_of('|') + 1);
				ImGui::Text("Anime[%d]:%s", i, animationName.c_str());
			}
			ImGui::EndChild();
			ImGui::TreePop();
		}
	}
	if (ImGui::TreeNode("Model")) 
	{

		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		//タブを表示　引数1タブの内部名後の引数タブ名+変数
		if (ImGui::TreeNode("Model Position", "Position:(X:%.3f,Y:%.3f,Z:%.3f )", modelPosition.x, modelPosition.y, modelPosition.z))
		{
			ImGui::SliderFloat("X", &sliderModelPositionX, -100.0f, 100.0f);
			ImGui::SliderFloat("Y", &sliderModelPositionY, -100.0f, 100.0f);
			ImGui::SliderFloat("Z", &sliderModelPositionZ, -100.0f, 100.0f);
			viewModel->SetPosition(Vector3(sliderModelPositionX, sliderModelPositionY, sliderModelPositionZ));
			if (ImGui::Button("Position_Reset"))
			{
				sliderModelPositionX = 0.0f;
				sliderModelPositionY = 1.0f;
				sliderModelPositionZ = 0.0f;
				viewModel->SetPosition(Vector3(sliderModelPositionX, sliderModelPositionY, sliderModelPositionZ));
			}
			ImGui::TreePop();
		}
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Model Rotation", "Rotation:(X:%.3f,Y:%.3f,Z:%.3f )", modelRotation.x, modelRotation.y, modelRotation.z))
		{
			ImGui::SliderFloat("RX", &sliderModelRotationX, 0.0f, 360.0f);
			ImGui::SliderFloat("RY", &sliderModelRotationY, 0.0f, 360.0f);
			ImGui::SliderFloat("RZ", &sliderModelRotationZ, 0.0f, 360.0f);
			viewModel->SetRotation(Vector3(sliderModelRotationX, sliderModelRotationY, sliderModelRotationZ));
			if (ImGui::Button("Rotation_Reset"))
			{
				sliderModelRotationX = 0.0f;
				sliderModelRotationY = 0.0f;
				sliderModelRotationZ = 0.0f;
				viewModel->SetRotation(Vector3(sliderModelRotationX, sliderModelRotationY, sliderModelRotationZ));
			}
			ImGui::TreePop();
		}
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Model Scale", "Scale:(X:%.3f,Y:%.3f,Z:%.3f )", modelScale.x, modelScale.y, modelScale.z))
		{
			ImGui::SliderFloat("Scale", &sliderModelScaleX, 0.1f, 100.0f);
			viewModel->SetScale(Vector3(sliderModelScaleX, sliderModelScaleX, sliderModelScaleX));
			if (ImGui::Button("Scale_Reset")) 
			{
				sliderModelScaleX = 10.0f;
				viewModel->SetScale(Vector3(sliderModelScaleX, sliderModelScaleX, sliderModelScaleX));
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void CModelViewImGui::LightWindowDraw()
{
	CDirectionalLight* directionalLight = directionalLight->GetInstance();
	if (!directionalLight)
	{
		return;
	}

	LIGHT light = directionalLight->GetLight();
	ImGui::Text("DirectionalLight");
	ImGui::Spacing();
	if (ImGui::TreeNode("Direction", "Direction:(X:%.3f,Y:%.3f,Z:%.3f )", light.Direction.x, light.Direction.y, light.Direction.z))
	{
		ImGui::SliderFloat("DirX", &light.Direction.x, -100.0f, 100.0f);
		ImGui::SliderFloat("DirY", &light.Direction.y, -100.0f, 100.0f);
		ImGui::SliderFloat("DirZ", &light.Direction.z, -100.0f, 100.0f);
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
		ImGui::SliderFloat("DifR", &light.Diffuse.r, 0.0f, 1.0f);
		ImGui::SliderFloat("DifG", &light.Diffuse.g, 0.0f, 1.0f);
		ImGui::SliderFloat("DifB", &light.Diffuse.b, 0.0f, 1.0f);
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
		ImGui::SliderFloat("AmbR", &light.Ambient.r, 0.0f, 1.0f);
		ImGui::SliderFloat("AmbG", &light.Ambient.g, 0.0f, 1.0f);
		ImGui::SliderFloat("AmbB", &light.Ambient.b, 0.0f, 1.0f);
		directionalLight->SetAmbient(light.Ambient);
		if (ImGui::Button("Ambient_Reset")) 
		{
			light.Ambient = directionalLight->GetDefaultLight().Ambient;
			directionalLight->ResetAmbient();
		}
		ImGui::TreePop();
	}
}

void CModelViewImGui::ShadowLightWindow()
{
	const Vector3 rotation = CShadowMap::GetLightRotation();
	float rotationX = rotation.x;
	float rotationY = rotation.y;
	float rotationZ = rotation.z;
	ShaderShadowLightProjection projection = CShadowMap::GetLightProjection();
	projection.View_Width = projection.View_Width;
	projection.View_Height = projection.View_Height;
	projection.FarZ = projection.FarZ;
	if (ImGui::TreeNode("Light"))
	{
		if (ImGui::TreeNode("Light Proj", "Proj:(Width%.2f,Height%.2f,Far%.2f)", projection.View_Width, projection.View_Height, projection.FarZ))
		{
			ImGui::SliderFloat("Width", &projection.View_Width, 1.0f, 300.0f);
			ImGui::SliderFloat("Height", &projection.View_Height, 1.0f, 300.0f);
			ImGui::SliderFloat("Far", &projection.FarZ, 1.0f, 300.0f);
			CShadowMap::SetLightProjection(projection);
			if (ImGui::Button("Projection_Reset")) 
			{
				CShadowMap::LightProjectionReset();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Light Rotation", "Rotation:(X:%.3f,Y:%.3f,Z:%.3f )", rotation.x, rotation.y, rotation.z))
		{
			ImGui::SliderFloat("RX", &rotationX, 0.0f, 360.0f);
			ImGui::SliderFloat("RY", &rotationY, 0.0f, 360.0f);
			ImGui::SliderFloat("RZ", &rotationZ, 0.0f, 360.0f);
			CShadowMap::SetLightRotation(Vector3(rotationX, rotationY, rotationZ));
			if (ImGui::Button("Rotation_Reset")) 
			{
				CShadowMap::LightRotationReset();
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}