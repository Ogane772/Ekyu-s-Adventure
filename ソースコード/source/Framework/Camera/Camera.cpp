#include "Camera.h"
#include "CameraSetting.h"
#include "../Renderer/Renderer.h"
#include "../System/SystemContext.h"
#include "../Input/InputInterface.h"
#include "../Text/Text.h"
#include "../Time/Time.h"
#include "../Collision/Collision.h"
#include "../Collision/CollisionTag.h"
#include "../../Game/Character/Player/Player.h"

//カメラの縦軸の回転値の制限
namespace
{
	constexpr float DEFAULT_CAMERA_DISTANCE = 12.0f;//デフォルトの注視点とカメラ間の長さ
	constexpr float CAMERA_RESET_TIME = 0.24f;//カメラリセット時
	constexpr float ADD_AT_HEIGHT = 3.0f;//注視点に足す高さ
	constexpr float LIMIT_CAMERA_ROTATIONY_UP = 28.0f;//カメラの上方向の上限回転角度
	constexpr float LIMIT_CAMERA_ROTATIONY_DOWN = -28.0f;//カメラの下方向の上限回転角度
	constexpr float HEIGHT_LERP_TIME = 0.2f;//高さ制限ボックス等のカメラ高さを変える際の補間時間
	constexpr float DEATH_LIMKIT_CAMERA_DISTANCE = 7.0f;//死亡時の注視点とカメラ間の距離の加減値
	constexpr float DEATH_DISTANCE_SPEED = 5.0f;//死亡時の中視点とカメラ間を縮める速度
	constexpr float DEATH_ADD_AT_HEIGHT = 1.5f;//死亡時の注視点に足す高さ
}

