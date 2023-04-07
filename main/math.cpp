#include "../includes.h"
#include "math.h"


Vector3D TMath::VectorTransform(const Vector3D& vecTransform, const matrix3x4_t& matrix)
{
	return Vector3D(vecTransform.DotProduct(matrix[0]) + matrix[0][3],
		vecTransform.DotProduct(matrix[1]) + matrix[1][3],
		vecTransform.DotProduct(matrix[2]) + matrix[2][3]);
}

void TMath::VectorTransform(const Vector3D& in1, const matrix3x4_t& in2, Vector3D& out) {
	out = {
		in1.DotProduct(Vector3D(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3],
		in1.DotProduct(Vector3D(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3],
		in1.DotProduct(Vector3D(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3]
	};
}

float TMath::SegmentToSegment(const Vector3D s1, const Vector3D s2, const Vector3D k1, const Vector3D k2)
{
	static auto constexpr epsilon = 0.00000001;

	auto u = s2 - s1;
	auto v = k2 - k1;
	const auto w = s1 - k1;

	const auto a = u.DotProduct(u);
	const auto b = u.DotProduct(v);
	const auto c = v.DotProduct(v);
	const auto d = u.DotProduct(w);
	const auto e = v.DotProduct(w);
	const auto D = a * c - b * b;
	float sn, sd = D;
	float tn, td = D;

	if (D < epsilon) {
		sn = 0.0;
		sd = 1.0;
		tn = e;
		td = c;
	}
	else {
		sn = b * e - c * d;
		tn = a * e - b * d;

		if (sn < 0.0) {
			sn = 0.0;
			tn = e;
			td = c;
		}
		else if (sn > sd) {
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	if (tn < 0.0) {
		tn = 0.0;

		if (-d < 0.0)
			sn = 0.0;
		else if (-d > a)
			sn = sd;
		else {
			sn = -d;
			sd = a;
		}
	}
	else if (tn > td) {
		tn = td;

		if (-d + b < 0.0)
			sn = 0;
		else if (-d + b > a)
			sn = sd;
		else {
			sn = -d + b;
			sd = a;
		}
	}

	const float sc = abs(sn) < epsilon ? 0.0 : sn / sd;
	const float tc = abs(tn) < epsilon ? 0.0 : tn / td;

	typedef __declspec(align(16)) union
	{
		float f[4];
		__m128 v;
	} m128;

	m128 n;
	auto dp = w + u * sc - v * tc;
	n.f[0] = dp.DotProduct(dp);
	const auto calc = sqrt_ps(n.v);
	return reinterpret_cast<const m128*>(&calc)->f[0];
}
float TMath::DistanceToRay(const Vector3D& pos, const Vector3D& rayStart, const Vector3D& rayEnd, float* along , Vector3D* pointOnRay )
{
	Vector3D to = pos - rayStart;
	Vector3D dir = rayEnd - rayStart;
	float length = dir.Normalize();

	float rangeAlong = dir.DotProduct(to);
	if (along)
		*along = rangeAlong;

	float range;

	if (rangeAlong < 0.0f)
	{
		range = -(pos - rayStart).Length();

		if (pointOnRay)
			*pointOnRay = rayStart;
	}
	else if (rangeAlong > length)
	{
		range = -(pos - rayEnd).Length();

		if (pointOnRay)
			*pointOnRay = rayEnd;
	}
	else
	{
		Vector3D onRay = rayStart + dir * rangeAlong;

		range = (pos - onRay).Length();

		if (pointOnRay)
			*pointOnRay = onRay;
	}

	return range;
}


float TMath::RemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = std::clamp(cVal, 0.0f, 1.0f);

	return C + (D - C) * cVal;
}
void TMath::AngleMatrix(const Vector3D& angView, const Vector3D& vecOrigin, matrix3x4_t& matOutput) {
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, DirectX::XMConvertToRadians(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, DirectX::XMConvertToRadians(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, DirectX::XMConvertToRadians(angView.z));

	matOutput.SetForward(Vector3D(cp * cy, cp * sy, -sp));

	const float crcy = cr * cy;
	const float crsy = cr * sy;
	const float srcy = sr * cy;
	const float srsy = sr * sy;

	matOutput.SetLeft(Vector3D(sp * srcy - crsy, sp * srsy + crcy, sr * cp));
	matOutput.SetUp(Vector3D(sp * crcy + srsy, sp * crsy - srcy, cr * cp));
	matOutput.SetOrigin(vecOrigin);
}
float TMath::GetFov(const Vector3D& vecViewAngle, const Vector3D& vecAimAngle)
{
	Vector3D vecDelta = vecAimAngle - vecViewAngle;
	vecDelta.NormalizeAngles();
	return min(sqrtf(powf(vecDelta.x, 2.0f) + powf(vecDelta.y, 2.0f)), 180.0f);
}

Vector3D TMath::CalculateAngle(const Vector3D& src, const Vector3D& dst) {
	Vector3D vecAngles;

	Vector3D vecDelta = src - dst;
	float flHyp = vecDelta.Length2D();

	vecAngles.y = std::atanf(vecDelta.y / vecDelta.x) * M_RADPI;
	vecAngles.x = std::atanf(-vecDelta.z / flHyp) * -M_RADPI;
	vecAngles.z = 0.0f;

	if (vecDelta.x >= 0.0f)
		vecAngles.y += 180.0f;

	return vecAngles;
}

Vector3D TMath::GetClosestPosition(Vector3D* Data, size_t Size, Vector3D vecPosition) {
	
	if (!Data || !Size)
		return Vector3D();

	if (Size == 1)
		return Data[0];

	Vector3D vecClosest = Data[0];
	float flClosestDistance = vecPosition.DistTo(vecClosest);

	for (size_t Current = 1; Current < Size; Current += 1)
	{
		float flDistance = vecPosition.DistTo(Data[Current]);

		if (flDistance < flClosestDistance) {
			vecClosest = Data[Current];
			flClosestDistance = flDistance;
		}
	}

	return vecClosest;
}


Vector3D TMath::Interpolate(const Vector3D flCurrent, const Vector3D flTarget, const int iProgress, const int iMaximum)
{
	return flCurrent + ((flTarget - flCurrent) / iMaximum) * iProgress;
}

float TMath::Interpolate(const float flCurrent, const float flTarget, const int iProgress, const int iMaximum)
{
	return flCurrent + ((flTarget - flCurrent) / iMaximum) * iProgress;
}

float TMath::NormalizeYaw(float flYaw)
{
	while (flYaw < -180.0f)
		flYaw += 360.0f;

	while (flYaw > 180.0f)
		flYaw -= 360.0f;

	return flYaw;
}

void TMath::RotateMovement(CUserCmd* cmd, float yaw, Vector3D vecEngineAngles)
{
	float flRotation = M_DEG2RAD(vecEngineAngles.y - yaw);

	float flCosRotation = cos(flRotation);
	float flSinRotation = sin(flRotation);

	cmd->flForwardMove = flCosRotation * cmd->flForwardMove - flSinRotation * cmd->flSideMove;
	cmd->flSideMove = flSinRotation * cmd->flForwardMove + flCosRotation * cmd->flSideMove;
}


void TMath::VectorAngles(const Vector3D& forward, Vector3D& up, Vector3D& angles)
{
	Vector3D left = up.CrossProduct(forward);
	left.NormalizeInPlace();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180.0f / (float)(M_PI);
		angles.y = atan2f(forward.y, forward.x) * 180.0f / (float)(M_PI);

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.z = atan2f(left.z, upZ) * 180.0f / (float)(M_PI);
	}
	else
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180.0f / (float)(M_PI);
		angles.y = atan2f(-left.x, left.y) * 180.0f / (float)(M_PI);
		angles.z = 0;
	}
}

void TMath::VectorAngles(const Vector3D& vecForward, Vector3D& angView)
{
	float flPitch, flYaw;

	if (vecForward.x == 0.f && vecForward.y == 0.f)
	{
		flPitch = (vecForward.z > 0.f) ? 270.f : 90.f;
		flYaw = 0.f;
	}
	else
	{
		flPitch = std::atan2f(-vecForward.z, vecForward.Length2D()) * 180.f / DirectX::XM_PI;

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = std::atan2f(vecForward.y, vecForward.x) * 180.f / DirectX::XM_PI;

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	angView.x = flPitch;
	angView.y = flYaw;
	angView.z = 0.f;
}


void TMath::AngleVectors(const Vector3D& angView, Vector3D* pForward, Vector3D* pRight, Vector3D* pUp)
{
	float sp, sy, sr, cp, cy, cr;

	DirectX::XMScalarSinCos(&sp, &cp, M_DEG2RAD(angView.x));
	DirectX::XMScalarSinCos(&sy, &cy, M_DEG2RAD(angView.y));
	DirectX::XMScalarSinCos(&sr, &cr, M_DEG2RAD(angView.z));

	if (pForward != nullptr)
	{
		pForward->x = cp * cy;
		pForward->y = cp * sy;
		pForward->z = -sp;
	}

	if (pRight != nullptr)
	{
		pRight->x = -1 * sr * sp * cy + -1 * cr * -sy;
		pRight->y = -1 * sr * sp * sy + -1 * cr * cy;
		pRight->z = -1 * sr * cp;
	}

	if (pUp != nullptr)
	{
		pUp->x = cr * sp * cy + -sr * -sy;
		pUp->y = cr * sp * sy + -sr * cy;
		pUp->z = cr * cp;
	}
}