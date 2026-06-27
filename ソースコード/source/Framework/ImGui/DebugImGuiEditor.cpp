#include "DebugImGuiEditor.h"
#include "ImGuiObjectIcon.h"
#include "../ImGui/Common/ImGuizmo.h"
#include "../Model/StaticMeshType.h"
#include "../TextureLoader/TextureLoader.h"
#include "../Camera/Camera.h"
#include "../Input/InputInterface.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Scene/SceneTypes.h"
#include "../System/SystemContext.h"
#include "../Mouse/MouseCursor.h"
#include "../SaveAndLoad/SaveAndLoad.h"
#include "../Collision/Box.h"
#include "../Collision/Sphere.h"
#include "../Collision/CollisionTag.h"
#include "../../Game/Object/ItemObject/Acorn.h"
#include "../../Game/Object/ItemObject/Heart.h"
#include "../../Game/Object/ItemObject/Star.h"
#include "../../Game/Character/Player/Player.h"
#include "../../Game/Character/Enemy/Enemy.h"
#include "../../Game/Object/Door/StarDoor.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"

namespace
{
	const ImVec2 IMAGE_BUTTON_SIZE = (ImVec2(40, 40));
	constexpr int AUTOSAVE_FRAME = 3600;
}

void CImGuiEditor::Init()
{
	m_MouseCursor = GetMouseCursor();
	m_Camera = GetCamera();

	//IMGUIセットアップ
	ImagePush(ETextureType::IMGUI_ICON_ACORN);
	ImagePush(ETextureType::UI_HEART);
	ImagePush(ETextureType::UI_STAR);
	ImagePush(ETextureType::IMGUI_ICON_THORNBOX);
	ImagePush(ETextureType::IMGUI_ICON_TILEBLOCK);
	ImagePush(ETextureType::IMGUI_ICON_WOODBRIDGE);
	ImagePush(ETextureType::IMGUI_ICON_STONETREE);
	ImagePush(ETextureType::IMGUI_ICON_STONEBLOCK);
	ImagePush(ETextureType::IMGUI_ICON_STUMP);
	ImagePush(ETextureType::IMGUI_ICON_GRASSWOOD);
	ImagePush(ETextureType::IMGUI_ICON_GRASSSTONE);
	ImagePush(ETextureType::IMGUI_ICON_GRASSPILLAR);
	ImagePush(ETextureType::IMGUI_ICON_NO_CLIFFBOX);
	ImagePush(ETextureType::IMGUI_ICON_SLIME);
	ImagePush(ETextureType::IMGUI_ICON_MONKEY);
	ImagePush(ETextureType::IMGUI_ICON_THORNMAN);
	ImagePush(ETextureType::IMGUI_ICON_TRIANGLE);
	ImagePush(ETextureType::IMGUI_ICON_PLANE_YELLOW);
	ImagePush(ETextureType::IMGUI_ICON_PLANE_STONE);
	ImagePush(ETextureType::IMGUI_ICON_PLANE_GREEN);
	ImagePush(ETextureType::IMGUI_ICON_PLANE_GRASS);
	ImagePush(ETextureType::IMGUI_ICON_PLANE_GRASS2);
	ImagePush(ETextureType::IMGUI_ICON_WALL_GRASS);
	ImagePush(ETextureType::IMGUI_ICON_WALL_WOOD);
	ImagePush(ETextureType::IMGUI_ICON_STAR_DOOR);
	ImagePush(ETextureType::IMGUI_ICON_TRIANGLE2);
	ImagePush(ETextureType::IMGUI_ICON_SIMPLE_GRASSBOX);
	ImagePush(ETextureType::IMGUI_ICON_GRASS_CIRCLE);
	ImagePush(ETextureType::IMGUI_ICON_GRASS_BOX_YELLOW);
	ImagePush(ETextureType::IMGUI_ICON_START_TABLE);
	ImagePush(ETextureType::IMGUI_ICON_SKY_DOME);
	ImagePush(ETextureType::IMGUI_ICON_PLAYER_START);
	ImagePush(ETextureType::IMGUI_ICON_WALL_GRASS);
	ImagePush(ETextureType::IMGUI_ICON_WALL_GRASS2);
	ImagePush(ETextureType::IMGUI_ICON_PLANE_YELLOW2);
	ImagePush(ETextureType::IMGUI_ICON_BLOCK_COLLISION);
	ImagePush(ETextureType::IMGUI_ICON_BUSH);
	ImagePush(ETextureType::IMGUI_ICON_BUSH2);
	ImagePush(ETextureType::IMGUI_ICON_FORN);
	ImagePush(ETextureType::IMGUI_ICON_BUSH3);
	ImagePush(ETextureType::IMGUI_ICON_BUSH4);
	ImagePush(ETextureType::IMGUI_ICON_BUSH5);
	ImagePush(ETextureType::IMGUI_ICON_PLANT);
	ImagePush(ETextureType::IMGUI_ICON_GRASSES);
	ImagePush(ETextureType::IMGUI_ICON_FLOWER);
	ImagePush(ETextureType::IMGUI_ICON_FLOWER2);
	ImagePush(ETextureType::IMGUI_ICON_FLOWER3);
	ImagePush(ETextureType::IMGUI_ICON_ROCK);
	ImagePush(ETextureType::IMGUI_ICON_ROCK2);
	ImagePush(ETextureType::IMGUI_ICON_ROCK3);
	ImagePush(ETextureType::IMGUI_ICON_ROCK4);
	ImagePush(ETextureType::IMGUI_ICON_ROCK5);
	ImagePush(ETextureType::IMGUI_ICON_TREE);
	ImagePush(ETextureType::IMGUI_ICON_FIRSTBOSS_FIELD);
	ImagePush(ETextureType::IMGUI_ICON_FIRSTBOSS_WALL);
	ImagePush(ETextureType::IMGUI_ICON_SPHERE);
	ImagePush(ETextureType::IMGUI_ICON_TEST_BOX);
	ImagePush(ETextureType::IMGUI_ICON_WINDOWSOUTWARDS);
	ImagePush(ETextureType::IMGUI_ICON_WINDOWSOUTWARDSFRAME);
	ImagePush(ETextureType::IMGUI_ICON_WINDOWSSINGLEHIGH);
	ImagePush(ETextureType::IMGUI_ICON_WINDOWSDOUBLEHIGH);
	ImagePush(ETextureType::IMGUI_ICON_WINDOWSROUND);
	ImagePush(ETextureType::IMGUI_ICON_WINDOWSSINGLEHIGHBOARDS);
	ImagePush(ETextureType::IMGUI_ICON_DOCK);
	ImagePush(ETextureType::IMGUI_ICON_DOOR2STEP);
	ImagePush(ETextureType::IMGUI_ICON_DOOR3);
	ImagePush(ETextureType::IMGUI_ICON_DOOR4);
	ImagePush(ETextureType::IMGUI_ICON_DOOR1);
	ImagePush(ETextureType::IMGUI_ICON_HOUSE1);
	ImagePush(ETextureType::IMGUI_ICON_HOUSE2);
	ImagePush(ETextureType::IMGUI_ICON_HOUSE3);
	ImagePush(ETextureType::IMGUI_ICON_HOUSE4);
	ImagePush(ETextureType::IMGUI_ICON_ONEHOUSE1);
	ImagePush(ETextureType::IMGUI_ICON_ONEHOUSE2);
	ImagePush(ETextureType::IMGUI_ICON_ONEHOUSE3);
	ImagePush(ETextureType::IMGUI_ICON_ONEHOUSE4);
	ImagePush(ETextureType::IMGUI_ICON_BALCONY);
	ImagePush(ETextureType::IMGUI_ICON_BALCONY2);
	ImagePush(ETextureType::IMGUI_ICON_CHIMNEY);
	ImagePush(ETextureType::IMGUI_ICON_CHIMNEY2);
	ImagePush(ETextureType::IMGUI_ICON_ROOFFRAME);
	ImagePush(ETextureType::IMGUI_ICON_ROOFFRAME2);
	ImagePush(ETextureType::IMGUI_ICON_ROOFFRAME3);
	ImagePush(ETextureType::IMGUI_ICON_ROOFFRAME4);
	ImagePush(ETextureType::IMGUI_ICON_STAIRWIDE);
	ImagePush(ETextureType::IMGUI_ICON_STAIRWIDE2);
	ImagePush(ETextureType::IMGUI_ICON_ROOFFRAME5);
	ImagePush(ETextureType::IMGUI_ICON_ROOFFRAME6);
	ImagePush(ETextureType::IMGUI_ICON_VARIANT);
	ImagePush(ETextureType::IMGUI_ICON_COLUMN);

	ImagePush(ETextureType::IMGUI_ICON_ARCBIG);
	ImagePush(ETextureType::IMGUI_ICON_ARCCOMER);
	ImagePush(ETextureType::IMGUI_ICON_ARCCROSS);
	ImagePush(ETextureType::IMGUI_ICON_ARCTRIPLE);
	ImagePush(ETextureType::IMGUI_ICON_BARREL);
	ImagePush(ETextureType::IMGUI_ICON_COFFIN);
	ImagePush(ETextureType::IMGUI_ICON_COFFINCOVER);
	ImagePush(ETextureType::IMGUI_ICON_PEREKR);
	ImagePush(ETextureType::IMGUI_ICON_WOODBOX);

	ImagePush(ETextureType::IMGUI_ICON_CLAIMEDWALL2C);
	ImagePush(ETextureType::IMGUI_ICON_CLAIMEDE);
	ImagePush(ETextureType::IMGUI_ICON_DIRTDARKE);
	ImagePush(ETextureType::IMGUI_ICON_FLOOR);
	ImagePush(ETextureType::IMGUI_ICON_STONE);
	ImagePush(ETextureType::IMGUI_ICON_TOP);
	ImagePush(ETextureType::IMGUI_ICON_CLAIMEDE);
	ImagePush(ETextureType::IMGUI_ICON_CLAIMEDWALL2C);
	ImagePush(ETextureType::IMGUI_ICON_DIRTDARKE);
	ImagePush(ETextureType::IMGUI_ICON_FLOOR);
	ImagePush(ETextureType::IMGUI_ICON_STONE);
	ImagePush(ETextureType::IMGUI_ICON_WALL2_1);
	ImagePush(ETextureType::IMGUI_ICON_WALL2_1);
	ImagePush(ETextureType::IMGUI_ICON_WALL22);
	ImagePush(ETextureType::IMGUI_ICON_WALL22);
	ImagePush(ETextureType::IMGUI_ICON_TOP);
	ImagePush(ETextureType::IMGUI_ICON_DAISY);
	ImagePush(ETextureType::IMGUI_ICON_SELECT_PLANE);
	ImagePush(ETextureType::IMGUI_ICON_CHAIR);
	ImagePush(ETextureType::IMGUI_ICON_CHEST);
	ImagePush(ETextureType::IMGUI_ICON_SMALLTABLE);
	ImagePush(ETextureType::IMGUI_ICON_TABLE);
	ImagePush(ETextureType::IMGUI_ICON_GREENBOOK);
	ImagePush(ETextureType::IMGUI_ICON_PURPLEBOOK);
	ImagePush(ETextureType::IMGUI_ICON_YELLOWBOOK);
	ImagePush(ETextureType::IMGUI_ICON_REDBOOK);
	ImagePush(ETextureType::IMGUI_ICON_STAGE_FRAME);
	ImagePush(ETextureType::IMGUI_ICON_STAGE_FRAME2);
	ImagePush(ETextureType::IMGUI_ICON_STAGE_FRAME3);
	ImagePush(ETextureType::IMGUI_ICON_BOSS_FIGURE);
	ImagePush(ETextureType::IMGUI_ICON_DOLLY);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN2);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN3);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN4);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN5);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN6);
	ImagePush(ETextureType::IMGUI_ICON_WOODEN7);
	ImagePush(ETextureType::IMGUI_ICON_CHEST2);
	ImagePush(ETextureType::IMGUI_ICON_CHESTTOP2);
	ImagePush(ETextureType::IMGUI_ICON_ROCKS);
	ImagePush(ETextureType::IMGUI_ICON_STALAGS);
	ImagePush(ETextureType::IMGUI_ICON_COINS);
	ImagePush(ETextureType::IMGUI_ICON_RAIL);
	ImagePush(ETextureType::IMGUI_ICON_CLAIMEDWALL2C);
	ImagePush(ETextureType::IMGUI_ICON_CLAIMEDE);
	ImagePush(ETextureType::IMGUI_ICON_TEST_STAGE);
}

