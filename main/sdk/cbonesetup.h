
class CIKContext;
struct CBoneSetup
{
    CStudioHdr* m_pStudioHdr;
    int m_boneMask;
    float* m_flPoseParameter;
    void* m_pPoseDebugger;

    void InitPose(Vector3D pos[], Quaternion q[], CStudioHdr* hdr);
    void AccumulatePose(Vector3D pos[], Quaternion q[], int sequence, float cycle, float flWeight, float flTime, CIKContext* pIKContext);
    void CalcAutoplaySequences(Vector3D pos[], Quaternion q[], float flRealTime, CIKContext* pIKContext);
    void CalcBoneAdj(Vector3D pos[], Quaternion q[], float* controllers, int boneMask);
};