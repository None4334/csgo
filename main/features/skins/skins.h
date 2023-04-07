#pragma once


struct SkinInfo_t {
	int iSeed = -1;
	int iPaintKit;
	int rarity;
	std::string tagName;
	std::string     shortname;
	std::string     name;
};

namespace TPaintKits
{
	struct paint_kit
	{
		int id;
		std::string name;

		auto operator < (const paint_kit& other) const -> bool
		{
			return name < other.name;
		}
	};

	inline std::vector<paint_kit> skin_kits;
	inline std::vector<paint_kit> glove_kits;
	inline std::vector<paint_kit> sticker_kits;
	inline std::unordered_map<std::string, std::set<std::string>> weaponSkins;
	inline std::unordered_map<std::string, SkinInfo_t> skinMap;
	inline std::unordered_map<std::string, std::string> skinNames;

	void Initialize();
	int ParseSkins();
}

struct HudWeapons_t {
	std::int32_t* GetWeaponCount() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

struct WeaponName_t {

	constexpr WeaponName_t(int32_t iDefIndex, const char* charNam) :
		iDefinitionIndex(iDefIndex),
		charName(charNam) {
	}

	int iDefinitionIndex = 0;
	const char* charName = nullptr;
};
struct WeaponInfo_t {
	constexpr WeaponInfo_t(const char* charMdl, const char* charIcn = nullptr) :
		charModel(charMdl),
		charIcon(charIcn)
	{}

