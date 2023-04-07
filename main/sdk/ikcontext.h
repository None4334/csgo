

class CIKContext
{
    uint8_t uPad[0x1070];

public:
    void* operator new(size_t size);
    void  operator delete(void* ptr);

    static void Construct(CIKContext* ik);
    static void Destruct(CIKContext* ik);
    void Init(const CStudioHdr* hdr, const Vector3D& local_angles, const Vector3D& local_origin, float current_time, int frame_count, int bone_mask);
    void UpdateTargets(Vector3D* pos, Quaternion* q, matrix3x4_t* bone_cache, void* computed);
    void SolveDependencies(Vector3D* pos, Quaternion* q, matrix3x4_t* bone_cache, void* computed);
    void ClearTargets();
    void AddDependencies(mstudioseqdesc_t& seqdesc, int iSequence, float flCycle, const float poseParameters[], float flWeight);
    void CopyTo(CIKContext* other, const unsigned short* iRemapping);
};