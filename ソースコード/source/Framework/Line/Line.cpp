#include "Line.h"
#include <D3DX11async.h>
#include "../Scene/Scene.h"
#include "../Scene/SceneManager.h"
#include "../Renderer/Renderer.h"
#include "../Camera/Camera.h"
#include "../Object/CommonObject.h"
#include "../GameObject/GameObject.h"
#include "../Collision/CollisionTag.h"
#include "../System/SystemContext.h"

ID3D11Buffer* CLine::m_LineConstantBuffer = nullptr;
ID3D11InputLayout* CLine::m_LineVertexLayout = nullptr;
ID3D11VertexShader* CLine::m_LineVertexShader = nullptr;
ID3D11PixelShader* CLine::m_LinePixelShader = nullptr;
ID3D11Buffer* CLine::m_ConstantBuffer = nullptr;
ID3D11Buffer* CLine::m_BufferInput = nullptr;
ID3D11Buffer* CLine::m_BufferResult = nullptr;
ID3D11ShaderResourceView* CLine::m_BufferInputSRV = nullptr;
ID3D11UnorderedAccessView* CLine::m_BufferResultUAV = nullptr;

void CLine::ShaderInit()
{
	ID3D11Device* device = CRenderer::GetDevice();
	if (!device)
	{
		return;
	}
	//hlslファイル読み込み ブロブ作成
	ID3D10Blob *pCompiledShader = nullptr;
	ID3D10Blob *pErrors = nullptr;
	//頂点インプットレイアウトを定義
	UINT numElements = 0;
	D3D11_INPUT_ELEMENT_DESC layout[3];

	//レイ用バーテックスシェーダー
	D3DX11CompileFromFile("shader/LineShaderVS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr);
	if (!pCompiledShader)
	{
		return;
	}
	device->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_LineVertexShader);

	//レイ用バーテックスレイアウト
	D3D11_INPUT_ELEMENT_DESC tmp[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = 1;
	memcpy(layout, tmp, sizeof(D3D11_INPUT_ELEMENT_DESC)*numElements);
	device->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_LineVertexLayout);

	//レイ用ピクセルシェーダー
	D3DX11CompileFromFile("shader/LineShaderPS.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr);
	device->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_LinePixelShader);

	if (pCompiledShader)
	{
		pCompiledShader->Release();
		pCompiledShader = nullptr;
	}
	D3D11_BUFFER_DESC cb;
	//コンスタントバッファー作成　レイ用
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(LINE_CBUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, nullptr, &m_LineConstantBuffer)))
	{
		return;
	}

	//コンスタントバッファー作成　Start,End渡し用
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(CBUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, NULL, &m_ConstantBuffer)))
	{
		return;
	}
}

void CLine::ShaderUnInit()
{
	if (m_LineConstantBuffer)
	{
		m_LineConstantBuffer->Release();
	}
	if (m_LineVertexLayout)
	{
		m_LineVertexLayout->Release();
	}
	if (m_LineVertexShader)
	{
		m_LineVertexShader->Release();
	}
	if (m_LinePixelShader)
	{
		m_LinePixelShader->Release();
	}
	if (m_ConstantBuffer)
	{
		m_ConstantBuffer->Release();
	}
}

CLine::CLine(Vector3* Pos, const Vector3& Start, const Vector3& End)
{
	ID3D11Device* device = CRenderer::GetDevice();
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!device || !deviceContext)
	{
		return;
	}
	//VertexBuffer作成
	D3D11_BUFFER_DESC cb;
	//レイのバーテックスバッファー作成
	Vector3 linePosition[2];
	linePosition[0] = Start;
	linePosition[1] = End;

	ZeroMemory(&cb, sizeof(cb));
	cb.Usage = D3D11_USAGE_DEFAULT;
	cb.ByteWidth = sizeof(Vector3) * 2;
	cb.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	cb.CPUAccessFlags = 0;
	cb.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = linePosition;
	device->CreateBuffer(&cb, &InitData, &m_LineVertexBuffer);

	m_PositionPointer = Pos;
	
	deviceContext->UpdateSubresource(m_LineVertexBuffer, 0, nullptr, &linePosition, 0, 0);
	m_LineColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
}

