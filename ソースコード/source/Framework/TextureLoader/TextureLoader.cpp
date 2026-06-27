#include "TextureLoader.h"
#include "WICTextureLoader.h"
#include "../Renderer/Renderer.h"

#define TEXTURE_PREFIX "asset/texture/"
#define UI_PREFIX "asset/ui/"
#define UI_PAUSE_PREFIX "asset/ui/pause/"
#define EFFECT_PREFIX "asset/effect/"
#define IMGUI_PREFIX "asset/texture/ImGuiIcon/"
#define TITLE_PREFIX "asset/title/"
#define STAGESELECT_PREFIX "asset/stageselect/"
#define JPG_PREFIX ".jpg"
#define PNG_PREFIX ".png"
#define ADD_TEXTURE_PNG_PREFIX(str) TEXTURE_PREFIX str PNG_PREFIX
#define ADD_EFFECT_PNG_PREFIX(str) EFFECT_PREFIX str PNG_PREFIX
#define ADD_UI_PNG_PREFIX(str) UI_PREFIX str PNG_PREFIX
#define ADD_UI_PAUSE_PNG_PREFIX(str) UI_PAUSE_PREFIX str PNG_PREFIX
#define ADD_IMGUI_JPG_PREFIX(str) IMGUI_PREFIX str JPG_PREFIX
#define ADD_IMGUI_PNG_PREFIX(str) IMGUI_PREFIX str PNG_PREFIX
#define ADD_TITLE_PNG_PREFIX(str) TITLE_PREFIX str PNG_PREFIX
#define ADD_STAGESELECT_PNG_PREFIX(str) STAGESELECT_PREFIX str PNG_PREFIX