void CImGuiEditor::UnInit()
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}

	for (auto& texture : m_TextureList)
	{
		ID3D11ShaderResourceView* textureView = static_cast<ID3D11ShaderResourceView*>(texture.second);
		textureLoader->Release(textureView);
	}
}

XMFLOAT4X4 CImGuiEditor::ConvertMatrixForXMFLOAT4X4(const XMMATRIX& Matrix)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, Matrix);

	return temp;
}

void CImGuiEditor::EditSliderBox()
{
	if (!m_MouseCursor)
	{
		return;
	}
	CBox* box = m_MouseCursor->GetCollisionBox();
	if (!box)
	{
		box = m_MouseCursor->GetRangeBox();
		if (!box)
		{
			return;
		}
	}

	BoxData& boxData = box->GetEditBoxData();
	ImGui::SliderFloat("Max.x", &boxData.Max.x, -30.0f, 30.0f);
	ImGui::SliderFloat("Max.y", &boxData.Max.y, -30.0f, 30.0f);
	ImGui::SliderFloat("Max.z", &boxData.Max.z, -30.0f, 30.0f);
	ImGui::SliderFloat("Min.x", &boxData.Min.x, -30.0f, 30.0f);
	ImGui::SliderFloat("Min.y", &boxData.Min.y, -30.0f, 30.0f);
	ImGui::SliderFloat("Min.z", &boxData.Min.z, -30.0f, 30.0f);
	ImGui::SliderFloat("Position.x", &boxData.AddPosition.x, -30.0f, 30.0f);
	ImGui::SliderFloat("Position.y", &boxData.AddPosition.y, -30.0f, 30.0f);
	ImGui::SliderFloat("Position.z", &boxData.AddPosition.z, -30.0f, 30.0f);
	box->SetBoxSize(boxData.Max, boxData.Min);
	box->AddBoxPosition(boxData.AddPosition);
}

void CImGuiEditor::Draw()
{
	if (!IsEditMode())
	{
		return;
	}
	if (!m_MouseCursor)
	{
		return;
	}
	
	//エディタに選択しているオブジェクト名を送る
	const std::vector<MapObject>& mapObject = m_MouseCursor->GetMapObjectList();
	for (const MapObject& item : mapObject)
	{
		if (!item.Object || !m_MouseCursor->IsSelectObject())
		{
			continue;
		}
		char num[99];
		//オブジェクト名は名前＋そのオブジェクトの配置数となる 例 Hoge:0
		sprintf_s(num, "%s:%d", item.Name.c_str(), item.ObjectNumber);
		if (m_MouseCursor->GetCreateNumber() == item.Object->GetOriginalNumber())
		{
			m_ObjectName = num;
		}
	}
	EditObjectWindow();
	SelectObjectWindow();
}

void CImGuiEditor::ImagePush(const ETextureType Type)
{
	if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
	{
		std::weak_ptr<ID3D11ShaderResourceView> texture = textureLoader->Load(Type);
		if (texture.expired())
		{
			return;
		}
		ImTextureID imTextureId = static_cast<ImTextureID>(texture.lock().get());
		m_TextureList.emplace(Type, imTextureId);
	}
}

void CImGuiEditor::EditCommonObjectImage(const EEditorCreateObjectType CreateType, const EStaticMeshType MeshType, const ETextureType TextureType, const char* Text, const Vector3& ModelScale)
{
	if (!m_MouseCursor)
	{
		return;
	}
	const EEditorCreateObjectType createType = m_MouseCursor->GetCreateObjectType();
	const EStaticMeshType meshType = m_MouseCursor->GetMeshType();
	//ドラッグドロップ時に選択されているオブジェクトの画像の色を変更
	if (meshType == MeshType && createType == CreateType)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	//ボタン画像表示
	ImGui::ImageButton(m_TextureList[TextureType], IMAGE_BUTTON_SIZE, ImVec2(0, 0), ImVec2(1, 1), 5, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
	if (meshType == MeshType && createType == CreateType)
	{
		ImGui::PopStyleColor(1);
	}
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		//変数にドラッグドロップ番号を格納
		ImGui::SetDragDropPayload("DND_DEMO_CELL", &CreateType, sizeof(EEditorCreateObjectType));
		ImGui::Text(Text);
		ImGui::EndDragDropSource();
		m_MouseCursor->SetMeshType(MeshType);
		m_MouseCursor->StartDragDrop(CreateType);
		m_MouseCursor->SetObjectScale(ModelScale);
	}

	ImGui::SameLine();
	//テキストの改行を行う
	std::string lineBreak = "\n\n";
	lineBreak += Text;
	ImGui::Text(lineBreak.c_str());
}

void CImGuiEditor::EditObjectImage(const EEditorCreateObjectType CreateType, const ETextureType TextureType, const char* Text, const int Num /*= -1.0f*/)
{
	if (!m_MouseCursor)
	{
		return;
	}
	//ドラッグドロップ時に選択されているオブジェクトの画像の色を変更
	const EEditorCreateObjectType createType = m_MouseCursor->GetCreateObjectType();
	if (createType == CreateType)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	// 同じテクスチャが並んだ際にImGui側で区別できないためIDを設定する
	ImGui::PushID(static_cast<int>(CreateType));

	//ボタン画像表示
	ImGui::ImageButton(m_TextureList[TextureType], IMAGE_BUTTON_SIZE, ImVec2(0, 0), ImVec2(1, 1), 5, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
	if (createType == CreateType)
	{
		ImGui::PopStyleColor(1);
	}
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		//変数にドラッグドロップ番号を格納
		ImGui::SetDragDropPayload("DND_DEMO_CELL", &CreateType, sizeof(EEditorCreateObjectType));
		ImGui::Text(Text);
		ImGui::EndDragDropSource();
		m_MouseCursor->StartDragDrop(CreateType);
	}

	ImGui::PopID();

	ImGui::SameLine();

	if (Num >= 0)
	{
		ImGui::Text("\n\n%s:%d", Text, Num);
	}
	else
	{
		ImGui::Text("\n\n%s", Text);
	}
}