void CCamera::Init()
{
	m_StateMachine.AddState<CCamera>(
		ECameraState::NORMAL,
		this,
		nullptr,
		&CCamera::UpdateGameCamera
	);
	m_StateMachine.AddState<CCamera>(
		ECameraState::CAMERA_RESET,
		this,
		&CCamera::StartCameraReset,
		&CCamera::UpdateCameraReset
	);
	m_StateMachine.AddState<CCamera>(
		ECameraState::DEATH,
		this,
		nullptr,
		&CCamera::UpdateDeathCamera
	);
	m_StateMachine.AddState<CCamera>(
		ECameraState::STAGE_SELECT,
		this,
		&CCamera::StartStageSelect,
		nullptr
	);
	m_StateMachine.AddState<CCamera>(
		ECameraState::EVENT,
		this,
		nullptr,
		&CCamera::UpdateEventCamera
	);
	m_StateMachine.SetInitialState(ECameraState::NORMAL);
	if (CSystemContext* system = CSystemContext::GetInstance())
	{
		m_Viewport.left = 0;
		m_Viewport.top = 0;
		m_Viewport.right = static_cast<LONG>(system->GetDefaultScreenWidth());
		m_Viewport.bottom = static_cast<LONG>(system->GetDefaultScreenHeight());
	}
	m_CameraSpeedList =
	{
		{ ECameraSpeedSettingType::SLOW, 60.0f },
		{ ECameraSpeedSettingType::NORMAL, 120.0f },
		{ ECameraSpeedSettingType::HIGH, 180.0f },
	};
	m_ShakeDataList =
	{
		{ ECameraShakeType::LOW, CameraShakeData(0.2f, 0.5f, 1) },
		{ ECameraShakeType::MIDDLE, CameraShakeData(0.3f, 1.0f, 2) },
		{ ECameraShakeType::HIGH, CameraShakeData(0.4f, 1.5f, 3) },
	};
	m_IsNoCullingCheck = true;
	m_FrontLine = std::make_unique<CLine>(&m_Position, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_CameraLength = DEFAULT_CAMERA_DISTANCE;
	m_DefaultFront = m_Front = Vector3(0.0f, -0.15f, 1.0f);
	m_EditRightLeftRotSpeed = 15.0f;
	m_EditUpDownRotSpeed = 15.0f;
	m_EditCameraSpeed = 70.0f;
	m_CountLerpTime = HEIGHT_LERP_TIME;
}

void CCamera::ResetPlayerCamera()
{
	m_CameraLength = DEFAULT_CAMERA_DISTANCE;
	m_Front = Vector3(0.0f, -0.15f, 1.0f);
	m_DefaultFront = m_Front;
	m_Near = 1.0f;
	UpdateCameraPlayerPosition(0.0f);
	StartCameraReset();
}

void CCamera::Update(float DeltaTime)
{
	if (m_IsCameraDirection)
	{
		return;
	}

	if (IsEditMode())
	{
		UpdateDebugCamera(DeltaTime);
	}
	else
	{
		m_StateMachine.Update(DeltaTime);
	}
}

void CCamera::UpdateGameCamera(float DeltaTime)
{
	if (IsEvent())
	{
		return;
	}
	if (m_TargetPlayer)
	{
		if (m_TargetPlayer->IsCongratulationState())
		{
			return;
		}
	}
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	if (instance->IsCameraResetTrigger())
	{//カメラをプレイヤー背後にリセット
		ChangeState(ECameraState::CAMERA_RESET);
		return;
	}

	const Vector2 rightStick = instance->GetRightStick();
	//カメラ左右移動
	if (rightStick.x != 0.0f)
	{
		float x = rightStick.x;
		//オプションで左右反転であれば符号判定
		if (system->GetCameraRotationLeftAndRightType() == ECameraRotationSettingType::REVERSE)
		{
			x *= -1.0f;
		}
		x *= GetRotationSpeed() * DeltaTime;
		m_Rotation.y += x;
		const XMMATRIX mtxR = XMMatrixRotationY(XMConvertToRadians(x));
		m_Front = Vector3::TransformCoord(m_Front, mtxR);
	}

	//カメラ上下移動
	if (rightStick.y != 0.0f)
	{
		float y = rightStick.y;
		//オプションで上下反転であれば符号判定
		if (system->GetCameraRotationUpAndDownType() == ECameraRotationSettingType::REVERSE)
		{
			y *= -1.0f;
		}
		y *= GetRotationSpeed() * DeltaTime;
		m_Rotation.x += -y;
		const Vector3 front = m_Front;
		//カメラの上限回転値チェック
		if (IsLimitCameraRotationY())
		{
			m_Front = front;
			m_Rotation.x = std::clamp(m_Rotation.x, LIMIT_CAMERA_ROTATIONY_DOWN, LIMIT_CAMERA_ROTATIONY_UP);
		}
		else
		{
			const Vector3 right = Vector3::Cross(m_Front, Vector3(0.0f, 1.0f, 0.0f));
			const XMVECTOR xmRight = XMVectorSet(right.x, right.y, right.z, 1.0);
			const XMMATRIX mtxR = XMMatrixRotationAxis(xmRight, XMConvertToRadians(y));
			m_Front = Vector3::TransformCoord(m_Front, mtxR);
		}
	}

	UpdateCameraPlayerPosition(DeltaTime);
}

void CCamera::UpdateDebugCamera(float DeltaTime)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	
	//注視点を中心に回転
	CInputInterface* instance = CInputInterface::GetInstance();
	if (!instance)
	{
		return;
	}
	XMMATRIX mtxR;

	//マウスによるカメラ回転
	if (instance->IsDebugCameraScreenClickTrigger())
	{
		const RECT range = system->GetWindowRange();
		//マウスの座標を調整する
		GetCursorPos(&m_EditCameraCursorPos);
		m_EditCameraCursorPos.x = m_EditCameraCursorPos.x - range.left - (range.right - range.left) / 2;
		m_EditCameraCursorPos.y = (m_EditCameraCursorPos.y - (range.top) - (range.bottom - (range.top)) / 2) * (-1);
	}
	if (instance->IsDebugCameraRotationPress())
	{
		const RECT range = system->GetWindowRange();
		POINT point;
		//マウスの座標を調整する
		GetCursorPos(&point);
		point.x = point.x - range.left - (range.right - range.left) / 2;
		point.y = (point.y - (range.top) - (range.bottom - (range.top)) / 2) * (-1);
		float upDownRotation = (float)(point.x - m_EditCameraCursorPos.x);
		float rightLeftRotation = (float)(point.y - m_EditCameraCursorPos.y);
		upDownRotation *= m_EditUpDownRotSpeed * DeltaTime;
		rightLeftRotation *= m_EditRightLeftRotSpeed * DeltaTime;
		if (m_IsEditRightLeftRotChange)
		{
			upDownRotation = -upDownRotation;
		}
		if (m_IsEditUpDownRotChange)
		{
			rightLeftRotation = -rightLeftRotation;
		}

		mtxR = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rightLeftRotation), XMConvertToRadians(upDownRotation), XMConvertToRadians(0.0f));
		m_Front = Vector3::TransformCoord(m_Front, mtxR); 
		m_EditCameraCursorPos = point;
	}

	//入力によるカメラ回転
	Vector2 rightStick = instance->GetRightStick() * GetRotationSpeed() * DeltaTime;
	if (rightStick.Length() != 0.0f)
	{
		if (m_IsEditRightLeftRotChange)
		{
			rightStick.x = -rightStick.x;
		}
		if (m_IsEditUpDownRotChange)
		{
			rightStick.y = -rightStick.y;
		}
		const Vector3 right = Vector3::Cross(m_Front, Vector3(0.0f, 1.0f, 0.0f));
		const XMVECTOR xmRight = XMVectorSet(right.x, right.y, right.z, 1.0);
		mtxR = XMMatrixRotationAxis(xmRight, XMConvertToRadians(rightStick.y));
		m_Front = Vector3::TransformCoord(m_Front, mtxR);
		mtxR = XMMatrixRotationY(XMConvertToRadians(rightStick.x));
		m_Front = Vector3::TransformCoord(m_Front, mtxR);
	}

	//カメラ移動
	const Vector2 leftStick = instance->GetLeftStick();
	const float upDown = instance->GetEditCameraUpDown();
	if (upDown != 0.0f || leftStick.Length() != 0.0f)
	{
		const Vector3 cameraFrontVector = m_Front;
		const float inputRotation = (atan2(cameraFrontVector.x, cameraFrontVector.z));
		mtxR = XMMatrixRotationY(inputRotation);
		//Y軸を曲げるのでZに代入
		Vector3 gamePadMove = Vector3(leftStick.x, 0.0f, leftStick.y);
		gamePadMove = Vector3::TransformCoord(gamePadMove, mtxR);
		m_At += Vector3(gamePadMove.x, upDown, gamePadMove.z) * m_EditCameraSpeed * DeltaTime;
	}

	SetupCameraPosition(m_CameraLength);
}

