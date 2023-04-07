#pragma once


class IVRenderView {
private:
	virtual void pad0();
	virtual void pad1();
	virtual void pad2();
	virtual void pad3();

public:
	virtual void SetBlend(float blend) = 0;
	virtual float GetBlend(void) = 0;
	virtual void ModulateColor(float const* blend) = 0;
	virtual void GetColorModulation(float* blend) = 0;
};

class IVModelRender
{
private:
	template< typename t >
	t CallVirtual(void* name, int offset)
	{
		int* table = *(int**)name;
		int address = table[offset];
		return (t)(address);
	}

public:
	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType overrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		return TUtil::CallVFunc<void>(this, 1, pMaterial, overrideType, nOverrides);
	}

	bool IsForcedMaterialOverride()
	{
		return TUtil::CallVFunc<bool>(this, 2);
	}

	bool DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pBoneToWorld)
	{
		using Fn = bool(__thiscall*)(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
		return CallVirtual<Fn>(this, 21)(this, ctx, state, info, pBoneToWorld);
	}
};