void CImGuiEditor::SceneChangeWindow()
{
	//シーン名定義
	const char* items[] = { "NONE", "Test","Title","StageSelect","Tutorial", "First", "Second", "BOSS", "ModelView" };
	ImGui::Text("SceneName:%s", m_SceneName.c_str());
	ImGui::Combo("SetScene", &m_SelectSceneNumber, items, IM_ARRAYSIZE(items));
	//選択したシーンに移行するボタンが押下されたらシーン遷移
	if (ImGui::Button("Change"))
	{
		if (CSceneManager* manager = CSceneManager::GetInstance())
		{
			manager->SetInterruptScene(static_cast<ESceneType>(m_SelectSceneNumber));
		}
	}
}

void CImGuiEditor::SelectObjectWindow()
{
	if (!m_Camera || !m_MouseCursor)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGuizmo::BeginFrame();
	
	ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_None);
	ImGui::Text("ScreenPos");
	ImGui::SameLine();
	ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
	
	if (m_MouseCursor->GetSelectObjectType() != EEditorSelectObjectType::NONE)
	{
		if (m_MouseCursor->IsSelectObject())
		{
			ImGui::Text("ObjectName:%s", m_ObjectName.c_str());
			ImGui::Separator();
			//マウスがオブジェクトを選択していたらギズモ表示
			EditTransform();
		}
	}
	
	if (!io.WantCaptureMouse)
	{
		//マウスのホイール操作でカメラの距離を変える
		float* length = m_Camera->GetCameraLength();
		*length -= io.MouseWheel;
		*length = (std::max)(1.0f, *length);
		m_Camera->SetCameraLength(*length);
	}
	ImGui::End();
	
	DirectX::XMFLOAT4X4 v = ConvertMatrixForXMFLOAT4X4(m_Camera->GetInvViewMatrix());
	//右上へ選択しているオブジェクトのトランスフォームを表すカメラキューブを描画
	ImGuizmo::ViewManipulate(&v.m[0][0], 28.0f, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), 0x10101010);
}

void CImGuiEditor::EditTransform()
{
	if (!m_Camera || !m_MouseCursor)
	{
		return;
	}

	const EEditorSelectObjectType selectType = m_MouseCursor->GetSelectObjectType();
	XMFLOAT4X4 v = ConvertMatrixForXMFLOAT4X4(m_Camera->GetInvViewMatrix());
	XMFLOAT4X4 p = ConvertMatrixForXMFLOAT4X4(m_Camera->GetProjectionMatrix());
	XMFLOAT4X4 w = ConvertMatrixForXMFLOAT4X4(m_MouseCursor->GetWorld());
	
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	if (selectType == EEditorSelectObjectType::ACORN || selectType == EEditorSelectObjectType::HEART || selectType == EEditorSelectObjectType::STAR || selectType == EEditorSelectObjectType::NO_CLIFFBOX || selectType == EEditorSelectObjectType::MONKEY)
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	//各項目切り替えラジオボタン
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}
	if (selectType == EEditorSelectObjectType::COMMON || selectType == EEditorSelectObjectType::SLIME || selectType == EEditorSelectObjectType::THORNMAN || selectType == EEditorSelectObjectType::STAR_DOOR)
	{
		if (selectType != EEditorSelectObjectType::STAR_DOOR)
		{
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			{
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			}
		}
	}

	//選択しているオブジェクトを削除するボタン
	if (CInputInterface* instance = CInputInterface::GetInstance())
	{
		if (ImGui::Button("Delete") || instance->IsImGuiCaptureObjectDeleteTrigger())
		{
			m_MouseCursor->DeleteSelectObject();
			return;
		}
	}
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	//モデルのワールド座標から各マトリクスを取得
	ImGuizmo::DecomposeMatrixToComponents(&w.m[0][0], matrixTranslation, matrixRotation, matrixScale);

	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	if (selectType == EEditorSelectObjectType::COMMON || selectType == EEditorSelectObjectType::SLIME || selectType == EEditorSelectObjectType::MONKEY || selectType == EEditorSelectObjectType::THORNMAN || selectType == EEditorSelectObjectType::STAR_DOOR || selectType == EEditorSelectObjectType::PLAYER_START)
	{
		if (selectType == EEditorSelectObjectType::STAR_DOOR || selectType == EEditorSelectObjectType::PLAYER_START)
		{
			if (ImGui::InputFloat3("Rt", matrixRotation, 3))
			{
				m_MouseCursor->SetRotation(Vector3(0.0f, matrixRotation[1], 0.0f));
			}
		}
		if (selectType == EEditorSelectObjectType::SLIME || selectType == EEditorSelectObjectType::THORNMAN || selectType == EEditorSelectObjectType::COMMON)
		{
			Vector3* rot = m_MouseCursor->GetRotationAddr();
			float matrixRotation2[3];
			matrixRotation2[0] = rot->x;
			matrixRotation2[1] = rot->y;
			matrixRotation2[2] = rot->z;
			if (ImGui::InputFloat3("Rt", matrixRotation2, 3))
			{
				m_MouseCursor->SetRotation(Vector3(matrixRotation2[0], matrixRotation2[1], matrixRotation2[2]));
			}
			ImGui::InputFloat3("Sc", matrixScale, 3);
		}
		if (selectType == EEditorSelectObjectType::MONKEY)
		{
			Vector3* rot = m_MouseCursor->GetRotationAddr();
			float matrixRotation2[3];
			matrixRotation2[0] = rot->x;
			matrixRotation2[1] = rot->y;
			matrixRotation2[2] = rot->z;
			if (ImGui::InputFloat3("Rt", matrixRotation2, 3))
			{
				m_MouseCursor->SetRotation(Vector3(matrixRotation2[0], matrixRotation2[1], matrixRotation2[2]));
			}
		}
	}
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &w.m[0][0]);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
		{
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
		{
			mCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}
	EditObjectTips(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
	

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	//ギズモの移動を反映
	if (selectType == EEditorSelectObjectType::ACORN || selectType == EEditorSelectObjectType::HEART || selectType == EEditorSelectObjectType::STAR || selectType == EEditorSelectObjectType::NO_CLIFFBOX)
	{
		ImGuizmo::Manipulate(&v.m[0][0], &p.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &w.m[0][0], NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	}
	else
	{//エネミーの場合は違う計算を行う
		ImGuizmo::Manipulate2(&v.m[0][0], &p.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &w.m[0][0], NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	}
	if (ImGuizmo::IsUsing())
	{//ギズモで求めた座標を変換　ギズモを触っている時のみ更新
		ImGuizmo::DecomposeMatrixToComponents(&w.m[0][0], matrixTranslation, matrixRotation, matrixScale);
	}
	m_MouseCursor->SetPosition(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
	m_MouseCursor->SetScale(Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));

	EditCopy(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]), Vector3(matrixRotation[0], matrixRotation[1], matrixRotation[2]), Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));
}

void CImGuiEditor::EditCopy(const Vector3& Position, const Vector3& Rotation, const Vector3& Scale)
{
	if (!m_MouseCursor)
	{
		return;
	}

	const EEditorSelectObjectType selectType = m_MouseCursor->GetSelectObjectType();
	if (selectType == EEditorSelectObjectType::STAR_DOOR)
	{
		return;
	}
	if (selectType == EEditorSelectObjectType::PLAYER_START)
	{
		return;
	}

	if (ImGui::Button("Copy"))
	{	
		switch (selectType)
		{
			case EEditorSelectObjectType::ACORN:
				m_MouseCursor->CreateObject(EEditorCreateObjectType::ACORN, Position);
				break;
			case EEditorSelectObjectType::HEART:
				m_MouseCursor->CreateObject(EEditorCreateObjectType::HEART, Position);
				break;
			case EEditorSelectObjectType::STAR:
				m_MouseCursor->CreateObject(EEditorCreateObjectType::STAR, Position);
				break;
			case EEditorSelectObjectType::COMMON:
			case EEditorSelectObjectType::NO_MESH:
			{
				CSphere* sphere = m_MouseCursor->GetCullingSphere();
				if (selectType == EEditorSelectObjectType::COMMON || selectType == EEditorSelectObjectType::NO_MESH)
				{
					if (sphere)
					{
						m_MouseCursor->CreateCommonObject(EEditorCreateObjectType::COMMON, Position, Rotation, Scale, sphere->GetRadius(), Vector3(sphere->GetAddPosition()), m_MouseCursor->GetPlayerCollisionTag(), m_MouseCursor->GetObjectCollisionTag(),m_MouseCursor->GetCameraCollisionTag(),m_MouseCursor->GetStepsCollisionTag(),*m_MouseCursor->IsEnableLight(),m_MouseCursor->GetMaterial());
					}
					else
					{
						m_MouseCursor->CreateCommonObject(EEditorCreateObjectType::COMMON, Position, Rotation, Scale, 0.0f, Vector3(0, 0, 0), m_MouseCursor->GetPlayerCollisionTag(), m_MouseCursor->GetObjectCollisionTag(), m_MouseCursor->GetCameraCollisionTag(), m_MouseCursor->GetStepsCollisionTag(), *m_MouseCursor->IsEnableLight(), m_MouseCursor->GetMaterial());
					}
				}
				break;
			}	
			case EEditorSelectObjectType::NO_CLIFFBOX:
				m_MouseCursor->CreateObject(EEditorCreateObjectType::NO_CLIFFBOX, Position);
				break; 
		}
	}
	
}

