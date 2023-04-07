#include "../../includes.h"
#include "../sdk.h"
#include "bonesetuprebuilt.h"
#include "cbonesetup.h"
#include "ikcontext.h"

uintptr_t& GetBoneMerge(CBaseEntity* player)
{
    static auto bone_merge = TUtil::FindPattern(TSTRING("client.dll"), TSTRING("89 86 ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 75 08"));
    return *(uintptr_t*)((uintptr_t)player + *(uintptr_t*)(bone_merge + 0x2));
}


void GetSkeleton(CBaseAnimating* pBaseAnimating, CStudioHdr* StudioHdr, Vector3D* pos, Quaternion* q, int bone_mask, CIKContext* ik,float* flWorldPoses,float* flPoses)
{
    CBaseEntity* pBasePlayer = (CBaseEntity*)((uintptr_t)pBaseAnimating - 0x4);

    alignas(16) char buffer[32];
    alignas(16) auto boneSetup = (CBoneSetup*)&buffer;

    *boneSetup = CBoneSetup(StudioHdr, bone_mask, pBasePlayer->GetPoseParameter().data());

    boneSetup->InitPose(pos, q, StudioHdr);
    boneSetup->AccumulatePose(pos, q, pBasePlayer->GetSequence(), pBasePlayer->GetCycle(), 1.f, pBasePlayer->GetSimulationTime(), ik);

    constexpr int MAX_LAYER_COUNT = 15;
    int layer[MAX_LAYER_COUNT]{};
    for (int i = 0; i < pBasePlayer->GetAnimationOverlays().Count(); i++) {
        layer[i] = MAX_LAYER_COUNT;
    }

    for (int i = 0; i < pBasePlayer->GetAnimationOverlays().Count(); i++) {
        auto pLayer = &pBasePlayer->GetAnimationOverlays()[i];
        if ((pLayer->flWeight > 0.f) && pLayer->nSequence != -1 && pLayer->iOrder >= 0 && pLayer->iOrder < pBasePlayer->GetAnimationOverlays().Count()) {
            layer[pLayer->iOrder] = i;
        }
    }

    CBaseCombatWeapon* weapon = nullptr;
    CBaseEntity* weapon_world_model = nullptr;
    bool do_weapon_setup = false;

    weapon = TBaseEntity::GetWeapon(pBasePlayer);
     if (weapon) {
            weapon_world_model = (CBaseEntity*)TInterfaces::ClientEntityList->GetClientEntityFromHandle(weapon->GetWorldModelHandle());
            if (weapon_world_model && weapon_world_model->GetModelPtr()/* && weapon_world_model->HoldsPlayerAnimations()*/) {

               if (*weapon_world_model->GetBoneMerge())
                    do_weapon_setup = true;
         }
     }


  
    if (do_weapon_setup) {

        CStudioHdr* weapon_studio_hdr = weapon_world_model->GetModelPtr();

        (*weapon_world_model->GetBoneMerge())->MergeMatchingPoseParams(flWorldPoses,flPoses);

        char tmp_buffer[4208];
        auto weaponIK = (CIKContext*)tmp_buffer;
        weaponIK->Construct(weaponIK);
        weaponIK->Init(weapon_studio_hdr, pBasePlayer->GetAbsAngles(), pBasePlayer->GetAbsOrigin(), pBasePlayer->GetSimulationTime(), 0, BONE_USED_BY_BONE_MERGE);

        CBoneSetup weaponSetup(weapon_studio_hdr, BONE_USED_BY_BONE_MERGE, weapon_world_model->GetPoseParameter().data());
        Vector3D weaponPos[256];
        Quaternion weaponQ[256];

        weaponSetup.InitPose(weaponPos, weaponQ, weapon_studio_hdr);

        for (int i = 0; i < pBasePlayer->GetAnimationOverlays().Count(); i++) {
            CAnimationLayer* pLayer = &pBasePlayer->GetAnimationOverlays()[i];
            
            if (pLayer->nSequence <= 1 || pLayer->flWeight <= 0.f)
                continue;

            pBasePlayer->UpdateDispatchLayer(pLayer, weapon_studio_hdr, pLayer->nSequence);

          //  if (pLayer->nDispatchedDst <= 0 || pLayer->nDispatchedDst >= StudioHdr->GetNumSeq()) 
                boneSetup->AccumulatePose(pos, q, pLayer->nSequence, pLayer->flCycle, pLayer->flWeight, pBasePlayer->GetSimulationTime(), ik);
          //  else 
           // {
                /*
                (*weapon_world_model->GetBoneMerge())->CopyFromFollow(pos, q, BONE_USED_BY_BONE_MERGE, weaponPos, weaponQ);

                using oIKAddDependencies = void(__thiscall*)(CIKContext*, float, int, int, float, float);
                static auto add_dependencies = TUtil::FindPattern(TSTRING("client.dll"), TSTRING("55 8B EC 81 EC BC 00 00 00 53 56 57"));

                if (ik)
                    (oIKAddDependencies(add_dependencies) (ik, *(float*)((uintptr_t)pBasePlayer + 0xA14), pLayer->nSequence, pLayer->flCycle, boneSetup->m_flPoseParameter[2], pLayer->flWeight));

                weaponSetup.AccumulatePose(weaponPos, weaponQ, pLayer->nDispatchedDst, pLayer->flCycle, pLayer->flWeight, pBasePlayer->GetSimulationTime(), weaponIK);

                (*weapon_world_model->GetBoneMerge())->CopyToFollow(weaponPos, weaponQ, BONE_USED_BY_BONE_MERGE, pos, q);
                */
         //   }
            /*
            if (pLayer->nDispatchedDst <= 0 || pLayer->nDispatchedDst >= StudioHdr->GetNumSeq()) {
                boneSetup.AccumulatePose(pos, q, pLayer->nSequence, pLayer->flCycle, pLayer->flWeight, pBasePlayer->GetSimulationTime(), ik);
            }
            else {
               
                (*weapon_world_model->GetBoneMerge())->CopyFromFollow(pos, q, BONE_USED_BY_BONE_MERGE, weaponPos, weaponQ);

                mstudioseqdesc_t& seqdesc = StudioHdr->GetSeqdesc(pLayer->nSequence);
               
                ik->AddDependencies(seqdesc, pLayer->nSequence, pLayer->flCycle, pBasePlayer->GetPoseParameter().data(), pLayer->flWeight);

                weaponSetup.AccumulatePose(weaponPos, weaponQ, pLayer->nDispatchedDst, pLayer->flCycle, pLayer->flWeight, pBasePlayer->GetSimulationTime(), weaponIK);

                (*weapon_world_model->GetBoneMerge())->CopyToFollow(weaponPos, weaponQ, BONE_USED_BY_BONE_MERGE, pos, q);

               // weaponIK->CopyTo(ik, (unsigned short*)((uintptr_t)(*weapon_world_model->GetBoneMerge()) + 0x28));
            }
            */
        }

   
     //   weaponIK->Destruct(weaponIK);
     //   delete weaponIK;
         
    }

   else {
        for (int i = 0; i < pBasePlayer->GetAnimationOverlays().Count(); i++) {
            if (layer[i] >= 0 && layer[i] < pBasePlayer->GetAnimationOverlays().Count()) {
                CAnimationLayer pLayer = pBasePlayer->GetAnimationOverlays()[layer[i]];
                boneSetup->AccumulatePose(pos, q, pLayer.nSequence, pLayer.flCycle, pLayer.flWeight, pBasePlayer->GetSimulationTime(), ik);
            }
        }     
    }
  
   


   // static CIKContext* auto_ik = new CIKContext;
   // RtlSecureZeroMemory(auto_ik,sizeof CIKContext);
   // auto_ik->Construct(auto_ik);
    //CIKContext* auto_ik = new CIKContext;

    char tmp_buffer[4208];
    auto auto_ik = (CIKContext*)tmp_buffer;
    auto_ik->Construct(auto_ik);
    auto_ik->Init(StudioHdr, pBasePlayer->GetAbsAngles(), pBasePlayer->GetAbsOrigin(), pBasePlayer->GetSimulationTime(), 0, bone_mask);
    boneSetup->CalcAutoplaySequences(pos, q, pBasePlayer->GetSimulationTime(), auto_ik);

  //  auto_ik->Destruct(auto_ik);
   // delete auto_ik;
  
    if (StudioHdr->pStudioHdr->nBoneControllers > 0) {
       boneSetup->CalcBoneAdj(pos, q, pBasePlayer->GetEncodedController().data(), boneSetup->m_boneMask);
    }
    
   
}

