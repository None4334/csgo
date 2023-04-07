
class CBaseAnimating;
class CCSGOPlayerAnimState;
namespace THooks {

	void* __fastcall hkAllocKeyValuesMemory(IKeyValuesSystem* thisptr, int edx, int iSize);
	inline TDetourHook dtrAllocKeyValuesMemory;

	void __fastcall hkCreateMoveProxy(IBaseClientDll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive);
	inline TDetourHook dtrCreateMoveProxy;

	void __fastcall hkRunCommand(IPrediction* thisptr, int edx, CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* pMoveHelper);
	inline TDetourHook dtrRunCommand;

	void __fastcall hkFrameStageNotify(IBaseClientDll* thisptr, int edx, EClientFrameStage curStage);
	inline TDetourHook dtrFrameStageNotify;

	long D3DAPI hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	inline TDetourHook dtrReset;

	long D3DAPI hkEndScene(IDirect3DDevice9* pDevice);
	inline TDetourHook dtrEndScene;

	void __fastcall hkPaintTraverse(ISurface* thisptr, int edx, unsigned int uPanel, bool bForceRepaint, bool bForce);
	inline TDetourHook dtrPaintTraverse;

	bool __fastcall hkSetupBones(CBaseAnimating* pAnimating, void* edx, matrix3x4_t* matBoneToWorld, int nMaxBones, int nBoneMask, float flCurrentTime);
	inline TDetourHook dtrSetupBones;

	void __fastcall hkUpdateClientSideAnimations(CBaseEntity* pPlayer, uint32_t u);
	inline TDetourHook dtrUpdateClientSideAnimations;

	void __fastcall hkClampBonesInBBox(CBaseEntity* player, void* edx, matrix3x4_t* matrix, int mask);
	inline TDetourHook dtrClampBonesInBox;

	void __fastcall hkLockCursor(ISurface* thisptr, int edx);
	inline TDetourHook dtrLockCursor;

	float __fastcall hkGetViewModelFov(IClientModeShared* thisptr, int edx);
	inline TDetourHook  dtrGetViewModelFov;

	float  __fastcall hkGetAspectRatio(IEngineClient* thisptr, void* edx, int iWidth, int iHeight);
	inline TDetourHook  dtrGetAspectRatio;

	int __fastcall hkDoPostScreenEffects(IClientModeShared* thisptr, int edx, CViewSetup* pSetup);
	inline TDetourHook  dtrDoPostScreenEffects;

	void __fastcall hkOverrideView(IClientModeShared* thisptr, int edx, CViewSetup* pSetup);
	inline TDetourHook  dtrOverrideView;

	void __fastcall hkCalcViewModelBob(CBaseEntity* pEntity, void* edx, Vector3D& vecPosition);
	inline TDetourHook  dtrCalcViewModelBob;

	void hkGetFlashDuration(const CRecvProxyData* Data, void* pStruct, float* flFlashDuration);
	inline CRecvPropHook* recGetFlashDuration;

	void __fastcall hkModifyEyePosition(CCSGOPlayerAnimState* pAnimState, void* pEdx, Vector3D& vecAngles);
	inline TDetourHook  dtrModifyEyePosition;

	int hkProcessInterpolatedList();
	inline TDetourHook  dtrInterpolatedList;

	bool __fastcall hkSvCheatsGetBool(CConVar* thisptr, int edx);
	inline TDetourHook  dtrCheatsGetBool;

	bool __fastcall hkNetShowFragmentsGetBool(CConVar* thisptr, int edx);
	inline TDetourHook  dtrShowFragments;

	bool __fastcall hkInPrediction(IPrediction* thisptr, void* edx);
	inline TDetourHook  dtrInPrediction;

	inline std::unique_ptr<VMTHook> vmtClientModeHook = nullptr;
	bool __fastcall hkShouldDrawFog(IClientModeShared* pClientMode);

	bool __fastcall hkIsConnected(IEngineClient* thisptr, int edx);
	inline TDetourHook  dtrIsConnected;

	bool __fastcall hkIsHltv(IEngineClient* thisptr, void* edx);
	inline TDetourHook  dtrIsHltv;