CLine::~CLine()
{
	if (m_LineVertexBuffer)
	{
		m_LineVertexBuffer->Release();
	}
}

void CLine::Draw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (!system->IsDebugCollisionRender())
	{
		return;
	}
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext || !m_PositionPointer)
	{
		return;
	}

	// マトリクス設定
	XMMATRIX world;
	world = XMMatrixScaling(1, 1, 1);
	world *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToRadians(0.0f), XMConvertToRadians(0.0f));
	world *= XMMatrixTranslation(m_PositionPointer->x, m_PositionPointer->y, m_PositionPointer->z);

	//使用するシェーダーのセット
	deviceContext->VSSetShader(m_LineVertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_LinePixelShader, nullptr, 0);

	//カメラ情報受け取り
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return;
	}
	CCamera* camera = sceneManager->GetCamera();
	if (!camera)
	{
		return;
	}
	const XMMATRIX& view = camera->GetInvViewMatrix();
	const XMMATRIX& proj = camera->GetProjectionMatrix();
	//シェーダーのコンスタントバッファーに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE data;
	LINE_CBUFFER cb;
	if (SUCCEEDED(deviceContext->Map(m_LineConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
	{
		//ワールド、カメラ、射影行列を渡す
		cb.WVP = world * view * proj;
		cb.WVP = XMMatrixTranspose(cb.WVP);
		//カラーを渡す
		cb.Color = m_LineColor;
		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));
		deviceContext->Unmap(m_LineConstantBuffer, 0);
	}
	//このコンスタントバッファーを、どのシェーダーで使うかを指定
	deviceContext->VSSetConstantBuffers(0, 1, &m_LineConstantBuffer);//バーテックスバッファーで使う
	deviceContext->PSSetConstantBuffers(0, 1, &m_LineConstantBuffer);//ピクセルシェーダーでの使う
																		   //バーテックスバッファーをセット
	UINT stride = sizeof(Vector3);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_LineVertexBuffer, &stride, &offset);

	//頂点インプットレイアウトをセット
	deviceContext->IASetInputLayout(m_LineVertexLayout);
	//プリミティブ・トポロジーをセット
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//プリミティブをレンダリング
	deviceContext->Draw(2, 0);
}