void CImGuiEditor::EditObjectWindow()
{
	CInputInterface* instance = CInputInterface::GetInstance();
	CSaveAndLoad* save = CSaveAndLoad::GetInstance();
	if (!instance || !m_MouseCursor || !save)
	{
		return;
	}
	
	ImGui::Begin("ObjEdit", nullptr, ImGuiWindowFlags_None);
	m_AutoSaveFrame++;
	//60秒ごとにオートセーブする
	if (m_AutoSaveFrame == AUTOSAVE_FRAME)
	{
		save->Save("AutoSave.txt");
		m_AutoSaveFrame = 0;
	}
	
	ImGui::Combo("SaveFile", &m_SelectSaveNumber, m_SaveFiles, IM_ARRAYSIZE(m_SaveFiles));
	if (ImGui::Button("Save") || instance->IsImGuiSaveTrigger())
	{
		save->Save(m_SaveFiles[m_SelectSaveNumber]);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		m_MouseCursor->ResetMapObject();
		save->Load(m_SaveFiles[m_SelectSaveNumber]);
	}
	ImGui::SameLine();
	if (ImGui::Button("AddLoad"))
	{
		save->AddLoad(m_SaveFiles[m_SelectSaveNumber]);
	}
	
	if (ImGui::BeginTabBar("TabsObj"))
	{
		if (ImGui::BeginTabItem("ObjectEdit"))
		{
			if (ImGui::CollapsingHeader("CommonObject"))
			{
				EditAcornImage();
				EditHeartImage();
				EditStarImage();
				EditStarDoorImage();
				EditPlyerStartImage();
				EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::SKYDOME, ETextureType::IMGUI_ICON_SKY_DOME, "SkyDome", Vector3(1, 1, 1));
			}
			if (ImGui::CollapsingHeader("Plane"))
			{
				EditPlaneImage();
			}
			if (ImGui::CollapsingHeader("NoMesh"))
			{
				EditNoMeshImage();
			}
			if (ImGui::CollapsingHeader("FirstMap"))
			{
				EditFistMapImage();
			}
			if (ImGui::CollapsingHeader("SecondMap"))
			{
				EditSecondMapImage();
			}
			if (ImGui::CollapsingHeader("SecondMapPlane"))
			{
				EditSecondMapPlaneImage();
			}
			if (ImGui::CollapsingHeader("StageSelect"))
			{
				EditStageSelectImage();
			}		
			if (ImGui::CollapsingHeader("Collision"))
			{
				EditCollisionImage();
			}
			if (ImGui::CollapsingHeader("Enemy"))
			{
				EditEnemy();
			}
			if (ImGui::CollapsingHeader("FirstBoss"))
			{
				EditFirstBoss();
			}
			//ドラッグドロップ時ImGuiウィンドウの上でボタンを離したら
			//持っているオブジェクトを無くす
			if (instance->IsImGuiCaptureObjectRelease())
			{
				if (ImGui::IsAnyWindowHovered())
				{
					m_MouseCursor->ClearSelectObject();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("CameraSetting"))
		{
			EditCameraSetting();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("PutAcorn"))
		{
			PutAcornWindow();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("SceneChange"))
		{
			SceneChangeWindow();
			ImGui::EndTabItem();
		}
	}
	
	ImGui::EndTabBar();
	ImGui::End();
}

void CImGuiEditor::EditObjectTips(const Vector3& Position)
{
	if (!m_MouseCursor)
	{
		return;
	}
	const EEditorSelectObjectType selectType = m_MouseCursor->GetSelectObjectType();

	if (selectType == EEditorSelectObjectType::STAR || selectType == EEditorSelectObjectType::HEART)
	{
		ImGui::Checkbox("DisableShadow", m_MouseCursor->IsDisableShadow());
		ImGui::Checkbox("IsEnableLight", m_MouseCursor->IsEnableLight());
		if (*m_MouseCursor->IsEnableLight())
		{
			EditMaterial();
		}
	}
	if (selectType == EEditorSelectObjectType::COMMON)
	{

		CSphere* sphere = m_MouseCursor->GetCullingSphere();
		if (ImGui::Checkbox("IsNoCulling", m_MouseCursor->IsNoCullingCheck()))
		{
			m_MouseCursor->SetNoCullingCheck(*m_MouseCursor->IsNoCullingCheck());
		}
		ImGui::Checkbox("DisableDepth", m_MouseCursor->IsDisableDepth());
		ImGui::Checkbox("EditorOnlyDraw", m_MouseCursor->IsEditorOnlyDraw());
		ImGui::Checkbox("DisableShadow", m_MouseCursor->IsDisableShadow());
		ImGui::Checkbox("EnableLight", m_MouseCursor->IsEnableLight());
		ImGui::Checkbox("DisableSelfShadow", m_MouseCursor->IsDisableSelfShadow());
		if (m_MouseCursor->IsLoadNormalMap())
		{
			ImGui::Checkbox("EnableNormalMap", m_MouseCursor->IsEnableNormalMap());
		}
		if (*m_MouseCursor->IsEnableLight())
		{
			EditMaterial();
		}
		EditCullingSphere();
		if (sphere)
		{
			EditCollisionTag();
		}
		EditNormalPower();
	}
	if (selectType == EEditorSelectObjectType::NO_CLIFFBOX)
	{
		EditBox();
	}
	if (selectType == EEditorSelectObjectType::CAMERA_HEIGHTLOCK_BOX)
	{
		EditCameraBox();
		EditBox();
	}
	if (selectType == EEditorSelectObjectType::TUTORIAL_BOX)
	{
		EditTutorialNumber();
		EditBox();
	}
	if (selectType == EEditorSelectObjectType::STAR_DOOR)
	{
		EditStarDoor();
	}
	if (selectType == EEditorSelectObjectType::SLIME)
	{
		EditEnemyFirstPosition(Position);
		EditEnemyMeshCollision();
		EditEnemyTargetPoint();
		EditEnemyRangeSphere();
		EditEnemyRangeBox();
		EditEnemyBodySphere();
		EditRespawn();
		EditItemDrop();
	}
	if (selectType == EEditorSelectObjectType::THORNMAN)
	{
		EditEnemyFirstPosition(Position);
		EditEnemyTargetPoint();
		EditEnemyBodySphere();
		EditRespawn();
		EditItemDrop();
	}
	if (selectType == EEditorSelectObjectType::MONKEY)
	{
		EditEnemyFirstPosition(Position);
		EditEnemyRangeSphere();
		EditRespawn();
		EditItemDrop();
	}
}

void CImGuiEditor::EditCameraSetting()
{
	if (!m_Camera)
	{
		return;
	}

	ImGui::SliderFloat("Length", m_Camera->GetCameraLength(), 1.0f, 200.0f);
	ImGui::SliderFloat("MoveSpeed", m_Camera->GetEditCameraSpeed(), 1.0f, 200.0f);
	ImGui::SliderFloat("RightLeftRotSpeed", m_Camera->GetEditRightLeftRotSpeed(), 0.0f, 200.0f);
	ImGui::SliderFloat("UpDownRotSpeed", m_Camera->GetEditUpDownRotSpeed(), 0.0f, 200.0f);
	ImGui::Checkbox("RightLeftRotChange", m_Camera->IsEditRightLeftRotChange());
	ImGui::Checkbox("UpDownRotChange", m_Camera->IsEditUpDownRotChange());
}

void CImGuiEditor::EditMaterial()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::CollapsingHeader("Material"))
	{
		MATERIAL* material = m_MouseCursor->GetMaterial();
		ImGui::SliderFloat("Diffuse.r", &material->Diffuse.r, 0.0f, 2.0f);
		ImGui::SliderFloat("Diffuse.g", &material->Diffuse.g, 0.0f, 2.0f);
		ImGui::SliderFloat("Diffuse.b", &material->Diffuse.b, 0.0f, 2.0f);
		ImGui::SliderFloat("Diffuse.a", &material->Diffuse.a, 0.0f, 2.0f);
		ImGui::SliderFloat("Ambient.r", &material->Ambient.r, 0.0f, 2.0f);
		ImGui::SliderFloat("Ambient.g", &material->Ambient.g, 0.0f, 2.0f);
		ImGui::SliderFloat("Ambient.b", &material->Ambient.b, 0.0f, 2.0f);
		ImGui::SliderFloat("Ambient.a", &material->Ambient.a, 0.0f, 2.0f);
		ImGui::SliderFloat("Specular.r", &material->Specular.r, 0.0f, 2.0f);
		ImGui::SliderFloat("Specular.g", &material->Specular.g, 0.0f, 2.0f);
		ImGui::SliderFloat("Specular.b", &material->Specular.b, 0.0f, 2.0f);
		ImGui::SliderFloat("Specular.a", &material->Specular.a, 0.0f, 2.0f);
		ImGui::SliderFloat("Alpha", &material->Alpha.r, 0.0f, 1.0f);
		float* Diffuse[4];
		Diffuse[0] = &material->Diffuse.r;
		Diffuse[1] = &material->Diffuse.g;
		Diffuse[2] = &material->Diffuse.b;
		Diffuse[3] = &material->Diffuse.a;
		ImGui::InputFloat4("DiffuseIn", *Diffuse,4);
		float* Ambient[4];
		Ambient[0] = &material->Ambient.r;
		Ambient[1] = &material->Ambient.g;
		Ambient[2] = &material->Ambient.b;
		Ambient[3] = &material->Ambient.a;
		ImGui::InputFloat4("AmbientIn", *Ambient, 4);
		float* Specular[4];
		Specular[0] = &material->Specular.r;
		Specular[1] = &material->Specular.g;
		Specular[2] = &material->Specular.b;
		Specular[3] = &material->Specular.a;
		ImGui::InputFloat4("SpecularIn", *Specular, 4);
		float* Alpha;
		Alpha = &material->Alpha.r;
		ImGui::InputFloat("AlphaIn", Alpha);

		if (ImGui::Button("AllMaterialSet"))
		{
			m_MouseCursor->MaterialAllSet(*material);
		}
	}
}

