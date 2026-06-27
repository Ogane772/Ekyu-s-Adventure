#include "StaticMesh.h"
#include <D3DX11async.h>
#include "../Scene/SceneManager.h"
#include "../Renderer/Renderer.h"
#include "../TextureLoader/TextureLoader.h"
#include "../Camera/Camera.h"
#include "../Shadow/ShadowMap.h"
#include "../Shadow/ShadowType.h"
#include "../Light/DirectionalLight.h"
#include "../Shader/SkyShader.h"
#include "../System/SystemContext.h"

#pragma comment (lib, "assimp.lib")
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif
#define SAFE_RELEASE(x) if(x){ if(x)x->Release(); x = nullptr;}

namespace
{
	constexpr float DEFAULT_NORMALMAP_POWER = 2.0f;
	const std::string SKY_TEXTURE = "asset/texture/sky.jpg";
	const float SKY_SCALE = 100.0f;//スカイドームの雲の大きさ
	const float SKA_CLOUD_SPEED = 0.001f;//スカイドームの雲の移動速度
}

ID3D11VertexShader* CStaticMesh::m_VertexShader = nullptr;
ID3D11InputLayout* CStaticMesh::m_VertexLayout = nullptr;
ID3D11Buffer* CStaticMesh::m_MatrixAndFogConstantBuffer = nullptr;
ID3D11Buffer* CStaticMesh::m_LightConstantBuffer = nullptr;
ID3D11Buffer* CStaticMesh::m_NormalMapConstantBuffer = nullptr;
ID3D11Buffer* CStaticMesh::m_MaterialConstantBuffer = nullptr;
ID3D11Buffer* CStaticMesh::m_TexScrollConstantBuffer = nullptr;
ID3D11PixelShader* CStaticMesh::m_PixelShader = nullptr;
ID3D11PixelShader* CStaticMesh::m_NoNormalPixelShader = nullptr;
ID3D11VertexShader* CStaticMesh::m_NoShadowVertexShader = nullptr;
ID3D11PixelShader* CStaticMesh::m_NoShadowPixelShader = nullptr;
std::vector<CStaticMesh::StaticMeshData*> CStaticMesh::m_LoadMeshList;
std::unordered_map<EStaticMeshType, std::string> CStaticMesh::m_ModelFileList;