CTextureLoader::CTextureLoader()
{
	m_TexturePathDataList =
	{
		{ ETextureType::TEXT_FONT, ADD_TEXTURE_PNG_PREFIX("ascii") },
		{ ETextureType::ACORN_ANIME, ADD_TEXTURE_PNG_PREFIX("acorn_anime") },
		{ ETextureType::UI_HEART, ADD_UI_PNG_PREFIX("heart") },
		{ ETextureType::UI_DAMAGE_HEART, ADD_UI_PNG_PREFIX("damage_heart") },
		{ ETextureType::UI_CROSS, ADD_UI_PNG_PREFIX("cross") },
		{ ETextureType::UI_ACORN, ADD_UI_PNG_PREFIX("acorn") },
		{ ETextureType::UI_STAR, ADD_UI_PNG_PREFIX("star") },
		{ ETextureType::UI_NUMBER, ADD_UI_PNG_PREFIX("number") },
		{ ETextureType::UI_GET_STAR, ADD_UI_PNG_PREFIX("get_star") },
		{ ETextureType::UI_GAMEPAD_BUTTON, ADD_UI_PNG_PREFIX("gamepad_button") },
		{ ETextureType::UI_JUMP, ADD_UI_PNG_PREFIX("jump") },
		{ ETextureType::UI_CAMERA_RESET, ADD_UI_PNG_PREFIX("camera_reset") },
		{ ETextureType::UI_CROUCHING, ADD_UI_PNG_PREFIX("crouching") },
		{ ETextureType::UI_DIVE, ADD_UI_PNG_PREFIX("dive") },
		{ ETextureType::UI_SPIN, ADD_UI_PNG_PREFIX("spin") },
		{ ETextureType::UI_TAIL_ATTACK, ADD_UI_PNG_PREFIX("tail_attack") },
		{ ETextureType::UI_DASH_JUMP, ADD_UI_PNG_PREFIX("dash_jump") },
		{ ETextureType::UI_WALL_JUMP, ADD_UI_PNG_PREFIX("wall_jump") },
		{ ETextureType::UI_HIGH_JUMP, ADD_UI_PNG_PREFIX("high_jump") },
		{ ETextureType::UI_TURN_JUMP, ADD_UI_PNG_PREFIX("turn_jump") },
		{ ETextureType::UI_HIPDROP, ADD_UI_PNG_PREFIX("hipdrop") },
		{ ETextureType::UI_LB_BUTTON, ADD_UI_PNG_PREFIX("lb") },
		{ ETextureType::UI_RB_BUTTON, ADD_UI_PNG_PREFIX("rb") },
		{ ETextureType::UI_RT_BUTTON, ADD_UI_PNG_PREFIX("rt") },
		{ ETextureType::UI_LT_BUTTON, ADD_UI_PNG_PREFIX("lt") },
		{ ETextureType::UI_PAUSE, ADD_UI_PAUSE_PNG_PREFIX("pause") },
		{ ETextureType::UI_TITLE_BACK, ADD_UI_PAUSE_PNG_PREFIX("title_back") },
		{ ETextureType::UI_TITLE_BACK_RED, ADD_UI_PAUSE_PNG_PREFIX("title_back_red") },
		{ ETextureType::UI_OPTION, ADD_UI_PAUSE_PNG_PREFIX("option") },
		{ ETextureType::UI_OPTION_RED, ADD_UI_PAUSE_PNG_PREFIX("option_red") },
		{ ETextureType::UI_GAME_BACK, ADD_UI_PAUSE_PNG_PREFIX("game_back") },
		{ ETextureType::UI_CAMERA_UPDOWN, ADD_UI_PAUSE_PNG_PREFIX("camera_updown") },
		{ ETextureType::UI_CAMERA_LEFTRIGHT, ADD_UI_PAUSE_PNG_PREFIX("camera_leftright") },
		{ ETextureType::UI_CAMERA_SPEED, ADD_UI_PAUSE_PNG_PREFIX("camera_speed") },
		{ ETextureType::UI_REVERSE, ADD_UI_PAUSE_PNG_PREFIX("reverse") },
		{ ETextureType::UI_HIGH, ADD_UI_PAUSE_PNG_PREFIX("high") },
		{ ETextureType::UI_BACK, ADD_UI_PAUSE_PNG_PREFIX("back") },
		{ ETextureType::UI_SLOW, ADD_UI_PAUSE_PNG_PREFIX("slow") },
		{ ETextureType::UI_USUALLY, ADD_UI_PAUSE_PNG_PREFIX("usually") },
		{ ETextureType::UI_NORMAL, ADD_UI_PAUSE_PNG_PREFIX("normal") },
		{ ETextureType::UI_ARROW, ADD_UI_PAUSE_PNG_PREFIX("arrow") },
		{ ETextureType::UI_YES, ADD_UI_PAUSE_PNG_PREFIX("yes") },
		{ ETextureType::UI_NO, ADD_UI_PAUSE_PNG_PREFIX("no") },
		{ ETextureType::BLACK, ADD_EFFECT_PNG_PREFIX("black") },
		{ ETextureType::CIRCLE_FADEIN, ADD_EFFECT_PNG_PREFIX("circle_fadein") },
		{ ETextureType::CIRCLE_FADEOUT, ADD_EFFECT_PNG_PREFIX("circle_fadeout") },
		{ ETextureType::EFFECT_WALL_HIT, ADD_EFFECT_PNG_PREFIX("wall_hit") },
		{ ETextureType::EFFECT_DASH, ADD_EFFECT_PNG_PREFIX("dash") },
		{ ETextureType::EFFECT_JUMP, ADD_EFFECT_PNG_PREFIX("jump") },
		{ ETextureType::EFFECT_STUN, ADD_EFFECT_PNG_PREFIX("stun") },
		{ ETextureType::EFFECT_ITEM_GET, ADD_EFFECT_PNG_PREFIX("item_get") },
		{ ETextureType::EFFECT_SMOKE, ADD_EFFECT_PNG_PREFIX("smoke") },
		{ ETextureType::EFFECT_REVIVAL, ADD_EFFECT_PNG_PREFIX("revival") },
		{ ETextureType::EFFECT_EXPLOSION, ADD_EFFECT_PNG_PREFIX("explosion") },
		{ ETextureType::EFFECT_EXPLOSION2, ADD_EFFECT_PNG_PREFIX("explosion2") },
		{ ETextureType::EFFECT_HEART, ADD_EFFECT_PNG_PREFIX("heart") },
		{ ETextureType::EFFECT_ATTACK_HIT, ADD_EFFECT_PNG_PREFIX("attack_hit") },
		{ ETextureType::EFFECT_ATTACK_HIT2, ADD_EFFECT_PNG_PREFIX("attack_hit2") },
		{ ETextureType::EFFECT_ATTACK_HIT3, ADD_EFFECT_PNG_PREFIX("attack_hit3") },
		{ ETextureType::EFFECT_PLAYER_HIT, ADD_EFFECT_PNG_PREFIX("player_hit") },
		{ ETextureType::EFFECT_GROUND_HIT, ADD_EFFECT_PNG_PREFIX("ground_hit") },
		{ ETextureType::BOM_ANIME, ADD_TEXTURE_PNG_PREFIX("bom_anime") },
		{ ETextureType::NORMAL_BOM, ADD_TEXTURE_PNG_PREFIX("normal_bom") },
		{ ETextureType::TARGET, ADD_TEXTURE_PNG_PREFIX("target") },
		{ ETextureType::STAR_DOOR1, ADD_TEXTURE_PNG_PREFIX("star_door1") },
		{ ETextureType::STAR_DOOR2, ADD_TEXTURE_PNG_PREFIX("star_door2") },
		{ ETextureType::STAR_DOOR3, ADD_TEXTURE_PNG_PREFIX("star_door3") },
		{ ETextureType::STAR_DOOR4, ADD_TEXTURE_PNG_PREFIX("star_door4") },
		{ ETextureType::STAR_DOOR5, ADD_TEXTURE_PNG_PREFIX("star_door5") },
		{ ETextureType::STAR_DOOR6, ADD_TEXTURE_PNG_PREFIX("star_door6") },
		{ ETextureType::STAR_DOOR7, ADD_TEXTURE_PNG_PREFIX("star_door7") },
		{ ETextureType::BODY1, ADD_TEXTURE_PNG_PREFIX("body1") },
		{ ETextureType::BODY2, ADD_TEXTURE_PNG_PREFIX("body2") },
		{ ETextureType::BODY3, ADD_TEXTURE_PNG_PREFIX("body3") },
		{ ETextureType::STAGESELECT, ADD_STAGESELECT_PNG_PREFIX("satgeselect") },
		{ ETextureType::STAGESELECT_BUTTON, ADD_STAGESELECT_PNG_PREFIX("stageselect_button") },
		{ ETextureType::STAGESELECT_SLASH, ADD_STAGESELECT_PNG_PREFIX("slash") },
		{ ETextureType::STAGESELECT_CHUTORIAL, ADD_STAGESELECT_PNG_PREFIX("chutorial") },
		{ ETextureType::STAGESELECT_FIRSTSTAGE, ADD_STAGESELECT_PNG_PREFIX("firststage") },
		{ ETextureType::STAGESELECT_SECONDSTAGE, ADD_STAGESELECT_PNG_PREFIX("secondstage") },
		{ ETextureType::STAGESELECT_SELECT_ICON, ADD_STAGESELECT_PNG_PREFIX("select_icon") },
		{ ETextureType::TUTORIAL1, ADD_UI_PNG_PREFIX("tutorial1") },
		{ ETextureType::TUTORIAL2, ADD_UI_PNG_PREFIX("tutorial2") },
		{ ETextureType::TUTORIAL3, ADD_UI_PNG_PREFIX("tutorial3") },
		{ ETextureType::TUTORIAL4, ADD_UI_PNG_PREFIX("tutorial4") },
		{ ETextureType::TUTORIAL5, ADD_UI_PNG_PREFIX("tutorial5") },
		{ ETextureType::TUTORIAL6, ADD_UI_PNG_PREFIX("tutorial6") },
		{ ETextureType::TUTORIAL7, ADD_UI_PNG_PREFIX("tutorial7") },
		{ ETextureType::TUTORIAL8, ADD_UI_PNG_PREFIX("tutorial8") },
		{ ETextureType::UI_STAGE_CLEAR, ADD_UI_PNG_PREFIX("stage_clear") },
		{ ETextureType::UI_BACK_BUTTON, ADD_UI_PNG_PREFIX("back_button") },
		{ ETextureType::UI_NEXT, ADD_UI_PNG_PREFIX("next") },
		{ ETextureType::IMGUI_ICON_ACORN, ADD_IMGUI_PNG_PREFIX("acorn") },
		{ ETextureType::IMGUI_ICON_THORNBOX, ADD_IMGUI_PNG_PREFIX("thorn_box") },
		{ ETextureType::IMGUI_ICON_TILEBLOCK, ADD_IMGUI_PNG_PREFIX("tile_block") },
		{ ETextureType::IMGUI_ICON_WOODBRIDGE, ADD_IMGUI_PNG_PREFIX("wood_bridge") },
		{ ETextureType::IMGUI_ICON_STONETREE, ADD_IMGUI_PNG_PREFIX("stone_tree") },
		{ ETextureType::IMGUI_ICON_STONEBLOCK, ADD_IMGUI_PNG_PREFIX("stone_block") },
		{ ETextureType::IMGUI_ICON_STUMP, ADD_IMGUI_PNG_PREFIX("stump") },
		{ ETextureType::IMGUI_ICON_GRASSWOOD, ADD_IMGUI_PNG_PREFIX("grass_wood") },
		{ ETextureType::IMGUI_ICON_GRASSSTONE, ADD_IMGUI_PNG_PREFIX("grass_stone") },
		{ ETextureType::IMGUI_ICON_GRASSPILLAR, ADD_IMGUI_PNG_PREFIX("grass_pillar") },
		{ ETextureType::IMGUI_ICON_NO_CLIFFBOX, ADD_IMGUI_PNG_PREFIX("nocliffbox") },
		{ ETextureType::IMGUI_ICON_SLIME, ADD_IMGUI_PNG_PREFIX("slime") },
		{ ETextureType::IMGUI_ICON_MONKEY, ADD_IMGUI_PNG_PREFIX("monkey") },
		{ ETextureType::IMGUI_ICON_THORNMAN, ADD_IMGUI_PNG_PREFIX("thornman") },
		{ ETextureType::IMGUI_ICON_TRIANGLE, ADD_IMGUI_PNG_PREFIX("triangle") },
		{ ETextureType::IMGUI_ICON_PLANE_YELLOW, ADD_IMGUI_PNG_PREFIX("plane_yellow") },
		{ ETextureType::IMGUI_ICON_PLANE_STONE, ADD_IMGUI_PNG_PREFIX("plane_stone") },
		{ ETextureType::IMGUI_ICON_PLANE_GREEN, ADD_IMGUI_PNG_PREFIX("plane_green") },
		{ ETextureType::IMGUI_ICON_PLANE_GRASS, ADD_IMGUI_PNG_PREFIX("plane_grass") },
		{ ETextureType::IMGUI_ICON_PLANE_GRASS2, ADD_IMGUI_PNG_PREFIX("plane_grass2") },
		{ ETextureType::IMGUI_ICON_WALL_GRASS, ADD_IMGUI_PNG_PREFIX("wall_grass") },
		{ ETextureType::IMGUI_ICON_WALL_WOOD, ADD_IMGUI_PNG_PREFIX("wall_wood") },
		{ ETextureType::IMGUI_ICON_STAR_DOOR, ADD_IMGUI_PNG_PREFIX("star_door") },
		{ ETextureType::IMGUI_ICON_TRIANGLE2, ADD_IMGUI_PNG_PREFIX("triangle2") },
		{ ETextureType::IMGUI_ICON_SIMPLE_GRASSBOX, ADD_IMGUI_PNG_PREFIX("simple_grassbox") },
		{ ETextureType::IMGUI_ICON_GRASS_CIRCLE, ADD_IMGUI_PNG_PREFIX("grass_circle") },
		{ ETextureType::IMGUI_ICON_GRASS_BOX_YELLOW, ADD_IMGUI_PNG_PREFIX("grass_box_yellow") },
		{ ETextureType::IMGUI_ICON_START_TABLE, ADD_IMGUI_PNG_PREFIX("start_table") },
		{ ETextureType::IMGUI_ICON_SKY_DOME, ADD_IMGUI_PNG_PREFIX("skydome") },
		{ ETextureType::IMGUI_ICON_PLAYER_START, ADD_IMGUI_PNG_PREFIX("player_start") },
		{ ETextureType::IMGUI_ICON_WALL_GRASS2, ADD_IMGUI_PNG_PREFIX("wall_grass2") },
		{ ETextureType::IMGUI_ICON_PLANE_YELLOW2, ADD_IMGUI_PNG_PREFIX("plane_yellow2") },
		{ ETextureType::IMGUI_ICON_BLOCK_COLLISION, ADD_IMGUI_PNG_PREFIX("block_collision") },
		{ ETextureType::IMGUI_ICON_BUSH, ADD_IMGUI_PNG_PREFIX("bush") },
		{ ETextureType::IMGUI_ICON_BUSH2, ADD_IMGUI_PNG_PREFIX("bush2") },
		{ ETextureType::IMGUI_ICON_FORN, ADD_IMGUI_PNG_PREFIX("forn") },
		{ ETextureType::IMGUI_ICON_BUSH3, ADD_IMGUI_PNG_PREFIX("bush3") },
		{ ETextureType::IMGUI_ICON_BUSH4, ADD_IMGUI_PNG_PREFIX("bush4") },
		{ ETextureType::IMGUI_ICON_BUSH5, ADD_IMGUI_PNG_PREFIX("bush5") },
		{ ETextureType::IMGUI_ICON_PLANT, ADD_IMGUI_PNG_PREFIX("plant") },
		{ ETextureType::IMGUI_ICON_GRASSES, ADD_IMGUI_PNG_PREFIX("grasses") },
		{ ETextureType::IMGUI_ICON_FLOWER, ADD_IMGUI_PNG_PREFIX("flower") },
		{ ETextureType::IMGUI_ICON_FLOWER2, ADD_IMGUI_PNG_PREFIX("flower2") },
		{ ETextureType::IMGUI_ICON_FLOWER3, ADD_IMGUI_PNG_PREFIX("flower3") },
		{ ETextureType::IMGUI_ICON_ROCK, ADD_IMGUI_PNG_PREFIX("rock") },
		{ ETextureType::IMGUI_ICON_ROCK2, ADD_IMGUI_PNG_PREFIX("rock2") },
		{ ETextureType::IMGUI_ICON_ROCK3, ADD_IMGUI_PNG_PREFIX("rock3") },
		{ ETextureType::IMGUI_ICON_ROCK4, ADD_IMGUI_PNG_PREFIX("rock4") },
		{ ETextureType::IMGUI_ICON_ROCK5, ADD_IMGUI_PNG_PREFIX("rock5") },
		{ ETextureType::IMGUI_ICON_TREE, ADD_IMGUI_PNG_PREFIX("tree") },
		{ ETextureType::IMGUI_ICON_FIRSTBOSS_FIELD, ADD_IMGUI_PNG_PREFIX("firstboss_field") },
		{ ETextureType::IMGUI_ICON_FIRSTBOSS_WALL, ADD_IMGUI_PNG_PREFIX("firstboss_wall") },
		{ ETextureType::IMGUI_ICON_SPHERE, ADD_IMGUI_PNG_PREFIX("sphere") },
		{ ETextureType::IMGUI_ICON_TEST_BOX, ADD_IMGUI_PNG_PREFIX("test_box") },
		{ ETextureType::IMGUI_ICON_WINDOWSOUTWARDS, ADD_IMGUI_PNG_PREFIX("windowsoutwards") },
		{ ETextureType::IMGUI_ICON_WINDOWSOUTWARDSFRAME, ADD_IMGUI_PNG_PREFIX("windowsoutwardsframe") },
		{ ETextureType::IMGUI_ICON_WINDOWSSINGLEHIGH, ADD_IMGUI_PNG_PREFIX("windowssinglehigh") },
		{ ETextureType::IMGUI_ICON_WINDOWSDOUBLEHIGH, ADD_IMGUI_PNG_PREFIX("windowsdoublehigh") },
		{ ETextureType::IMGUI_ICON_WINDOWSROUND, ADD_IMGUI_PNG_PREFIX("windowsround") },
		{ ETextureType::IMGUI_ICON_WINDOWSSINGLEHIGHBOARDS, ADD_IMGUI_PNG_PREFIX("windowssinglehighboards") },
		{ ETextureType::IMGUI_ICON_DOCK, ADD_IMGUI_PNG_PREFIX("dock") },
		{ ETextureType::IMGUI_ICON_DOOR2STEP, ADD_IMGUI_PNG_PREFIX("door2step") },
		{ ETextureType::IMGUI_ICON_DOOR3, ADD_IMGUI_PNG_PREFIX("door3") },
		{ ETextureType::IMGUI_ICON_DOOR4, ADD_IMGUI_PNG_PREFIX("door4") },
		{ ETextureType::IMGUI_ICON_DOOR1, ADD_IMGUI_PNG_PREFIX("door1") },
		{ ETextureType::IMGUI_ICON_HOUSE1, ADD_IMGUI_PNG_PREFIX("house1") },
		{ ETextureType::IMGUI_ICON_HOUSE2, ADD_IMGUI_PNG_PREFIX("house2") },
		{ ETextureType::IMGUI_ICON_HOUSE3, ADD_IMGUI_PNG_PREFIX("house3") },
		{ ETextureType::IMGUI_ICON_HOUSE4, ADD_IMGUI_PNG_PREFIX("house4") },
		{ ETextureType::IMGUI_ICON_ONEHOUSE1, ADD_IMGUI_PNG_PREFIX("onehouse1") },
		{ ETextureType::IMGUI_ICON_ONEHOUSE2, ADD_IMGUI_PNG_PREFIX("onehouse2") },
		{ ETextureType::IMGUI_ICON_ONEHOUSE3, ADD_IMGUI_PNG_PREFIX("onehouse3") },
		{ ETextureType::IMGUI_ICON_ONEHOUSE4, ADD_IMGUI_PNG_PREFIX("onehouse4") },
		{ ETextureType::IMGUI_ICON_BALCONY, ADD_IMGUI_PNG_PREFIX("Balcony1") },
		{ ETextureType::IMGUI_ICON_BALCONY2, ADD_IMGUI_PNG_PREFIX("Balcony2") },
		{ ETextureType::IMGUI_ICON_CHIMNEY, ADD_IMGUI_PNG_PREFIX("Chimney") },
		{ ETextureType::IMGUI_ICON_CHIMNEY2, ADD_IMGUI_PNG_PREFIX("Chimney2") },
		{ ETextureType::IMGUI_ICON_ROOFFRAME, ADD_IMGUI_PNG_PREFIX("roofframe") },
		{ ETextureType::IMGUI_ICON_ROOFFRAME2, ADD_IMGUI_PNG_PREFIX("roofframe2") },
		{ ETextureType::IMGUI_ICON_ROOFFRAME3, ADD_IMGUI_PNG_PREFIX("roofframe3") },
		{ ETextureType::IMGUI_ICON_ROOFFRAME4, ADD_IMGUI_PNG_PREFIX("roofframe4") },
		{ ETextureType::IMGUI_ICON_STAIRWIDE, ADD_IMGUI_PNG_PREFIX("stairwide") },
		{ ETextureType::IMGUI_ICON_STAIRWIDE2, ADD_IMGUI_PNG_PREFIX("stairwide2") },
		{ ETextureType::IMGUI_ICON_ROOFFRAME5, ADD_IMGUI_PNG_PREFIX("roofframe5") },
		{ ETextureType::IMGUI_ICON_ROOFFRAME6, ADD_IMGUI_PNG_PREFIX("roofframe6") },
		{ ETextureType::IMGUI_ICON_VARIANT, ADD_IMGUI_PNG_PREFIX("variant") },
		{ ETextureType::IMGUI_ICON_COLUMN, ADD_IMGUI_PNG_PREFIX("column") },
		{ ETextureType::IMGUI_ICON_ARCBIG, ADD_IMGUI_PNG_PREFIX("arcbig") },
		{ ETextureType::IMGUI_ICON_ARCCOMER, ADD_IMGUI_PNG_PREFIX("arccomer") },
		{ ETextureType::IMGUI_ICON_ARCCROSS, ADD_IMGUI_PNG_PREFIX("arccross") },
		{ ETextureType::IMGUI_ICON_ARCTRIPLE, ADD_IMGUI_PNG_PREFIX("arctriple") },
		{ ETextureType::IMGUI_ICON_BARREL, ADD_IMGUI_PNG_PREFIX("barrel") },
		{ ETextureType::IMGUI_ICON_COFFIN, ADD_IMGUI_PNG_PREFIX("coffin") },
		{ ETextureType::IMGUI_ICON_COFFINCOVER, ADD_IMGUI_PNG_PREFIX("coffincover") },
		{ ETextureType::IMGUI_ICON_PEREKR, ADD_IMGUI_PNG_PREFIX("Perekr") },
		{ ETextureType::IMGUI_ICON_WOODBOX, ADD_IMGUI_PNG_PREFIX("woodbox") },
		{ ETextureType::IMGUI_ICON_CLAIMEDE, ADD_IMGUI_PNG_PREFIX("ClaimedE") },
		{ ETextureType::IMGUI_ICON_CLAIMEDWALL2C, ADD_IMGUI_PNG_PREFIX("Claimedwall2C") },
		{ ETextureType::IMGUI_ICON_DIRTDARKE, ADD_IMGUI_PNG_PREFIX("Dirt_darkE") },
		{ ETextureType::IMGUI_ICON_FLOOR, ADD_IMGUI_JPG_PREFIX("floor") },
		{ ETextureType::IMGUI_ICON_STONE, ADD_IMGUI_JPG_PREFIX("stone") },
		{ ETextureType::IMGUI_ICON_TOP, ADD_IMGUI_JPG_PREFIX("top") },
		{ ETextureType::IMGUI_ICON_WALL2_1, ADD_IMGUI_JPG_PREFIX("wall2_1") },
		{ ETextureType::IMGUI_ICON_WALL22, ADD_IMGUI_JPG_PREFIX("wall22") },
		{ ETextureType::IMGUI_ICON_DAISY, ADD_IMGUI_JPG_PREFIX("daisy") },
		{ ETextureType::IMGUI_ICON_SELECT_PLANE, ADD_IMGUI_JPG_PREFIX("select_plane") },
		{ ETextureType::IMGUI_ICON_CHAIR, ADD_IMGUI_PNG_PREFIX("chair") },
		{ ETextureType::IMGUI_ICON_CHEST, ADD_IMGUI_PNG_PREFIX("chest") },
		{ ETextureType::IMGUI_ICON_SMALLTABLE, ADD_IMGUI_PNG_PREFIX("smalltable") },
		{ ETextureType::IMGUI_ICON_TABLE, ADD_IMGUI_PNG_PREFIX("table") },
		{ ETextureType::IMGUI_ICON_GREENBOOK, ADD_IMGUI_PNG_PREFIX("greenbook") },
		{ ETextureType::IMGUI_ICON_PURPLEBOOK, ADD_IMGUI_PNG_PREFIX("purple_book") },
		{ ETextureType::IMGUI_ICON_YELLOWBOOK, ADD_IMGUI_PNG_PREFIX("yellow_book") },
		{ ETextureType::IMGUI_ICON_REDBOOK, ADD_IMGUI_PNG_PREFIX("red_book") },
		{ ETextureType::IMGUI_ICON_STAGE_FRAME, ADD_IMGUI_PNG_PREFIX("stage_frame") },
		{ ETextureType::IMGUI_ICON_STAGE_FRAME2, ADD_IMGUI_PNG_PREFIX("stage_frame2") },
		{ ETextureType::IMGUI_ICON_STAGE_FRAME3, ADD_IMGUI_PNG_PREFIX("stage_frame3") },
		{ ETextureType::IMGUI_ICON_BOSS_FIGURE, ADD_IMGUI_PNG_PREFIX("BossFigure") },
		{ ETextureType::IMGUI_ICON_DOLLY, ADD_IMGUI_PNG_PREFIX("dolly") },
		{ ETextureType::IMGUI_ICON_WOODEN, ADD_IMGUI_PNG_PREFIX("wooden") },
		{ ETextureType::IMGUI_ICON_WOODEN2, ADD_IMGUI_PNG_PREFIX("wooden2") },
		{ ETextureType::IMGUI_ICON_WOODEN3, ADD_IMGUI_PNG_PREFIX("wooden3") },
		{ ETextureType::IMGUI_ICON_WOODEN4, ADD_IMGUI_PNG_PREFIX("wooden4") },
		{ ETextureType::IMGUI_ICON_WOODEN5, ADD_IMGUI_PNG_PREFIX("wooden5") },
		{ ETextureType::IMGUI_ICON_WOODEN6, ADD_IMGUI_PNG_PREFIX("wooden6") },
		{ ETextureType::IMGUI_ICON_WOODEN7, ADD_IMGUI_PNG_PREFIX("wooden7") },
		{ ETextureType::IMGUI_ICON_CHEST2, ADD_IMGUI_PNG_PREFIX("chest2") },
		{ ETextureType::IMGUI_ICON_CHESTTOP2, ADD_IMGUI_PNG_PREFIX("chesttop2") },
		{ ETextureType::IMGUI_ICON_ROCKS, ADD_IMGUI_PNG_PREFIX("rocks") },
		{ ETextureType::IMGUI_ICON_STALAGS, ADD_IMGUI_PNG_PREFIX("stalags") },
		{ ETextureType::IMGUI_ICON_COINS, ADD_IMGUI_PNG_PREFIX("coins") },
		{ ETextureType::IMGUI_ICON_RAIL, ADD_IMGUI_PNG_PREFIX("rail") },
		{ ETextureType::IMGUI_ICON_TEST_STAGE, ADD_IMGUI_PNG_PREFIX("TestStage") },
	};
}

