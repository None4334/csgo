
struct TConfigVarBool {
	bool bEnabled = false;
	TColor Color;
};

enum EKeybindID {

	KEYBINDID_DEF=0,
	KEYBINDID_DT,
	KEYBINDID_HS,
	KEYBINDID_FD
};

enum {
	REMOVAL_NOSCOPE,
	REMOVAL_NOSCOPEZOOM,
	REMOVAL_NO_SMOKE, 
	REMOVAL_NO_FLASH, 
	REMOVAL_NO_VISUAL_RECOIL,
	REMOVAL_FOG,
	REMOVAL_SHADOW,
	REMOVAL_LANDING_BOB,
	REMOVAL_POST_PROCESSING,
	REMOVAL_SCOPE_SENSITIVITY_MODIFIER,
	REMOVAL_MAX
};

struct TConfigVarKeybind {
	
	TConfigVarKeybind() {

	}

	TConfigVarKeybind(EKeybindID ID) {
		KeyState.KeybindID = ID;
	}

	bool bEnabled = false;
	int iValue = 0;
	int iStyle = 0;
	struct {
		bool bState = false;
		bool bHolding = false;
		EKeybindID KeybindID = KEYBINDID_DEF;
	} KeyState;

};

struct TConfigVarInt {
	bool bEnabled = false;
	int Value = 0;
	TColor Color;
};

struct TConfigVarMap {
	bool bEnabled = false;
	std::unordered_map<int, bool> mapValues;
};

struct TConfigVarSelectable {
	bool bEnabled = false;
	TColor Color;
	int iSelectable = 0;
};

struct TConfigVarFloat {
	bool bEnabled = false;
	TColor Color;
	float flValue = 0;
};


enum EConfigHitboxList {
	CFG_HITBOXHEAD = 0,
	CFG_HITBOXCHEST,
	CFG_HITBOXSTOMACH,
	CFG_HITBOXARMS,
	CFG_HITBOXLEGS,
	CFG_HITBOXFEET,
	CFG_HITBOXMAX
};

enum EConfigSafepointConditions {
	SPC_STANDING = 0,
	SPC_MOVING,
	SPC_INAIR,
	SPC_CROUCHING,
	SPC_MAX
};


enum EConfigAutostopConditions {
	ASC_EARLY = 0,
	ASC_BETWEENSHOTS,
	ASC_INAIR,
	ASC_SLOWMOTION,
	ASC_MAX
};

#define CONFIGVARBOOL inline TConfigVarBool

namespace TVariables {

	inline int iMenuKey = VK_INSERT;
	inline bool bMenuOpen = false;
	inline bool bMenuInitialized = false;

	Vector4D ColorToVector(float* flColor, bool bDormant = false,float flCustomAlpha = -1.f);
	Color ConvertToColor(float* flColor);

	void UpdateKeyState(TConfigVarKeybind* Keybind);
	void UpdateKeybindStates();
	
	namespace TRagebot {
		inline bool bEnabled = false;
		inline TConfigVarMap HitboxList;
		inline TConfigVarMap MultipointList;
		inline TConfigVarMap ForceSafePointList;
		inline TConfigVarInt Headscale;
		inline TConfigVarInt Bodyscale;
		inline TConfigVarBool PreferBaim;
		inline TConfigVarInt Hitchance;
		inline TConfigVarInt MinimumDamage;
		inline TConfigVarBool AutoFire;
		inline TConfigVarBool AutoPenetration;
		inline TConfigVarBool SilentAim;
		inline TConfigVarInt Fov;

		inline TConfigVarMap Autostop;
		inline TConfigVarMap ForceSafePointConditions;
		inline TConfigVarBool AccuracyBoost;

		inline TConfigVarKeybind Doubletap;
		inline TConfigVarKeybind Hideshots;
		inline TConfigVarMap DoubletapAutostop;
	}

	namespace TVisuals {

		inline bool bEnabled = false;

		namespace TPlayer {
			inline TConfigVarBool BoundingBox;
			inline TConfigVarBool Name;
			inline TConfigVarBool HealthBar;
			inline TConfigVarBool HealthText;
			inline TConfigVarBool AmmoBar;
			inline TConfigVarBool Flags;
			inline TConfigVarBool Skeleton;
			inline TConfigVarBool WeaponText;
			inline TConfigVarBool WeaponIcon;
			inline TConfigVarBool Dormant;
			inline TConfigVarSelectable Glow;
		}

		namespace TLocalPlayer {
			inline TConfigVarBool NadePrediction;
			inline TConfigVarBool NadePredictionSecondColor;
			inline TConfigVarBool NadeWarning;
			inline TConfigVarBool NadeWarningSecondColor;
			inline TConfigVarSelectable Glow;
			inline TConfigVarKeybind ThirdPerson;
			inline TConfigVarInt ThirdPersonDistance;
			inline TConfigVarBool ThirdPersonWhileSpecating;
			inline TConfigVarInt ViewFov;
			inline TConfigVarInt ViewModelFov;
			inline TConfigVarInt AspectRatio = {false,0};

		}

		namespace TOthers {
			inline TConfigVarMap Removals;
			inline TConfigVarBool ServerImpacts;
			inline TConfigVarBool ClientImpacts;
			inline TConfigVarBool Nightmode;
			inline TConfigVarBool Propscol;
			inline TConfigVarBool Fullbright;
			inline TConfigVarBool BulletTracers;
			inline TConfigVarBool HitMarker;
			inline TConfigVarBool DamageMarker;
			inline TConfigVarBool HitSound;
			inline TConfigVarBool NadeInfo;
			inline TConfigVarBool NadeInfoRadius;
			inline TConfigVarBool BombInfo;
			inline TConfigVarBool BarColor;
			inline TConfigVarBool DroppedWeaponText;
			inline TConfigVarBool DroppedWeaponIcon;
			inline TConfigVarBool DroppedWeaponAmmo;

		}
	}

