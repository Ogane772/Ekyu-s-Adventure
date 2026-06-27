#include "MouseCursor.h"
#include "../Model/StaticMeshType.h"
#include "../Model/StaticMesh.h"
#include "../Input/InputInterface.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Camera/Camera.h"
#include "../Line/Line.h"
#include "../ImGui/ImGuiObjectIcon.h"
#include "../ImGui/DebugImGui.h"
#include "../Collision/CollisionTag.h"
#include "../Collision/Collision.h"
#include "../Collision/Box.h"
#include "../Object/NoCliffBox.h"
#include "../Object/CameraHeightLockBox.h"
#include "../Object/CommonObject.h"
#include "../Object/TutorialBox.h"
#include "../Renderer/Renderer.h"
#include "../System/SystemContext.h"
#include "../../Game/Character/Enemy/Enemy.h"
#include "../../Game/Character/Enemy/Mob/MobEnemy.h"
#include "../../Game/Object/Door/StarDoor.h"
#include "../../Game/Object/PlayerStart/PlayerStart.h"
#include "../../Game/Object/ItemObject/Heart.h"
#include "../../Game/Object/ItemObject/Star.h"
#include "../../Game/Object/ItemObject/Acorn.h"
#include "../../Game/Effect/EffectTypes.h"

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

namespace
{
	const Vector3 DEFAULT_SCALL = Vector3(0.3f, 0.3f, 0.3f);
}

// vectorからindex番目の要素を削除する
template<typename T>
void remove(std::vector<T>& vector, UINT Index)
{
	vector.erase(vector.begin() + Index);
}

void CMouseCursor::Init()
{
	m_IsNoCullingCheck = true;
	m_Position = Vector3(0.0f, -10.0f, 0.0f);
	m_Scale = DEFAULT_SCALL;
	m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
	m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::SPHERE);
	m_MeshType = EStaticMeshType::SPHERE;
	m_Line = std::make_unique<CLine>(&m_Position, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_CreateObjectType = EEditorCreateObjectType::NONE;
	m_SelectObjectType = EEditorSelectObjectType::NONE;
	m_Camera = GetCamera();
}

void CMouseCursor::UnInit()
{
	if (m_SelectObject)
	{
		m_SelectObject = nullptr;
	}
}

void CMouseCursor::ObjectLineCheck(Vector3& Out, const Vector2& ScreenPoint, const Vector2& ScreenSize, const XMMATRIX& View, const XMMATRIX& Projection)
{
	if (!m_Camera || !m_Line)
	{
		return;
	}
	Vector3 farPosition;
	CalcScreenToWorld(farPosition, ScreenPoint, 1.0f, ScreenSize, View, Projection);

	RayCastInfo m_DownCastInfo;
	if (m_Line->LineHitCheck(ECheckCollisionType::EDITOR, m_Camera->GetPosition(), farPosition, &m_DownCastInfo))
	{
		Out = m_DownCastInfo.Point;
	}
}

void CMouseCursor::CalcScreenToWorld(Vector3& Out, const Vector2& ScreenPoint, const float ProjectionZ, const Vector2& ScreenSize, const XMMATRIX& View, const XMMATRIX& Projection) 
{
	// 各行列の逆行列を算出
	XMMATRIX invView, invPrj, VP, invViewport;

	invView = XMMatrixInverse(nullptr, View);
	invPrj = XMMatrixInverse(nullptr, Projection);

	VP = XMMatrixIdentity();
	XMFLOAT4X4 XMFLOAT_VP;
	XMStoreFloat4x4(&XMFLOAT_VP, XMMatrixTranspose(VP));
	XMFLOAT_VP._11 = ScreenSize.x / 2.0f; XMFLOAT_VP._22 = -ScreenSize.y / 2.0f;
	XMFLOAT_VP._41 = ScreenSize.x / 2.0f; XMFLOAT_VP._42 = ScreenSize.y / 2.0f;

	VP = XMLoadFloat4x4(&XMFLOAT_VP);

	invViewport = XMMatrixInverse(nullptr, VP);

	// 逆変換
	XMMATRIX tmp = invViewport * invPrj * invView;
	Out = Vector3::TransformCoord(Vector3(ScreenPoint.x, ScreenPoint.y, ProjectionZ), tmp);
}

void CMouseCursor::Update(float DeltaTime)
{
	if (!IsEditMode())
	{
		return;
	}
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}

	const float screenWidth = system->GetCurrentScreenWidth();
	const float screenHeight = system->GetCurrentScreenHeight();

	POINT pt;
	GetCursorPos(&pt);//カーソル位置の絶対座標を取得
	ScreenToClient(system->GetWindow(), &pt);//カーソル絶対座標から、そのウィンドウ内の座標に補正
	const Vector2 point = Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	ObjectLineCheck(m_Position, point, Vector2(screenWidth, screenHeight), m_Camera->GetInvViewMatrix(), m_Camera->GetProjectionMatrix());
;
	if (m_EffectCreate)
	{
		if (m_EffectId != UINT_MAX)
		{
			UpdateEffectPosition(m_EffectId, m_Position);
		}
	}

	//ImGuiウィンドウ外にいるか調べる
	CImGui* imGui = nullptr;
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	imGui = scene.lock()->GetGameObject<CImGui>(ESceneObjectType::UI);
	if (imGui)
	{
		m_WantCaptureMouse = imGui->WantCaptureMouse();
	}
	//ドラッグドロップによるオブジェクト配置処理
	ObjectInstallation();
	//マップに配置されているオブジェクト取得
	MapObjectCapture();

	UpdateWorldMatrix();
}