void ConcatTransforms(const matrix3x4_t& m0, const matrix3x4_t& m1, matrix3x4_t& out)
{
    for (int i = 0; i < 3; i++) {
        out[i][3] = m1[0][3] * m0[i][0] + m1[1][3] * m0[i][1] + m1[2][3] * m0[i][2] + m0[i][3]; 

        for (int j = 0; j < 3; j++)
        {
            out[i][j] = m0[i][0] * m1[0][j] + m0[i][1] * m1[1][j] + m0[i][2] * m1[2][j];
        }
    }
}
void BuildMatrices(CBaseAnimating* pBaseAnimating, CStudioHdr* studio_hdr, Vector3D* pos, Quaternion* q, matrix3x4_t* bone_to_world, int bone_mask)
{
    CBaseEntity* pBasePlayer = (CBaseEntity*)((uintptr_t)pBaseAnimating - 0x4);

    int i = 0, j = 0;
    int chain[MAXSTUDIOBONES] = {};
    int chain_length = studio_hdr->pStudioHdr->nBones;

    for (i = 0; i < studio_hdr->pStudioHdr->nBones; i++) {
        chain[chain_length - i - 1] = i;
    }

    matrix3x4_t bone_matrix = {};
    matrix3x4_t rotation_matrix = {};

    rotation_matrix.AngleMatrix(pBasePlayer->GetAbsAngles(), pBasePlayer->GetAbsOrigin());

    for (j = chain_length - 1; j >= 0; j--) {
        i = chain[j];

        auto bone = studio_hdr->pStudioHdr->GetBone(i);
        if (bone->iFlags & bone_mask) {
            bone_matrix.QuaternionMatrix(q[i], pos[i]);

            int bone_parent = bone->iParent;
            if (bone_parent == -1) {
                ConcatTransforms(rotation_matrix, bone_matrix, bone_to_world[i]);
            }
            else {
                ConcatTransforms(bone_to_world[bone_parent], bone_matrix, bone_to_world[i]);
            }
        }
    }
}