	const char* charModel;
	const char* charIcon;
};

struct ModelAddress_t {
	std::string szMenuName;
	std::string szAddress;
	bool bIsTSideModel = false;
};

static const std::vector<ModelAddress_t> vecModels = {
	{"None", "None", true},
	{"Silent", "models/player/custom_player/legacy/tm_professional_varf1.mdl", true},
	{"Vypa Sista of the Revolution", "models/player/custom_player/legacy/tm_jungle_raider_variante.mdl", true},
	{"'Medium Rare' Crasswater", "models/player/custom_player/legacy/tm_jungle_raider_variantb2.mdl", true},
	{"Crasswater The Forgotten", "models/player/custom_player/legacy/tm_jungle_raider_variantb.mdl", true},
	{"Skullhead", "models/player/custom_player/legacy/tm_professional_varf2.mdl", true},
	{"Chef d'Escadron Rouchard", "models/player/custom_player/legacy/ctm_gendarmerie_variantc.mdl", false},
	{"Loudmouth", "models/player/custom_player/legacy/tm_professional_varf4.mdl", true},
	{"Getaway Sally", "models/player/custom_player/legacy/tm_professional_varj.mdl", true},
	{"Elite Trapper Solman", "models/player/custom_player/legacy/tm_jungle_raider_varianta.mdl", true},
	{"Bloody Darryl The Strapped", "models/player/custom_player/legacy/tm_professional_varf5.mdl", true},
	{"Chem-Haz Capitaine", "models/player/custom_player/legacy/ctm_gendarmerie_variantb.mdl", false},
	{"Lieutenant Rex Krikey", "models/player/custom_player/legacy/ctm_diver_variantc.mdl", false},
	{"Col. Mangos Dabisi", "models/player/custom_player/legacy/tm_jungle_raider_variantd.mdl", true},
	{"Trapper", "models/player/custom_player/legacy/tm_jungle_raider_variantf2.mdl", true},
	{"Lieutenant 'Tree Hugger' Farlow", "models/player/custom_player/legacy/ctm_swat_variantk.mdl", false},
	{"Sous-Lieutenant Medic", "models/player/custom_player/legacy/ctm_gendarmerie_varianta.mdl", false},
	{"Primeiro Tenente", "models/player/custom_player/legacy/ctm_st6_variantn.mdl", false},
	{"Trapper Aggressor", "models/player/custom_player/legacy/tm_jungle_raider_variantf.mdl", true},
	{"AGENT Gandon", "models/player/custom_player/legacy/tm_professional_vari.mdl", true},
	{"Safecracker Voltzmann", "models/player/custom_player/legacy/tm_professional_varg.mdl", true},
	{"Little Kev", "models/player/custom_player/legacy/tm_professional_varh.mdl", true},
	{"Blackwolf", "models/player/custom_player/legacy/tm_balkan_variantj.mdl", true},
	{"Rezan The Ready", "models/player/custom_player/legacy/tm_balkan_variantg.mdl", true},
	{"Maximus", "models/player/custom_player/legacy/tm_balkan_varianti.mdl", true},
	{"Dragomir", "models/player/custom_player/legacy/tm_balkan_variantf.mdl", true},
	{"Lt. Commander Ricksaw", "models/player/custom_player/legacy/ctm_st6_varianti.mdl", false},
	{"'Two Times' McCoy", "models/player/custom_player/legacy/ctm_st6_variantm.mdl", false},
	{"Buckshot", "models/player/custom_player/legacy/ctm_st6_variantg.mdl", false},
	{"Seal Team 6 Soldier", "models/player/custom_player/legacy/ctm_st6_variante.mdl", false},
	{"3rd Commando Company", "models/player/custom_player/legacy/ctm_st6_variantk.mdl", false},
	{"'The Doctor' Romanov", "models/player/custom_player/legacy/tm_balkan_varianth.mdl", true},
	{"John 'Van Healen' Kask", "models/player/custom_player/legacy/ctm_swat_variantg.mdl", false},
	{"Chem-Haz Specialist", "models/player/custom_player/legacy/ctm_swat_variantj.mdl", false},
	{"The Elite Mr. Muhlik", "models/player/custom_player/legacy/tm_leet_variantf.mdl", true},
	{"Lab Rat","models/player/custom_player/legacy/tm_jumpsuit_varianta.mdl", true},
};

const std::vector<WeaponName_t> KnifeNames =
{
	{0, "default"},
	{WEAPON_KNIFE_BAYONET, "bayonet"},
	{WEAPON_KNIFE_CSS, "classic knife"},
	{WEAPON_KNIFE_CORD, "cord knife"},
	{WEAPON_KNIFE_CANIS, "canis knife"},
	{WEAPON_KNIFE_OUTDOOR, "outdoor knife"},
	{WEAPON_KNIFE_SKELETON, "skeleton knife"},
	{WEAPON_KNIFE_FLIP, "flip knife"},
	{WEAPON_KNIFE_GUT, "gut knife"},
	{WEAPON_KNIFE_KARAMBIT, "karambit"},
	{WEAPON_KNIFE_M9_BAYONET, "m9 bayonet"},
	{WEAPON_KNIFE_TACTICAL, "huntsman knife"},
	{WEAPON_KNIFE_FALCHION, "falchion knife"},
	{WEAPON_KNIFE_SURVIVAL_BOWIE, "bowie knife"},
	{WEAPON_KNIFE_BUTTERFLY, "butterfly knife"},
	{WEAPON_KNIFE_PUSH, "shadow daggers"},
	{WEAPON_KNIFE_URSUS, "ursus knife"},
	{WEAPON_KNIFE_GYPSY_JACKKNIFE, "navaja knife"},
	{WEAPON_KNIFE_STILETTO, "stiletto knife"},
	{WEAPON_KNIFE_WIDOWMAKER, "talon knife"}
};

const std::vector< WeaponName_t> WeaponNamesFull =
{
{WEAPON_KNIFE, "Knife"},
{GLOVE_T, "Glove"},
{ WEAPON_AK47, "AK-47" },
{ WEAPON_AUG, "AUG" },
{ WEAPON_AWP, "AWP" },
{ WEAPON_CZ75A, "CZ75 Auto" },
{ WEAPON_DEAGLE, "Desert Eagle" },
{ WEAPON_ELITE, "Dual Berettas" },
{ WEAPON_FAMAS, "FAMAS" },
{ WEAPON_FIVESEVEN, "Five-SeveN" },
{ WEAPON_G3SG1, "G3SG1" },
{ WEAPON_GALILAR, "Galil AR" },
{ WEAPON_GLOCK, "Glock-18" },
{ WEAPON_M249, "M249" },
{ WEAPON_M4A1_SILENCER, "M4A1-S" },
{ WEAPON_M4A1, "M4A4" },
{ WEAPON_MAC10, "MAC-10" },
{ WEAPON_MAG7, "MAG-7" },
{ WEAPON_MP7, "MP7" },
{ WEAPON_MP5SD, "MP5" },
{ WEAPON_MP9, "MP9" },
{ WEAPON_NEGEV, "Negev" },
{ WEAPON_NOVA, "Nova" },
{ WEAPON_HKP2000, "P2000" },
{ WEAPON_P250, "P250" },
{ WEAPON_P90, "P90" },
{ WEAPON_BIZON, "PP-Bizon" },
{ WEAPON_REVOLVER, "R8 Revolver" },
{ WEAPON_SAWEDOFF, "Sawed-Off" },
{ WEAPON_SCAR20, "SCAR-20" },
{ WEAPON_SSG08, "SSG 08" },
{ WEAPON_SG556, "SG 553" },
{ WEAPON_TEC9, "Tec-9" },
{ WEAPON_UMP45, "UMP-45" },
{ WEAPON_USP_SILENCER, "USP-S" },
{ WEAPON_XM1014, "XM1014" },
};

const std::map<size_t, WeaponInfo_t> WeaponInfo =
{
	{WEAPON_KNIFE,{"models/weapons/v_knife_default_ct.mdl", "models/weapons/w_knife_default_ct.mdl"}},
	{WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "models/weapons/w_knife_default_t.mdl"}},
	{WEAPON_KNIFE_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "models/weapons/w_knife_bayonet.mdl"}},
	{WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "models/weapons/w_knife_flip.mdl"}},
	{WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "models/weapons/w_knife_gut.mdl"}},
	{WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "models/weapons/w_knife_karam.mdl"}},
	{WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "models/weapons/w_knife_m9_bay.mdl"}},
	{WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "models/weapons/w_knife_tactical.mdl"}},
	{WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "models/weapons/w_knife_falchion_advanced.mdl"}},
	{WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "models/weapons/w_knife_survival_bowie.mdl"}},
	{WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "models/weapons/w_knife_butterfly.mdl"}},
	{WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "models/weapons/w_knife_push.mdl"}},
	{WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "models/weapons/w_knife_ursus.mdl"}},
	{WEAPON_KNIFE_GYPSY_JACKKNIFE,{"models/weapons/v_knife_gypsy_jackknife.mdl", "models/weapons/w_knife_gypsy_jackknife.mdl"}},
	{WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "models/weapons/w_knife_stiletto.mdl"}},
	{WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "models/weapons/w_knife_widowmaker.mdl"}},
	{WEAPON_KNIFE_CSS,{"models/weapons/v_knife_css.mdl", "models/weapons/w_knife_css.mdl"}},
	{WEAPON_KNIFE_SKELETON,{"models/weapons/v_knife_skeleton.mdl", "models/weapons/w_knife_skeleton.mdl"}},
	{WEAPON_KNIFE_OUTDOOR,{"models/weapons/v_knife_outdoor.mdl", "models/weapons/w_knife_outdoor.mdl"}},
	{WEAPON_KNIFE_CANIS, {"models/weapons/v_knife_canis.mdl", "models/weapons/w_knife_canis.mdl"}},
	{WEAPON_KNIFE_CORD, {"models/weapons/v_knife_cord.mdl", "models/weapons/w_knife_cord.mdl"}},
	{GLOVE_STUDDED_BLOODHOUND,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"}},
	{GLOVE_T,{"models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl"}},
	{GLOVE_CT,{"models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl"}},
	{GLOVE_SPORTY,{"models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"}},
	{GLOVE_SLICK,{"models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"}},
	{GLOVE_LEATHER_HANDWRAPS,{"models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"}},
	{GLOVE_MOTORCYCLE,{"models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"}},
	{GLOVE_SPECIALIST,{"models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"}},
	{GLOVE_STUDDED_HYDRA,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"}}
};

const std::vector<const char*> glove_skinz_blood = { "charred",
	"snakebite",
	"bronzed",
	"guerilla"
};

const std::vector<const char*> glove_skinz_sport = {
	"hedge maze",
	"pandoras box",
	"superconductor",
	"arid",
	"vice",
	"omega",
	"amphibious",
	"bronze morph"
};

const std::vector<const char*> glove_skinz_driver = {
"lunar weave",
"convoy",
"crimson weave",
"diamondback",
"overtake",
"racing green",
"king snake",
"imperial plaid"
};

const std::vector<const char*> glove_skinz_handwar = {
"leather",
"spruce ddpat",
"slaughter",
"badlands",
"cobalt skulls",
"overprint",
"duct tape",
"arboreal"
};

const std::vector<const char*> glove_skinz_motor = {
"eclipse",
"spearmint",
"boom",
"cool mint",
"turtle",
"transport",
"polygon",
"pow"
};
const std::vector<const char*> glove_skinz_specialist = {
"forest ddpat",
"crimson kimono",
"emerald web",
"foundation",
"crimson web",
"buckshot",
"fade",
"mogul"
};

const std::vector<const char*> glove_skinz_hydra = {
"emerald",
"mangrove",
"rattler",
"case hardened"
};

const std::vector<const char*> glove_skinz_fang = {
 "jade",
 "needle poin",
 "unhinged",
 "yellow-banded"
};

struct ItemSetting_t {
	char charName[32] = "";
	int iDefinitionVectorIndex = 0;
	int iDefinitionIndex = 1;
	int iPaintKitVectorIndex = 0;
	int iPaintKitIndex = 0;
	int iDefinitionOverrideVectorIndex = 0;
	int iDefinitionOverrideIndex = 0;
	int iSeed = 0;
	bool bEnableStatTrack = false;
	int iStatTrack = 0;
	float flWear = 0.0f;
};

struct Options_t {
	std::map<int, ItemSetting_t> Items;
	std::unordered_map<std::string, std::string> IconOverrides;
};

class CBaseAttributableItem;
namespace TSkinChanger
{
	CreateClientClassFn CreateWearable();
	CBaseAttributableItem* MakeGlove(int entry, int serial);
	int GetGloveSkinByMenu(int gloveIndex, int skinIndex);
	bool ApplyGloveModel(CBaseAttributableItem* glove, const char* model) noexcept;
	bool ApplyGloveSkins(CBaseAttributableItem* glove, int item_definition_index, int paint_kit, int model_index, int world_model_index, int entity_quality, float fallback_wear) noexcept;
	void RenderMenu();
	bool IsKnife(const int i);
	template <typename T>
	void PickCurrentWeapon(CBaseEntity* pLocalPlayer, int* idx, int* vec_idx, std::vector<T> arr);
	void PostDataUpdate(CBaseEntity* pLocalPlayer);
	void Start(bool m_bEnd);
	void UpdateGloves(CBaseEntity* pLocalPlayer);
	void UpdateSkins();
	void SetupConfig();
	void CheckSkinUpdate();
	void ModelChanger(int Stage , bool bForced = false);
	inline std::unordered_map<std::string, std::string> skinFixedNames;
	inline bool bStorePlayerModel = true;
	inline int iWeaponIndexSkins = 7;
	inline	int iWeaponVectorIndexSkins = 0;
	inline Options_t options;
	inline const char* GetIconOverride(const std::string szOriginal);
	inline void EraseOverrideIfExsistsByIndex(const int iDefinitionIndex);
	inline void ApplyConfigOnAttributableItem(CBaseEntity* pLocalPlayer, CBaseAttributableItem* pItem, const ItemSetting_t* pConfig, const unsigned xUidLow);
	inline const char* charIndex;
}