void CMouseCursor::Draw()
{
	if (!IsEditMode())
	{
		return;
	}
	// マトリクス設定
	CRenderer::SetWorldMatrix(m_World);
	if (!m_EffectCreate)
	{
		if (m_StaticMesh)
		{
			m_StaticMesh->Draw(m_World);
		}
	}
	//ターゲットポイントドロー
	if (CEnemy* selectEnemy = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		selectEnemy->TargetPointDraw();
	}
	if (m_SingleCollisonDraw)
	{
		if (m_SelectObject)
		{
			m_SelectObject->SingleCollisonDraw();
		}
	}
}

void CMouseCursor::StartDragDrop(const EEditorCreateObjectType Type)
{
	//エディタでオブジェクトをドラッグドロップした時のオブジェクトを設定
	if (m_CreateObjectType == Type)
	{
		return;
	}

	m_CreateObjectType = Type;
	m_SelectObjectType = EEditorSelectObjectType::NONE;
	//ドラッグドロップ中に表示するものをセット
	switch (m_CreateObjectType)
	{
	case EEditorCreateObjectType::ACORN:
		m_EffectId = PlayEffect(EEffectType::ACORN, m_Position, 2.0f);
		m_EffectCreate = true;
		break;
	case EEditorCreateObjectType::HEART:
		m_MeshType = EStaticMeshType::HEART;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::HEART);
		m_Scale = Vector3(1.1f, 1.1f, 1.1f);
		break;
	case EEditorCreateObjectType::STAR:
		m_MeshType = EStaticMeshType::STAR;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::STAR);
		m_Scale = Vector3(1.0f, 1.0f, 1.0f);
		break;
	case EEditorCreateObjectType::STAR_DOOR:
		m_MeshType = EStaticMeshType::STAR_DOOR;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::STAR_DOOR);
		m_Scale = Vector3(1.5f, 1.5f, 1.5f);
		break;
	case EEditorCreateObjectType::COMMON:
	case EEditorCreateObjectType::NO_MESH:
		//このタイプの場合はエディタからモデルタイプが設定される
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(m_MeshType);
		m_Scale = Vector3(1.0f, 1.0f, 1.0f);
		break;
	case EEditorCreateObjectType::NO_CLIFFBOX:
	case EEditorCreateObjectType::CAMERA_HEIGHTLOCK_BOX:
	case EEditorCreateObjectType::TUTORIAL_BOX:
		m_MeshType = EStaticMeshType::BOX;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::BOX);
		m_Scale = Vector3(1.0f, 1.0f, 1.0f);
		break;
	case EEditorCreateObjectType::PLAYER_START:
		m_MeshType = EStaticMeshType::PLAYER_START;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::PLAYER_START);
		m_Scale = Vector3(1.0f, 1.0f, 1.0f);
		break;;
	case EEditorCreateObjectType::SLIME:
		m_MeshType = EStaticMeshType::SLIME;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::SLIME);
		m_Scale = Vector3(1.0f, 1.0f, 1.0f);
		break;
	case EEditorCreateObjectType::MONKEY:
		m_MeshType = EStaticMeshType::MONKEY;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::MONKEY);
		m_Scale = Vector3(1.0f, 1.0f, 1.0f);
		break;
	case EEditorCreateObjectType::THORNMAN:
		m_MeshType = EStaticMeshType::THORNMAN;
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::THORNMAN);
		m_Scale = Vector3(1.6f, 1.6f, 1.6f);
		break;
	}
}

void CMouseCursor::ClearSelectObject()
{
	if (m_EffectId != UINT_MAX)
	{
		StopEffect(m_EffectId);
		m_EffectCreate = false;
		m_EffectId = UINT_MAX;
	}
	if (m_MeshType != EStaticMeshType::SPHERE)
	{
		m_StaticMesh.reset();
		m_StaticMesh = std::make_unique<CStaticMesh>(EStaticMeshType::SPHERE);
		m_Scale = DEFAULT_SCALL;
	}
	m_CreateObjectType = EEditorCreateObjectType::NONE;
}

void CMouseCursor::EditOff()
{
	m_CreateObjectType = EEditorCreateObjectType::NONE;
	m_SelectObjectType = EEditorSelectObjectType::NONE;
}

void CMouseCursor::DeleteSelectObject()
{
	if (!m_SelectObject)
	{
		return;
	}
	const int number = m_SelectObject->GetOriginalNumber();;
	
	EStaticMeshType meshType = EStaticMeshType::INDEX_MAX;
	int objectNumber = 0;
	//一致する番号を持つオブジェクトをリストから削除する
	for (size_t i = 0; i < m_MapObjectList.size(); i++)
	{
		if (m_MapObjectList[i].Number == number)
		{
			meshType = m_MapObjectList[i].MeshType;
			objectNumber = m_MapObjectList[i].ObjectNumber;
			m_MapObjectList[i].Object = nullptr;
			remove(m_MapObjectList, static_cast<UINT>(i));
			break;
		}
	}

	//同じオブジェクトが複数あったら番号を1つ減らす
	for (size_t i = 0; i < m_MapObjectList.size(); i++)
	{
		if (m_MapObjectList[i].MeshType == meshType)
		{
			if (m_MapObjectList[i].ObjectNumber > objectNumber)
			{
				m_MapObjectList[i].ObjectNumber--;
			}
		}
	}
	m_SelectObjectType = EEditorSelectObjectType::NONE;
	m_SelectObject->SetDestroyEdit();
	m_SelectObject = nullptr;
}