void CCamera::UpdateDeathCamera(float DeltaTime)
{
	if (m_CameraLength > DEATH_LIMKIT_CAMERA_DISTANCE)
	{
		m_CameraLength -= DEATH_DISTANCE_SPEED * DeltaTime;
	}
	m_Position.x = m_At.x - m_Front.x * m_CameraLength;
	m_Position.y = (m_At.y - DEATH_ADD_AT_HEIGHT) - m_Front.y * m_CameraLength;
	m_Position.z = m_At.z - m_Front.z * m_CameraLength;
}

void CCamera::StartStageSelect()
{
	m_At = Vector3(4.629f, 4.0f, 51.455f);
	m_CameraLength = 12.0f;
	m_Front = XMFLOAT3(0.035f, -0.079f, 0.996f);

	SetupCameraPosition(DEFAULT_CAMERA_DISTANCE);
}

void CCamera::UpdateEventCamera(float DeltaTime)
{
	m_At = m_CenterAt;
	if (m_EnableCameraShake)
	{
		UpdateCameraShake(DeltaTime);
	}

	SetupCameraPosition(m_CameraLength);
}

void CCamera::UpdateCameraShake(float DeltaTime)
{
	m_CountShakeTime += DeltaTime;

	// 振動時間に対して何回振動するかから角速度を計算
	const float omega = XM_2PI * static_cast<float>(m_CurrentShakeData.ShakeCount) / m_CurrentShakeData.ShakeTime;

	// 振動終了に向けて振幅を徐々に小さくする
	const float rate = 1.0f - (m_CountShakeTime / m_CurrentShakeData.ShakeTime);

	//移動量を求める
	const float offsetY = sinf(m_CountShakeTime * omega) * m_CurrentShakeData.ShakeAmplitude * rate;

	m_At.y += offsetY;

	// 振動終了
	if (m_CountShakeTime >= m_CurrentShakeData.ShakeTime)
	{
		m_EnableCameraShake = false;
	}
}