void CStaticMesh::InitModelData()
{
	m_ModelFileList =
	{
		{ EStaticMeshType::BOX,"asset/model/collision/box.fbx" },
		{ EStaticMeshType::SPHERE,"asset/model/collision/sphere.fbx" },
		{ EStaticMeshType::POINT_LIGHT,"asset/model/collision/pointlight.fbx" },
		{ EStaticMeshType::TEST_STAGE,"asset/map/test/stage2.fbx" },
		{ EStaticMeshType::FIRST_WALL,"asset/map/first/wall.obj" },
		{ EStaticMeshType::FIRST_GRASS_BOX,"asset/map/first/grass_box.obj" },
		{ EStaticMeshType::FIRST_START,"asset/map/first/start_obj.obj" },
		{ EStaticMeshType::FIRST_DOWN_CENTER,"asset/map/first/down_center.obj" },
		{ EStaticMeshType::FIRST_DOWN_LEFT,"asset/map/first/down_left.obj" },
		{ EStaticMeshType::FIRST_LEFT_STUMP,"asset/map/first/left_stump.obj" },
		{ EStaticMeshType::FIRST_START_WALL,"asset/map/first/start_wall.obj" },
		{ EStaticMeshType::FIRST_UP_RIGHT,"asset/map/first/up_right.obj" },
		{ EStaticMeshType::FIRST_UP_BLOCK,"asset/map/first/up_block.obj" },
		{ EStaticMeshType::FIRST_UP_CENTER,"asset/map/first/up_center.obj" },
		{ EStaticMeshType::FIRST_UP_LEFT,"asset/map/first/up_left.obj" },
		{ EStaticMeshType::FIRST_CENTER,"asset/map/first/center.obj" },
		{ EStaticMeshType::FIRST_CENTER_STUMP,"asset/map/first/center_stump.obj" },
		{ EStaticMeshType::FIRST_BOSS_FIELD,"asset/map/first/first_boss_field.obj" },
		{ EStaticMeshType::HIT_BLOCK,"asset/model/normal/hit_block.fbx" },
		{ EStaticMeshType::STAR,"asset/model/normal/star.obj" },
		{ EStaticMeshType::STAR_DOOR,"asset/model/normal/star_door.obj" },
		{ EStaticMeshType::CLEAR_DOOR,"asset/model/normal/clear_door.obj" },
		{ EStaticMeshType::TEST_SLOPE,"asset/map/test/slope.obj" },
		{ EStaticMeshType::HEART,"asset/model/normal/heart.obj" },
		{ EStaticMeshType::THORN_BOX,"asset/model/normal/thorn_box.fbx" },
		{ EStaticMeshType::ARROW,"asset/model/normal/arrow.obj" },
		{ EStaticMeshType::STUMP,"asset/map/first/stump.obj" },
		{ EStaticMeshType::STONE_BOX,"asset/map/first/stone_block.obj" },
		{ EStaticMeshType::TILE_BOX,"asset/map/first/tile_block.obj" },
		{ EStaticMeshType::STONE_TREE,"asset/map/first/stone_tree.obj" },
		{ EStaticMeshType::GRASS_STONE,"asset/map/first/grass_stone.obj" },
		{ EStaticMeshType::GRASS_WOOD,"asset/map/first/grass_wood.obj" },
		{ EStaticMeshType::WOOD_BRIDGE,"asset/map/first/wood_bridge.obj" },
		{ EStaticMeshType::GRASS_PILLAR,"asset/map/first/grass_pillar.obj" },
		{ EStaticMeshType::SLIME,"asset/model/normal/slime.obj" },
		{ EStaticMeshType::MONKEY,"asset/model/normal/monkey.obj" },
		{ EStaticMeshType::THORNMAN,"asset/model/normal/thorn_man.obj" },
		{ EStaticMeshType::TRIANGLE,"asset/map/first/triangle.obj" },
		{ EStaticMeshType::PLANE_YELLOW,"asset/map/first/plane_yellow.obj" },
		{ EStaticMeshType::PLANE_STONE,"asset/map/first/plane_stone.obj" },
		{ EStaticMeshType::PLANE_GREEN,"asset/map/first/plane_green.obj" },
		{ EStaticMeshType::PLANE_GRASS,"asset/map/first/plane_grass.obj" },
		{ EStaticMeshType::PLANE_GRASS2,"asset/map/first/plane_grass2.obj" },
		{ EStaticMeshType::WALL_GRASS,"asset/map/first/wall_grass.obj" },
		{ EStaticMeshType::WALL_WOOD,"asset/map/first/wall_wood.obj" },
		{ EStaticMeshType::TRIANGLE2,"asset/map/first/triangle2.obj" },
		{ EStaticMeshType::SIMPLE_GRASSBOX,"asset/map/first/simple_grass_box.obj" },
		{ EStaticMeshType::GRASS_CIRCLE,"asset/map/first/grass_circle.obj" },
		{ EStaticMeshType::GRASS_BOX_YELLOW,"asset/map/first/grass_box_yellow.obj" },
		{ EStaticMeshType::START_TABLE,"asset/map/first/start_table.obj" },
		{ EStaticMeshType::SKYDOME,"asset/model/normal/skydome.obj" },
		{ EStaticMeshType::PLAYER_START,"asset/model/normal/player_start.obj" },
		{ EStaticMeshType::WALL_GRASS_PLANE,"asset/model/normal/wall_grass2.obj" },
		{ EStaticMeshType::WALL_GRASS2,"asset/model/normal/wall_grass3.obj" },
		{ EStaticMeshType::PLANE_YELLOW2,"asset/map/first/plane_yellow2.obj" },
		{ EStaticMeshType::BLOCK_COLLISION,"asset/model/normal/block_collision.obj" },
		{ EStaticMeshType::BUSH,"asset/model/normal/bush.fbx" },
		{ EStaticMeshType::BUSH2,"asset/model/normal/bush2.obj" },
		{ EStaticMeshType::FERN,"asset/model/normal/fern.obj" },
		{ EStaticMeshType::BUSH3,"asset/model/normal/bush3.obj" },
		{ EStaticMeshType::BUSH4,"asset/model/normal/bush4.obj" },
		{ EStaticMeshType::BUSH5,"asset/model/normal/bush5.obj" },
		{ EStaticMeshType::PLANT,"asset/model/normal/plant.obj" },
		{ EStaticMeshType::GRASSES,"asset/model/normal/grasses.obj" },
		{ EStaticMeshType::FLOWER,"asset/model/normal/flower.obj" },
		{ EStaticMeshType::FLOWER2,"asset/model/normal/flower2.obj" },
		{ EStaticMeshType::FLOWER3,"asset/model/normal/flower3.obj" },
		{ EStaticMeshType::ROCK,"asset/model/normal/rock.fbx" },
		{ EStaticMeshType::ROCK2,"asset/model/normal/rock2.obj" },
		{ EStaticMeshType::ROCK3,"asset/model/normal/rock3.obj" },
		{ EStaticMeshType::ROCK4,"asset/model/normal/rock4.obj" },
		{ EStaticMeshType::ROCK5,"asset/model/normal/rock5.obj" },
		{ EStaticMeshType::TREE,"asset/model/normal/tree.obj" },
		{ EStaticMeshType::FIRST_BOSS_WALL,"asset/map/first/first_boss_wall.obj" },
		{ EStaticMeshType::TEST_NORMAL_BOX,"asset/model/normal/test.fbx" },
		{ EStaticMeshType::SECOND_WINDOWSOUTWARDS,"asset/map/second/WindowsOutwards.obj" },
		{ EStaticMeshType::SECOND_WINDOWSOUTWARDSFRAME,"asset/map/second/WindowsOutwardsFrame.obj" },
		{ EStaticMeshType::SECOND_WINDOWSSINGLEHIGH,"asset/map/second/WindowsSingleHigh.obj" },
		{ EStaticMeshType::SECOND_WINDOWSDOUBLEHIGH,"asset/map/second/WindowsDoubleHigh.obj" },
		{ EStaticMeshType::SECOND_WINDOWSROUND,"asset/map/second/WindowsRound.obj" },
		{ EStaticMeshType::SECOND_WINDOWSSINGLEHIGHBOARDS,"asset/map/second/WindowsSingleHighBoards.obj" },
		{ EStaticMeshType::SECOND_DOCK,"asset/map/second/dock.obj" },
		{ EStaticMeshType::SECOND_DOOR2STEP,"asset/map/second/Door2Step.obj" },
		{ EStaticMeshType::SECOND_DOOR3,"asset/map/second/Door3.obj" },
		{ EStaticMeshType::SECOND_DOOR4,"asset/map/second/Door4.obj" },
		{ EStaticMeshType::SECOND_DOOR1,"asset/map/second/Door1.obj" },
		{ EStaticMeshType::SECOND_HOUSE1,"asset/map/second/House1.obj" },
		{ EStaticMeshType::SECOND_HOUSE2,"asset/map/second/House2.obj" },
		{ EStaticMeshType::SECOND_HOUSE3,"asset/map/second/House3.obj" },
		{ EStaticMeshType::SECOND_HOUSE4,"asset/map/second/House4.obj" },
		{ EStaticMeshType::SECOND_ONEHOUSE1,"asset/map/second/OneHouse1.obj" },
		{ EStaticMeshType::SECOND_ONEHOUSE2,"asset/map/second/OneHouse2.obj" },
		{ EStaticMeshType::SECOND_ONEHOUSE3,"asset/map/second/OneHouse3.obj" },
		{ EStaticMeshType::SECOND_ONEHOUSE4,"asset/map/second/OneHouse4.obj" },
		{ EStaticMeshType::SECOND_BALCONY,"asset/map/second/Balcony.obj" },
		{ EStaticMeshType::SECOND_BALCONY2,"asset/map/second/Balcony2.obj" },
		{ EStaticMeshType::SECOND_CHIMNEY,"asset/map/second/Chimney.obj" },
		{ EStaticMeshType::SECOND_CHIMNEY2,"asset/map/second/Chimney2.obj" },
		{ EStaticMeshType::SECOND_ROOFFRAME,"asset/map/second/RoofFrame.obj" },
		{ EStaticMeshType::SECOND_ROOFFRAME2,"asset/map/second/RoofFrame2.obj" },
		{ EStaticMeshType::SECOND_ROOFFRAME3,"asset/map/second/RoofFrame3.obj" },
		{ EStaticMeshType::SECOND_ROOFFRAME4,"asset/map/second/RoofFrame4.obj" },
		{ EStaticMeshType::SECOND_STAIRWIDE,"asset/map/second/StairWide.obj" },
		{ EStaticMeshType::SECOND_STAIRWIDE2,"asset/map/second/StairWide2.obj" },
		{ EStaticMeshType::SECOND_ROOFFRAME5,"asset/map/second/RoofFrame5.obj" },
		{ EStaticMeshType::SECOND_ROOFFRAME6,"asset/map/second/RoofFrame6.obj" },
		{ EStaticMeshType::SECOND_VARIANT,"asset/map/second/Variant.obj" },
		{ EStaticMeshType::SECOND_COLUMN,"asset/map/second/Column.obj" },
		{ EStaticMeshType::SECOND_ARCBIG,"asset/map/second/ArcBig.obj" },
		{ EStaticMeshType::SECOND_ARCCOMER,"asset/map/second/ArcComer.obj" },
		{ EStaticMeshType::SECOND_ARCCROSS,"asset/map/second/ArcCross.obj" },
		{ EStaticMeshType::SECOND_ARCTRIPLE,"asset/map/second/ArcTriple.obj" },
		{ EStaticMeshType::SECOND_BARREL,"asset/map/second/Barrel.obj" },
		{ EStaticMeshType::SECOND_COFFIN,"asset/map/second/Coffin.obj" },
		{ EStaticMeshType::SECOND_COFFINCOVER,"asset/map/second/CoffinCover.obj" },
		{ EStaticMeshType::SECOND_PEREKR,"asset/map/second/Perekr.obj" },
		{ EStaticMeshType::SECOND_WOODBOX,"asset/map/second/WoodBox.obj" },
		{ EStaticMeshType::SECOND_PLANE_CLAIMEDWALL2C,"asset/map/second/Plane_Claimedwall2C.obj" },
		{ EStaticMeshType::SECOND_PLANE_CLAIMEDE,"asset/map/second/Plane_ClaimedE.obj" },
		{ EStaticMeshType::SECOND_PLANE_DIRT_DARKE,"asset/map/second/Plane_Dirt_darkE.obj" },
		{ EStaticMeshType::SECOND_PLANE_FLOOR,"asset/map/second/Plane_Floor.obj" },
		{ EStaticMeshType::SECOND_PLANE_STONE,"asset/map/second/Plane_Stone.obj" },
		{ EStaticMeshType::SECOND_PLANE_TOP,"asset/map/second/Plane_Top.obj" },
		{ EStaticMeshType::SECOND_WALL_CLAIMEDE,"asset/map/second/Wall_ClaimedE.obj" },
		{ EStaticMeshType::SECOND_WALL_CLAIMEDWALL2C,"asset/map/second/Wall_Claimedwall2C.obj" },
		{ EStaticMeshType::SECOND_WALL_DIRT_DARKE,"asset/map/second/Wall_Dirt_darkE.obj" },
		{ EStaticMeshType::SECOND_WALL_FLOOR,"asset/map/second/Wall_Floor.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE,"asset/map/second/Wall_Stone.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE2,"asset/map/second/Wall_Stone2.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE3,"asset/map/second/Wall_Stone3.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE4,"asset/map/second/Wall_Stone4.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE5,"asset/map/second/Wall_Stone5.obj" },
		{ EStaticMeshType::SECOND_WALL_TOP,"asset/map/second/Wall_Top.obj" },
		{ EStaticMeshType::STAGESELECT_WALL,"asset/map/stage_select/select_wall.obj" },
		{ EStaticMeshType::STAGESELECT_PLANE,"asset/map/stage_select/select_plane.obj" },
		{ EStaticMeshType::STAGESELECT_CHAIR,"asset/map/stage_select/Chair.obj" },
		{ EStaticMeshType::STAGESELECT_CHEST,"asset/map/stage_select/Chest.obj" },
		{ EStaticMeshType::STAGESELECT_SMALLTABLE,"asset/map/stage_select/SmallTable.obj" },
		{ EStaticMeshType::STAGESELECT_TABLE,"asset/map/stage_select/table.obj" },
		{ EStaticMeshType::STAGESELECT_GREEN_BOOK,"asset/map/stage_select/book.obj" },
		{ EStaticMeshType::STAGESELECT_PURPLE_BOOK,"asset/map/stage_select/purple_book.obj" },
		{ EStaticMeshType::STAGESELECT_YELLOW_BOOK,"asset/map/stage_select/yellow_book.obj" },
		{ EStaticMeshType::STAGESELECT_RED_BOOK,"asset/map/stage_select/red_book.obj" },
		{ EStaticMeshType::STAGESELECT_SECOND_STAGE_FRAME,"asset/map/stage_select/stage_frame.obj" },
		{ EStaticMeshType::STAGESELECT_FIRST_STAGE_FRAME,"asset/map/stage_select/stage_frame2.obj" },
		{ EStaticMeshType::STAGESELECT_TUTOLIAL_STAGE_FRAME,"asset/map/stage_select/stage_frame3.obj" },
		{ EStaticMeshType::STAGESELECT_BOSS_FIGURE,"asset/map/stage_select/BossFigure.obj" },
		{ EStaticMeshType::SECOND_DOLLY,"asset/map/second/Dolly.obj" },
		{ EStaticMeshType::SECOND_WOODEN,"asset/map/second/Wooden.obj" },
		{ EStaticMeshType::SECOND_WOODEN2,"asset/map/second/Wooden2.obj" },
		{ EStaticMeshType::SECOND_WOODEN3,"asset/map/second/Wooden3.obj" },
		{ EStaticMeshType::SECOND_WOODEN4,"asset/map/second/Wooden4.obj" },
		{ EStaticMeshType::SECOND_WOODEN5,"asset/map/second/Wooden5.obj" },
		{ EStaticMeshType::SECOND_WOODEN6,"asset/map/second/Wooden6.obj" },
		{ EStaticMeshType::SECOND_WOODEN7,"asset/map/second/Wooden7.obj" },
		{ EStaticMeshType::SECOND_CHEST2,"asset/map/second/Chest2.obj" },
		{ EStaticMeshType::SECOND_CHESTTOP2,"asset/map/second/ChestTop2.obj" },
		{ EStaticMeshType::SECOND_ROCKS,"asset/map/second/Rocks.obj" },
		{ EStaticMeshType::SECOND_STALAGS,"asset/map/second/Stalags.obj" },
		{ EStaticMeshType::SECOND_COINS,"asset/map/second/Coins.obj" },
		{ EStaticMeshType::SECOND_RAIL,"asset/map/second/Rail.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE6,"asset/map/second/Wall_Stone6.obj" },
		{ EStaticMeshType::SECOND_WALL_STONE7,"asset/map/second/Wall_Stone7.obj" },
	};
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}

	//D3D11関連の初期化
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/shader3DNormalMappingVS.hlsl", nullptr, nullptr, "main", "vs_4_1", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_VertexShader)))
	{
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/NoShadowNormalModelVS.hlsl", nullptr, nullptr, "main", "vs_4_1", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_NoShadowVertexShader)))
	{
		MessageBox(0, "バーテックスシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//頂点インプットレイアウトを定義	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0 ,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0 ,DXGI_FORMAT_R32G32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA , 0 }
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	if (FAILED(CRenderer::GetDevice()->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_VertexLayout)))
	{
		return;
	}
	//頂点インプットレイアウトをセット
	deviceContext->IASetInputLayout(m_VertexLayout);

	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/shader3DNormalMappingPS.hlsl", nullptr, nullptr, "main", "ps_4_1", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_PixelShader)))
	{
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	SAFE_RELEASE(pCompiledShader);
	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/shader3DNoNormalMappingPS.hlsl", nullptr, nullptr, "main", "ps_4_1", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_NoNormalPixelShader)))
	{
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile("shader/NoShadowNormalModelPS.hlsl", nullptr, nullptr, "main", "ps_4_1", 0, 0, nullptr, &pCompiledShader, &pErrors, nullptr)))
	{
		MessageBox(0, "hlsl読み込み失敗", nullptr, MB_OK);
		return;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(CRenderer::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), nullptr, &m_NoShadowPixelShader)))
	{
		MessageBox(0, "ピクセルシェーダー作成失敗", nullptr, MB_OK);
		return;
	}

	SAFE_RELEASE(pCompiledShader);
	D3D11_BUFFER_DESC cb;
	//コンスタントバッファー0作成
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ShaderMatrixAndFogData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_MatrixAndFogConstantBuffer)))
	{
		return;
	}
	//コンスタントバッファー1作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(LIGHT);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_LightConstantBuffer)))
	{
		return;
	}
	//コンスタントバッファー2作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(ShaderNormalMapData);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_NormalMapConstantBuffer)))
	{
		return;
	}

	//コンスタントバッファーマテリアル作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(MATERIAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_MaterialConstantBuffer)))
	{
		return;
	}
	//コンスタントバッファーテクスチャスクロール作成  
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(TEX_SCROLL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(CRenderer::GetDevice()->CreateBuffer(&cb, nullptr, &m_TexScrollConstantBuffer)))
	{
		return;
	}
}