void TBoneSetup::SetupBones(CBaseAnimating* pBaseAnimating, matrix3x4_t* bone_to_world, int max_bones, int bone_mask)
{
    CBaseEntity* pBasePlayer = (CBaseEntity*)((uintptr_t)pBaseAnimating - 0x4);

    
    TInterfaces::MDLCache->BeginLock();
    {
        auto StudioHdr = pBasePlayer->GetModelPtr();
        
        if (StudioHdr) {

            alignas(16) Vector3D pos[MAXSTUDIOBONES];
            alignas(16) Quaternion q[MAXSTUDIOBONES];

            float flPoseParameters[24];
            float flWorldPoses[24];

            RtlCopyMemory(flPoseParameters,pBasePlayer->GetPoseParameter().data(), 24 * sizeof(float));
            RtlCopyMemory(flWorldPoses, pBasePlayer->GetPoseParameter().data(), 24 * sizeof(float));

            uint8_t bone_computed[32]{};
            RtlSecureZeroMemory(&bone_computed, sizeof bone_computed);

           // static CIKContext* ik = new CIKContext;
           // RtlSecureZeroMemory(ik, sizeof CIKContext);
           // ik->Construct(ik);

            CIKContext* ik = new CIKContext;
            auto BackupContext = *pBasePlayer->GetIKContext();
          
            *pBasePlayer->GetIKContext() = ik;

            ik->Init(StudioHdr, pBasePlayer->GetAbsAngles(), pBasePlayer->GetAbsOrigin(), pBasePlayer->GetSimulationTime(), TInterfaces::Globals->iTickCount, bone_mask);
            GetSkeleton(pBaseAnimating, StudioHdr, pos, q, bone_mask, ik, flWorldPoses, flPoseParameters);

            ik->UpdateTargets(pos, q, bone_to_world, &bone_computed);
            pBasePlayer->CalculateIKLocks(pBasePlayer->GetSimulationTime());
            ik->SolveDependencies(pos, q, bone_to_world, &bone_computed);
           
            BuildMatrices(pBaseAnimating, StudioHdr, pos, q, pBasePlayer->GetCachedBoneData().Base(), bone_mask);
           
            *pBasePlayer->GetIKContext() = BackupContext;
           


            delete ik;
        }

        if (bone_to_world && max_bones >= pBasePlayer->GetCachedBoneData().Count()) {
            memcpy(bone_to_world, pBasePlayer->GetCachedBoneData().Base(), sizeof(matrix3x4_t) * pBasePlayer->GetCachedBoneData().Count());
        }
    }
    TInterfaces::MDLCache->EndLock();

}