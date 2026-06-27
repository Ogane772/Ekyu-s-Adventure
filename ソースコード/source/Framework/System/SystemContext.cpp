#include "SystemContext.h"
#include <DirectXMath.h>
#include "../Scene/SceneManager.h"
#include "../Scene/SceneTypes.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraSetting.h"
using namespace DirectX;

CSystemContext::CSystemContext()
{
	m_CameraRotationUpAndDownType = ECameraRotationSettingType::NORMAL;
	m_CameraRotationLeftAndRightType = ECameraRotationSettingType::NORMAL;
	m_CameraSpeedType = ECameraSpeedSettingType::NORMAL;
	
	m_SceneNameList =
	{
			{ ESceneType::TEST, "Test" },
			{ ESceneType::TITLE, "Title"},
			{ ESceneType::STAGE_SELECT, "StageSelect"},
			{ ESceneType::TUTORIAL, "Tutorial"},
			{ ESceneType::FIRST,"First"},
			{ ESceneType::SECOND, "Second"},
			{ ESceneType::BOSS, "Boss"},
			{ ESceneType::MODEL_VIEW, "ModelView"},
	};
	//セーブデータを使用するSceneのみセーブデータ名を定義
	m_SceneLoadSaveFileList =
	{
			{ ESceneType::TEST, "test.txt" },
			{ ESceneType::STAGE_SELECT, "SelectMap.txt"},
			{ ESceneType::TUTORIAL, "TutorialMap.txt"},
			{ ESceneType::FIRST,"FirstMap.txt"},
			{ ESceneType::SECOND, "SecondMap.txt"},
	};
}

std::string CSystemContext::GetLoadSaveFile(const ESceneType Type)
{
	if (m_SceneLoadSaveFileList.contains(Type))
	{
		return m_SceneLoadSaveFileList[Type];
	}
	return std::string();
}

std::string CSystemContext::GetSceneName(const ESceneType Type)
{
	if (m_SceneNameList.contains(Type))
	{
		return m_SceneNameList[Type];
	}
	return std::string();
}

bool CSystemContext::VFCulling(const Vector3& Position, const float Radius) const
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return false;
	}
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return false;
	}
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return false;
	}
	const float angle = XM_PI / 3.0f;
	const float nearClip = 1.0f;
	const float farClip = 300.0f;
	const float aspect = system->GetCurrentScreenWidth() / system->GetCurrentScreenHeight();
	XMVECTOR vfLeftPlane, vfRightPlane, vfTopPlane, vfBottomPlane;
	Vector3 pos = Position;
	
	const XMMATRIX& pmView = camera->GetInvViewMatrix();
	//まず、ジオメトリの位置ベクトルをワールドからビュー空間に変換
	pos = Vector3::TransformCoord(pos, pmView);
	//左右、上下の平面を計算
	{
		XMVECTOR p1, p2, p3;
		p1 = XMVectorZero();
		//左面
		p2 = XMVectorSet(-farClip * ((angle / 2.0f) * aspect),
			-farClip * tan(angle / 2.0f),
			farClip,
			0.0f);
		p3 = XMVectorSet(XMVectorGetX(p2),
			-XMVectorGetY(p2),
			XMVectorGetZ(p2),
			0.0f);
		vfLeftPlane = XMPlaneNormalize(XMPlaneFromPoints(p1, p2, p3));
		//右面
		p2 = XMVectorSet(farClip * (tan(angle / 2.0f) * aspect),
			farClip * tan(angle / 2.0f),
			farClip,
			0.0f);
		p3 = XMVectorSet(XMVectorGetX(p2),
			-XMVectorGetY(p2),
			XMVectorGetZ(p2),
			0.0f);
		vfRightPlane = XMPlaneNormalize(XMPlaneFromPoints(p1, p2, p3));
		//上面
		p2 = XMVectorSet(-farClip * (tan(angle / 2.0f) * aspect),
			farClip * tan(angle / 2.0f),
			farClip,
			0.0f);
		p3 = XMVectorSet(-XMVectorGetX(p2),
			XMVectorGetY(p2),
			XMVectorGetZ(p2),
			0.0f);
		vfTopPlane = XMPlaneNormalize(XMPlaneFromPoints(p1, p2, p3));
		//下面
		p2 = XMVectorSet(farClip * (tan(angle / 2.0f) * aspect),
			-farClip * tan(angle / 2.0f),
			farClip,
			0.0f);
		p3 = XMVectorSet(-XMVectorGetX(p2),
			XMVectorGetY(p2),
			XMVectorGetZ(p2),
			0.0f);
		vfBottomPlane = XMPlaneNormalize(XMPlaneFromPoints(p1, p2, p3));
	}

	//6つの平面とジオメトリ境界球をチェック
	float distance = 0.0f;
	{
		// 点ベクトル（w = 1）を作る
		const XMVECTOR posVec = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);

		//ニアクリップ面
		if ((pos.z + Radius) < nearClip)
		{
			return false;
		}
		//ファークリップ面
		if ((pos.z - Radius) > farClip)
		{
			return false;
		}
		//左クリップ面
		{
			distance = XMVectorGetX(XMPlaneDotCoord(vfLeftPlane, posVec));
			if (distance > Radius)
			{
				return false;
			}
		}
		//右クリップ面に
		{
			distance = XMVectorGetX(XMPlaneDotCoord(vfRightPlane, posVec));
			if (distance > Radius)
			{
				return false;
			}
		}
		//上クリップ面
		{
			distance = XMVectorGetX(XMPlaneDotCoord(vfTopPlane, posVec));
			if (distance > Radius)
			{
				return false;
			}
		}
		//下クリップ面
		{
			distance = XMVectorGetX(XMPlaneDotCoord(vfBottomPlane, posVec));
			if (distance > Radius)
			{
				return false;
			}
		}
	}

	return true;
}

