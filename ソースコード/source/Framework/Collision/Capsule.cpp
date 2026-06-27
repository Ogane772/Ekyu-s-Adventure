#include "Capsule.h"
#include "../Renderer/Renderer.h"
#include "../Scene/SceneManager.h"
#include "../Model/StaticMesh.h"
#include "../TextureLoader/TextureTypes.h"
#include "../System/SystemContext.h"

CCapsule::CCapsule(const Vector3& UpPosition, const Vector3& DownPosition, const Vector3& Vector, const float Radius, const UINT Slices, const UINT Stacks_1_2)
{
	m_CapsuleData.UpSpherePosition = DefaultUpSpherePosition = UpPosition;
	m_CapsuleData.DownSpherePosition = DefaultDownSpherePosition = DownPosition;
	m_CapsuleData.CapsuleVector = Vector;
	m_CapsuleData.Radius = Radius;
	m_Texture = LoadTexture(ETextureType::BODY2);
	m_Model = std::make_unique<CStaticMesh>(EStaticMeshType::SPHERE);

	//以下デバッグ描画用のカプセルの頂点、インデックスの作成
	 
	// 分割数
	const float invSlices = 1.0f / Slices;
	const float invStacks = 1.0f / Stacks_1_2;

	//--  カプセル座標の単位ベクトルを作成する  --//
	Vector3 uz = UpPosition - DownPosition;
	Vector3 uy;			// vy : vzとvと直角なベクトル
	uy = Vector3::Cross(uz, Vector);
	Vector3 ux;			// vx : vzとvyと直角なベクトル
	ux = Vector3::Cross(uz, uy);
	// vz・vyが0ベクトルでなければvxも0ベクトルではないのでチェックは不要

	// ベクトルの大きさをワールド座標でのradiusにする
	ux.Normalize();
	ux = Vector3::Scale(ux, -Radius);
	uy.Normalize();
	uy = Vector3::Scale(uy, Radius);
	uz.Normalize();
	uz = Vector3::Scale(uz, Radius);
	// 頂点バッファの数(格子点+端点)
	const UINT numberOfVB = (1 + Slices * Stacks_1_2) * 2;

	//--  頂点計算  --//
	VERTEX_3D* capsuleDataVertex = new VERTEX_3D[numberOfVB];
	if (!capsuleDataVertex)
	{
		return;
	}
	// 上部のスフィア側の計算
	SetVertex(capsuleDataVertex, UpPosition, uz);
	for (UINT i = 0; i < Stacks_1_2; i++)
	{
		const float t = (XM_PI * 0.5f) * invStacks*(i + 1);
		const float r = sinf(t);
		const Vector3 vx = ux * r;
		const Vector3 vy = uy * r;
		const Vector3 vz = uz * cosf(t);

		for (UINT j = 0; j < Slices; j++)
		{
			const float u = (XM_PI*2.0f)*invSlices*j;
			const Vector3 vxCos = vx * cosf(u);
			const Vector3 vySin = vy * sinf(u);
			const Vector3 v = vxCos + vySin + vz;
			SetVertex(capsuleDataVertex, UpPosition, v);
		}
	}

	// 下部のスフィア側の計算
	for (UINT i = 0; i< Stacks_1_2; i++)
	{
		const float t = (XM_PI*0.5f)*invStacks*i;
		const float r = cosf(t);
		const Vector3 vx = ux * r;
		const Vector3 vy = uy * r;
		const Vector3 vz = uz * -sinf(t);

		for (UINT j = 0; j< Slices; j++)
		{
			const float u = (XM_PI*2.0f)*invSlices*j;
			const Vector3 vxCos = vx * cosf(u);
			const Vector3 vySin = vy * sinf(u);
			const Vector3 v = vxCos + vySin + vz;
			SetVertex(capsuleDataVertex, DownPosition, v);
		}
	}

	SetVertex(capsuleDataVertex, DownPosition, -uz);

	{
		//バッファーのサイズを調べる
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * numberOfVB;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		//頂点バッファ作成
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = capsuleDataVertex;
		//m_VertexBufferにデータが入る
		CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}
	delete[] capsuleDataVertex;

	const UINT numberOfTriangle =Slices * Stacks_1_2 * 2 * 2;
	const UINT numberOfIB = numberOfTriangle * 3;
	// インデックスバッファの数(片面あたりの四角形ポリゴンの数*2+端点周りの三角形ポリゴンの数*2+筒部のポリゴン数)
	m_IndexNum = numberOfIB;
	unsigned short* pI = new unsigned short[numberOfIB];
	if (!pI)
	{
		return;
	}
	int count = 0;
	// P1側端点周りの計算
	for (UINT j = 0; j < Slices; j++)
	{
		pI[count++] = 0;
		pI[count++] = 1 + j;
		pI[count++] = 1 + (j + 1) % Slices;
	}

	// P1-P2間の計算
	UINT imax = Stacks_1_2 * 2 - 1;
	for (UINT i = 0; i < imax; i++)
	{
		UINT ibase = 1 + Slices*i;
		for (UINT j = 0; j< Slices; j++)
		{
			UINT jnext = (j + 1) % Slices;
			pI[count++] = ibase + j;
			pI[count++] = ibase + j + Slices;
			pI[count++] = ibase + jnext;

			pI[count++] = ibase + jnext;
			pI[count++] = ibase + j + Slices;
			pI[count++] = ibase + jnext + Slices;
		}
	}

	// P2側端点周りの計算
	UINT ibase = 1 + Slices*imax;
	UINT ilast = numberOfVB - 1;
	for (UINT j = 0; j < Slices; j++)
	{
		pI[count++] = ibase + j;
		pI[count++] = ilast;
		pI[count++] = ibase + (j + 1) % Slices;
	}
	{
		//バッファーのサイズを調べる
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned short)* m_IndexNum;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		//頂点バッファ作成
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = pI;
		CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	}	
	delete[] pI;
}

