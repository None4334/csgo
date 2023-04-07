



#define M_METRE2INCH( x )	( ( x ) / 0.0254f )
#define M_INCH2METRE( x )	( ( x ) * 0.0254f )
#define M_METRE2FOOT( x )	( ( x ) * 3.28f )
#define M_FOOT2METRE( x )	( ( x ) / 3.28f )
#define M_RAD2DEG( x )		DirectX::XMConvertToDegrees( x )
#define M_DEG2RAD( x )		DirectX::XMConvertToRadians( x )

constexpr double M_PI = 3.14159265358979323846;
constexpr float M_RADPI = 57.295779513082f;
constexpr float M_PI_F = static_cast<float>(M_PI);

namespace TMath {
	constexpr static float PI = std::numbers::pi_v<float>;
	constexpr static float PI_2 = std::numbers::pi_v<float> *2.0f;

	template <class T>
	inline T Lerp(float flPercent, T const& A, T const& B)
	{
		return A + (B - A) * flPercent;
	}
	void AngleMatrix(const Vector3D& angView, const Vector3D& vecOrigin, matrix3x4_t& matOutput);
	float GetFov(const Vector3D& vecViewAngle, const Vector3D& vecAimAngle);
	Vector3D CalculateAngle(const Vector3D& src, const Vector3D& dst);
	Vector3D GetClosestPosition(Vector3D* Data,size_t Size,Vector3D vecPosition);
	Vector3D VectorTransform(const Vector3D& vecTransform, const matrix3x4_t& matrix);
	void VectorTransform(const Vector3D& in1, const matrix3x4_t& in2, Vector3D& out);
	void AngleVectors(const Vector3D& angView, Vector3D* pForward, Vector3D* pRight = nullptr, Vector3D* pUp = nullptr);
	void VectorAngles(const Vector3D& vecForward, Vector3D& angView);
	void VectorAngles(const Vector3D& forward, Vector3D& up, Vector3D& angles);
	void RotateMovement(CUserCmd* cmd, float yaw, Vector3D vecEngineAngles);
	float NormalizeYaw(float flYaw);
	Vector3D Interpolate(const Vector3D flCurrent, const Vector3D flTarget, const int iProgress, const int iMaximum);
	float Interpolate(const float flCurrent, const float flTarget, const int iProgress, const int iMaximum);
	float RemapValClamped(float val, float A, float B, float C, float D);
	float DistanceToRay(const Vector3D& pos, const Vector3D& rayStart, const Vector3D& rayEnd, float* along = nullptr, Vector3D* pointOnRay = nullptr);
	float SegmentToSegment(const Vector3D s1, const Vector3D s2, const Vector3D k1, const Vector3D k2);

}