bool CLine::LineHitCheckSimple(const ECheckCollisionType Type, const Vector3& Start, const Vector3& End, RayCastInfo* CastInfo, CCommonObject* Object)
{
	if (!Object || !CastInfo)
	{
		return false;
	}
	//オブジェクトとの判定を取るか格納
	if (Type == ECheckCollisionType::PLAYER)
	{
		if (!Object->IsCulling() || Object->GetPlayerCollisionTag() == ECollisionTagName::NO_LINE_HIT || Object->GetPlayerCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
		{
			return false;
		}
	}
	else if (Type == ECheckCollisionType::OBJECT)
	{
		if (Object->GetObjectCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
		{
			return false;
		}
		if (Object->GetObjectCollisionTag() != ECollisionTagName::OBJECT_COLISION)
		{
			if (Object->GetObjectCollisionTag() != ECollisionTagName::ALL_COLISION)
			{
				return false;
			}
		}
	}
	else if (Type == ECheckCollisionType::CAMERA)
	{
		if (!Object->IsCulling() || Object->GetPlayerCollisionTag() == ECollisionTagName::NO_LINE_HIT || Object->GetPlayerCollisionTag() == ECollisionTagName::NO_COLLISION_HIT)
		{
			Object->SetAlpha(1.0f);
			return false;
		}
		else if (Object->GetCameraCollisionTag() == ECollisionTagCameraName::NONE)
		{
			Object->SetAlpha(1.0f);
			return false;
		}
		else if (Object->GetCameraCollisionTag() == ECollisionTagCameraName::CLEAR)
		{
			Object->SetAlpha(1.0f);
		}
	}
	else if (Type == ECheckCollisionType::EDITOR)
	{
		if (!Object->IsCulling())
		{
			return false;
		}
	}

	const Vector3 worldLineStart = Start;
	const Vector3 worldLineEnd = End;

	bool hit = false;
	XMMATRIX mat;

	std::vector<CollisionMesh>* collision_mesh = Object->GetNormalModelMeshCollision();
	int size = (int)collision_mesh->size();

	const XMMATRIX invTargetMat = XMMatrixInverse(nullptr, Object->GetWorld());
	const Vector3 localStart = Vector3::TransformCoord(worldLineStart, invTargetMat);
	const Vector3 localEnd = Vector3::TransformCoord(worldLineEnd, invTargetMat);

	for (int i = 0; i < size; i++)
	{
		RayCastInfo addInfo;
		if (!JudgeFace(localStart, localEnd, &(*collision_mesh)[i], &addInfo)) continue;

		// ローカル -> ワールドに変換
		Vector3 worldPoint = Vector3::TransformCoord(addInfo.Point, Object->GetWorld());
		XMMATRIX invTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, Object->GetWorld()));
		Vector3 worldNormal = Vector3::TransformNormal(addInfo.Normal, invTranspose);
		worldNormal.Normalize();
		float worldDistance = (worldPoint - worldLineStart).Length();

		addInfo.Point = worldPoint;
		addInfo.Normal = worldNormal;
		addInfo.Distance = worldDistance;

		if (!hit)
		{
			*CastInfo = addInfo;
			mat = Object->GetWorld();
			hit = true;
		}
		else if (CastInfo->Distance > addInfo.Distance)
		{
			mat = Object->GetWorld();
			*CastInfo = addInfo;
		}

		//コリジョンタグを設定
		if (Type == ECheckCollisionType::PLAYER)
		{
			CastInfo->CollisionTag = Object->GetPlayerCollisionTag();
			CastInfo->StepsCollisionTag = Object->GetStepsCollisionTag();
		}
		else if (Type == ECheckCollisionType::OBJECT)
		{
			CastInfo->CollisionTag = Object->GetObjectCollisionTag();
		}
		else if (Type == ECheckCollisionType::CAMERA)
		{
			CastInfo->CameraCollisionTag = Object->GetCameraCollisionTag();
			//半透明オブジェクトはカメラとの押し出し判定を行う必要が無いためhitを無効化し半透明にする
			if (Object->GetCameraCollisionTag() == ECollisionTagCameraName::CLEAR)
			{
				Object->SetAlpha(0.5f);
				hit = false;
			}
		}
	}

	if (hit)
	{
		CastInfo->Distance = (CastInfo->Point - worldLineStart).Length();
		CastInfo->Normal.Normalize();
		m_LineColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	return hit;
}

bool CLine::LineHitCheck(const ECheckCollisionType Type, const Vector3& Start, const Vector3& End, RayCastInfo* CastInfo)
{
	CSceneManager* sceneManager = CSceneManager::GetInstance();
	if (!sceneManager)
	{
		return false;
	}
	std::weak_ptr<CScene> scene = sceneManager->GetScene();
	if (scene.expired())
	{
		return false;
	}
	std::vector<CCommonObject*> models;
	scene.lock()->GetGameObjects<CCommonObject>(ESceneObjectType::MESH_OBJECT, models);
	bool isHit = false;

	float distance = FLT_MAX;
	for (CCommonObject* model : models)
	{
		RayCastInfo info;
		if (LineHitCheckSimple(Type, Start, End, &info, model))
		{
			isHit = true;
			//最短距離のオブジェクトを格納する
			if (info.Distance < distance)
			{
				distance = info.Distance;
				*CastInfo = info;
			}
		}
	}

	return isHit;
}

bool CLine::LineHitCheck(const Vector3& Start, const Vector3& End, RayCastInfo* CastInfo, CCommonObject* Object)
{
	return LineHitCheckSimple(ECheckCollisionType::EDITOR, Start, End, CastInfo, Object);
}

bool CLine::JudgeFace(const Vector3& Start, const Vector3& End, CollisionMesh* CollisionMesh, RayCastInfo* CastInfo)
{
	if (!CollisionMesh)
	{
		return false;
	}
	// 法線
	const Vector3 normal = CollisionMesh->Normal;

	// d を計算
	const float d = -((normal.x * CollisionMesh->Point[0].x) + (normal.y * CollisionMesh->Point[0].y) + (normal.z * CollisionMesh->Point[0].z));
	const float d1 = (normal.x * End.x) + (normal.y * End.y) + (normal.z * End.z);
	const float d2 = (normal.x * Start.x) + (normal.y * Start.y) + (normal.z * Start.z);
	//パラメトリック方程式の媒介変数 t を解く。
	const float t = -((d1 + d) / (d2 - d1));
	//t = 0.0792653486;
	// t が０から１の間であるなら交差していることになる（この時点では、まだ無限遠平面との交差）
	if (t >= 0 && t <= 1.0)
	{
		//交点座標を得る　ｔが分かっていれば両端点から簡単に求まる
		Vector3 hitPoint;
		hitPoint = (Start * t) + (End * (1 - t));

		//交点が三角形の内か外かを判定　（ここで内部となれば、完全な交差となる）
		if (IsInside(hitPoint, CollisionMesh->Point[0], CollisionMesh->Point[1], CollisionMesh->Point[2], CollisionMesh->Normal) == true)
		{
			CastInfo->Point = hitPoint;
			CastInfo->Normal = CollisionMesh->Normal;
			CastInfo->Distance = (hitPoint - Start).Length();
			return true;
		}
	}

	return false;
}

bool CLine::IsInside(const Vector3& Point, const Vector3& P1, const Vector3& P2, const Vector3& P3, const Vector3& Normal)
{
	//辺ベクトル
	Vector3 vAB, vBC, vCA;
	
	vAB = P2 - P1;
	vBC = P3 - P2;
	vCA = P1 - P3;
	//辺ベクトルと「頂点から交点へ向かうベクトル」との、それぞれの外積用
	Vector3 vCrossAB, vCrossBC, vCrossCA;
	//「外積結果のベクトル」と平面法線ベクトルとの、それぞれの内積用
	float fAB, fBC, fCA;

	// 各頂点から交点Iに向かうベクトルをvVとする
	Vector3 vV;
	// 辺ABベクトル（頂点Bベクトル-頂点Aベクトル)と、頂点Aから交点Iへ向かうベクトル、の外積を求める
	vV = Point - P1;
	vCrossAB = Vector3::Cross(vAB, vV);
	// 辺BCベクトル（頂点Cベクトル-頂点Bベクトル)と、頂点Bから交点Iへ向かうベクトル、の外積を求める
	vV = Point - P2;
	vCrossBC = Vector3::Cross(vBC, vV);
	// 辺CAベクトル（頂点Aベクトル-頂点Cベクトル)と、頂点Cから交点Iへ向かうベクトル、の外積を求める
	vV = Point - P3;
	vCrossCA = Vector3::Cross(vCA, vV);
	// それぞれの、外積ベクトルとの内積を計算する
	fAB = Vector3::Dot(Normal, vCrossAB);
	fBC = Vector3::Dot(Normal, vCrossBC);
	fCA = Vector3::Dot(Normal, vCrossCA);
	
	// ３つの内積結果のうち、一つでもマイナス符号のものがあれば、交点は外にある。
	if (fAB >= 0 && fBC >= 0 && fCA >= 0)
	{
		// 交点は、面の内にある
		return true;
	}
	// 交点は面の外にある
	return false;
}