float CCamera::GetRotationSpeed()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return 0.0f;
	}
	const ECameraSpeedSettingType type = system->GetCameraSpeedType();
	return m_CameraSpeedList[type];
}

void CCamera::Draw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	// ビューポート設定
	D3D11_VIEWPORT dxViewport;
	dxViewport.Width = system->GetCurrentScreenWidth() - (float)m_Viewport.left;
	dxViewport.Height = system->GetCurrentScreenHeight() - (float)m_Viewport.top;
	dxViewport.MinDepth = 0.0f;
	dxViewport.MaxDepth = 1.0f;
	dxViewport.TopLeftX = (float)m_Viewport.left;
	dxViewport.TopLeftY = (float)m_Viewport.top;

	CRenderer::GetDeviceContext()->RSSetViewports(1, &dxViewport);

	const Vector3 up = Vector3(0, 1, 0);
	Vector3::MatrixLookAtLH(m_InvViewMatrix, m_Position, m_At, up);//世界の方に代わってくれる行列
	
	XMMATRIX cameraView = XMLoadFloat4x4(&m_ViewMatrix);
	cameraView = m_InvViewMatrix;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixTranspose(cameraView));

	CRenderer::SetViewMatrix(cameraView);

	// プロジェクションマトリクス設定
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(1.0f, dxViewport.Width / dxViewport.Height, m_Near, 450.0f);

	CRenderer::SetProjectionMatrix(m_ProjectionMatrix);

	CRenderer::SetCameraPos(XMFLOAT4(m_Position.x, m_Position.y, m_Position.z, 0.0f));
}

void CCamera::StartCameraReset()
{
	if (!m_TargetPlayer)
	{
		return;
	}

	const Vector3 rot = m_TargetPlayer->GetRotation();
	if (m_Rotation.y != rot.y || m_Front.y != m_DefaultFront.y)
	{
		//補間先の回転値を保存
		m_TemporaryRotation.y = rot.y;
		m_TemporaryRotation.x = rot.x;
	}
}

void CCamera::UpdateCameraReset(float DeltaTime)
{
	//徐々に回転させる
	if (m_CameraResetInterpValue < 1.0f)
	{
		m_CameraResetInterpValue += DeltaTime / CAMERA_RESET_TIME;
		m_CameraResetInterpValue = (std::min)(m_CameraResetInterpValue, 1.0f);
		//各ベクトルを初期に戻す
		m_Front = m_DefaultFront;
		//回転補間を行う
		const XMVECTOR startRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
		const XMVECTOR goalRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_TemporaryRotation.x), XMConvertToRadians(m_TemporaryRotation.y), XMConvertToRadians(m_Rotation.z));
		const XMVECTOR currentRotation = XMQuaternionSlerp(startRotation, goalRotation, m_CameraResetInterpValue);

		////前ベクトルをプレイヤーの背後までの回転分曲げる
		const XMMATRIX mtxR = XMMatrixRotationQuaternion(currentRotation);
		m_Front = Vector3::TransformCoord(m_Front, mtxR);
	}
	else
	{
		m_CameraResetInterpValue = 0.0f;
		m_Rotation = m_TemporaryRotation;
		ChangeState(ECameraState::NORMAL);
		return;
	}

	UpdateCameraPlayerPosition(DeltaTime);
}

bool CCamera::IsLimitCameraRotationY() const
{
	return m_Rotation.x >= LIMIT_CAMERA_ROTATIONY_UP || m_Rotation.x <= LIMIT_CAMERA_ROTATIONY_DOWN;
}

void CCamera::SetupCameraPosition(const float CameraDistance)
{
	m_Position = m_At - m_Front * CameraDistance;
}