void CMouseCursor::SetMapObjectNull(const int Number)
{
	if (m_MapObjectList.size() <= Number)
	{
		return;
	}
	MapObject& object = m_MapObjectList[Number];
	if (!object.Object)
	{
		return;
	}
	if (object.Object->GetOriginalNumber() != Number)
	{
		return;
	}
	object.Object = nullptr;
}

void CMouseCursor::CreateObject(const EEditorCreateObjectType Type, const Vector3& Position)
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}

	switch (Type)
	{
		case EEditorCreateObjectType::ACORN:
			AcornCreate();
			break;
		case EEditorCreateObjectType::HEART:
		{
			CHeart* heart = new CHeart(Position, false);
			if (!heart)
			{
				return;
			}
			heart->InitBodyCollisionCheck();
			scene.lock()->AddGameObject<CHeart>(ESceneObjectType::NOMESH_OBJECT, heart);
			m_SelectObject = heart;
			PushMapObject(heart, "Heart", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::STAR:
		{
			CStar* star = new CStar(Position);
			if (!star)
			{
				return;
			}
			star->InitBodyCollisionCheck();
			scene.lock()->AddGameObject<CStar>(ESceneObjectType::NOMESH_OBJECT, star);
			m_SelectObject = star;
			PushMapObject(star, "Star", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::STAR_DOOR:
		{
			CStarDoor* starDoor = new CStarDoor(Position, Vector3(0.0f, 0.0f, 0.0f), 2);
			if (!starDoor)
			{
				return;
			}
			scene.lock()->AddGameObject<CStarDoor>(ESceneObjectType::MESH_OBJECT, starDoor);
			m_SelectObject = starDoor;
			PushMapObject(starDoor, "StarDoor", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::COMMON:
		{
			CCommonObject *common = new CCommonObject();
			if (!common)
			{
				return;
			}
			common->Init(m_MeshType, Position, m_Scale, Vector3(0.0f, 0.0f, 0.0f));
			common->SetPlayerCollisionTag(ECollisionTagName::ALL_COLISION);
			common->SetObjectCollisionTag(ECollisionTagName::OBJECT_COLISION);
			common->CreateAutoCullingSphere();
			common->SetCameraCollisionTag(ECollisionTagCameraName::NONE);
			scene.lock()->AddGameObject<CCommonObject>(ESceneObjectType::MESH_OBJECT, common);
			m_SelectObject = common;
			m_SelectObjectType = EEditorSelectObjectType::COMMON;
			PushMapObject(common, CStaticMesh::GetModelFileName(m_MeshType), m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::NO_MESH:
		{
			CCommonObject *common = new CCommonObject();
			if (!common)
			{
				return;
			}
			common->Init(m_MeshType, Position, m_Scale, Vector3(0.0f, 0.0f, 0.0f));
			common->SetPlayerCollisionTag(ECollisionTagName::NO_COLLISION_HIT);
			common->SetObjectCollisionTag(ECollisionTagName::NO_COLLISION_HIT);
			common->CreateAutoCullingSphere();
			common->SetCameraCollisionTag(ECollisionTagCameraName::NONE);
			scene.lock()->AddGameObject<CCommonObject>(ESceneObjectType::MESH_OBJECT, common);
			m_SelectObject = common;
			PushMapObject(common, CStaticMesh::GetModelFileName(m_MeshType), m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::NO_CLIFFBOX:
		{
			CNoCliffBox* noCliffBox = new CNoCliffBox(Position, Vector3(2, 2, 2), Vector3(0.0f, 0.0f, 0.0f));
			if (!noCliffBox)
			{
				return;
			}
			scene.lock()->AddGameObject<CNoCliffBox>(ESceneObjectType::NOMESH_OBJECT, noCliffBox);
			m_SelectObject = noCliffBox;
			PushMapObject(noCliffBox, "NoCliffBox", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::CAMERA_HEIGHTLOCK_BOX:
		{
			CCameraHeightLockBox* cameraBox = new CCameraHeightLockBox(Position, Vector3(2, 2, 2), Vector3(0.0f, 0.0f, 0.0f));
			if (!cameraBox)
			{
				return;
			}
			scene.lock()->AddGameObject<CCameraHeightLockBox>(ESceneObjectType::NOMESH_OBJECT, cameraBox);
			m_SelectObject = cameraBox;
			PushMapObject(cameraBox, "CameraBox", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::TUTORIAL_BOX:
		{
			CTutorialBox* tutorialBox = new CTutorialBox(Position, Vector3(2, 2, 2), Vector3(0.0f, 0.0f, 0.0f));
			if (!tutorialBox)
			{
				return;
			}
			scene.lock()->AddGameObject<CTutorialBox>(ESceneObjectType::NOMESH_OBJECT, tutorialBox);
			m_SelectObject = tutorialBox;
			PushMapObject(tutorialBox, "TutorialBox", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::PLAYER_START:
		{
			CPlayerStart* start = new CPlayerStart(Position, Vector3(0, 0, 0));
			if (!start)
			{
				return;
			}
			scene.lock()->AddGameObject<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT, start);
			m_SelectObject = start;
			PushMapObject(start, "PlayerStart", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::SLIME:
		{
			CMobEnemy* enemy = nullptr;
			enemy = dynamic_cast<CMobEnemy*>(CEnemy::Create(EEnemyType::SLIME, Position));
			if (!enemy)
			{
				return;
			}
			enemy->SetTargetPoint(1, Vector3(0, 0, 0));
			enemy->SetRespawn(true, 600);
			scene.lock()->AddGameObject<CEnemy>(ESceneObjectType::ENEMY, enemy);
			m_SelectObject = enemy;
			PushMapObject(enemy, "Slime", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::MONKEY:
		{
			CMobEnemy* enemy = nullptr;
			enemy = dynamic_cast<CMobEnemy*>(CEnemy::Create(EEnemyType::MONKEY, Position));
			if (!enemy)
			{
				return;
			}
			enemy->SetRespawn(true, 600);
			scene.lock()->AddGameObject<CEnemy>(ESceneObjectType::ENEMY, enemy);
			m_SelectObject = enemy;
			PushMapObject(enemy, "Monkey", m_MeshType, m_SelectObjectType);
			break;
		}
		case EEditorCreateObjectType::THORNMAN:
		{
			CMobEnemy* enemy = nullptr;
			enemy = dynamic_cast<CMobEnemy*>(CEnemy::Create(EEnemyType::THORN_MAN, Position));
			if (!enemy)
			{
				return;
			}
			enemy->SetTargetPoint(1, Vector3(0, 0, 0));
			enemy->SetRespawn(true, 600);
			scene.lock()->AddGameObject<CEnemy>(ESceneObjectType::ENEMY, enemy);
			m_SelectObject = enemy;
			PushMapObject(enemy, "ThornMan", m_MeshType, m_SelectObjectType);
			break;
		}
	}
}

void CMouseCursor::ObjectInstallation()
{	
	//マップにドラッグドロップで配置時の処理
	if (m_WantCaptureMouse)
	{
		return;
	}
	
	if (m_CreateObjectType == EEditorCreateObjectType::NONE)
	{
		return;
	}

	//置いたオブジェクトを選択状態にする
	
	switch (m_CreateObjectType)
	{
	case EEditorCreateObjectType::ACORN:
		m_SelectObjectType = EEditorSelectObjectType::ACORN;
		break;
	case EEditorCreateObjectType::HEART:
		m_SelectObjectType = EEditorSelectObjectType::HEART;
		break;
	case EEditorCreateObjectType::STAR:
		m_SelectObjectType = EEditorSelectObjectType::STAR;
		break;
	case EEditorCreateObjectType::STAR_DOOR:
		m_SelectObjectType = EEditorSelectObjectType::STAR_DOOR;
		break;
	case EEditorCreateObjectType::COMMON:
	case EEditorCreateObjectType::NO_MESH:
		m_SelectObjectType = EEditorSelectObjectType::COMMON;
		break;
	case EEditorCreateObjectType::NO_CLIFFBOX:
		m_SelectObjectType = EEditorSelectObjectType::NO_CLIFFBOX;
		break;
	case EEditorCreateObjectType::CAMERA_HEIGHTLOCK_BOX :
		m_SelectObjectType = EEditorSelectObjectType::CAMERA_HEIGHTLOCK_BOX;
		break;
	case EEditorCreateObjectType::TUTORIAL_BOX:
		m_SelectObjectType = EEditorSelectObjectType::TUTORIAL_BOX;
		break;
	case EEditorCreateObjectType::PLAYER_START:
		m_SelectObjectType = EEditorSelectObjectType::PLAYER_START;
		break;
	case EEditorCreateObjectType::SLIME:
		m_SelectObjectType = EEditorSelectObjectType::SLIME;
		break;
	case EEditorCreateObjectType::MONKEY:
		m_SelectObjectType = EEditorSelectObjectType::MONKEY;
		break;
	case EEditorCreateObjectType::THORNMAN:
		m_SelectObjectType = EEditorSelectObjectType::THORNMAN;
		break;
	}
	//オブジェクト生成
	CreateObject(m_CreateObjectType, m_Position);
	//マウスが保持していたオブジェクトを開放
 	ClearSelectObject();
}

void CMouseCursor::MapObjectCapture()
{//マップで選択したオブジェクトを取得
	if (m_WantCaptureMouse)
	{
		return;
	}
	if (!m_Line || !m_Camera)
	{
		return;
	}
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	if (!instance->IsImGuiCaptureObjectTrigger())//マウス　左ボタン押下
	{
		return;
	}
	if (m_CreateObjectType != EEditorCreateObjectType::NONE)
	{
		return;
	}
	//ドラッグドロップオブジェクトを何ももっていない時かつオブジェクトを選択したらそのオブジェクト情報を持つ
	//オブジェクトは以下の処理順毎にチェックされ、ヒットした段階で終了する
	const Vector3 cameraPosition = m_Camera->GetPosition();
	std::vector<CPlayerStart*> starts;
	scene.lock()->GetGameObjects<CPlayerStart>(ESceneObjectType::NOMESH_OBJECT, starts);
	for (CPlayerStart* start : starts)
	{
		if (!start)
		{
			continue;
		}
		if (CCollison::CollisionCheck(cameraPosition, m_Position, start->GetSphere()))
		{
			m_SelectObject = start;
			m_SelectObjectType = EEditorSelectObjectType::PLAYER_START;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CAcorn*> acorns;
	scene.lock()->GetGameObjects<CAcorn>(ESceneObjectType::BILLBOARD, acorns);
	for (CAcorn* acorn : acorns)
	{
		if (!acorn)
		{
			continue;
		}
		if (CCollison::CollisionCheck(cameraPosition, m_Position, acorn->GetSphere()))
		{
			m_SelectObject = acorn;
			m_SelectObjectType = EEditorSelectObjectType::ACORN;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CHeart*> hearts;
	scene.lock()->GetGameObjects<CHeart>(ESceneObjectType::NOMESH_OBJECT, hearts);
	for (CHeart* heart : hearts)
	{
		if (!heart)
		{
			continue;
		}
		if (CCollison::CollisionCheck(cameraPosition, m_Position, heart->GetSphere()))
		{
			m_SelectObject = heart;
			m_SelectObjectType = EEditorSelectObjectType::HEART;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CStar*> stars;
	scene.lock()->GetGameObjects<CStar>(ESceneObjectType::NOMESH_OBJECT, stars);
	for (CStar* star : stars)
	{
		if (!star)
		{
			continue;
		}
		if (CCollison::CollisionCheck(cameraPosition, m_Position, star->GetSphere()))
		{
			m_SelectObject = star;
			m_SelectObjectType = EEditorSelectObjectType::STAR;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CNoCliffBox*> noCliffs;
	scene.lock()->GetGameObjects<CNoCliffBox>(ESceneObjectType::NOMESH_OBJECT, noCliffs);
	for (CNoCliffBox* noCliff : noCliffs)
	{
		if (!noCliff)
		{
			continue;
		}
		if (CCollison::OBBVsLine(noCliff->GetCollisionBox(), cameraPosition, m_Position))
		{
			m_SelectObject = noCliff;
			m_SelectObjectType = EEditorSelectObjectType::NO_CLIFFBOX;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CCameraHeightLockBox*> cameraBoxs;
	scene.lock()->GetGameObjects<CCameraHeightLockBox>(ESceneObjectType::NOMESH_OBJECT, cameraBoxs);
	for (CCameraHeightLockBox* cameraBox : cameraBoxs)
	{
		if (!cameraBox)
		{
			continue;
		}
		if (CCollison::OBBVsLine(cameraBox->GetCollisionBox(), cameraPosition, m_Position))
		{
			m_SelectObject = cameraBox;
			m_SelectObjectType = EEditorSelectObjectType::CAMERA_HEIGHTLOCK_BOX;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CTutorialBox*> tutorialBoxs;
	scene.lock()->GetGameObjects<CTutorialBox>(ESceneObjectType::NOMESH_OBJECT, tutorialBoxs);
	for (CTutorialBox* tutorialBox : tutorialBoxs)
	{
		if (!tutorialBox)
		{
			continue;
		}
		if (CCollison::OBBVsLine(tutorialBox->GetCollisionBox(), cameraPosition, m_Position))
		{
			m_SelectObject = tutorialBox;
			m_SelectObjectType = EEditorSelectObjectType::TUTORIAL_BOX;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CEnemy*> enemys;
	scene.lock()->GetGameEnemyObjects(ESceneObjectType::ENEMY, enemys);
	auto setEnemy = [=, this](CEnemy* Enemy)
	{
		m_SelectObject = Enemy;
		m_CreateNumber = m_SelectObject->GetOriginalNumber();
		switch (Enemy->GetEnemyType())
		{
		case EEnemyType::SLIME:
			m_SelectObjectType = EEditorSelectObjectType::SLIME;
			break;
		case EEnemyType::MONKEY:
			m_SelectObjectType = EEditorSelectObjectType::MONKEY;
			break;
		case EEnemyType::THORN_MAN:
			m_SelectObjectType = EEditorSelectObjectType::THORNMAN;
			break;
		}
		return;
	};
	for (CEnemy* enemy : enemys)
	{
		if (!enemy)
		{
			continue;
		}
		if (enemy->GetSphere())
		{
			if (CCollison::CollisionCheck(cameraPosition, m_Position, enemy->GetSphere()))
			{
				setEnemy(enemy);
			}
		}
		else if (enemy->GetCapsule())
		{
			if (CCollison::CollisionCheck(cameraPosition, m_Position, enemy->GetCapsule()))
			{
				setEnemy(enemy);
			}
		}
	}
	RayCastInfo castInfo;
	std::vector<CStarDoor*> starDoors;
	scene.lock()->GetGameObjects<CStarDoor>(ESceneObjectType::MESH_OBJECT, starDoors);
	for (CStarDoor* starDoor : starDoors)
	{
		if (!starDoor)
		{
			continue;
		}
		if (m_Line->LineHitCheck(cameraPosition, m_Position, &castInfo, starDoor->GetStarOpenDoor()))
		{
			m_SelectObject = starDoor;
			m_SelectObjectType = EEditorSelectObjectType::STAR_DOOR;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			return;
		}
	}
	std::vector<CCommonObject*> commons;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, commons);
	for (CCommonObject* common : commons)
	{
		if (!common)
		{
			continue;
		}
		if (common->GetObjectReference())
		{
			return;
		}
		if (m_Line->LineHitCheck(cameraPosition, m_Position, &castInfo, common))
		{
			m_SelectObject = common;
			m_CreateNumber = m_SelectObject->GetOriginalNumber();
			m_SelectObjectType = EEditorSelectObjectType::COMMON;
			m_MeshType = common->GetMeshType();
			return;
		}
	}
}

XMMATRIX CMouseCursor::GetWorld()
{
	if (m_SelectObjectType == EEditorSelectObjectType::ACORN)
	{
		const Vector3 position = m_SelectObject->GetPosition();
		m_World = XMMatrixScaling(1, 1, 1);
		m_World *= XMMatrixTranslation(position.x, position.y, position.z);
		return m_World;
	}
	else
	{
		const Vector3 positon = m_SelectObject->GetPosition();
		const Vector3 rotation = m_SelectObject->GetRotation();
		const Vector3 scale = m_SelectObject->GetScale();
		m_World = XMMatrixScaling(scale.x, scale.y, scale.z);
		m_World *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
		m_World *= XMMatrixTranslation(positon.x, positon.y, positon.z);
		return m_World;
	}
}

void CMouseCursor::SetMouseObject(CGameObject* Object, const EEditorSelectObjectType SelectType, const EStaticMeshType MeshType)
{
	if (!Object)
	{
		return;
	}
	m_SelectObject = Object;
	m_CreateNumber = m_SelectObject->GetOriginalNumber();
	m_SelectObjectType = SelectType;
	m_MeshType = MeshType;
}

void CMouseCursor::SetScale(const Vector3& SetScale)
{
	if (!m_SelectObject)
	{
		return;
	}
	m_SelectObject->SetScale(SetScale);
}

void CMouseCursor::SetRotation(const Vector3& SetRot)
{
	if (!m_SelectObject)
	{
		return;
	}
	m_SelectObject->SetRotation(SetRot);
}

void CMouseCursor::SetPosition(const Vector3& SetPos)
{
	if (!m_SelectObject)
	{
		return;
	}
	m_SelectObject->SetPosition(SetPos);
}

Vector3* CMouseCursor::GetRotationAddr()
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->GetRotationAddr();
}

void CMouseCursor::AcornCreate()
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	if (m_IsAcornLine)
	{
		for (int i = 0; i < m_AcornSetNumber; i++)
		{
			const Vector3 pos = LineSet(i, m_AcornSetNumber, Vector3(m_Position), Vector3(m_Position.x + m_EndAcornPosition.x, m_Position.y + m_EndAcornPosition.y, m_Position.z + m_EndAcornPosition.z));
			CAcorn* acorn = new CAcorn(Vector3(m_Position.x + pos.x, m_Position.y + pos.y, m_Position.z + pos.z));
			if (!acorn)
			{
				continue;
			}
			acorn->InitBodyCollisionCheck();
			scene.lock()->AddGameObject<CAcorn>(ESceneObjectType::BILLBOARD, acorn);
			if (i == 0)
			{
				m_SelectObject = acorn;
			}
			PushMapObject(acorn, "Acorn", EStaticMeshType::SPHERE, m_SelectObjectType);
		}
		m_IsAcornLine = false;
	}
	else if (m_IsAcornCircle)
	{
		//円形配置処理
		for (int i = 0; i < m_AcornSetNumber; i++)
		{
			const Vector2 pos = CirclesSet(i, m_AcornSetNumber, m_CirclesAngle, m_CirclesRadius, 0.0f, 0.0f);
			CAcorn* acorn = new CAcorn(Vector3(m_Position.x + pos.x, m_Position.y, m_Position.z + pos.y));
			if (!acorn)
			{
				continue;
			}
			acorn->InitBodyCollisionCheck();
			scene.lock()->AddGameObject<CAcorn>(ESceneObjectType::BILLBOARD, acorn);
			if (i == 0)
			{
				m_SelectObject = acorn;
			}
			PushMapObject(acorn, "Acorn", EStaticMeshType::SPHERE, m_SelectObjectType);
		}
		m_IsAcornCircle = false;
	}
	else
	{
		CAcorn* acorn = new CAcorn(Vector3(m_Position));
		if (!acorn)
		{
			return;
		}
		acorn->InitBodyCollisionCheck();
		scene.lock()->AddGameObject<CAcorn>(ESceneObjectType::BILLBOARD, acorn);
		m_SelectObject = acorn;
		PushMapObject(acorn, "Acorn", EStaticMeshType::SPHERE, m_SelectObjectType);
	}
}

CCommonObject* CMouseCursor::SelectCastCommonObject() const
{
	if (!m_SelectObject)
	{
		nullptr;
	}
	return dynamic_cast<CCommonObject*>(m_SelectObject);
}

MATERIAL* CMouseCursor::GetMaterial() const
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->GetMaterial();
}

void CMouseCursor::SetChoiceObjectNumber(const EEditorSelectObjectType Type)
{
	m_SelectObjectType = Type;
}

bool CMouseCursor::IsSelectObject() const
{
	if (!m_SelectObject)
	{
		return false;
	}
	return m_SelectObject;
}

bool* CMouseCursor::IsDisableShadow()
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->IsDisableShadow();
}

bool CMouseCursor::IsLoadNormalMap() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->IsLoadNormalMap();
	}
	return false;
}

ShaderNormalMapData*  CMouseCursor::GetNormalMapPower() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->GetNormalMapPower();
	}
	return nullptr;
}

CBox* CMouseCursor::GetCollisionBox() const
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->GetCollisionBox();
}

CSphere* CMouseCursor::GetCullingSphere() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->GetCullingSphere();
	}
	return nullptr;
}

void CMouseCursor::SetPlayerCollisionTag(const ECollisionTagName CollisionTag)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetPlayerCollisionTag(CollisionTag);
	}
}

void CMouseCursor::SetObjectCollisionTag(const ECollisionTagName CollisionTag)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetObjectCollisionTag(CollisionTag);
	}
}

void CMouseCursor::SetCameraCollisionTag(const ECollisionTagCameraName CollisionTag)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetCameraCollisionTag(CollisionTag);
	}
}

void CMouseCursor::SetStepsCollisionTag(const ECollisionTagSteps CollisionTag)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetStepsCollisionTag(CollisionTag);
	}
}

ECollisionTagName CMouseCursor::GetPlayerCollisionTag() const
{//当たってないときはNO_COLLISIONになるのでデフォルトのを渡す
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->GetPlayerCollisionTag();
	}
	return ECollisionTagName::NO_COLLISION_HIT;
}

ECollisionTagName CMouseCursor::GetObjectCollisionTag() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->GetObjectCollisionTag();
	}
	return ECollisionTagName::NO_COLLISION_HIT;
}

ECollisionTagCameraName CMouseCursor::GetCameraCollisionTag() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->GetCameraCollisionTag();
	}
	return ECollisionTagCameraName::NONE;
}

ECollisionTagSteps CMouseCursor::GetStepsCollisionTag() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->GetStepsCollisionTag();
	}
	return ECollisionTagSteps::NORMAL;
}

std::vector<Vector3>* CMouseCursor::GetTargetPoint()
{
	if (CEnemy* selectEnemy = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		return selectEnemy->GetTargetPoint();
	}
	return nullptr;
}

void CMouseCursor::AddTargetPosition(const Vector3& AddPos)
{
	if (CEnemy* selectEnemy = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		selectEnemy->AddTargetPosition(AddPos);
	}
}

void CMouseCursor::DeleteTargetPosition(const int index)
{
	if (CEnemy* selectEnemy = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		selectEnemy->DeleteTargetPosition(index);
	}
}

void CMouseCursor::SetTargetPointImagePosition(const int index, const Vector3& SetPos)
{
	if (CEnemy* selectEnemy = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		selectEnemy->SetTargetPointImagePosition(index, SetPos);
	}
}

Vector3 CMouseCursor::GetFirstPosition()
{
	if (CEnemy* selectEnemy = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		return selectEnemy->GetFirstPosition();
	}
	return Vector3();
}

CSphere* CMouseCursor::GetRangeSphere() const
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->GetRangeSphere();
}

CBox* CMouseCursor::GetRangeBox() const
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->GetRangeBox();
}

CSphere* CMouseCursor::GetSphere() const
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->GetSphere();
}

bool* CMouseCursor::IsRespawn()
{
	if (CMobEnemy* selectEnemy = dynamic_cast<CMobEnemy*>(m_SelectObject))
	{
		return selectEnemy->IsRespawn();
	}
	return nullptr;
}

int* CMouseCursor::GetRespawnTime()
{
	if (CMobEnemy* selectEnemy = dynamic_cast<CMobEnemy*>(m_SelectObject))
	{
		return selectEnemy->GetRespawnTime();
	}
	return nullptr;
}

void CMouseCursor::CreateCommonObject(const EEditorCreateObjectType Type, const Vector3& Position, const Vector3& Rotation, const Vector3& Scale, const float Radius, const Vector3& SpherePosition, const ECollisionTagName PlayerCollision, const ECollisionTagName ObjectCollision, const ECollisionTagCameraName CameraCollision, const ECollisionTagSteps StepCollision, const bool IsLight, const MATERIAL* Material)
{
	std::weak_ptr<CScene> scene = GetScene();
	if (scene.expired())
	{
		return;
	}
	CCommonObject* common = new CCommonObject();
	if (!common)
	{
		return;
	}
	common->Init(m_MeshType, Position, Scale, Rotation);
	common->CreateCullingSphere(Radius, SpherePosition);
	common->SetEnableLight(IsLight);
	common->SetMaterial(*Material);
	//radiusが0ならノーメッシュと判断
	if (Radius == 0.0f)
	{
		common->SetPlayerCollisionTag(ECollisionTagName::NO_COLLISION_HIT);
		common->SetObjectCollisionTag(ECollisionTagName::NO_COLLISION_HIT);
	}
	else
	{
		common->SetPlayerCollisionTag(PlayerCollision);
		common->SetObjectCollisionTag(ObjectCollision);
		common->SetCameraCollisionTag(CameraCollision);
		common->SetStepsCollisionTag(StepCollision);
	}
	scene.lock()->AddGameObject<CCommonObject>(ESceneObjectType::MESH_OBJECT, common);
	m_SelectObject = common;
	PushMapObject(common, CStaticMesh::GetModelFileName(m_MeshType), m_MeshType, m_SelectObjectType);
}

bool* CMouseCursor::IsNoCullingCheck()
{ 
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->IsNoCullingCheck();
	}
	return nullptr;
}

void CMouseCursor::SetNoCullingCheck(const bool Type)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetNoCullingCheck(Type);
	}
}

