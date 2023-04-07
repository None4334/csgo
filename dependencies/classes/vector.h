#include <cmath>
#include <limits>

class Vector2D
{
public:
	constexpr Vector2D(float x = 0.f, float y = 0.f) :
		x(x), y(y) { }

	Vector2D operator-(const Vector2D& v) const {
		return Vector2D(x - v.x, y - v.y);
	}

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO);
	}

public:
	float x, y;
};

class Vector3D
{
public:
	constexpr Vector3D(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) { }

	constexpr Vector3D(const float* arrVector) :
		x(arrVector[0]), y(arrVector[1]), z(arrVector[2]) { }

	constexpr Vector3D(const Vector2D& vecBase2D) :
		x(vecBase2D.x), y(vecBase2D.y), z(0.0f) { }

	[[nodiscard]] bool IsValid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}

	constexpr void Invalidate()
	{
		this->x = this->y = this->z = std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] float* data()
	{
		return reinterpret_cast<float*>(this);
	}

	[[nodiscard]] const float* data() const
	{
		return reinterpret_cast<float*>(const_cast<Vector3D*>(this));
	}

	float& operator[](const std::size_t i)
	{
		return this->data()[i];
	}

	const float& operator[](const std::size_t i) const
	{
		return this->data()[i];
	}

	bool operator==(const Vector3D& vecBase) const
	{
		return this->IsEqual(vecBase);
	}

	bool operator!=(const Vector3D& vecBase) const
	{
		return !this->IsEqual(vecBase);
	}

	constexpr Vector3D& operator=(const Vector3D& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z;
		return *this;
	}

	constexpr Vector3D& operator=(const Vector2D& vecBase2D)
	{
		this->x = vecBase2D.x; this->y = vecBase2D.y; this->z = 0.0f;
		return *this;
	}


	Vector3D GetNormalizeInPlace()
	{
		Vector3D& vec = *this;

		float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

		float iradius = 1.f / (radius + FLT_EPSILON);

		vec.x *= iradius;
		vec.y *= iradius;
		vec.z *= iradius;

		return vec;
	}

	constexpr Vector3D& operator+=(const Vector3D& vecBase)
	{
		this->x += vecBase.x; this->y += vecBase.y; this->z += vecBase.z;
		return *this;
	}

	constexpr Vector3D& operator-=(const Vector3D& vecBase)
	{
		this->x -= vecBase.x; this->y -= vecBase.y; this->z -= vecBase.z;
		return *this;
	}

	constexpr Vector3D& operator*=(const Vector3D& vecBase)
	{
		this->x *= vecBase.x; this->y *= vecBase.y; this->z *= vecBase.z;
		return *this;
	}

	constexpr Vector3D& operator/=(const Vector3D& vecBase)
	{
		this->x /= vecBase.x; this->y /= vecBase.y; this->z /= vecBase.z;
		return *this;
	}

	constexpr Vector3D& operator+=(const float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr Vector3D& operator-=(const float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr Vector3D& operator*=(const float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr Vector3D& operator/=(const float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}

	Vector3D operator+(const Vector3D& vecAdd) const
	{
		return Vector3D(this->x + vecAdd.x, this->y + vecAdd.y, this->z + vecAdd.z);
	}

	Vector3D operator-(const Vector3D& vecSubtract) const
	{
		return Vector3D(this->x - vecSubtract.x, this->y - vecSubtract.y, this->z - vecSubtract.z);
	}

	Vector3D operator*(const Vector3D& vecMultiply) const
	{
		return Vector3D(this->x * vecMultiply.x, this->y * vecMultiply.y, this->z * vecMultiply.z);
	}

	Vector3D operator/(const Vector3D& vecDivide) const
	{
		return Vector3D(this->x / vecDivide.x, this->y / vecDivide.y, this->z / vecDivide.z);
	}

	Vector3D operator+(const float flAdd) const
	{
		return Vector3D(this->x + flAdd, this->y + flAdd, this->z + flAdd);
	}

	Vector3D operator-(const float flSubtract) const
	{
		return Vector3D(this->x - flSubtract, this->y - flSubtract, this->z - flSubtract);
	}

	Vector3D operator*(const float flMultiply) const
	{
		return Vector3D(this->x * flMultiply, this->y * flMultiply, this->z * flMultiply);
	}

	Vector3D operator/(const float flDivide) const
	{
		return Vector3D(this->x / flDivide, this->y / flDivide, this->z / flDivide);
	}

	[[nodiscard]] bool IsEqual(const Vector3D& vecEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const
	{
		return (std::fabsf(this->x - vecEqual.x) < flErrorMargin &&
			std::fabsf(this->y - vecEqual.y) < flErrorMargin &&
			std::fabsf(this->z - vecEqual.z) < flErrorMargin);
	}

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO &&
			std::fpclassify(this->z) == FP_ZERO);
	}

	[[nodiscard]] Vector2D ToVector2D() const
	{
		return Vector2D(this->x, this->y);
	}

	[[nodiscard]] float Length() const
	{
		return std::sqrtf(this->LengthSqr());
	}

	[[nodiscard]] constexpr float LengthSqr() const
	{
		return DotProduct(*this);
	}

	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(this->Length2DSqr());
	}

	[[nodiscard]] constexpr float Length2DSqr() const
	{
		return (this->x * this->x + this->y * this->y);
	}

	[[nodiscard]] float DistTo(const Vector3D& vecEnd) const
	{
		return (*this - vecEnd).Length();
	}

	[[nodiscard]] constexpr float DistToSqr(const Vector3D& vecEnd) const
	{
		return (*this - vecEnd).LengthSqr();
	}

	void ClampAngles() {

		if (x > 89.f)
			x = 89.f;

		if (x < -89.f)
			x = -89.f;

		if (y > 180.f)
			y = 180.f;

		if (y < -180.f)
			y = -180.f;

		if (z > 45.f)
			z = 45.f;

		if (z < -45.f)
			z = -45.f;
	}


	void NormalizeAngles() {
		x = std::isfinite(x) ? std::remainderf(x, 360.0f) : 0.0f;

		y = fmod(y, 360.0f);

		if (y > 180.0f)
			y -= 360.0f;

		if (y < -180.0f)
			y += 360.0f;

	}

	[[nodiscard]] Vector3D NormalizedAngles() const
	{
		Vector3D vecOut = *this;
		vecOut.NormalizeAngles();
		return vecOut;
	}

	[[nodiscard]] Vector3D Normalized() const
	{
		Vector3D vecOut = *this;
		vecOut.NormalizeInPlace();
		return vecOut;
	}

	void NormalizeInPlaceRetNil() {
		Vector3D res = *this;
		float l = res.Length();
		if (l != 0.0f) {
			res /= l;
		}
		else {
			res.x = res.y = res.z = 0.0f;
		}
		*this= res;
	}
	float NormalizeInPlace()
	{
		const float flLength = this->Length();
		const float flRadius = 1.0f / (flLength + std::numeric_limits<float>::epsilon());

		this->x *= flRadius;
		this->y *= flRadius;
		this->z *= flRadius;

		return flLength;
	}

	float Normalize() const
	{
		Vector3D res = *this;
		float l = res.Length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res.x = res.y = res.z = 0.0f;
		}
		return l;
	}

	[[nodiscard]] constexpr float DotProduct(const Vector3D& vecDot) const
	{
	return (this->x * vecDot.x + this->y * vecDot.y + this->z * vecDot.z);
	}

	[[nodiscard]] constexpr Vector3D CrossProduct(const Vector3D& vecCross) const
	{
		return Vector3D(this->y * vecCross.z - this->z * vecCross.y, this->z * vecCross.x - this->x * vecCross.z, this->x * vecCross.y - this->y * vecCross.x);
	}

public:
	float x, y, z;
};

class Vector4D
{
public:
	constexpr Vector4D(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) :
		x(x), y(y), z(z), w(w) { }

public:
	float x, y, z, w;
};

class __declspec(align(16)) VectorAligned : public Vector3D
{
public:
	VectorAligned() = default;

	explicit VectorAligned(const Vector3D& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
	}

	constexpr VectorAligned& operator=(const Vector3D& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
		return *this;
	}

public:
	float w;
};

class Quaternion {
public:
	float x, y, z, w;
};