void CStaticMesh::UnInitModelData()
{
	SAFE_RELEASE(m_VertexShader);
	SAFE_RELEASE(m_VertexLayout);
	SAFE_RELEASE(m_MatrixAndFogConstantBuffer);
	SAFE_RELEASE(m_LightConstantBuffer);
	SAFE_RELEASE(m_NormalMapConstantBuffer);
	SAFE_RELEASE(m_PixelShader);
	SAFE_RELEASE(m_NoNormalPixelShader);
	SAFE_RELEASE(m_MaterialConstantBuffer);
	SAFE_RELEASE(m_NoShadowVertexShader);
	SAFE_RELEASE(m_NoShadowPixelShader);
	SAFE_RELEASE(m_TexScrollConstantBuffer);
}

CStaticMesh::CStaticMesh(const EStaticMeshType Type)
{
	//同じモデルがロード済みモデルリストにあったらそれから読むこむ
	//なかったらモデル情報を作成しロード済みモデルリストに代入する
	bool isLoadModel = false;
	m_MeshType = Type;
	for (StaticMeshData* mesh : m_LoadMeshList)
	{
		if (m_MeshType == mesh->ModelIndex)
		{
			isLoadModel = true;
			m_Mesh = mesh;
			m_Material.Diffuse = COLOR(0.6f, 0.6f, 0.6f, 1.0f);
			m_Material.Ambient = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_Material.Emission = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_Material.Specular = COLOR(0.5f, 0.5f, 0.5f, 1.0f);
			m_Material.Alpha = COLOR(1.0f, 1.0f, 1.0f, 1.0f);
			m_Material.Shininess = 0.0f;
			break;
		}
	}
	if (!isLoadModel)
	{
		Load(m_ModelFileList[m_MeshType]);
	}
	//スカイドームは専用のシェーダーを使用する
	if (m_MeshType == EStaticMeshType::SKYDOME)
	{
		m_SkyShader = new CSkyShader();
	}
}