	void hkImpactCallback(const CEffectData& data);
	inline ClientEffectCallback oImpactCallback;

	bool __fastcall hkOverrideConfig(void* pThis, void* edx, MaterialSystemConfig_t* pConfig, bool bUpdate);
	inline TDetourHook  dtrOverrideConfig;

	void __fastcall hkDrawModel(IStudioRender* thisptr, int edx, DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector3D& vecModelOrigin, int nFlags);
	inline TDetourHook  dtrDrawModel;

	int __fastcall hkListLeavesInBox(void* thisptr, int edx, const Vector3D& vecMins, const Vector3D& vecMaxs, unsigned short* puList, int nListMax);
	inline TDetourHook  dtrListLeavesInBox;

	int __fastcall hkRetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize);
	inline TDetourHook  dtrRetrieveMessage;

	void __fastcall hkPhysicsSimulate(CBaseEntity* pBasePlayer, void* edx);
	inline TDetourHook  dtrPhysicsSimulate;

	Vector3D* __fastcall hkGetEyeAngles(CBaseEntity* pPlayer, void* edx);
	inline TDetourHook  dtrGetEyeAngles;

	void __fastcall hkBuildTransformations(CBaseEntity* pPlayer, uint32_t edx, CStudioHdr* pHdr, Vector3D* vecPosition, Quaternion* Quat, const matrix3x4_t& matTransform, int iMask, byte* byteBoneComputed);
	inline TDetourHook  dtrBuildTransformations;

	void __fastcall	hkStandardBlendingRules(CBaseEntity* pPlayer, int i, studiohdr_t* pHdr, Vector3D* vecPosition, Quaternion* Quat, float flCurrentTime, int boneMask);
	inline TDetourHook  dtrStandardBlendingRules;

	bool __fastcall hkShouldSkipAnimationFrame(void* ecx,void* edx);
	inline TDetourHook  dtrShouldSkipAnimationFrame;

	void _fastcall hkSetupAliveLoop(CCSGOPlayerAnimState* thisptr, void* edx);
	inline TDetourHook  dtrSetupAliveLoop;

	LPVOID hkGetClientModelRenderable(void* thisptr, void* edx);
	inline TDetourHook  dtrGetClientModeRenderable;

	void _fastcall hkPlayerMove(IGameMovement* thisptr, void* edx);
	inline TDetourHook  dtrPlayerMove;
	
	void __fastcall hkProcessMovement(IGameMovement* thisptr, void* edx, CBaseEntity* pPlayer, CMoveData* m_MoveData);
	inline TDetourHook dtrProcessMovement;

	bool __fastcall hkClPredDoResetLatch(CConVar* thisptr, int edx);
	inline TDetourHook  dtrPredDoResetLatch;

	bool __fastcall hkProcessTempEntities(void* thisptr, void* edx, void* pMsg);
	inline TDetourHook  dtrProcessTempEntities;

	void hkClMove(float flFrameTime, bool bIsFinalTick);
	inline TDetourHook  dtrClMove;

	void hkRecvProxyViewModel(CRecvProxyData* pData, void* pStruct, void* pOut);
	void hkSetViewModelSequence(const CRecvProxyData* pDataConst, void* pStruct, void* pOut);

	void AnimationHook();
	void AnimationUnHook();

	int __fastcall hkCanLoadThirdPartyFiles(void* ecx, void* edx);
	int __stdcall hkGetUnverifiedFileHashes(void* ecx, void* ptr, int nMaxFiles);
	inline std::unique_ptr<VMTHook> vmtFileSystem = nullptr;

	void __fastcall hkPacketStart(void* ecx, void*, int iIncoming, int iOutgoing);
	void __fastcall hkPacketEnd(void* Ecx, void* Edx);
	inline std::unique_ptr<VMTHook> pNetChannelHook = nullptr;

	inline RecvVarProxyFn fnSequenceProxyFn = nullptr;
	inline RecvVarProxyFn oRecvnModelIndex;

	bool Hook();
	bool Unhook();
}