void CImGuiEditor::EditCullingSphere()
{
	if (!m_MouseCursor)
	{
		return;
	}
	CSphere* sphere = m_MouseCursor->GetCullingSphere();
	if (!sphere)
	{
		return;
	}
	if (*m_MouseCursor->IsNoCullingCheck())
	{
		return;
	}
	if (ImGui::CollapsingHeader("CollisionCullingSphere"))
	{
		if (ImGui::Button("AutoCreate"))
		{
			m_MouseCursor->UpdateAutoCullingSphere();
		}
		SphereData& data = sphere->GetEditSphereData();
		ImGui::SliderFloat("Radius", &data.Radius, 0.0f, 40.0f);
		ImGui::SliderFloat("Position.x", &data.AddPosition.x, -20.0f, 50.0f);
		ImGui::SliderFloat("Position.y", &data.AddPosition.y, -20.0f, 50.0f);
		ImGui::SliderFloat("Position.z", &data.AddPosition.z, -20.0f, 50.0f);
		ImGui::InputFloat("RadiusInput", &data.Radius);
		ImGui::InputFloat("Position.xInput", &data.AddPosition.x);
		ImGui::InputFloat("Position.yInput", &data.AddPosition.y);
		ImGui::InputFloat("Position.zInput", &data.AddPosition.z);
		const char* items[] = { "YELLOW","RED","BLUE" };
		//コンボボックス格納変数
		static int currentColor = 0;
		if (ImGui::Combo("COLOR", &currentColor, items, IM_ARRAYSIZE(items)))
		{
			switch (currentColor)
			{
			case 0:
				m_MouseCursor->ChangeSphereTexture(ETextureType::BODY3);
				break;
			case 1:
				m_MouseCursor->ChangeSphereTexture(ETextureType::BODY2);
				break;
			case 2:
				m_MouseCursor->ChangeSphereTexture(ETextureType::BODY1);
				break;
			}
		}
		sphere->SetRadius(sphere->GetRadius());
	}
}

void CImGuiEditor::EditCollisionTag()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::CollapsingHeader("CollisionTag"))
	{
		const char* items[] = { "ALL_COLISION", "NO_WALL_HIT", "NO_MESH_HIT", "NO_CLIFF_HIT", "NO_LINE_HIT", "NO_COLLISION_HIT", "NO_SLOPESLIP_HIT"};
		//コンボボックス格納変数
		int currentPlayerCollison = static_cast<int>(m_MouseCursor->GetPlayerCollisionTag());
	
		if (ImGui::Combo("PlayerCollisionTag", &currentPlayerCollison, items, IM_ARRAYSIZE(items)))
		{
			switch (currentPlayerCollison)
			{
			case 0:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::ALL_COLISION);
				break;
			case 1:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::NO_WALL_HIT);
				break;
			case 2:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::NO_MESH_HIT);
				break;
			case 3:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::NO_CLIFF_HIT);
				break;
			case 4:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::NO_LINE_HIT);
				break;
			case 5:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::NO_COLLISION_HIT);
				break;
			case 6:
				m_MouseCursor->SetPlayerCollisionTag(ECollisionTagName::NO_SLOPESLIP_HIT);
				break;
			}
		}
		const char* obj_items[] = { "OBJECT_COLISION", "NO_COLLISION_HIT" };
		//コンボボックス格納変数
		int currentObjectCollison = static_cast<int>(m_MouseCursor->GetObjectCollisionTag());
		switch (currentObjectCollison)
		{
		case static_cast<int>(ECollisionTagName::OBJECT_COLISION):
			currentObjectCollison = 0;
			break;
		case static_cast<int>(ECollisionTagName::NO_COLLISION_HIT):
			currentObjectCollison = 1;
			break;
		}
		if (ImGui::Combo("ObjectCollisionTag", &currentObjectCollison, obj_items, IM_ARRAYSIZE(obj_items)))
		{
			switch (currentObjectCollison)
			{
			case 0:
				m_MouseCursor->SetObjectCollisionTag(ECollisionTagName::OBJECT_COLISION);
				break;
			case 1:
				m_MouseCursor->SetObjectCollisionTag(ECollisionTagName::NO_COLLISION_HIT);
				break;
			}
		}

		const char* camera_items[] = { "NO_COLISION", "BLOCK_COLISION", "CLEAR_COLISION" };
		//コンボボックス格納変数
		int currentCameraCollison = static_cast<int>(m_MouseCursor->GetCameraCollisionTag());
		switch (currentCameraCollison)
		{
		case static_cast<int>(ECollisionTagCameraName::NONE):
			currentCameraCollison = 0;
			break;
		case static_cast<int>(ECollisionTagCameraName::BLOCK):
			currentCameraCollison = 1;
			break;
		case static_cast<int>(ECollisionTagCameraName::CLEAR):
			currentCameraCollison = 2;
			break;
		}
		if (ImGui::Combo("CameraCollisionTag", &currentCameraCollison, camera_items, IM_ARRAYSIZE(camera_items)))
		{
			switch (currentCameraCollison)
			{
			case 0:
				m_MouseCursor->SetCameraCollisionTag(ECollisionTagCameraName::NONE);
				break;
			case 1:
				m_MouseCursor->SetCameraCollisionTag(ECollisionTagCameraName::BLOCK);
				break;
			case 2:
				m_MouseCursor->SetCameraCollisionTag(ECollisionTagCameraName::CLEAR);
				break;
			}
		}

		const char* steps_items[] = { "NORMAL", "BRIDGE", "ROCK" };
		//コンボボックス格納変数
		int currentStepsCollision = static_cast<int>(m_MouseCursor->GetStepsCollisionTag());
		switch (currentStepsCollision)
		{
		case static_cast<int>(ECollisionTagSteps::NORMAL):
			currentStepsCollision = 0;
			break;
		case static_cast<int>(ECollisionTagSteps::BRIDGE):
			currentStepsCollision = 1;
			break;
		case static_cast<int>(ECollisionTagSteps::ROCK):
			currentStepsCollision = 2;
			break;
		}
		if (ImGui::Combo("StepsCollisionTag", &currentStepsCollision, steps_items, IM_ARRAYSIZE(steps_items)))
		{
			switch (currentStepsCollision)
			{
			case 0:
				m_MouseCursor->SetStepsCollisionTag(ECollisionTagSteps::NORMAL);
				break;
			case 1:
				m_MouseCursor->SetStepsCollisionTag(ECollisionTagSteps::BRIDGE);
				break;
			case 2:
				m_MouseCursor->SetStepsCollisionTag(ECollisionTagSteps::ROCK);
				break;
			}
		}
	}
}

void CImGuiEditor::EditNormalPower()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (!m_MouseCursor->IsLoadNormalMap())
	{
		return;
	}

	if (ImGui::CollapsingHeader("NormalPower"))
	{
		ShaderNormalMapData* normal = m_MouseCursor->GetNormalMapPower();
		ImGui::SliderFloat("NormalPower", &normal->NormalPower.x, 0.1f, 15.0f);
		ImGui::SliderFloat("SpecularPower", &normal->SpecularPower.x, 0.1f, 100.0f);
		ImGui::InputFloat("SpecularPowerInput", &normal->SpecularPower.x);
	}
}

void CImGuiEditor::EditBox()
{
	if (ImGui::CollapsingHeader("CollisionBox"))
	{
		EditSliderBox();
	}
}

void CImGuiEditor::EditCameraBox()
{
	if (!m_MouseCursor)
	{
		return;
	}
	ImGui::InputFloat("YLockPos", m_MouseCursor->GetHeightLockPosition());
}