RECT CSystemContext::GetWindowRange()
{
	GetWindowRect(m_Window, &m_WindowRange);
	return m_WindowRange;
}

ECameraRotationSettingType CSystemContext::ChangeCameraRotationSettingType(ECameraRotationSettingType Type)
{
	if (Type == ECameraRotationSettingType::NORMAL)
	{
		Type = ECameraRotationSettingType::REVERSE;
	}
	else if (Type == ECameraRotationSettingType::REVERSE)
	{
		Type = ECameraRotationSettingType::NORMAL;
	}
	return Type;
}

void CSystemContext::ChangeCameraRotationUpAndDownType()
{
	m_CameraRotationUpAndDownType = ChangeCameraRotationSettingType(m_CameraRotationUpAndDownType);
}

void CSystemContext::ChangeCameraRotationLeftAndRightType()
{
	m_CameraRotationLeftAndRightType = ChangeCameraRotationSettingType(m_CameraRotationLeftAndRightType);
}

void CSystemContext::PrevCameraSpeedType()
{
	int number = static_cast<int>(m_CameraSpeedType);
	number--;
	if (number < static_cast<int>(ECameraSpeedSettingType::SLOW))
	{
		number = static_cast<int>(ECameraSpeedSettingType::HIGH);
	}
	m_CameraSpeedType = static_cast<ECameraSpeedSettingType>(number);
}

void CSystemContext::NextCameraSpeedType()
{
	int number = static_cast<int>(m_CameraSpeedType);
	number++;
	if (number > static_cast<int>(ECameraSpeedSettingType::HIGH))
	{
		number = 0;
	}
	m_CameraSpeedType = static_cast<ECameraSpeedSettingType>(number);
}

void CSystemContext::ConvertToTextureRelativePath(const std::string& FilePath, const std::string& AbsolutePath, std::string& OutRelativePath) const
{
	std::string basePath = FilePath.substr(0, FilePath.find_last_of("\\/") + 1);

	std::string fileName = AbsolutePath;
	//ファイルパスが相対じゃなくても読むようにする
	size_t pos = fileName.find_last_of("\\/");
	std::string texName;
	(pos != std::string::npos) ?	// ファイルパスが含まれていたら取り除く
		texName = fileName.substr(pos + 1, fileName.size() - pos) :
		texName = fileName;
	OutRelativePath = basePath + texName;
}