CTextureLoader::~CTextureLoader()
{
	m_TexturePathDataList.clear();
	m_TextureResourceDataList.clear();
	m_TextureLoadDataList.clear();
}

std::weak_ptr<ID3D11ShaderResourceView> CTextureLoader::Load(const ETextureType TextureType)
{
	if (!m_TexturePathDataList.contains(TextureType))
	{
		return std::weak_ptr<ID3D11ShaderResourceView>();
	}
	return Load(m_TexturePathDataList[TextureType]);
}

std::weak_ptr<ID3D11ShaderResourceView> CTextureLoader::Load(const std::string& FileName)
{
	//すでにロード済みのテクスチャがあればそれを返す
	if (m_TextureLoadDataList.contains(FileName))
	{
		TextureData& textureData = m_TextureLoadDataList[FileName];
		textureData.IncrementLoadCount();
		return textureData.GetTexture();
	}
	ID3D11ShaderResourceView* loadTexture = nullptr;
	//WICが扱えるようにwChar_tに変換
	size_t converted = 0;
	wchar_t wchar[MAX_PATH] = { 0 };
	mbstowcs_s(&converted, wchar, MAX_PATH, FileName.c_str(), _TRUNCATE);

	HRESULT hr = CreateWICTextureFromFile((CRenderer::GetDevice()), wchar, nullptr, &loadTexture);
	assert(SUCCEEDED(hr));//テクスチャの読み込みに失敗した場合は止める

	//読み込んだテクスチャを管理リストに追加
	m_TextureResourceDataList.emplace(loadTexture, FileName);
	TextureData textureData(loadTexture);
	m_TextureLoadDataList.emplace(FileName, textureData);
	return textureData.GetTexture();
}

