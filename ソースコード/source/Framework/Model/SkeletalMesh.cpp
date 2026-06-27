#include "SkeletalMesh.h"
#include <D3DX11async.h>
#include "../Renderer/Renderer.h"
#include "../TextureLoader/TextureLoader.h"
#include "../Camera/Camera.h"
#include "../Scene/SceneManager.h"
#include "../Light/DirectionalLight.h"
#include "../System/SystemContext.h"
#include "../Shadow/ShadowMap.h"
#include "../Shadow/ShadowType.h"

#pragma comment (lib, "assimp.lib")
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

std::unordered_map<ESkeletalMeshType, std::string> CSkeletalMesh::m_ModelFileList;
ID3D11VertexShader* CSkeletalMesh::m_VertexShader = nullptr;
ID3D11InputLayout* CSkeletalMesh::m_VertexLayout = nullptr;
ID3D11Buffer* CSkeletalMesh::m_LightConstantBuffer = nullptr;
ID3D11Buffer* CSkeletalMesh::m_MatrixConstantBuffer = nullptr;
ID3D11Buffer* CSkeletalMesh::m_BoneConstantBuffer = nullptr;
ID3D11Buffer* CSkeletalMesh::m_MaterialConstantBuffer = nullptr;

namespace
{
	constexpr float ANIMATION_FRAME = 60.0f;//アニメーションのフレームレート
	constexpr float BLEND_END_TIME = 0.17f;//ブレンドを終了として扱う値
}

void CSkeletalMesh::ShaderInit()
{
	m_ModelFileList =
	{
		{  ESkeletalMeshType::PLAYER,"asset/model/skin/player/player.fbx" },
		{  ESkeletalMeshType::SLIME,"asset/model/skin/enemy/slime.fbx" },
		{  ESkeletalMeshType::MONKEY,"asset/model/skin/enemy/monkey.fbx" },
		{  ESkeletalMeshType::THORN_MAN,"asset/model/skin/enemy/thorn_man.fbx" },
		{  ESkeletalMeshType::GORILLA,"asset/model/skin/enemy/gorilla.fbx" },
	};
	ID3D11Device* device = CRenderer::GetDevice();
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!device || !deviceContext)
	{
		return;
	}
	//D3D11関連の初期化
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/SkinShaderVS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(device->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_VertexShader)))
	{
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}
	//頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	if (FAILED(device->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_VertexLayout)))
	{
		return;
	}
	//頂点インプットレイアウトをセット
	deviceContext->IASetInputLayout(m_VertexLayout);

	SAFE_RELEASE(pCompiledShader);

	D3D11_BUFFER_DESC cb;
	//コンスタントバッファー0作成
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(LIGHT);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, nullptr, &m_LightConstantBuffer)))
	{
		return;
	}
	//コンスタントバッファー1作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ShaderSkeletalMatrix);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, nullptr, &m_MatrixConstantBuffer)))
	{
		return;
	}
	//コンスタントバッファーボーン用　作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ShaderBoneData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, nullptr, &m_BoneConstantBuffer)))
	{
		return;
	}
	//コンスタントバッファーマテリアル用　作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(MATERIAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(device->CreateBuffer(&cb, nullptr, &m_MaterialConstantBuffer)))
	{
		return;
	}
}

void CSkeletalMesh::ShaderUnInit()
{
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_LightConstantBuffer);
	SAFE_RELEASE(m_MatrixConstantBuffer);
	SAFE_RELEASE(m_BoneConstantBuffer);
	SAFE_RELEASE(m_MaterialConstantBuffer);
}

CSkeletalMesh::CSkeletalMesh(const ESkeletalMeshType Type)
{
	Load(m_ModelFileList[Type]);
}

CSkeletalMesh::CSkeletalMesh(const std::string& FileName)
{
	Load(FileName);
}

