
class CConVar;

struct TIntConsoleVar {

	TIntConsoleVar() {
		pBase = nullptr;
		iBackup = 0;
	}

	TIntConsoleVar(CConVar* Base) {
		pBase = Base;
		if (pBase)
			iBackup = pBase->GetInt();
		
	}

	void Restore() {
		if (pBase)
			pBase->SetValue(iBackup);
	}

	CConVar* pBase = nullptr;
	int iBackup = 0;
};

struct TFloatConsoleVar {

	TFloatConsoleVar() {
		pBase = nullptr;
		flBackup = 0;
	}

	TFloatConsoleVar(CConVar* Base) {
		pBase = Base;
		if (pBase)
			flBackup = pBase->GetFloat();

	}

	void Restore() {
		if (pBase)
			pBase->SetValue(flBackup);
	}

	CConVar* pBase = nullptr;
	float flBackup = 0;
};



namespace TConvarManager {


	inline TIntConsoleVar varInterp;
	inline TIntConsoleVar varInterpRatio;
	inline TIntConsoleVar varClientMinInterpRatio;
	inline TIntConsoleVar varClientMaxInterpRatio;
	inline TIntConsoleVar varUpdateRate;
	inline TIntConsoleVar varMinUpdateRate;
	inline TIntConsoleVar varMaxUpdateRate;

	inline TIntConsoleVar varWeaponNoSpread;
	inline TIntConsoleVar varDamageScaleCtHead;
	inline TIntConsoleVar varDamageScaleTHead;
	inline TIntConsoleVar varDamageScaleCTBody;
	inline TIntConsoleVar varDamageScaleTBody;

	inline TIntConsoleVar varDamageReductionBullets;
	inline TIntConsoleVar varPredDoResetLatch;
	inline TIntConsoleVar varServerCheats;
	inline TIntConsoleVar varNetShowFragments;
	inline TIntConsoleVar varLagCompensation;
	inline TIntConsoleVar varCompMinSpec;
	inline TFloatConsoleVar varInterpolation;
	
	inline TFloatConsoleVar varViewModelX;
	inline TFloatConsoleVar varViewModelY;
	inline TFloatConsoleVar varViewModelZ;
	
	inline TFloatConsoleVar varMolotovMaxDetonationSlope;

	inline TIntConsoleVar varCsmShadows;
	inline TIntConsoleVar varFootContactShadows;
	inline TIntConsoleVar varCsmStaticPropShadows;
	inline TIntConsoleVar varCsmWorldShadows;
	inline TIntConsoleVar varCsmViewmodelShadows;
	inline TIntConsoleVar varCsmSpriteShadows;
	inline TIntConsoleVar varCsmRopeShadows;
	inline TIntConsoleVar varShadows;
	inline TFloatConsoleVar varZoomSensitivityRatioMouse;

	inline TIntConsoleVar varDrawSpecificStaticProp;

	inline TIntConsoleVar var3Dsky;
	inline TIntConsoleVar varFogOverride;
	inline TIntConsoleVar varFogStart;
	inline TIntConsoleVar varFogEnd;
	inline TFloatConsoleVar varFogMaxDensity;
	inline TIntConsoleVar varFogColor;

	inline TIntConsoleVar varTeammatesAreEnemies;

	inline TFloatConsoleVar varForwardSpeed;
	inline TFloatConsoleVar varSideSpeed;
	inline TFloatConsoleVar varUpSpeed;

	inline TFloatConsoleVar varGravity;

	float GetLerp();
	inline float flLerp = 0.0f;
	void Restore();
	void Initialize();
}
