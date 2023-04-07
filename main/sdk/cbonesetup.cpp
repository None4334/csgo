#include "../../includes.h"
#include "cbonesetup.h"

void CBoneSetup::InitPose(Vector3D pos[], Quaternion q[], CStudioHdr* hdr)
{
    static auto init_pose = TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 10 53 8B D9 89 55 F8 56 57 89 5D F4 8B 0B 89 4D F0"));

    __asm
    {
        mov eax, this
        mov esi, q
        mov edx, pos
        push dword ptr[hdr + 4]
        mov ecx, [eax]
        push esi
        call init_pose
        add esp, 8
    }
}

void CBoneSetup::AccumulatePose(Vector3D pos[], Quaternion q[], int sequence, float cycle, float flWeight, float flTime, CIKContext* pIKContext)
{
    using AccumulatePoseFn = void(__thiscall*)(CBoneSetup*, Vector3D* a2, Quaternion* a3, int a4, float a5, float a6, float a7, CIKContext* a8);
    static auto accumulate_pose = (AccumulatePoseFn)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? A1"));

    return accumulate_pose(this, pos, q, sequence, cycle, flWeight, flTime, pIKContext);
}

void CBoneSetup::CalcAutoplaySequences(Vector3D pos[], Quaternion q[], float flRealTime, CIKContext* pIKContext)
{
    static auto calc_autoplay_sequences = TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 10 53 56 57 8B 7D 10 8B D9 F3 0F 11 5D ??"));

    __asm
    {
        movss   xmm3, flRealTime
        mov eax, pIKContext
        mov ecx, this
        push eax
        push q
        push pos
        call calc_autoplay_sequences
    }
}

void CBoneSetup::CalcBoneAdj(Vector3D pos[], Quaternion q[], float* controllers, int boneMask)
{
    static auto calc_bone_adj = TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 81 EC ?? ?? ?? ?? 8B C1 89 54 24 04 89 44 24 2C 56 57 8B 00"));

    __asm
    {
        mov     eax, controllers
        mov     ecx, this
        mov     edx, pos; a2
        push    dword ptr[ecx + 4]; a5
        mov     ecx, [ecx]; a1
        push    eax; a4
        push    q; a3
        call    calc_bone_adj
        add     esp, 0xC
    }
}