CSkeletalMesh::~CSkeletalMesh()
{
	
	for (unsigned int m = 0; m < m_Scene->mNumMeshes; m++)
	{
		if (m_Mesh[m].VertexBuffer)
		{
			m_Mesh[m].VertexBuffer->Release();
			m_Mesh[m].VertexBuffer = nullptr;
		}
		if (m_Mesh[m].IndexBuffer)
		{
			m_Mesh[m].IndexBuffer->Release();
			m_Mesh[m].IndexBuffer = nullptr;
		}
		if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
		{
			textureLoader->Release(m_Mesh[m].Texture);
		}
	}
	delete[] m_Mesh;
	if (m_Scene)
	{
		aiReleaseImport(m_Scene);
	}
}

void CSkeletalMesh::UpdateAnimation(const float AnimationTime)
{
	if (!m_Scene || !m_AiAnimation)
	{
		return;
	}
	aiNodeAnim* nodeAnim;
	BoneMatrix* bone;
	aiQuaternion rotation;
	aiVector3D position;
	aiVector3D scale;
	std::string nodeName;

	for (unsigned int c = 0; c < m_AiAnimation->mNumChannels; c++)
	{
		nodeAnim = m_AiAnimation->mChannels[c];
		if (!nodeAnim)
		{
			continue;
		}
		nodeName = nodeAnim->mNodeName.C_Str();
		if (!m_Bone.contains(nodeName))
		{
			continue;
		}

		//各種アニメーションキーを取得
		SamplingScaling(nodeAnim, AnimationTime, scale);
		SamplingRotation(nodeAnim, AnimationTime, rotation);
		SamplingPosition(nodeAnim, AnimationTime, position);

		// DirectX用にスケールはzとyを反転
		bone = &m_Bone[nodeName];
		if (bone)
		{
			bone->AnimationMatrix = aiMatrix4x4(aiVector3D(scale.x, scale.z, scale.y), rotation, position);
		}
	}

	UpdateBoneMatrix(m_Scene->mRootNode, aiMatrix4x4());
}

void CSkeletalMesh::UpdateAnimationBlend(const int PrevAnime, const int CurrentAnime, const float BlendTime, const float AnimationTime)
{
	if (!m_Scene)
	{
		return;
	}
	aiAnimation* prevAnimation = m_Scene->mAnimations[PrevAnime];
	aiAnimation* currentAnimation = m_Scene->mAnimations[CurrentAnime];
	if (!prevAnimation || !currentAnimation)
	{
		return;
	}
	aiQuaternion prevRotation, currentRotation, outRotation;
	aiVector3D prevPosition, currentPosition, outPosition;
	aiVector3D prevScale, currentScale, outScale;
	std::string nodeName;
	BoneMatrix* bone;

	for (unsigned int c = 0; c < currentAnimation->mNumChannels; c++)
	{
		aiNodeAnim* nodeCurrentAnim = currentAnimation->mChannels[c];
		if (!nodeCurrentAnim)
		{
			continue;
		}
		nodeName = nodeCurrentAnim->mNodeName.C_Str();
		if (!m_Bone.contains(nodeName))
		{
			continue;
		}
		aiNodeAnim* nodePrevAnim = nullptr;
		// prev のチャネルが存在するかチェック
		if (PrevAnime < static_cast<int>(prevAnimation->mNumChannels))
		{
			nodePrevAnim = prevAnimation->mChannels[c];
		}
		else
		{
			// 存在しなければ現在のチャンネルを参照する
			nodePrevAnim = nodeCurrentAnim;
		}

		const float prevTime = m_AnimationData.PrevTime;
		const float blendAlpha = BlendTime / BLEND_END_TIME;

		// 1つ前のアニメーションと現在のアニメーション、それぞれのキーを補間する
		SamplingRotation(nodePrevAnim, prevTime, prevRotation);
		SamplingRotation(nodeCurrentAnim, AnimationTime, currentRotation);
		outRotation = prevRotation;
		aiQuaternion::Interpolate(outRotation, prevRotation, currentRotation, blendAlpha);

		SamplingPosition(nodePrevAnim, prevTime, prevPosition);
		SamplingPosition(nodeCurrentAnim, AnimationTime, currentPosition);
		outPosition = prevPosition + (currentPosition - prevPosition) * blendAlpha;

		SamplingScaling(nodePrevAnim, prevTime, prevScale);
		SamplingScaling(nodeCurrentAnim, AnimationTime, currentScale);
		outScale = prevScale + (currentScale - prevScale) * blendAlpha;

		bone = &m_Bone[nodeName];
		if (bone)
		{
			bone->AnimationMatrix = aiMatrix4x4(aiVector3D(outScale.x, outScale.z, outScale.y), outRotation, outPosition);
		}
	}

	UpdateBoneMatrix(m_Scene->mRootNode, aiMatrix4x4());
}