	namespace TModels {

		namespace TGeneral {
			inline bool bEnabled = false;
		}

		namespace TLocal {

			inline TConfigVarInt Model;
			inline TConfigVarBool Overlay;

			inline TConfigVarInt FakeModel;
			inline TConfigVarBool FakeOverlay;

			inline TConfigVarInt FakelagModel;
			inline TConfigVarBool FakelagFakeOverlay;

			inline TConfigVarInt Weapon;
			inline TConfigVarBool WeaponOverlay;

			inline TConfigVarInt Arms;
			inline TConfigVarBool ArmsOverlay;

			inline TConfigVarInt Attachment;
			inline TConfigVarBool AttachmentOverlay;
		}

		namespace TEnemy {
			inline TConfigVarInt ModelVisible;
			inline TConfigVarInt ModelHidden;
			inline TConfigVarBool ModelOverlayVisible;
			inline TConfigVarBool ModelOverlayHidden;

			inline TConfigVarInt BacktrackVisible;
			inline TConfigVarInt BacktrackHidden;
			inline TConfigVarBool BacktrackOverlayVisible;
			inline TConfigVarBool BacktrackOverlayHidden;
		}

		namespace TTeam {
			inline TConfigVarInt ModelVisible;
			inline TConfigVarInt ModelHidden;
			inline TConfigVarBool ModelOverlayVisible;
			inline TConfigVarBool ModelOverlayHidden;
		}


	}

	namespace TAntiAim {

		namespace TFakeAngles {
			inline bool bEnabled = false;
			inline int iPitchMode = 0;
			inline int iYawMode = 0;
			inline int iYawValue = 0;
			inline int iYawJitterMode = 0;
			inline int iYawJitterValue = 0;
			inline TConfigVarKeybind Fake;
			inline int iFakeMode = 0;
			inline int iFakeValue = 0;
			inline int iRollValue = 0;
		}
		
		namespace TFakelag {
			inline bool bFakelagEnabled = false;
			inline int iFakelagVariance = 0;
			inline int iFakelagValue = 0;
		}

		namespace TOthers {
			inline TConfigVarKeybind Slowmotion;
			inline TConfigVarKeybind Fakeduck;
			inline int iSlowmotionSpeed = 0;
		}

	}

	namespace TMisc {
		inline TConfigVarBool MenuColor;
		inline TConfigVarBool ElementsColor;
		inline TConfigVarBool UnlockInventory;
		inline TConfigVarBool ServerPureBypass;
		inline TConfigVarBool PreserveDeathNotice;
		inline TConfigVarBool AutoAcceptMatchMaking;
		inline TConfigVarBool BunnyHop;
		inline TConfigVarBool ShotLogger;
		inline TConfigVarBool BuyLogger;
		inline TConfigVarBool ItemPurchaseLog;
		inline TConfigVarBool QuickStop;
		inline TConfigVarBool AutomaticKnife;
		inline TConfigVarBool AutomaticZeus;
		inline TConfigVarBool InfiniteDuck;
		inline TConfigVarInt AutoStrafer;
		inline TConfigVarInt AutomaticPeek;
		inline TConfigVarBool AntiUntrusted;

		inline TConfigVarBool BuyBot;
		inline TConfigVarInt PrimaryWeapon;
		inline TConfigVarInt SecondaryWeapon;
		inline TConfigVarInt ArmorType;
		inline TConfigVarMap MapGrenades;
		inline TConfigVarBool Defuser;
		inline TConfigVarBool Taser;

	}

	namespace TSkins {
		inline TConfigVarInt KnifeModel;
		inline TConfigVarInt KnifeSkin;
		inline TConfigVarFloat GloveWear;
		inline TConfigVarInt GloveModel;
		inline TConfigVarInt GloveSkinBlood;
		inline TConfigVarInt GloveSkinSport;
		inline TConfigVarInt GloveSkinDriver;
		inline TConfigVarInt GloveSkinHandWrap;
		inline TConfigVarInt GloveSkinMotor;
		inline TConfigVarInt GloveSkinSpecialist;
		inline TConfigVarInt GloveSkinHydra;
		inline TConfigVarInt GloveSkinFang;
		inline TConfigVarInt ModelChanger;

	}
}

template <typename T>
struct ConfigItem_t {
	T* pPointer;
	std::string szCategory;
	std::string szName;
	T pDefaultValue;
};

namespace TConfigManager {
	namespace TVariableStack {
		inline std::vector<ConfigItem_t<bool>> vecBooleans;
		inline std::vector<ConfigItem_t<int>> vecIntegers;
		inline std::vector<ConfigItem_t<float>> vecFloats;
		inline std::string szCurrentSection;
		void SetSection(std::string szSection);
		void PushVar(bool* pointer);
		void PushVar(int* pointer);
		void PushVar(float* pointer);
		void PushMap(std::unordered_map<int, bool>& pBuffer, int iSize);
	}
	namespace TVarUtil {
		void PushConfigVar(TConfigVarFloat* Var);
		void PushConfigVar(TConfigVarBool* Var);
		void PushConfigVar(TConfigVarKeybind* Var);
		void PushConfigVar(TConfigVarInt* Var);
		void PushConfigVar(TConfigVarMap* Var,int SizeOfMap);
		void PushConfigVar(TConfigVarSelectable* Var);
	}

	void Initialize();
	void ReadConfig(std::string path);
	void SaveConfig(std::string path);
	void ResetConfig();
}