void CImGuiEditor::EditTutorialNumber()
{
	if (!m_MouseCursor)
	{
		return;
	}
	const char* items[] = { "0", "1", "2", "3", "4", "5" };
	//コンボボックス格納変数
	int *number = m_MouseCursor->GetTutorialNumber();

	ImGui::Combo("TutorialNumber", number, items, IM_ARRAYSIZE(items));
}

void CImGuiEditor::EditEnemyFirstPosition(Vector3 pos)
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::Button("SetFirstPosition"))
	{
		m_MouseCursor->SetFirstPosition(pos);
	}
}

void CImGuiEditor::EditEnemyMeshCollision()
{
	if (!m_MouseCursor)
	{
		return;
	}
	ImGui::Checkbox("IsBodySphereCollisionCheck", m_MouseCursor->IsBodySphereCollisionCheck());
	ImGui::Checkbox("IsDownLineCollisionCheck", m_MouseCursor->IsDownLineCollisionCheck());
}

void CImGuiEditor::EditEnemyTargetPoint()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::CollapsingHeader("TargetPoint"))
	{
		if (ImGui::Button("Add"))
		{
			m_MouseCursor->AddTargetPosition(Vector3(0, 0, 0));
		}
	
		std::vector<Vector3>* target = m_MouseCursor->GetTargetPoint();
		int size = static_cast<int>(target->size());
		for (int i = 0; i < size; i++)
		{	//ターゲットポイントの中を表示　個別認識するため(void*)(intptr_t)iを付ける
			if (ImGui::TreeNode((void*)(intptr_t)i, "TargetPoint:%d", i))
			{
				if (size != 1)
				{
					if (ImGui::Button("Remove"))
					{
						m_MouseCursor->DeleteTargetPosition(i);
						ImGui::TreePop();
						break;
					}
				}
				const Vector3 first = m_MouseCursor->GetFirstPosition();
				Vector3 local =  (*target)[i] - first;
				ImGui::SliderFloat("Point.x", &local.x, -30.0f, 30.0f);
				ImGui::SliderFloat("Point.y", &local.y, -30.0f, 30.0f);
				ImGui::SliderFloat("Point.z", &local.z, -30.0f, 30.0f);
				(*target)[i] = first + local;
				m_MouseCursor->SetTargetPointImagePosition(i,(*target)[i]);
				ImGui::TreePop();
			}
		}
	}
}

void CImGuiEditor::EditEnemyRangeSphere()
{
	if (!m_MouseCursor)
	{
		return;
	}
	CSphere* sphere = m_MouseCursor->GetRangeSphere();
	if (!sphere)
	{
		return;
	}
	if (ImGui::CollapsingHeader("RangeSphere"))
	{
		SphereData& data = sphere->GetEditSphereData();
		ImGui::SliderFloat("Radius", &data.Radius, 0.0f, 40.0f);
		ImGui::SliderFloat("Position.x", &data.AddPosition.x, -20.0f, 50.0f);
		ImGui::SliderFloat("Position.y", &data.AddPosition.y, -20.0f, 50.0f);
		ImGui::SliderFloat("Position.z", &data.AddPosition.z, -20.0f, 50.0f);
		sphere->SetRadius(data.Radius);
	}
}

void CImGuiEditor::EditEnemyRangeBox()
{
	if (ImGui::CollapsingHeader("RangeBox"))
	{
		EditSliderBox();
	}
}

void CImGuiEditor::EditEnemyBodySphere()
{
	if (!m_MouseCursor)
	{
		return;
	}
	CSphere* sphere = m_MouseCursor->GetSphere();
	if (!sphere)
	{
		return;
	}

	if (ImGui::CollapsingHeader("BodySphere"))
	{
		SphereData& data = sphere->GetEditSphereData();
		ImGui::SliderFloat("BodyRadius", &data.Radius, 0.0f, 40.0f);
		ImGui::SliderFloat("BodyPosition.x", &data.AddPosition.x, -20.0f, 50.0f);
		ImGui::SliderFloat("BodyPosition.y", &data.AddPosition.y, -20.0f, 50.0f);
		ImGui::SliderFloat("BodyPosition.z", &data.AddPosition.z, -20.0f, 50.0f);
		sphere->SetRadius(data.Radius);
	}
}

void CImGuiEditor::EditRespawn()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::CollapsingHeader("Respawn"))
	{
		ImGui::Checkbox("IsRespawn", m_MouseCursor->IsRespawn());
		if (*m_MouseCursor->IsRespawn())
		{
			ImGui::InputInt("RespawnTime", m_MouseCursor->GetRespawnTime());
		}
	}
}

void CImGuiEditor::EditItemDrop()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::CollapsingHeader("ItemDrop"))
	{
		const char* items[] = { "NO_ITEM", "ACORN", "HEART", "STAR" };
		//コンボボックス格納変数
		static int currentItem = 0;
		ItemDropData* index = m_MouseCursor->GetItemDropData();
		currentItem = static_cast<int>(index->ItemDropType);
		if (ImGui::Combo("ITEM_LIST", &currentItem, items, IM_ARRAYSIZE(items)))
		{
			index->ItemDropType = static_cast<EItemDropType>(currentItem);
		}
		if (currentItem == static_cast<int>(EItemDropType::HEART))
		{
			ImGui::SliderInt("Percent", &index->Percent, 1, 100);
		}
	}
}

void CImGuiEditor::EditStarDoor()
{
	if (!m_MouseCursor)
	{
		return;
	}
	if (ImGui::CollapsingHeader("OpenStar"))
	{
		const char* items[] = { "1", "2", "3", "4" , "5" , "6", "7" };
		//コンボボックス格納変数
		static int* openStar = 0;
		openStar = m_MouseCursor->GetOpenStar();
		int current = *openStar - 1;
		if (ImGui::Combo("OPEN_STAR", &current, items, IM_ARRAYSIZE(items)))
		{
			*openStar = current + 1;
			switch (current)
			{
			case 0:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR1, 0);
				break;
			case 1:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR2, 0);
				break;
			case 2:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR3, 0);
				break;
			case 3:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR4, 0);
				break;
			case 4:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR5, 0);
				break;
			case 5:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR6, 0);
				break;
			case 6:
				m_MouseCursor->ChangeTexture(ETextureType::STAR_DOOR7, 0);
				break;
			}
		}
	}
}

void CImGuiEditor::EditAcornImage()
{
	std::vector<CAcorn*> acorns;
	std::weak_ptr<CScene> scene = GetScene();
	if (!scene.expired())
	{
		 scene.lock()->GetGameObjects<CAcorn>(ESceneObjectType::BILLBOARD, acorns);
	}
	EditObjectImage(EEditorCreateObjectType::ACORN, ETextureType::IMGUI_ICON_ACORN, "Acorn", static_cast<int>(acorns.size()));
}

void CImGuiEditor::EditHeartImage()
{
	std::vector<CHeart*> hearts;
	std::weak_ptr<CScene> scene = GetScene();
	if (!scene.expired())
	{
		 scene.lock()->GetGameObjects<CHeart>(ESceneObjectType::NOMESH_OBJECT, hearts);
	}
	EditObjectImage(EEditorCreateObjectType::HEART, ETextureType::UI_HEART, "Heart", static_cast<int>(hearts.size()));
}

void CImGuiEditor::EditStarImage()
{
	std::vector<CStar*> stars;
	std::weak_ptr<CScene> scene = GetScene();
	if (!scene.expired())
	{
		scene.lock()->GetGameObjects<CStar>(ESceneObjectType::NOMESH_OBJECT, stars);
	}
	EditObjectImage(EEditorCreateObjectType::STAR, ETextureType::UI_STAR, "Star", static_cast<int>(stars.size()));
}

void CImGuiEditor::EditStarDoorImage()
{
	std::vector<CStarDoor*> stars;
	std::weak_ptr<CScene> scene = GetScene();
	if (!scene.expired())
	{
		scene.lock()->GetGameObjects<CStarDoor>(ESceneObjectType::MESH_OBJECT, stars);
	}
	EditObjectImage(EEditorCreateObjectType::STAR_DOOR, ETextureType::IMGUI_ICON_STAR_DOOR, "StarDoor", static_cast<int>(stars.size()));
}

void CImGuiEditor::EditFistMapImage()
{
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::TILE_BOX, ETextureType::IMGUI_ICON_TILEBLOCK, "TileBlock", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::WOOD_BRIDGE, ETextureType::IMGUI_ICON_WOODBRIDGE, "WoodBridge", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STONE_TREE, ETextureType::IMGUI_ICON_STONETREE, "StoneTree", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STONE_BOX, ETextureType::IMGUI_ICON_STONEBLOCK, "StoneBlock", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STUMP, ETextureType::IMGUI_ICON_STUMP, "Kirikabu", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::GRASS_WOOD, ETextureType::IMGUI_ICON_GRASSWOOD, "GrassWood", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::GRASS_STONE, ETextureType::IMGUI_ICON_GRASSSTONE, "GrassStone", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::GRASS_PILLAR, ETextureType::IMGUI_ICON_GRASSPILLAR, "GrassPillar", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::THORN_BOX, ETextureType::IMGUI_ICON_THORNBOX, "ThornBox", Vector3(10, 10, 10));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::TRIANGLE, ETextureType::IMGUI_ICON_TRIANGLE, "Triangle", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::TRIANGLE2, ETextureType::IMGUI_ICON_TRIANGLE2, "Triangle2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::START_TABLE, ETextureType::IMGUI_ICON_START_TABLE, "StartTable", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::ROCK, ETextureType::IMGUI_ICON_ROCK, "Rock", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::ROCK2, ETextureType::IMGUI_ICON_ROCK2, "Rock2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::ROCK3, ETextureType::IMGUI_ICON_ROCK3, "Rock3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::ROCK4, ETextureType::IMGUI_ICON_ROCK4, "Rock4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::ROCK5, ETextureType::IMGUI_ICON_ROCK5, "Rock5", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::TREE, ETextureType::IMGUI_ICON_TREE, "Tree", Vector3(1, 1, 1));
}