CStaticMesh::CStaticMesh(const std::string& FileName)
{
	m_MeshType = EStaticMeshType::INDEX_MAX;
	Load(FileName);
}

CStaticMesh::~CStaticMesh()
{
	if (m_SkyShader)
	{
		delete m_SkyShader;
	}
}

void CStaticMesh::Load(const std::string& FileName)
{
	Assimp::Importer importer;
	m_Scene = aiImportFile(FileName.c_str(), aiProcess_Triangulate
		| aiProcess_MakeLeftHanded
		| aiProcess_FlipUVs
		| aiProcess_FlipWindingOrder
		| aiProcess_JoinIdenticalVertices
		| aiProcess_OptimizeMeshes
		| aiProcess_LimitBoneWeights
		| aiProcess_RemoveRedundantMaterials);// 左手座標系で読みこむ

	if (!m_Scene)
	{
		return;
	}

	m_MeshNum = m_Scene->mNumMeshes;
	m_Mesh = new StaticMeshData[m_MeshNum];
	if (!m_Mesh)
	{
		return;
	}
	m_TriangleList.resize(m_MeshNum); 
	m_Mesh->ModelIndex = m_MeshType;

	// メッシュ情報を作成
	for (unsigned int m = 0; m < m_MeshNum; m++)
	{
		aiMesh* mesh = m_Scene->mMeshes[m];
		if (!mesh)
		{
			continue;
		}
		const aiMaterial* material = m_Scene->mMaterials[mesh->mMaterialIndex];
		if (!material)
		{
			continue;
		}
		ShaderVertexData* vertex = new ShaderVertexData[mesh->mNumFaces * 3];
		if (!vertex)
		{
			continue;
		}
		unsigned short* index = new unsigned short[mesh->mNumFaces * 3];
		if (!index)
		{
			continue;
		}
		LoadMaterial(material,m, FileName);
		m_TriangleList[m].reserve(mesh->mNumFaces * 3);

		int count = 0;
		for (unsigned int  i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace* face = &mesh->mFaces[i];
			if (!face)
			{
				continue;
			}

			for (unsigned int j = 0; j < face->mNumIndices; j++)
			{
				//faceのj番目の頂点のインデックスを取得
				const int face_index = face->mIndices[j];
				vertex[count].Position = XMFLOAT3(mesh->mVertices[face_index].x, mesh->mVertices[face_index].y, mesh->mVertices[face_index].z);
				vertex[count].Normal = XMFLOAT3(mesh->mNormals[face_index].x, mesh->mNormals[face_index].y, mesh->mNormals[face_index].z);
				vertex[count].Diffuse = XMFLOAT4(m_Material.Diffuse.r, m_Material.Diffuse.g, m_Material.Diffuse.b, 1.0f);
				vertex[count].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][face_index].x, mesh->mTextureCoords[0][face_index].y);
				if (m_MeshType == EStaticMeshType::SKYDOME)
				{
					vertex[count].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][face_index].x * SKY_SCALE, mesh->mTextureCoords[0][face_index].y * SKY_SCALE);
				}
				if (mesh->mTangents)
				{
					// 正規化してから格納
					Vector3 tan = Vector3(mesh->mTangents[face_index].x, mesh->mTangents[face_index].y, mesh->mTangents[face_index].z).Normalize();
					vertex[count].Tangent = XMFLOAT3(tan.x, tan.y, tan.z);
				}
				// 従法線
				if (mesh->mBitangents)
				{
					// 正規化してから格納
					Vector3 biNor = Vector3(mesh->mBitangents[face_index].x, mesh->mBitangents[face_index].y, mesh->mBitangents[face_index].z).Normalize();
					vertex[count].Binormal = XMFLOAT3(biNor.x, biNor.y, biNor.z);
				}
				index[count] = count;
				m_TriangleList[m].emplace_back(vertex[count].Position);
				count++;
			}
		}

		ID3D11Device* device = CRenderer::GetDevice();
		if (!device)
		{
			return;
		}

		{//VertexBuffer作成
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ShaderVertexData) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;
			device->CreateBuffer(&bd, &sd, &m_Mesh[m].VertexBuffer);
		}
		delete[] vertex;
		m_Mesh[m].IndexNum = mesh->mNumFaces * 3;
		{//IndexBuffer作成
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short)* m_Mesh[m].IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;
			device->CreateBuffer(&bd, &sd, &m_Mesh[m].IndexBuffer);
		}
		delete[] index;
	}
	m_Mesh->Scene = m_Scene;
	m_Mesh->TriangleList = m_TriangleList;
	m_Mesh->MeshNum = m_MeshNum;
	m_LoadMeshList.emplace_back(m_Mesh);
}