ItemDropData* CMouseCursor::GetItemDropData()
{
	if (CMobEnemy* enemyObject = dynamic_cast<CMobEnemy*>(m_SelectObject))
	{
		return enemyObject->GetItemDropData();
	}
	return nullptr;
}

void CMouseCursor::ChangeTexture(const ETextureType Type, const int Index)
{
	if (!m_SelectObject)
	{
		return;
	}
	m_SelectObject->ChangeTexture(Type, Index);
}

int* CMouseCursor::GetOpenStar()
{
	if (CStarDoor* starDoor = dynamic_cast<CStarDoor*>(m_SelectObject))
	{
		return starDoor->GetOpenStar();
	}
	return nullptr;
}

void CMouseCursor::ChangeSphereTexture(const ETextureType Type)
{
	if (!m_SelectObject)
	{
		return;
	}
	m_SelectObject->ChangeSphereTexture(Type);
}

void CMouseCursor::PushMapObject(CGameObject* Object, const std::string& ModelPath, const EStaticMeshType MeshType, const EEditorSelectObjectType SelectType)
{
	MapObject map;
	map.Object = Object;
	map.Name = ModelPath;
	map.MeshType = MeshType;
	map.SelectObjectType = SelectType;
	map.Object->SetOriginalNumber(m_Number);
	map.Number = m_Number;
	m_CreateNumber = m_Number;
	m_Number++;
	//モデルの名前だけ抽出
	map.Name = map.Name.substr(map.Name.find_last_of('/') + 1);
	const int pos = (int)map.Name.find(".");
	if (pos > -1)
	{
		map.Name.erase(pos);
	}

	//同じモデルが何個置いてあるか数える
	int meshNum = 0;
	for (MapObject& object : m_MapObjectList)
	{
		if (object.MeshType == map.MeshType)
		{
			meshNum++;
		}
	}
	map.ObjectNumber = meshNum;

	m_MapObjectList.emplace_back(map);
}