void CImGuiEditor::EditSecondMapImage()
{
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WINDOWSOUTWARDS, ETextureType::IMGUI_ICON_WINDOWSOUTWARDS, "Outwards", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WINDOWSOUTWARDSFRAME, ETextureType::IMGUI_ICON_WINDOWSOUTWARDSFRAME, "OutwardsFrame", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WINDOWSSINGLEHIGH, ETextureType::IMGUI_ICON_WINDOWSSINGLEHIGH, "SingleHigh", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WINDOWSDOUBLEHIGH, ETextureType::IMGUI_ICON_WINDOWSDOUBLEHIGH, "DoubleHigh", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WINDOWSROUND, ETextureType::IMGUI_ICON_WINDOWSROUND, "Round", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WINDOWSSINGLEHIGHBOARDS, ETextureType::IMGUI_ICON_WINDOWSSINGLEHIGHBOARDS, "SingleHighBoards", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_DOCK, ETextureType::IMGUI_ICON_DOCK, "Dock", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_DOOR1, ETextureType::IMGUI_ICON_DOOR1, "Door1", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_DOOR2STEP, ETextureType::IMGUI_ICON_DOOR2STEP, "Door2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_DOOR3, ETextureType::IMGUI_ICON_DOOR3, "Door3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_DOOR4, ETextureType::IMGUI_ICON_DOOR4, "Door4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_HOUSE1, ETextureType::IMGUI_ICON_HOUSE1, "House1", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_HOUSE2, ETextureType::IMGUI_ICON_HOUSE2, "House2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_HOUSE3, ETextureType::IMGUI_ICON_HOUSE3, "House3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_HOUSE4, ETextureType::IMGUI_ICON_HOUSE4, "House4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ONEHOUSE1, ETextureType::IMGUI_ICON_ONEHOUSE1, "OneHouse1", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ONEHOUSE2, ETextureType::IMGUI_ICON_ONEHOUSE2, "OneHouse2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ONEHOUSE3, ETextureType::IMGUI_ICON_ONEHOUSE3, "OneHouse3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ONEHOUSE4, ETextureType::IMGUI_ICON_ONEHOUSE4, "OneHouse4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_BALCONY, ETextureType::IMGUI_ICON_BALCONY, "Balcony", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_BALCONY2, ETextureType::IMGUI_ICON_BALCONY2, "Balcony2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_CHIMNEY, ETextureType::IMGUI_ICON_CHIMNEY , "Chimney", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_CHIMNEY2, ETextureType::IMGUI_ICON_CHIMNEY2, "Chimney2", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROOFFRAME, ETextureType::IMGUI_ICON_ROOFFRAME, "RoofFrame", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROOFFRAME2, ETextureType::IMGUI_ICON_ROOFFRAME2, "RoofFrame2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROOFFRAME3, ETextureType::IMGUI_ICON_ROOFFRAME3, "RoofFrame3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROOFFRAME4, ETextureType::IMGUI_ICON_ROOFFRAME4, "RoofFrame4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROOFFRAME5, ETextureType::IMGUI_ICON_ROOFFRAME5, "RoofFrame5", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROOFFRAME6, ETextureType::IMGUI_ICON_ROOFFRAME6, "RoofFrame6", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_STAIRWIDE, ETextureType::IMGUI_ICON_STAIRWIDE, "StairWide", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_STAIRWIDE2, ETextureType::IMGUI_ICON_STAIRWIDE2, "StairWide2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_VARIANT, ETextureType::IMGUI_ICON_VARIANT, "Variant", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_COLUMN, ETextureType::IMGUI_ICON_COLUMN, "Column", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ARCBIG, ETextureType::IMGUI_ICON_ARCBIG, "ArcBig", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ARCCOMER, ETextureType::IMGUI_ICON_ARCCOMER, "ArcComer", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ARCCROSS, ETextureType::IMGUI_ICON_ARCCROSS, "ArcCross", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ARCTRIPLE, ETextureType::IMGUI_ICON_ARCTRIPLE, "ArcTriple", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_BARREL, ETextureType::IMGUI_ICON_BARREL, "Barrel", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_COFFIN, ETextureType::IMGUI_ICON_COFFIN, "Coffin", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_COFFINCOVER, ETextureType::IMGUI_ICON_COFFINCOVER, "CoffinCover", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PEREKR, ETextureType::IMGUI_ICON_PEREKR, "Perekr", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODBOX, ETextureType::IMGUI_ICON_WOODBOX, "WoodBox", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_DOLLY , ETextureType::IMGUI_ICON_DOLLY, "Dolly", Vector3(1, 1, 1));
	
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN, ETextureType::IMGUI_ICON_WOODEN, "Wooden", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN2, ETextureType::IMGUI_ICON_WOODEN2, "Wooden2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN3, ETextureType::IMGUI_ICON_WOODEN3, "Wooden3", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN4, ETextureType::IMGUI_ICON_WOODEN4, "Wooden4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN5, ETextureType::IMGUI_ICON_WOODEN5, "Wooden5", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN6, ETextureType::IMGUI_ICON_WOODEN6, "Wooden6", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WOODEN7, ETextureType::IMGUI_ICON_WOODEN7, "Wooden7", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_CHEST2, ETextureType::IMGUI_ICON_CHEST2, "Chest2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_CHESTTOP2, ETextureType::IMGUI_ICON_CHESTTOP2, "ChestTop2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_ROCKS, ETextureType::IMGUI_ICON_ROCKS, "Rocks", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_STALAGS, ETextureType::IMGUI_ICON_STALAGS, "Stalags", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_COINS, ETextureType::IMGUI_ICON_COINS, "Coins", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_RAIL, ETextureType::IMGUI_ICON_RAIL, "Rail", Vector3(1, 1, 1));

}

void CImGuiEditor::EditSecondMapPlaneImage()
{
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PLANE_CLAIMEDWALL2C, ETextureType::IMGUI_ICON_CLAIMEDWALL2C, "Plane_ClaimedWall2C", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PLANE_CLAIMEDE, ETextureType::IMGUI_ICON_CLAIMEDE, "Plane_Claimede", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PLANE_DIRT_DARKE, ETextureType::IMGUI_ICON_DIRTDARKE, "Plane_DirtDarke", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PLANE_FLOOR, ETextureType::IMGUI_ICON_FLOOR, "Plane_Floor", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PLANE_STONE, ETextureType::IMGUI_ICON_STONE, "Plane_Stone", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_PLANE_TOP, ETextureType::IMGUI_ICON_TOP, "Plane_Top", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_CLAIMEDE, ETextureType::IMGUI_ICON_CLAIMEDE, "Wall_Claimede", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_CLAIMEDWALL2C, ETextureType::IMGUI_ICON_CLAIMEDWALL2C, "Wall_ClaimedWall2c", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_DIRT_DARKE, ETextureType::IMGUI_ICON_DIRTDARKE, "Wall_DirtDarke", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_FLOOR, ETextureType::IMGUI_ICON_FLOOR, "Wall_Floor", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_STONE, ETextureType::IMGUI_ICON_STONE, "Wall_Stone", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_STONE2, ETextureType::IMGUI_ICON_WALL2_1, "Wall_Stone2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_STONE3, ETextureType::IMGUI_ICON_WALL2_1, "Wall_Stone3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_STONE4, ETextureType::IMGUI_ICON_WALL22, "Wall_Stone4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_STONE5, ETextureType::IMGUI_ICON_WALL22, "Wall_Stone5", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::SECOND_WALL_TOP, ETextureType::IMGUI_ICON_WALL22, "Wall_Top", Vector3(1, 1, 1));
}