void CStaticMesh::LoadMaterial(const aiMaterial* AiMaterial, const int Index, const std::string& FileName)
{
	CSystemContext* system = CSystemContext::GetInstance();
	if (!system)
	{
		return;
	}
	if (!AiMaterial)
	{
		return;
	}

	// マテリアルパラメータ読み込み
	float alpha = 0.0f;
	AiMaterial->Get(AI_MATKEY_OPACITY, alpha);
	aiColor3D color;
	AiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	m_Material.Diffuse = COLOR(color.r, color.g, color.b, 1.0f);
	AiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
	m_Material.Ambient = COLOR(color.r, color.g, color.b, 1.0f);
	AiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	m_Material.Emission = COLOR(color.r, color.g, color.b, 1.0f);
	AiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
	m_Material.Specular = COLOR(color.r, color.g, color.b, 1.0f);
	m_Material.Alpha.r = 1.0f;
	float shininess = 0.0f;
	AiMaterial->Get(AI_MATKEY_SHININESS, shininess);
	m_Material.Shininess = shininess;

	// テクスチャ読み込み
	aiString path;
	if (AiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), path) == AI_SUCCESS)
	{
		std::string texturePath;
		system->ConvertToTextureRelativePath(FileName, path.C_Str(), texturePath);

		if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
		{
			if (m_MeshType == EStaticMeshType::SKYDOME)
			{
				m_Mesh[Index].Texture = textureLoader->Load(SKY_TEXTURE);
			}
			else
			{
				m_Mesh[Index].Texture = textureLoader->Load(texturePath);
			}
		}
	}
	//ノーマルマップがあったら読み込む
	if (AiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), path) == AI_SUCCESS)
	{
		m_IsLoadNormalMap = true;
		m_NormalMapPower.NormalPower.x = DEFAULT_NORMALMAP_POWER;
		m_NormalMapPower.SpecularPower.x = DEFAULT_NORMALMAP_POWER;
		std::string texturePath;
		system->ConvertToTextureRelativePath(FileName, path.C_Str(), texturePath);
		
		if (CTextureLoader* textureLoader = CTextureLoader::GetInstance())
		{
			m_Mesh[Index].NormalTexture = textureLoader->Load(texturePath);
		}
	}
}