void CCamera::UpdateCameraPlayerPosition(float DeltaTime)
{
	if (!m_FrontLine)
	{
		return;
	}
	if (!m_TargetPlayer)
	{
		return;
	}

	//カメラ位置設定。高さのみ固定される場合があるため補間を行う
	const Vector3 targetPosition = m_TargetPlayer->GetPosition();
	float lerpAlpha = 1.0f;
	if (m_CountLerpTime < HEIGHT_LERP_TIME)
	{
		m_CountLerpTime += DeltaTime;
		lerpAlpha = m_CountLerpTime / HEIGHT_LERP_TIME;
	}
	else
	{
		m_LerpStartCameraHeight = targetPosition.y + ADD_AT_HEIGHT;
	}

	if (m_IsHeightLock)
	{
		m_At.y = std::lerp(m_LerpStartCameraHeight, m_HeightLockPosition, lerpAlpha);
	}
	else
	{
		m_At.y = std::lerp(m_LerpStartCameraHeight, targetPosition.y + ADD_AT_HEIGHT, lerpAlpha);
	}
	m_At.x = targetPosition.x;
	m_At.z = targetPosition.z;

	if (m_EnableCameraShake)
	{
		UpdateCameraShake(DeltaTime);
	}

	//注視点を中心にカメラ位置を求める
	m_Position = m_At - m_Front * DEFAULT_CAMERA_DISTANCE;

	//カメラ補正
	//カメラから正面、左右、下方向にレイを飛ばし当たっていたらその分縮める
	float final_length = -1.0f;
	
	RayCastInfo m_DownCastInfo;

	XMMATRIX mtxR = XMMatrixRotationY(XMConvertToRadians(m_Rotation.y));
	const Vector3 rightVector = Vector3::TransformCoord(Vector3(-1.0f, 0.0f, 0.0f), mtxR);

	std::vector<Vector3> lineEndPosition =
	{
		m_Position,
		m_Position + rightVector * 3,
		m_Position - rightVector * 3,
		m_Position +  Vector3(0.0f, -2.0f, 2.0f)
	};

	for (int i = 0; i < lineEndPosition.size(); i++)
	{
		const bool lineHit = m_FrontLine->LineHitCheck(ECheckCollisionType::CAMERA, m_At, lineEndPosition[i], &m_DownCastInfo);
		if (!lineHit)
		{
			continue;
		}

		if (final_length > m_DownCastInfo.Distance || final_length == -1.0f)
		{
			final_length = m_DownCastInfo.Distance;
		}
	}

	//カメラを近くに寄せる
	if (final_length > 0.0f && final_length < DEFAULT_CAMERA_DISTANCE)
	{
		SetupCameraPosition(final_length);
		m_Near = 0.1f;
	}
	else
	{
		m_CameraLength = DEFAULT_CAMERA_DISTANCE;
		m_Near = 1.0f;
	}
}

void CCamera::PlayCameraShake(const ECameraShakeType Type)
{
	if (m_EnableCameraShake)
	{
		return;
	}
	if (!m_ShakeDataList.contains(Type))
	{
		return;
	}
	m_EnableCameraShake = true;
	m_CountShakeTime = 0.0f;
	m_CurrentShakeData = m_ShakeDataList[Type];
}

void CCamera::ResetCameraLength()
{
	m_CameraLength = DEFAULT_CAMERA_DISTANCE;
}

void CCamera::SetCameraPosition(const Vector3& AtPosition)
{
	m_At = AtPosition;
	m_At.y += ADD_AT_HEIGHT;
	SetupCameraPosition(m_CameraLength);
}

void CCamera::SetCameraCenterPosition(const Vector3& AtPosition)
{
	m_CenterAt = m_At = AtPosition;
	SetupCameraPosition(m_CameraLength);
}

void CCamera::StartCameraDirection(const Vector3 SetPos, const Vector3 SetFront, const float Length)
{
	m_IsCameraDirection = true;

	m_SavePosition = m_Position;
	m_TemporaryFront = m_Front;

	m_Position = SetPos;
	m_CameraLength = Length;
	m_Front = SetFront;
	//正規化
	m_Front.Normalize();

	//自分を中心に回転
	m_At = m_Front * m_CameraLength + m_Position;

	//カメラ位置設定
	SetupCameraPosition(m_CameraLength);
}

void CCamera::EndCameraDirection()
{
	m_CameraLength = DEFAULT_CAMERA_DISTANCE;
	m_Position = m_SavePosition;
	m_Front = m_TemporaryFront;
	m_IsCameraDirection = false;
}

void CCamera::ChangeState(const ECameraState State)
{
	m_StateMachine.ChangeState(State);
}