void CSkeletalMesh::Update(float DeltaTime)
{
	if (!m_AiAnimation)
	{
		return;
	}

	//アニメーション終了時間
	const float duration = static_cast<float>(m_AiAnimation->mDuration);
	//現在のフレームのアニメーション経過時間
	const float addTime = DeltaTime * m_AnimationData.AnimationSpeed * ANIMATION_FRAME;

	//アニメーションが終了してなければアニメーションを進める
	if (!m_AnimationData.IsEnd)
	{
		m_AnimationData.Time += addTime;
		
	}
	m_AnimationData.BlendTime += DeltaTime;

	//ループ時はアニメーション終了時間を超えたら超えた分だけ戻す
	if (m_AnimationData.IsLoop)
	{
		if (m_AnimationData.Time >= duration)
		{
			m_AnimationData.Time = fmod(m_AnimationData.Time, duration);
		}
	}
	//非ループ時はアニメーション終了時間を超えたら終了フラグを立てる
	else if (!m_AnimationData.IsEnd)
	{
		if (m_AnimationData.Time >= duration)
		{
			m_AnimationData.Time = duration;
			m_AnimationData.IsEnd = true;
		}
	}

	const bool isBlendEnd = m_AnimationData.BlendTime >= BLEND_END_TIME;
	if (!isBlendEnd)
	{
		m_AnimationData.BlendTime = (std::min)(m_AnimationData.BlendTime, BLEND_END_TIME);
	}

	if (isBlendEnd)
	{
		UpdateAnimation(m_AnimationData.Time);
	}
	else
	{
		UpdateAnimationBlend(m_AnimationData.PrevAnimeNumber, m_AnimationData.CurrentAnimeNumber, m_AnimationData.BlendTime, m_AnimationData.Time);
	}
}

void CSkeletalMesh::Draw(const XMMATRIX Matrix)
{
	if (!m_Scene)
	{
		return;
	}
	if (!m_Scene->mRootNode)
	{
		return;
	}
	DrawMesh(m_Scene->mRootNode, Matrix);
}

void CSkeletalMesh::ShadowOnModelDraw(const EShadowType Type, const XMMATRIX Matrix)
{
	if (!m_Scene)
	{
		return;
	}
	if (!m_Scene->mRootNode)
	{
		return;
	}
	//ここで使うシェーダーは、影描画も行う。
	CShadowMap::SetSkinConstantBuffers();
	CShadowMap::SetSkinLightCameraPosition();
	//このパスで使用するシェーダーの登録
	CShadowMap::SetSkinMeshShader();
	//個々のモデルレンダリング
	DrawShadowMesh(Type, m_Scene->mRootNode, Matrix);
}

void CSkeletalMesh::ShadowDraw(const EShadowType Type, const XMMATRIX Matrix)
{
	if (!m_Scene)
	{
		return;
	}
	if (!m_Scene->mRootNode)
	{
		return;
	}
	//shadow用コンスタントバッファー設定
	//フレーム単位のコンスタントバッファーに各種データをセット
	CShadowMap::SetSkinConstantBuffers();
	//ライトメッシュのワールド行列からライト視点のビュー行列を作る
	CShadowMap::SetSkinLightCameraPosition();
	//1パス目　シーンを　”深度テクスチャー上に”　レンダリングする。
	//このパスで使用するシェーダーの登録
	CShadowMap::SetSkinDepthShader();
	//個々のモデルレンダリング	
	DrawShadowMesh(Type, m_Scene->mRootNode, Matrix);
}

