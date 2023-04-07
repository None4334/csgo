#include "../../includes.h"
#include "ikcontext.h"

void* CIKContext::operator new(size_t size)
{
    CIKContext* ptr = (CIKContext*)new BYTE[size];
    Construct(ptr);

    return ptr;
}

void CIKContext::operator delete(void* ptr)
{
    delete ptr;
}

void CIKContext::Destruct(CIKContext* ik) {
    using IKDestructor = void(__thiscall*)(CIKContext*);
    static auto ik_dector = (IKDestructor)TUtil::FindPattern(CLIENT_STR, TSTRING("56 8B F1 57 8D 8E ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 8E ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 BE ?? ?? ?? ?? ??"));
    ik_dector(ik);
}

void CIKContext::Construct(CIKContext* ik)
{
    using ConstructFn = CIKContext * (__fastcall*)(CIKContext*);
    static ConstructFn oConstruct = reinterpret_cast<ConstructFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("53 8B D9 F6 C3 03 74 0B FF 15 ?? ?? ?? ?? 84 C0 74 01 CC C7 83 ?? ?? ?? ?? ?? ?? ?? ?? 8B CB")));
    oConstruct(ik);
}

void CIKContext::Init(const CStudioHdr* hdr, const Vector3D& local_angles, const Vector3D& local_origin, float current_time, int frame_count, int bone_mask)
{
    using InitFn = void(__thiscall*)(CIKContext*, const CStudioHdr*, const Vector3D&, const Vector3D&, float, int, int);
    static InitFn oInit = reinterpret_cast<InitFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D")));
    oInit(this, hdr, local_angles, local_origin, current_time, frame_count, bone_mask);
}

void CIKContext::UpdateTargets(Vector3D* pos, Quaternion* q, matrix3x4_t* bone_cache, void* computed)
{
    using UpdateTargetsFn = void(__thiscall*)(CIKContext*, Vector3D*, Quaternion*, matrix3x4_t*, void*);
    static UpdateTargetsFn oUpdateTargets = reinterpret_cast<UpdateTargetsFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F0 81 ? ? ? ? ? 33 D2 89")));
    oUpdateTargets(this, pos, q, bone_cache, computed);
}


void CIKContext::SolveDependencies(Vector3D* pos, Quaternion* q, matrix3x4_t* bone_cache, void* computed)
{
    using SolveDependenciesFn = void(__thiscall*)(CIKContext*, Vector3D*, Quaternion*, matrix3x4_t*, void*);
    static SolveDependenciesFn oSolveDependencies = reinterpret_cast<SolveDependenciesFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F0 81 ? ? ? ? ? 8B ? ? ? ? ? 56 57 89")));
    oSolveDependencies(this, pos, q, bone_cache, computed);
}

struct CIKTarget {
    int m_iFrameCount;

private:
    char pad_00004[0x51];
};

void CIKContext::ClearTargets()
{
    int m_iTargetCount = *(int*)((uintptr_t)this + 0xFF0);
    auto m_pIkTarget = (CIKTarget*)((uintptr_t)this + 0xD0);
    for (int i = 0; i < m_iTargetCount; i++) {
        m_pIkTarget->m_iFrameCount = -9999;
        m_pIkTarget++;
    }
}

void CIKContext::AddDependencies(mstudioseqdesc_t& seqdesc, int iSequence, float flCycle, const float poseParameters[], float flWeight)
{
    using AddDependenciesFn = void(__thiscall*)(CIKContext*, mstudioseqdesc_t&, int, float, const float[], float);
    static AddDependenciesFn oAddDependencies = reinterpret_cast<AddDependenciesFn>(TUtil::FindPattern(SERVER_STR, TSTRING("55 8B EC 81 EC ?? ?? ?? ?? 53 56 57 8B F9 0F 28 CB F3 0F 11 4D ??")));
    oAddDependencies(this, seqdesc, iSequence, flCycle, poseParameters, flWeight);
}

void CIKContext::CopyTo(CIKContext* other, const unsigned short* iRemapping)
{
    using CopyToFn = void(__thiscall*)(CIKContext*, CIKContext*, const unsigned short*);
    static CopyToFn oCopyTo = reinterpret_cast<CopyToFn>(TUtil::FindPattern(SERVER_STR, TSTRING("55 8B EC 83 EC 24 8B 45 08 57 8B F9 89 7D F4 85 C0")));
    oCopyTo(this, other, iRemapping);
}