CCapsule::~CCapsule()
{
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
	}
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
	}
	ReleaseTexture(m_Texture);
}

void CCapsule::SetVertex(VERTEX_3D* Vertex, const Vector3& Position, const Vector3& Vector)
{
	if (!Vertex)
	{
		return;
	}
	Vertex[m_VertexNum].Position.x = (Position.x + Vector.x);
	Vertex[m_VertexNum].Position.y = (Position.y + Vector.y);
	Vertex[m_VertexNum].Position.z = (Position.z + Vector.z);
	Vertex[m_VertexNum].Normal.x = (Vector.x);
	Vertex[m_VertexNum].Normal.y = (Vector.y);
	Vertex[m_VertexNum].Normal.z = (Vector.z);
	Vertex[m_VertexNum].Diffuse = XMFLOAT4(1.0, 1.0, 1.0, 1.0);
	Vertex[m_VertexNum].TexCoord.x = 1.0f;
	Vertex[m_VertexNum].TexCoord.y = 1.0f;
	m_VertexNum++;
}

void CCapsule::Update(const Vector3 Position)
{	
	m_CapsuleData.UpSpherePosition = DefaultUpSpherePosition + Position;
	m_CapsuleData.DownSpherePosition = DefaultDownSpherePosition + Position;
	m_Position = Position;
}

void CCapsule::CollisionDraw()
{
	if (!m_Model)
	{
		return;
	}
	CRenderer::SetShader();
	// マトリクス設定
	XMMATRIX world;
	world = XMMatrixScaling(1, 1, 1);
	world *= XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	CRenderer::SetWorldMatrix(world);

	CRenderer::SetVertexBuffers(m_VertexBuffer);
	// インデックスバッファ設定
	CRenderer::SetIndexBuffer(m_IndexBuffer);
	// テクスチャ設定
	CRenderer::SetTexture(m_Texture);
	CRenderer::SetWireframeMode();
	CRenderer::DrawIndexed(m_IndexNum, 0, 0);
	CRenderer::SetSolidMode();

	//カプセルの円の中心点を描画
	world = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	world *= XMMatrixTranslation(m_Position.x + DefaultUpSpherePosition.x, m_Position.y + DefaultUpSpherePosition.y, m_Position.z + DefaultUpSpherePosition.z);
	m_Model->Draw(world);

	world = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	world *= XMMatrixTranslation(m_Position.x + DefaultDownSpherePosition.x, m_Position.y + DefaultDownSpherePosition.y, m_Position.z + DefaultDownSpherePosition.z);
	m_Model->Draw(world);
}

void CCapsule::Draw()
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

	CollisionDraw();
}

void CCapsule::SingleDraw()
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (!system->IsDebugCollisionRender() && system->IsEditMode())
	{
		CollisionDraw();
	}
}

void CCapsule::SetCapsulePosition(const Vector3& UpSpherePosition, const Vector3& DownSpherePosition)
{
	DefaultUpSpherePosition = UpSpherePosition;
	DefaultDownSpherePosition = DownSpherePosition;
}

void CCapsule::SetCapsuleSize(const Vector3& UpSpherePosition, const Vector3& DownSpherePosition, const  float Radius)
{
	DefaultUpSpherePosition = UpSpherePosition;
	DefaultDownSpherePosition = DownSpherePosition;
	m_CapsuleData.Radius = Radius;
}