void CSkeletalMesh::DrawShadowMesh(const EShadowType Type, const aiNode* Node, const XMMATRIX Matrix)
{
	if (!Node)
	{
		return;
	}
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (system->IsDebugDisableAnimationModelRender())
	{
		return;
	}

	unsigned int m;
	aiMesh* mesh;

	for (unsigned int n = 0; n < Node->mNumMeshes; n++)
	{
		m = Node->mMeshes[n];
		mesh = m_Scene->mMeshes[m];
		if (!mesh)
		{
			continue;
		}
		CShadowMap::SetSkinMeshMatrix(Type, mesh->mNumBones, m_BoneList, Matrix);

		//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
		UINT stride = sizeof(ShaderVertexData);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_Mesh[m].VertexBuffer, &stride, &offset);

		//インデックスバッファーをセット
		deviceContext->IASetIndexBuffer(m_Mesh[m].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		CShadowMap::SetSkinMeshMaterial(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(m_Material.Alpha.r, 0.0f, 0.0f, 0.0f));
		// テクスチャ設定
		CRenderer::SetTexture(m_Mesh[m].Texture);
		CRenderer::DrawIndexed(m_Mesh[m].IndexNum, 0, 0);
	}

	for (UINT i = 0; i < Node->mNumChildren; i++)
	{
		DrawShadowMesh(Type, Node->mChildren[i], Matrix);
	}
}

void CSkeletalMesh::UpdateBoneMatrix(const aiNode* Node, const aiMatrix4x4 Matrix)
{
	if (!Node)
	{
		return;
	}
	const std::string nodeName = Node->mName.C_Str();
	BoneMatrix* bone = &m_Bone[nodeName];
	if (!bone)
	{
		return;
	}
	aiMatrix4x4 world = Matrix * bone->AnimationMatrix;//bone->AnimationMatrix=自分のマトリクス Matrix=親のマトリクス
	bone->Matrix = world * bone->OffsetMatrix;
	const int size = static_cast<int>(m_BoneList.size());

	for (int j = 0; j < size; j++)
	{//一致するボーンの名前のマトリクスを代入
		if (m_BoneList[j].Name == nodeName)
		{
			m_BoneList[j].Matrix = XMLoadFloat4x4((XMFLOAT4X4*)&bone->Matrix);
			break;
		}
	}

	for (UINT i = 0; i < Node->mNumChildren; i++)
	{
		UpdateBoneMatrix(Node->mChildren[i], world);
	}
}