void CTextureLoader::Release(std::weak_ptr<ID3D11ShaderResourceView>& Texture)
{
	if (!Texture.expired())
	{
		ID3D11ShaderResourceView* texture = Texture.lock().get();
		Release(texture);
		Texture.reset();
	}
}

void CTextureLoader::Release(ID3D11ShaderResourceView* Texture)
{
	if (!Texture)
	{
		return;
	}

	if (!m_TextureResourceDataList.contains(Texture))
	{
		return;
	}
	
	//参照テクスチャが0になったらアンロード
	const std::string fileName = m_TextureResourceDataList[Texture];
	TextureData& textureData = m_TextureLoadDataList[fileName];
	textureData.DecrementLoadCount();
	if (!textureData.IsUnloaded())
	{
		return;
	}
	m_TextureResourceDataList.erase(Texture);
	m_TextureLoadDataList.erase(fileName);
}

UINT CTextureLoader::GetWidth(const ETextureType TextureType) const
{
	if (!m_TexturePathDataList.contains(TextureType))
	{
		return 0;
	}
	const std::string& fileName = m_TexturePathDataList.at(TextureType);
	if (!m_TextureLoadDataList.contains(fileName))
	{
		return 0;
	}
	const TextureData& textureData = m_TextureLoadDataList.at(fileName);
	return textureData.GetWidth();
}

UINT CTextureLoader::GetHeight(const ETextureType TextureType) const
{
	if (!m_TexturePathDataList.contains(TextureType))
	{
		return 0;
	}
	const std::string& fileName = m_TexturePathDataList.at(TextureType);
	if (!m_TextureLoadDataList.contains(fileName))
	{
		return 0;
	}
	const TextureData& textureData = m_TextureLoadDataList.at(fileName);
	return textureData.GetHeight();
}