void CStaticMesh::SetShader(const int Index, const XMMATRIX& World)
{
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
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	const Vector3 eye = camera->GetPosition();
	const XMMATRIX& view = camera->GetInvViewMatrix();
	const XMMATRIX& proj = camera->GetProjectionMatrix();

	//スカイドームは専用のシェーダーを使用
	if(m_SkyShader)
	{
		m_TexScroll.x += SKA_CLOUD_SPEED;//雲の移動
		m_SkyShader->Set(World, camera, m_TexScroll.x);
		CRenderer::SetTexture(m_Mesh[Index].Texture);
		return;
	}

	deviceContext->VSSetConstantBuffers(0, 1, &m_MatrixAndFogConstantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_MatrixAndFogConstantBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_LightConstantBuffer);

	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(deviceContext->Map(m_MatrixAndFogConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		ShaderMatrixAndFogData cb;
		//ワールド、カメラ、射影行列　を渡す
		cb.WorldMatrix = XMMatrixTranspose(World);
		cb.ProjectionMatrix = XMMatrixTranspose(proj);
		cb.ViewMatrix = XMMatrixTranspose(view);
		cb.CameraPos = XMFLOAT4(eye.x, eye.y, eye.z, 0.0f);
		cb.FogParam = CRenderer::GetFogConstant().FogParam;
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		deviceContext->Unmap(m_MatrixAndFogConstantBuffer, 0);
	}
	if (SUCCEEDED(deviceContext->Map(m_LightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		CDirectionalLight* directionalLight = directionalLight->GetInstance();
		if (!directionalLight)
		{
			return;
		}
		const LIGHT light = directionalLight->GetLight();
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&light), sizeof(light));
		deviceContext->Unmap(m_LightConstantBuffer, 0);
	}
	if (SUCCEEDED(deviceContext->Map(m_MaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		BufferMaterial sg;
		sg.vAmbient = XMFLOAT4(m_Material.Ambient.r, m_Material.Ambient.g, m_Material.Ambient.b, m_Material.Ambient.a);
		sg.vDiffuse = XMFLOAT4(m_Material.Diffuse.r, m_Material.Diffuse.g, m_Material.Diffuse.b, m_Material.Diffuse.a);
		sg.vSpecular = XMFLOAT4(m_Material.Specular.r, m_Material.Specular.g, m_Material.Specular.b, m_Material.Specular.a);
		sg.vAlpha = XMFLOAT4(m_Material.Alpha.r, m_Material.Alpha.r, m_Material.Alpha.b, m_Material.Alpha.a);
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(BufferMaterial));
		deviceContext->Unmap(m_MaterialConstantBuffer, 0);
	}

	// 入力レイアウト設定
	deviceContext->IASetInputLayout(m_VertexLayout);

	//モデルの設定による各種コンスタントバッファ設定

	//ノーマルテクスチャがある場合
	if (m_Mesh[Index].NormalTexture.expired() && m_IsLoadNormalMap)
	{
		deviceContext->VSSetConstantBuffers(2, 1, &m_NormalMapConstantBuffer);
		deviceContext->VSSetConstantBuffers(3, 1, &m_MaterialConstantBuffer);
		if (SUCCEEDED(deviceContext->Map(m_NormalMapConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			ShaderNormalMapData cb;
			//Xのみ使用
			cb.SpecularPower = XMFLOAT4(m_NormalMapPower.NormalPower.x, 0.0f, 0.0f, 0.0f);
			cb.NormalPower = XMFLOAT4(m_NormalMapPower.SpecularPower.x, 0.0f, 0.0f, 0.0f);
			memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
			deviceContext->Unmap(m_NormalMapConstantBuffer, 0);
		}

		deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
		deviceContext->PSSetShader(m_PixelShader, nullptr, 0);

		CRenderer::SetTextureIndex(0, m_Mesh[Index].Texture);
		//1はシャドウマップで使用しているため2を使用
		CRenderer::SetTextureIndex(2, m_Mesh[Index].NormalTexture);
	}
	else
	{
		if (m_IsEnableLight)
		{
			deviceContext->VSSetConstantBuffers(2, 1, &m_MaterialConstantBuffer);
			deviceContext->VSSetConstantBuffers(3, 1, &m_TexScrollConstantBuffer);
			deviceContext->VSSetShader(m_NoShadowVertexShader, nullptr, 0);
			deviceContext->PSSetShader(m_NoShadowPixelShader, nullptr, 0);
		}
		else
		{
			deviceContext->VSSetConstantBuffers(2, 1, &m_NormalMapConstantBuffer);
			deviceContext->VSSetConstantBuffers(3, 1, &m_MaterialConstantBuffer);
			deviceContext->VSSetShader(m_VertexShader, nullptr, 0);
			deviceContext->PSSetShader(m_NoNormalPixelShader, nullptr, 0);
		}
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(deviceContext->Map(m_MaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			BufferMaterial sg;
			sg.vAmbient = XMFLOAT4(m_Material.Ambient.r, m_Material.Ambient.g, m_Material.Ambient.b, m_Material.Ambient.a);
			sg.vDiffuse = XMFLOAT4(m_Material.Diffuse.r, m_Material.Diffuse.g, m_Material.Diffuse.b, m_Material.Diffuse.a);
			sg.vSpecular = XMFLOAT4(m_Material.Specular.r, m_Material.Specular.g, m_Material.Specular.b, m_Material.Specular.a);
			sg.vAlpha = XMFLOAT4(m_Material.Alpha.r, m_Material.Alpha.r, m_Material.Alpha.b, m_Material.Alpha.a);
			memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(BufferMaterial));
			deviceContext->Unmap(m_MaterialConstantBuffer, 0);
		}
		CRenderer::SetTexture(m_Mesh[Index].Texture);
	}
}

void CStaticMesh::Update(const XMMATRIX Matrix)
{
	if (!m_Mesh)
	{
		return;
	}
	if (!m_Mesh->Scene)
	{
		return;
	}
	UpdateMatrix(m_Mesh->Scene->mRootNode, Matrix);
}

void CStaticMesh::UpdateMatrix(const aiNode* Node, const  XMMATRIX Matrix)
{
	if (!Node)
	{
		return;
	}
	aiMatrix4x4 matrix = Node->mTransformation;//自分のマトリクスが入っている
	aiTransposeMatrix4(&matrix);//転置行列を作る。DirectX用に行列を変換

	m_World = XMLoadFloat4x4((XMFLOAT4X4*)&matrix);//aiMatrix4x4をXMMATRIXに変換
	m_World *= Matrix;//親のマトリクス　これをすることで子は親についてくる
	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		UpdateMatrix(Node->mChildren[i], m_World);
	}
}

void CStaticMesh::Draw(const XMMATRIX Matrix)
{
	if (!m_Mesh)
	{
		return;
	}
	if (!m_Mesh->Scene)
	{
		return;
	}
	DrawMesh(m_Mesh->Scene->mRootNode, Matrix);
}

void CStaticMesh::ShadowOnModelDraw(const EShadowType Type, const XMMATRIX Matrix)
{
	if (!m_Mesh)
	{
		return;
	}
	if (!m_Mesh->Scene)
	{
		return;
	}
	//2パス目　シーンを通常のバックバッファー上にレンダリング。
	//ここで使うシェーダーは、影描画も行う。
	
	//個々のモデルレンダリング
	DrawShadowMesh(Type, m_Mesh->Scene->mRootNode, Matrix);
}

void CStaticMesh::ShadowDraw(const EShadowType Type, const XMMATRIX Matrix)
{
	if (!m_Mesh)
	{
		return;
	}
	if (!m_Mesh->Scene)
	{
		return;
	}
	//shadow用コンスタントバッファー設定
	//フレーム単位のコンスタントバッファーに各種データをセット
	CShadowMap::SetConstantBuffers();	
	//ライトメッシュのワールド行列からライト視点のビュー行列を作る
	CShadowMap::SetLightCameraPosition();
	//1パス目　シーンを深度テクスチャー上にレンダリングする。
	//このパスで使用するシェーダーの登録
	CShadowMap::SetDepthShader();
	//個々のモデルレンダリング			
	DrawShadowDepth(Type, m_Mesh->Scene->mRootNode, Matrix);
}

void CStaticMesh::DrawMesh(const aiNode* Node, const  XMMATRIX Matrix)
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

	//ノードで親子関係を表しノードにメッシュが入っている
	aiMatrix4x4 matrix = Node->mTransformation;//自分のマトリクスが入っている
	aiTransposeMatrix4(&matrix);//転置行列を作る。DirectX用に行列を変換

	m_World = XMLoadFloat4x4((XMFLOAT4X4*)&matrix);//aiMatrix4x4をXMMATRIXに変換
	m_World *= Matrix;//親のマトリクス　これをすることで子は親についてくる
;
	UINT stride;
	UINT offset;
	for (unsigned int n = 0; n < Node->mNumMeshes; n++)
	{
		unsigned int m = Node->mMeshes[n];//メッシュの番号を取り出す
		
		//バーテックスバッファーをセット
		stride = sizeof(ShaderVertexData);
		offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_Mesh[m].VertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		deviceContext->IASetIndexBuffer(m_Mesh[m].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		SetShader(m, m_World);

		CRenderer::DrawIndexed(m_Mesh[m].IndexNum, 0, 0);
	}

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		DrawMesh(Node->mChildren[i], m_World);
	}
}

void CStaticMesh::DrawShadowMesh(const EShadowType Type, const aiNode* Node, const XMMATRIX Matrix)
{
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}
	if (!Node)
	{
		return;
	}
	//ノードで親子関係を表しノードにメッシュが入っている
	aiMatrix4x4 matrix = Node->mTransformation;//自分のマトリクスが入っている
	aiTransposeMatrix4(&matrix);//転置行列を作る。Directx用に行列を変換

	m_World = XMLoadFloat4x4((XMFLOAT4X4*)&matrix);//aiMatrix4x4をXMMATRIXに変換
	m_World *= Matrix;//親のマトリクス　これをすることで子は親についてくる

	UINT stride;
	UINT offset;
	for (unsigned int n = 0; n < Node->mNumMeshes; n++)
	{
		unsigned int m = Node->mMeshes[n];//メッシュの番号を取り出す
		
		//モデルの設定によってシェーダーを切り替える
		if (m_Mesh[m].NormalTexture.expired() && m_IsLoadNormalMap)
		{
			CShadowMap::SetNormalMappingModelShader();
		}
		else
		{
			if (m_IsEnableLight)
			{
				if (m_IsDisableSelfShadow)
				{
					CShadowMap::SetNormalModelDisableSelfLightShader();
				}
				else
				{
					CShadowMap::SetNormalModelLightShader();
				}
			}
			else
			{
				CShadowMap::SetNormalModelShader();
			}
		}
		
		CShadowMap::SetConstantBuffers();

		//バーテックスバッファーをセット
		stride = sizeof(ShaderVertexData);
		offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_Mesh[m].VertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		deviceContext->IASetIndexBuffer(m_Mesh[m].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		//シェーダーのコンスタントバッファーに各種データを渡す
		//マトリクスを渡し射影行列を作成
		CShadowMap::SetNormalModelMatrix(Type, m_World);
		deviceContext->IASetInputLayout(m_VertexLayout);
		//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
		//マテリアルの各要素をエフェクト（シェーダー）に渡す
		CShadowMap::SetLightCameraPosition();
	
		CShadowMap::SetNormalModelMaterial(XMFLOAT4(m_Material.Ambient.r, m_Material.Ambient.g, m_Material.Ambient.b, m_Material.Ambient.a), XMFLOAT4(m_Material.Diffuse.r, m_Material.Diffuse.g, m_Material.Diffuse.b, m_Material.Diffuse.a), XMFLOAT4(m_Material.Specular.r, m_Material.Specular.g, m_Material.Specular.b, m_Material.Specular.a), XMFLOAT4(m_Material.Alpha.r, 0.0f, 0.0f, 0.0f));
		CShadowMap::SetNormalModelCameraLight(m_World);


		if (m_Mesh[m].NormalTexture.expired() && m_IsLoadNormalMap)
		{
			if (m_Mesh[m].Texture.expired())
			{
				deviceContext->VSSetConstantBuffers(5, 1, &m_NormalMapConstantBuffer);
				deviceContext->PSSetConstantBuffers(5, 1, &m_NormalMapConstantBuffer);
				D3D11_MAPPED_SUBRESOURCE pData;
				if (SUCCEEDED(deviceContext->Map(m_NormalMapConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
				{
					ShaderNormalMapData cb;
					//Xのみ使用
					cb.SpecularPower = XMFLOAT4(m_NormalMapPower.NormalPower.x, 0.0f, 0.0f, 0.0f);
					cb.NormalPower = XMFLOAT4(m_NormalMapPower.SpecularPower.x, 0.0f, 0.0f, 0.0f);
					memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
					deviceContext->Unmap(m_NormalMapConstantBuffer, 0);
				}
				CRenderer::SetTextureIndex(0, m_Mesh[m].Texture);
				CRenderer::SetTextureIndex(2, m_Mesh[m].NormalTexture);
			}
		}
		else
		{
			CRenderer::SetTexture(m_Mesh[m].Texture);
		}
		CRenderer::DrawIndexed(m_Mesh[m].IndexNum, 0, 0);
	}

	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		DrawShadowMesh(Type, Node->mChildren[i], m_World);
	}
}

void CStaticMesh::DrawShadowDepth(const EShadowType Type, const aiNode* Node, const XMMATRIX Matrix)
{	
	ID3D11DeviceContext* deviceContext = CRenderer::GetDeviceContext();
	if (!deviceContext)
	{
		return;
	}

	for (unsigned int n = 0; n < Node->mNumMeshes; n++)
	{
		aiMatrix4x4 matrix = Node->mTransformation;
		aiTransposeMatrix4(&matrix);

		m_World = XMLoadFloat4x4((XMFLOAT4X4*)&matrix);
		m_World *= Matrix;
		unsigned int m = Node->mMeshes[n];
		//バーテックスバッファーをセット
		UINT stride = sizeof(ShaderVertexData);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_Mesh[m].VertexBuffer, &stride, &offset);
		aiMesh* mesh = m_Mesh->Scene->mMeshes[m];
		// インデックスバッファ設定
		deviceContext->IASetIndexBuffer(m_Mesh[m].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		//シェーダーのコンスタントバッファーに各種データを渡す
		//マトリクスを渡し射影行列を作成
		CShadowMap::SetNormalModelMatrix(Type, m_World);

		//マテリアルの数だけ、それぞれのマテリアルのインデックスバッファ－を描画
		//マテリアルの各要素をエフェクト（シェーダー）に渡す
		CShadowMap::SetNormalModelMaterial(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(m_Material.Alpha.r, 0.0f, 0.0f, 0.0f));

		// テクスチャ設定
		CRenderer::SetTexture(m_Mesh[m].Texture);
		CRenderer::DrawIndexed(m_Mesh[m].IndexNum, 0, 0);
	}
	
	for (unsigned int i = 0; i < Node->mNumChildren; i++)
	{
		DrawShadowDepth(Type, Node->mChildren[i], m_World);
	}
}

std::vector<CollisionMesh>* CStaticMesh::GetNormalModelMeshCollision()
{
	return &m_CollisionMeshList;
}

void CStaticMesh::CreateCollisionMesh()
{
	m_CollisionMeshList.clear();
	//コリジョンメッシュに頂点情報と法線を格納する
	float maxRad = 0.0f;	
	for (unsigned int j = 0; j < m_Mesh->MeshNum; j++)
	{
		const int size = static_cast<int>(m_Mesh->TriangleList[j].size()) / 3;
		for (int i = 0; i < size; i++)
		{
			const int v = i * 3;
			CollisionMesh face;
			face.Point[0] = m_Mesh->TriangleList[j][v];
			face.Point[1] = m_Mesh->TriangleList[j][v + 1];
			face.Point[2] = m_Mesh->TriangleList[j][v + 2];

			// このメッシュの最大の大きさを調べる
			if (face.Point[0].x > maxRad) maxRad = face.Point[0].x;
			if (face.Point[0].y > maxRad) maxRad = face.Point[0].y;
			if (face.Point[0].z > maxRad) maxRad = face.Point[0].z;
			if (face.Point[1].x > maxRad) maxRad = face.Point[1].x;
			if (face.Point[1].y > maxRad) maxRad = face.Point[1].y;
			if (face.Point[1].z > maxRad) maxRad = face.Point[1].z;
			if (face.Point[2].x > maxRad) maxRad = face.Point[2].x;
			if (face.Point[2].y > maxRad) maxRad = face.Point[2].y;
			if (face.Point[2].z > maxRad) maxRad = face.Point[2].z;

			const Vector3 vecAB = face.Point[1] - face.Point[0];
			const Vector3 vecBC = face.Point[2] - face.Point[1];
			const Vector3 normal = Vector3::Cross(vecAB, vecBC).Normalize();
			face.Normal = normal;

			m_CollisionMeshList.emplace_back(face);
		}
	}
}

void CStaticMesh::ChangeTexture(const ETextureType Type, const int Index)
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();
	if (!textureLoader)
	{
		return;
	}
	textureLoader->Release(m_Mesh[Index].Texture);
	m_Mesh[Index].Texture = textureLoader->Load(Type);
}

void CStaticMesh::SetAlpha(const float Alpha)
{
	m_Material.Alpha.r = Alpha;
}

bool* CStaticMesh::IsEnableLight()
{
	return& m_IsEnableLight;
}

void CStaticMesh::SetEnableLight(const bool Enable)
{
	m_IsEnableLight = Enable;
}

bool* CStaticMesh::IsDisableSelfShadow()
{
	return &m_IsDisableSelfShadow;
}

void CStaticMesh::SetDisableSelfShadow(const bool Disable)
{
	m_IsDisableSelfShadow = Disable;
}

bool* CStaticMesh::IsEnableNormalMap()
{
	return &m_IsLoadNormalMap;
}

void CStaticMesh::SetEnableNormalMap(const bool Enable)
{
	m_IsLoadNormalMap = Enable;
}

void CStaticMesh::SetMaterial(const MATERIAL& Material)
{
	m_Material = Material;
}

void CStaticMesh::AllLoadModelDelete()
{
	CTextureLoader* textureLoader = CTextureLoader::GetInstance();

	for (StaticMeshData* mesh : m_LoadMeshList)
	{
		if (!mesh)
		{
			continue;
		}

		for (unsigned int m = 0; m < mesh->MeshNum; m++)
		{
			if (mesh->Scene && mesh->Scene->mMetaData)
			{
				aiReleaseImport(mesh->Scene);
			}
			if (mesh[m].VertexBuffer)
			{
				mesh[m].VertexBuffer->Release();
			}
			if (mesh[m].IndexBuffer)
			{
				mesh[m].IndexBuffer->Release();
			}

			if (!textureLoader)
			{
				continue;
			}
			textureLoader->Release(mesh[m].Texture);
			textureLoader->Release(mesh[m].NormalTexture);
		}
		delete []mesh;
	}
	m_LoadMeshList.clear();
	m_LoadMeshList.shrink_to_fit();
}

void CStaticMesh::ConvertAssimpMatrix(Matrix4& OutMatrix, const aiMatrix4x4& AiMatrix)
{
	OutMatrix.Matrix(0, 0) = AiMatrix.a1;
	OutMatrix.Matrix(0, 1) = AiMatrix.a2;
	OutMatrix.Matrix(0, 2) = AiMatrix.a3;
	OutMatrix.Matrix(0, 3) = AiMatrix.a4;

	OutMatrix.Matrix(1, 0) = AiMatrix.b1;
	OutMatrix.Matrix(1, 1) = AiMatrix.b2;
	OutMatrix.Matrix(1, 2) = AiMatrix.b3;
	OutMatrix.Matrix(1, 3) = AiMatrix.b4;

	OutMatrix.Matrix(2, 0) = AiMatrix.c1;
	OutMatrix.Matrix(2, 1) = AiMatrix.c2;
	OutMatrix.Matrix(2, 2) = AiMatrix.c3;
	OutMatrix.Matrix(2, 3) = AiMatrix.c4;

	OutMatrix.Matrix(3, 0) = AiMatrix.d1;
	OutMatrix.Matrix(3, 1) = AiMatrix.d2;
	OutMatrix.Matrix(3, 2) = AiMatrix.d3;
	OutMatrix.Matrix(3, 3) = AiMatrix.d4;
}