void CMouseCursor::MaterialAllSet(const MATERIAL& Material)
{
	for (MapObject& object : m_MapObjectList)
	{
		if (!object.Object)
		{
			continue;
		}
		if (object.MeshType != m_MeshType)
		{
			continue;
		}
		
		object.Object->SetEnableLight(true);
		if (CCommonObject* common = dynamic_cast<CCommonObject*>(object.Object))
		{
			common->SetMaterial(Material);
		}
	}
}

void CMouseCursor::ResetMapObject()
{
	m_MapObjectList.clear();
	m_MapObjectList.shrink_to_fit();
	m_Number = 0;
	m_CreateNumber = 0;
	m_SelectObject = nullptr;
}

bool* CMouseCursor::IsEditorOnlyDraw()
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->IsEditorOnlyDraw();
	}
	return nullptr;
}

bool* CMouseCursor::IsEnableLight()
{
	if (!m_SelectObject)
	{
		return nullptr;
	}
	return m_SelectObject->IsEnableLight();
}

void CMouseCursor::SetEnableLight(const bool Enable)
{
	if (!m_SelectObject)
	{
		return;
	}
	m_SelectObject->SetEnableLight(Enable);
}

bool* CMouseCursor::IsDisableSelfShadow() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->IsDisableSelfShadow();
	}
	return nullptr;
}