void CSkeletalMesh::DrawMesh(const aiNode* Node, const XMMATRIX Matrix)
{
	if (!Node)
	{
		return;
	}
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (system->IsDebugDisableAnimationModelRender())
	{
		return;
	}

	aiMesh* mesh;
	//カメラ情報受け取り
	CSceneManager* manager = CSceneManager::GetInstance();
	if (!manager)
	{
		return;
	}
	CCamera* camera = manager->GetCamera();
	if (!camera)
	{
		return;
	}
	Vector3 eye = camera->GetPosition();
	const XMMATRIX& view = camera->GetInvViewMatrix();
	const XMMATRIX& proj = camera->GetProjectionMatrix();
	for (UINT n = 0; n < Node->mNumMeshes; n++)
	{
		const unsigned int m = Node->mMeshes[n];
		mesh = m_Scene->mMeshes[m];
		if (!mesh)
		{
			continue;
		}

		D3D11_MAPPED_SUBRESOURCE data;

		//使用するシェーダーのセット
		deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
		CRenderer::SetPixelShader();
		//シェーダーにボーン行列を渡す
		if (SUCCEEDED(deviceContext->Map(m_BoneConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
		{
			ShaderBoneData sg;
			for (UINT i = 0; i < mesh->mNumBones; i++)
			{
				sg.Bones[i] = m_BoneList[i].Matrix;
			}
			memcpy_s(data.pData, data.RowPitch, (void*)&sg, sizeof(ShaderBoneData));
			deviceContext->Unmap(m_BoneConstantBuffer, 0);
		}
		deviceContext->VSSetConstantBuffers(2, 1, &m_BoneConstantBuffer);

		//バーテックスバッファーをセット（バーテックスバッファーは一つでいい）
		UINT stride = sizeof(ShaderVertexData);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_Mesh[m].VertexBuffer, &stride, &offset);

		//頂点インプットレイアウトをセット
		deviceContext->IASetInputLayout(m_VertexLayout);

		//インデックスバッファーをセット
		deviceContext->IASetIndexBuffer(m_Mesh[m].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		//ディレクショナルライトをシェーダーに渡す
		if (SUCCEEDED(deviceContext->Map(m_LightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
		{
			CDirectionalLight* directionalLight = directionalLight->GetInstance();
			if (!directionalLight)
			{
				return;
			}
			LIGHT light = directionalLight->GetLight();
			light.Direction.y *= -1;
			memcpy_s(data.pData, data.RowPitch, (void*)&light, sizeof(LIGHT));
			deviceContext->Unmap(m_LightConstantBuffer, 0);
		}
		deviceContext->VSSetConstantBuffers(0, 1, &m_LightConstantBuffer);

		//シェーダーにカメラの行列とモデル行列を渡す
		if (SUCCEEDED(deviceContext->Map(m_MatrixConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
		{
			ShaderSkeletalMatrix shaderMatrix;
			shaderMatrix.World = XMMatrixTranspose(Matrix);
			shaderMatrix.View = XMMatrixTranspose(view);
			shaderMatrix.Projection = XMMatrixTranspose(proj);
			memcpy_s(data.pData, data.RowPitch, (void*)&shaderMatrix, sizeof(ShaderSkeletalMatrix));
			shaderMatrix.Eye = XMFLOAT4(eye.x, eye.y, eye.z, 0);
			deviceContext->Unmap(m_MatrixConstantBuffer, 0);
		}
		deviceContext->VSSetConstantBuffers(1, 1, &m_MatrixConstantBuffer);

		//シェーダーにマテリアルを渡す
		if (SUCCEEDED(deviceContext->Map(m_MaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
		{
			m_Material.Ambient = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_Material.Diffuse = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_Material.Specular = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
			memcpy_s(data.pData, data.RowPitch, (void*)&m_Material, sizeof(MATERIAL));
			deviceContext->Unmap(m_MaterialConstantBuffer, 0);
		}
		deviceContext->VSSetConstantBuffers(3, 1, &m_MaterialConstantBuffer);

		// テクスチャ設定
		CRenderer::SetTexture(m_Mesh[m].Texture);
		CRenderer::DrawIndexed(m_Mesh[m].IndexNum, 0, 0);
	}

	for (UINT i = 0; i < Node->mNumChildren; i++)
	{
		DrawMesh(Node->mChildren[i], Matrix);
	}
	CRenderer::SetShader();
}

void CSkeletalMesh::Load(const std::string& FileName)
{
	ID3D11Device* device = CRenderer::GetDevice();
	if (!device)
	{
		return;
	}
	m_Scene = aiImportFile(FileName.c_str(), aiProcess_Triangulate
		| aiProcess_MakeLeftHanded
		| aiProcess_FlipUVs
		| aiProcess_FlipWindingOrder
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeMeshes
		| aiProcess_LimitBoneWeights
		| aiProcess_RemoveRedundantMaterials);

	if (!m_Scene)
	{
		return;
	}

	CreateBone(m_Scene->mRootNode);
	CreateBoneNormal(m_Scene->mRootNode);
	unsigned int meshNum = m_Scene->mNumMeshes;
	m_Mesh = new ShaderMeshData[meshNum];

	for (unsigned int m = 0; m < meshNum; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];
		if (!mesh)
		{
			continue;
		}
		ShaderVertexData* skinVertex = new ShaderVertexData[mesh->mNumFaces * 3];
		unsigned short* index = new unsigned short[mesh->mNumFaces * 3];
		aiMaterial* material = m_Scene->mMaterials[mesh->mMaterialIndex];
		if (!material)
		{
			continue;
		}
		//	マテリアル設定
		aiColor4D diffuse;
		aiColor4D Ambient;
		material->Get(AI_MATKEY_COLOR_AMBIENT, Ambient);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &Ambient);
		m_Material.Diffuse = COLOR(diffuse.r, diffuse.g, diffuse.b, 1.0f);
		m_Material.Ambient = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		m_Material.Specular = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		std::vector<std::vector<ShaderWeightData>> weightList;
		weightList.resize(mesh->mNumFaces * 3);

		//ボーンを探し出し。ウェイトとインデックスを取得
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];
			if (!bone)
			{
				continue;
			}
			const char* boneName = bone->mName.C_Str();
			//ノードの名前はCreateBoneで知っているので名前を配列に入れる
			m_Bone[boneName].OffsetMatrix = bone->mOffsetMatrix;
			const int boneIndex = BoneNameIndex(boneName);
			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				ShaderWeightData weightData;
				weightData.Index = boneIndex;
				weightData.Weight = bone->mWeights[w].mWeight;
				weightList[bone->mWeights[w].mVertexId].emplace_back(weightData);
			}
		}
		for (unsigned int v = 0; v < mesh->mNumFaces * 3; v++)
		{
			for (int g = 4 - (int)weightList[v].size(); g > 0; g--)
			{
				ShaderWeightData weightData;
				weightList[v].emplace_back(weightData);
			}
		}
		
		//頂点、スキン情報を設定
		int count = 0;
		for (int i = 0; i < (signed)mesh->mNumFaces; i++)
		{
			aiFace* face = &mesh->mFaces[i];
			if (!face)
			{
				continue;
			}
			for (unsigned int k = 0; k < face->mNumIndices; k++)
			{
				//faceのk番目の頂点のインデックスを取得
				const int faceIndex = face->mIndices[k];
				skinVertex[count].Position = XMFLOAT3(mesh->mVertices[faceIndex].x, mesh->mVertices[faceIndex].y, mesh->mVertices[faceIndex].z);
				skinVertex[count].Normal = XMFLOAT3(mesh->mNormals[faceIndex].x, mesh->mNormals[faceIndex].y, mesh->mNormals[faceIndex].z);
				skinVertex[count].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][faceIndex].x, mesh->mTextureCoords[0][faceIndex].y);
				for (int j = 0; j < 4; j++)
				{
					skinVertex[count].BoneIndex[j] = weightList[faceIndex][j].Index;
					skinVertex[count].BoneWeight[j] = weightList[faceIndex][j].Weight;
				}
				index[count] = count;
				count++;
			}
		}
		//テクスチャ読み込み
		for (unsigned int t = 0; t < m_Scene->mNumMaterials; t++)
		{
			aiString path;
			if (m_Scene->mMaterials[t]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				std::string texturePath;
				if (CSystemContext* system = CSystemContext::GetInstance())
				{
					system->ConvertToTextureRelativePath(FileName, path.C_Str(), texturePath);
				}

				if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
				{
					m_Mesh[t].Texture = textureLoader->Load(texturePath);
				}
			}
		}

		//最後にバッファ作成
		{
			//バッファーのサイズを調べる
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ShaderVertexData) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			//頂点バッファ作成
			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = skinVertex;
			//VertexBufferにデータが入る
			device->CreateBuffer(&bd, &sd, &m_Mesh[m].VertexBuffer);
		}
		m_Mesh[m].IndexNum = mesh->mNumFaces * 3;
		{
			//バッファーのサイズを調べる
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * m_Mesh[m].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			//頂点バッファ作成
			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;
			device->CreateBuffer(&bd, &sd, &m_Mesh[m].IndexBuffer);
		}
		delete[] index;
		delete[] skinVertex;
	}
	
	//右の[]はFBXに格納されているアニメーション番号
	m_AiAnimation = m_Scene->mAnimations[m_AnimationData.CurrentAnimeNumber];
}

void CSkeletalMesh::CreateBone(aiNode* Node)
{
	if (!Node)
	{
		return;
	}
	//ノードは中身空っぽだが親子関係を表せる
	//Meshは中身があるが親子関係がわからない
	//なのでMeshにノードを渡せば親子関係がわかる
	for (unsigned int m = 0; m < Node->mNumMeshes; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[Node->mMeshes[m]];
		if (!mesh)
		{
			continue;
		}
		for (unsigned int n = 0; n < mesh->mNumBones; n++)
		{//ボーンを順番に入れておく
			aiBone* bone = mesh->mBones[n];
			if (!bone)
			{
				continue;
			}
			const char* boneName = bone->mName.C_Str();
			if (BoneNameIndex(boneName) == -1)
			{
				BoneNameMatrixData bone_data;
				bone_data.Matrix = XMLoadFloat4x4((XMFLOAT4X4*)&Node->mTransformation);
				bone_data.AnimationMatrix = XMLoadFloat4x4((XMFLOAT4X4*)&Node->mTransformation);
				bone_data.OffsetMatrix = XMLoadFloat4x4((XMFLOAT4X4*)&bone->mOffsetMatrix);
				bone_data.Name = boneName;
				m_BoneList.emplace_back(bone_data);
			}
		}
	}
	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		CreateBone(Node->mChildren[i]);
	}
}

int CSkeletalMesh::BoneNameIndex(const std::string& Name)
{
	for (int i = 0; i < m_BoneList.size(); i++)
	{
		if (m_BoneList[i].Name == Name)
		{
			return i;
		}
	}
	return -1;
}

void CSkeletalMesh::CreateBoneNormal(aiNode* Node)
{
	if (!Node)
	{
		return;
	}

	BoneMatrix bone;
	m_Bone[Node->mName.C_Str()] = bone;
	for (UINT i = 0; i < Node->mNumChildren; i++)
	{
		CreateBone(Node->mChildren[i]);
	}
}

int CSkeletalMesh::SetAnimationChange(const int AnimationNumber)
{
	return SetAnimationChange(false, AnimationNumber, false, 1.0f);
}

int CSkeletalMesh::SetAnimationChange(const bool IsLoop, const int AnimationNumber, const bool IsBlend, const float AnimationSpeed)
{
	if (AnimationNumber == m_AnimationData.CurrentAnimeNumber)
	{
		return AnimationNumber;
	}
	if (IsBlend)
	{
		m_AnimationData.BlendTime = 0.0f;
	}
	else
	{
		m_AnimationData.BlendTime =	BLEND_END_TIME;
	}
	m_AnimationData.PrevAnimeNumber = m_AnimationData.CurrentAnimeNumber;
	m_AnimationData.CurrentAnimeNumber = AnimationNumber;
	m_AiAnimation = m_Scene->mAnimations[AnimationNumber];
	m_AnimationData.PrevTime = m_AnimationData.Time;
	m_AnimationData.Time = 0.0;
	m_AnimationData.IsEnd = false;
	m_AnimationData.IsLoop = IsLoop;
	m_AnimationData.AnimationSpeed = AnimationSpeed;
	return m_AnimationData.CurrentAnimeNumber;
}

void CSkeletalMesh::SamplingPosition(const aiNodeAnim* Channel, const float AnimationTime, aiVector3D& Out)
{
	if (!Channel)
	{
		return;
	}
	if (Channel->mNumPositionKeys == 0) 
	{ 
		Out = aiVector3D(0, 0, 0); 
		return; 
	}

	if (Channel->mNumPositionKeys == 1) 
	{ 
		Out = Channel->mPositionKeys[0].mValue; 
		return; 
	}

	unsigned int idx = 0;
	for (unsigned int i = 0; i + 1 < Channel->mNumPositionKeys; ++i)
	{
		if (AnimationTime < Channel->mPositionKeys[i + 1].mTime)
		{
			idx = i;
			break;
		}
		if (i + 1 == Channel->mNumPositionKeys - 1)
		{
			idx = i;
		}
	}
	const aiVectorKey& A = Channel->mPositionKeys[idx];
	const aiVectorKey& B = Channel->mPositionKeys[(std::min)((unsigned int)idx + 1, Channel->mNumPositionKeys - 1)];
	const float segDur = static_cast<float>(B.mTime - A.mTime);
	const float factor = (segDur > 0.0) ? static_cast<float>((AnimationTime - A.mTime) / segDur) : 0.0f;
	Out = A.mValue + (B.mValue - A.mValue) * factor;
}

void CSkeletalMesh::SamplingRotation(const aiNodeAnim* Channel, const float AnimationTime, aiQuaternion& Out)
{
	if (!Channel)
	{
		return;
	}
	if (Channel->mNumRotationKeys == 0)
	{
		Out = aiQuaternion();
		return;
	}
	if (Channel->mNumRotationKeys == 1) 
	{ 
		Out = Channel->mRotationKeys[0].mValue; 
		return; 
	}

	unsigned int idx = 0;
	for (unsigned int i = 0; i + 1 < Channel->mNumRotationKeys; ++i)
	{
		if (AnimationTime < Channel->mRotationKeys[i + 1].mTime)
		{
			idx = i;
			break;
		}
		if (i + 1 == Channel->mNumRotationKeys - 1)
		{
			idx = i;
		}
	}
	const aiQuatKey& A = Channel->mRotationKeys[idx];
	const aiQuatKey& B = Channel->mRotationKeys[(std::min)((unsigned int)idx + 1, Channel->mNumRotationKeys - 1)];
	const float segDur = static_cast<float>(B.mTime - A.mTime);
	const float factor = (segDur > 0.0) ? static_cast<float>((AnimationTime - A.mTime) / segDur) : 0.0f;
	Out = A.mValue;
	aiQuaternion::Interpolate(Out, A.mValue, B.mValue, factor);
	Out.Normalize();
}

void CSkeletalMesh::SamplingScaling(const aiNodeAnim* Channel, const float AnimationTime, aiVector3D& Out)
{
	if (!Channel)
	{
		return;
	}
	if (Channel->mNumScalingKeys == 0) 
	{ 
		Out = aiVector3D(1, 1, 1); 
		return; 
	}
	if (Channel->mNumScalingKeys == 1) 
	{ 
		Out = Channel->mScalingKeys[0].mValue; 
		return; 
	}

	unsigned int idx = 0;
	for (unsigned int i = 0; i + 1 < Channel->mNumScalingKeys; ++i)
	{
		if (AnimationTime < Channel->mScalingKeys[i + 1].mTime)
		{
			idx = i;
			break;
		}
		if (i + 1 == Channel->mNumScalingKeys - 1)
		{
			idx = i;
		}
	}
	const aiVectorKey& A = Channel->mScalingKeys[idx];
	const aiVectorKey& B = Channel->mScalingKeys[(std::min)((unsigned int)idx + 1, Channel->mNumScalingKeys - 1)];
	const float segDur = static_cast<float>(B.mTime - A.mTime);
	const float factor = (segDur > 0.0) ? static_cast<float>((AnimationTime - A.mTime) / segDur) : 0.0f;
	Out = A.mValue + (B.mValue - A.mValue) * factor;
}