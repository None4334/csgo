
class CCSGOPlayerAnimState;
namespace TLocalAnimation {

	inline matrix3x4_t matModel[MAXSTUDIOBONES];
	inline matrix3x4_t matDesync[MAXSTUDIOBONES];
	inline matrix3x4_t matLag[MAXSTUDIOBONES];

	inline bool bInAnimUpdate = false;

	inline float flSpawnTime = 0.0f;
	inline int iFlags[2];
	inline int iMoveType[2];
	inline Vector3D vecAnglesSent;
	inline Vector3D vecAngles;
	inline bool bForceSentAngles = false;
	void OnCommandEnd(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool bSendPacket);
	void UpdateAnimationOverlay(CBaseEntity* pLocalPlayer, int nType);

	inline std::array < CAnimationLayer, ANIMATION_LAYER_COUNT > arrAnimationLayers;
	inline std::array < CAnimationLayer, ANIMATION_LAYER_COUNT > arrFakeAnimationLayers;
	inline std::array < CAnimationLayer, ANIMATION_LAYER_COUNT > arrOldAnimationLayers;

	inline std::array < float, 24 > arrOldPoseParameters;
	inline std::array < float, 24 > arrPoseParameters;
	inline std::array < float, 24 > arrFakePoseParameters;

	inline Vector3D arrBoneOrigins[MAXSTUDIOBONES];
	inline Vector3D arrFakeBoneOrigins[MAXSTUDIOBONES];

	std::array<CAnimationLayer, ANIMATION_LAYER_COUNT > GetAnimationLayers(CBaseEntity* pLocalPlayer);
	std::array<CAnimationLayer, ANIMATION_LAYER_COUNT > GetFakeAnimationLayers(CBaseEntity* pLocalPlayer);

}