void CMouseCursor::SetDisableSelfShadow(const bool Disable)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->SetDisableSelfShadow(Disable);
	}
}

bool* CMouseCursor::IsEnableNormalMap() const
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->IsEnableNormalMap();
	}
	return nullptr;
}

void CMouseCursor::SetEnableNormalMap(const bool Enable)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetEnableNormalMap(Enable);
	}
}

void CMouseCursor::SetFirstPosition(const Vector3& SetPos)
{
	if (CEnemy* enemyObject = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		enemyObject->SetFirstPosition(SetPos);
	}
}

bool* CMouseCursor::IsBodySphereCollisionCheck()
{
	if (CEnemy* enemyObject = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		return enemyObject->IsBodySphereCollisionCheck();
	}
	return nullptr;
}

bool* CMouseCursor::IsDownLineCollisionCheck()
{
	if (CEnemy* enemyObject = dynamic_cast<CEnemy*>(m_SelectObject))
	{
		return enemyObject->IsDownLineCollisionCheck();
	}
	return nullptr;
}

bool* CMouseCursor::IsDisableDepth()
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		return common->IsDisableDepth();
	}
	return nullptr;
}

float* CMouseCursor::GetHeightLockPosition()
{
	if (CCameraHeightLockBox* selectBox = dynamic_cast<CCameraHeightLockBox*>(m_SelectObject))
	{
		return selectBox->GetHeightLockPosition();
	}
	return nullptr;
}

int* CMouseCursor::GetTutorialNumber() const
{ 
	if (CTutorialBox* box = dynamic_cast<CTutorialBox*>(m_SelectObject))
	{
		return box->GetTutorialNumber();
	}
	return nullptr;
}

void CMouseCursor::SetAlpha(const float Alpha)
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->SetAlpha(Alpha);
	}
}

void CMouseCursor::UpdateAutoCullingSphere()
{
	if (CCommonObject* common = SelectCastCommonObject())
	{
		common->UpdateAutoCullingSphere();
	}
}

Vector2 CMouseCursor::CirclesSet(const int Index, const int CircleNum, const float Angle, const float Radius, const float Cx, const float Cz)
{
	Vector2 xz;
	const float num = static_cast<float>(CircleNum);
	const float rad = ((Index / num) * (XM_PI * 2.0f)) + Angle;
	xz.x = Radius * cosf(rad) + Cx;
	xz.y = Radius * sinf(rad) + Cz;
	return xz;
}

Vector3 CMouseCursor::LineSet(const int Index, const int Num, const Vector3& Start, const Vector3& End)
{
	const float num = static_cast<float>(Num);
	Vector3 position = End - Start;
	position = position * (Index / num);
	return position;
}