void CImGuiEditor::EditStageSelectImage()
{
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_WALL, ETextureType::IMGUI_ICON_DAISY, "Wall_Select", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_PLANE, ETextureType::IMGUI_ICON_SELECT_PLANE, "Plane_Select", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_CHAIR, ETextureType::IMGUI_ICON_CHAIR, "Chair", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_CHEST, ETextureType::IMGUI_ICON_CHEST, "Chest", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_SMALLTABLE, ETextureType::IMGUI_ICON_SMALLTABLE, "SmallTable", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_TABLE, ETextureType::IMGUI_ICON_TABLE, "Table", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_GREEN_BOOK, ETextureType::IMGUI_ICON_GREENBOOK, "GreenBook", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_PURPLE_BOOK, ETextureType::IMGUI_ICON_PURPLEBOOK, "PurpleBook", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_YELLOW_BOOK, ETextureType::IMGUI_ICON_YELLOWBOOK, "YellowBook", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_RED_BOOK, ETextureType::IMGUI_ICON_REDBOOK, "RedBook", Vector3(1, 1, 1));

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_SECOND_STAGE_FRAME, ETextureType::IMGUI_ICON_STAGE_FRAME , "StageFrame", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_FIRST_STAGE_FRAME, ETextureType::IMGUI_ICON_STAGE_FRAME2, "StageFrame2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_TUTOLIAL_STAGE_FRAME, ETextureType::IMGUI_ICON_STAGE_FRAME3, "StageFrame3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::STAGESELECT_BOSS_FIGURE, ETextureType::IMGUI_ICON_BOSS_FIGURE, "BossFigure", Vector3(1, 1, 1));
}

void CImGuiEditor::EditPlaneImage()
{
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::PLANE_YELLOW, ETextureType::IMGUI_ICON_PLANE_YELLOW, "PlaneYellow", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::PLANE_YELLOW2, ETextureType::IMGUI_ICON_PLANE_YELLOW2, "PlaneYellow2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::PLANE_STONE, ETextureType::IMGUI_ICON_PLANE_STONE, "PlaneStone", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::PLANE_GREEN, ETextureType::IMGUI_ICON_PLANE_GREEN, "PlaneGreen", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::PLANE_GRASS, ETextureType::IMGUI_ICON_GRASSES, "PlaneGrass", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::PLANE_GRASS2, ETextureType::IMGUI_ICON_PLANE_GRASS2, "PlaneGrass2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::WALL_GRASS, ETextureType::IMGUI_ICON_WALL_GRASS, "WallGrassPlane", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::WALL_GRASS_PLANE, ETextureType::IMGUI_ICON_WALL_GRASS, "WallGrass", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::WALL_GRASS2, ETextureType::IMGUI_ICON_WALL_GRASS2, "WallGrass2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::WALL_WOOD, ETextureType::IMGUI_ICON_WALL_WOOD, "WallWood", Vector3(1, 1, 1));
}

void CImGuiEditor::EditNoMeshImage()
{
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::SIMPLE_GRASSBOX, ETextureType::IMGUI_ICON_SIMPLE_GRASSBOX, "_NoGrassBox", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::GRASS_CIRCLE, ETextureType::IMGUI_ICON_GRASS_CIRCLE, "NoGrassCircle", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::GRASS_BOX_YELLOW, ETextureType::IMGUI_ICON_GRASS_BOX_YELLOW, "NoGrassBoxYellow", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::BUSH, ETextureType::IMGUI_ICON_BUSH, "Bush", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::BUSH2, ETextureType::IMGUI_ICON_BUSH2, "Bush2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::BUSH3, ETextureType::IMGUI_ICON_BUSH3, "Bush3", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::BUSH4, ETextureType::IMGUI_ICON_BUSH4, "Bush4", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::BUSH5, ETextureType::IMGUI_ICON_BUSH5, "Bush5", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::FERN , ETextureType::IMGUI_ICON_FORN, "Fern", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::PLANT, ETextureType::IMGUI_ICON_PLANT, "Plant", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::GRASSES, ETextureType::IMGUI_ICON_GRASSES, "Grasses", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::FLOWER, ETextureType::IMGUI_ICON_FLOWER, "Flower", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::FLOWER2, ETextureType::IMGUI_ICON_FLOWER2, "Flower2", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::NO_MESH, EStaticMeshType::FLOWER3, ETextureType::IMGUI_ICON_FLOWER3, "Flower3", Vector3(1, 1, 1));
}

void CImGuiEditor::EditCollisionImage()
{
	if (!m_MouseCursor)
	{
		return;
	}
	const EEditorCreateObjectType createType = m_MouseCursor->GetCreateObjectType();

	EditObjectImage(EEditorCreateObjectType::NO_CLIFFBOX, ETextureType::IMGUI_ICON_NO_CLIFFBOX, "NoCliffBox");

	EditObjectImage(EEditorCreateObjectType::CAMERA_HEIGHTLOCK_BOX, ETextureType::IMGUI_ICON_NO_CLIFFBOX, "CameraHeightLockBox");

	EditObjectImage(EEditorCreateObjectType::TUTORIAL_BOX, ETextureType::IMGUI_ICON_NO_CLIFFBOX, "TutorialBox");

	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::BLOCK_COLLISION, ETextureType::IMGUI_ICON_BLOCK_COLLISION, "BlockCollision", Vector3(1, 1, 1));
}

void CImGuiEditor::EditPlyerStartImage()
{
	std::vector<CPlayerStart*> starts;
	std::weak_ptr<CScene> scene = GetScene();
	if (!scene.expired())
	{
		scene.lock()->GetGameObjects<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT, starts);
	}
	EditObjectImage(EEditorCreateObjectType::PLAYER_START, ETextureType::IMGUI_ICON_PLAYER_START, "PlayerStart", static_cast<int>(starts.size()));
}

void CImGuiEditor::EditEnemy()
{
	int numSlime = 0;
	int numMonkey = 0;
	int numThorn = 0;
	std::vector<CEnemy*> enemys;
	std::weak_ptr<CScene> scene = GetScene();
	if (!scene.expired())
	{
		scene.lock()->GetGameEnemyObjects(ESceneObjectType::ENEMY, enemys);
	}
	for (CEnemy* enemy : enemys)
	{
		switch (enemy->GetEnemyType())
		{
		case EEnemyType::SLIME:
			numSlime++;
			break;
		case EEnemyType::MONKEY:
			numMonkey++;
			break;
		case EEnemyType::THORN_MAN:
			numThorn++;
			break;
		}
	}
	EditObjectImage(EEditorCreateObjectType::SLIME, ETextureType::IMGUI_ICON_SLIME, "Slime", numSlime);
	EditObjectImage(EEditorCreateObjectType::MONKEY, ETextureType::IMGUI_ICON_MONKEY, "Monkey", numMonkey);
	EditObjectImage(EEditorCreateObjectType::THORNMAN, ETextureType::IMGUI_ICON_THORNMAN, "ThornMan", numThorn);
}

void CImGuiEditor::EditFirstBoss()
{
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::FIRST_BOSS_FIELD, ETextureType::IMGUI_ICON_FIRSTBOSS_FIELD, "FirstBossField", Vector3(1, 1, 1));
	EditCommonObjectImage(EEditorCreateObjectType::COMMON, EStaticMeshType::FIRST_BOSS_WALL, ETextureType::IMGUI_ICON_FIRSTBOSS_WALL, "FirstBossWall", Vector3(1, 1, 1));
}

void CImGuiEditor::EditOff()
{
	if (!m_MouseCursor)
	{
		return;
	}
	m_MouseCursor->EditOff();
}

void CImGuiEditor::PutAcornWindow()
{
	if (!m_MouseCursor)
	{
		return;
	}
	bool* circle = m_MouseCursor->IsAcornCircle();
	bool* line = m_MouseCursor->IsAcornLine();
	if (!circle || !line)
	{
		return;
	}

	if (ImGui::Checkbox("Line", line))
	{
		*circle = false;
	}
	ImGui::Spacing();
	if (ImGui::Checkbox("Circle", circle))
	{
		*line = false;
	}
	ImGui::SliderInt("SetNumber", m_MouseCursor->GetAcornSetNumber(), 1, 20);
	if (*line)
	{
		Vector3* pos = m_MouseCursor->GetEndAcornPosition();
		ImGui::SliderFloat("EndPointX", &pos->x, -25.0f, 25.0f);
		ImGui::SliderFloat("EndPointY", &pos->y, -25.0f, 25.0f);
		ImGui::SliderFloat("EndPointZ", &pos->z, -25.0f, 25.0f);
	}
	if (*circle)
	{
		ImGui::SliderFloat("CirclesAngle", m_MouseCursor->GetCirclesAngle(), 0.0f, 10.0f);
		ImGui::SliderFloat("CirclesRadius", m_MouseCursor->GetCirclesRadius(), 1.0f, 16.0f);
	}
}

void CImGuiEditor::SetSaveFile(const ESceneType Type)
{ 
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	m_SceneName = system->GetSceneName(Type);

	//現在のシーンのセーブデータをSAVE_FILE_NUM分用意する
	//また、ImGuiが文字列はconst char* しか受け付けていないため文字列保存を行うようにstringの配列にもデータを保存する
	//セーブデータの規則は2番目以降は番号を付ける形にする 例 test.txt test2.tex ...等
	std::string fileName = system->GetLoadSaveFile(Type);
	const size_t pos = fileName.find_last_of('.');
	const std::string nameOnly = fileName.substr(0, pos);
	m_SaveFileStrings[0] = fileName;
	m_SaveFiles[0] = m_SaveFileStrings[0].c_str();

	for (int i = 1; i < SAVE_FILE_NUM; i++)
	{
		std::string saveFileName = nameOnly + std::to_string(i + 1) + ".txt";
		m_SaveFileStrings[i] = saveFileName;
		m_SaveFiles[i] = m_SaveFileStrings[i].c_str();
	}
}