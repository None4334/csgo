

constexpr auto RadPi = 3.14159265358979323846;
constexpr auto DegPi = 180.0;

using matrix3x3_t = float[3][3];
struct matrix3x4_t
{
	matrix3x4_t() = default;

	constexpr matrix3x4_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23)
	{
		arrData[0][0] = m00; arrData[0][1] = m01; arrData[0][2] = m02; arrData[0][3] = m03;
		arrData[1][0] = m10; arrData[1][1] = m11; arrData[1][2] = m12; arrData[1][3] = m13;
		arrData[2][0] = m20; arrData[2][1] = m21; arrData[2][2] = m22; arrData[2][3] = m23;
	}

	constexpr matrix3x4_t(const Vector3D& xAxis, const Vector3D& yAxis, const Vector3D& zAxis, const Vector3D& vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	constexpr void Init(const Vector3D& vecForward, const Vector3D& vecLeft, const Vector3D& vecUp, const Vector3D& vecOrigin)
	{
		SetForward(vecForward);
		SetLeft(vecLeft);
		SetUp(vecUp);
		SetOrigin(vecOrigin);
	}

	constexpr void SetForward(const Vector3D& vecForward)
	{
		this->arrData[0][0] = vecForward.x;
		this->arrData[1][0] = vecForward.y;
		this->arrData[2][0] = vecForward.z;
	}

	constexpr void SetLeft(const Vector3D& vecLeft)
	{
		this->arrData[0][1] = vecLeft.x;
		this->arrData[1][1] = vecLeft.y;
		this->arrData[2][1] = vecLeft.z;
	}

	constexpr void SetUp(const Vector3D& vecUp)
	{
		this->arrData[0][2] = vecUp.x;
		this->arrData[1][2] = vecUp.y;
		this->arrData[2][2] = vecUp.z;
	}

	constexpr void SetOrigin(const Vector3D& vecOrigin)
	{
		this->arrData[0][3] = vecOrigin.x;
		this->arrData[1][3] = vecOrigin.y;
		this->arrData[2][3] = vecOrigin.z;
	}

	constexpr void Invalidate()
	{
		for (auto& arrSubData : arrData)
		{
			for (auto& flData : arrSubData)
				flData = std::numeric_limits<float>::infinity();
		}
	}

	float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	[[nodiscard]] constexpr Vector3D at(const int nIndex) const
	{
		return Vector3D(arrData[0][nIndex], arrData[1][nIndex], arrData[2][nIndex]);
	}

	float* Base()
	{
		return &arrData[0][0];
	}

	[[nodiscard]] const float* Base() const
	{
		return &arrData[0][0];
	}

	[[nodiscard]] float GetZDelta() const
	{
		return at(3).z;
	}


	[[nodiscard]] void SetZDelta(float z)
	{
		arrData[2][3] = z;
	}

	[[nodiscard]] Vector3D GetOrigin() const
	{
		return at(3);
	}

	template<typename T>
	T ToRadians(T degrees)
	{
		return (degrees * (static_cast<T>(RadPi) / static_cast<T>(DegPi)));
	}

	void AngleMatrix(const Vector3D& angles)
	{
		float sr, sp, sy, cr, cp, cy;
		DirectX::XMScalarSinCos(&sy, &cy, ToRadians(angles[1]));
		DirectX::XMScalarSinCos(&sp, &cp, ToRadians(angles[0]));
		DirectX::XMScalarSinCos(&sr, &cr, ToRadians(angles[2]));

		arrData[0][0] = cp * cy;
		arrData[1][0] = cp * sy;
		arrData[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		arrData[0][1] = sp * srcy - crsy;
		arrData[1][1] = sp * srsy + crcy;
		arrData[2][1] = sr * cp;

		arrData[0][2] = (sp * crcy + srsy);
		arrData[1][2] = (sp * crsy - srcy);
		arrData[2][2] = cr * cp;

		arrData[0][3] = 0.0f;
		arrData[1][3] = 0.0f;
		arrData[2][3] = 0.0f;
	}

	void MatrixSetColumn(const Vector3D& in, int column)
	{
		arrData[0][column] = in.x;
		arrData[1][column] = in.y;
		arrData[2][column] = in.z;
	}

	void AngleMatrix(const Vector3D& angles, const Vector3D& position)
	{
		AngleMatrix(angles);
		MatrixSetColumn(position, 3);
	}

	void QuaternionMatrix(const Quaternion& q)
	{
		arrData[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
		arrData[1][0] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
		arrData[2][0] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

		arrData[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
		arrData[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
		arrData[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

		arrData[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
		arrData[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
		arrData[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

		arrData[0][3] = 0.0f;
		arrData[1][3] = 0.0f;
		arrData[2][3] = 0.0f;
	}

	void QuaternionMatrix(const Quaternion& q, const Vector3D& pos)
	{
		QuaternionMatrix(q);

		arrData[0][3] = pos.x;
		arrData[1][3] = pos.y;
		arrData[2][3] = pos.z;
	}

	Vector3D operator*(const Vector3D& vVec) const {
		auto& m = arrData;
		Vector3D vRet;
		vRet.x = m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z + m[0][3];
		vRet.y = m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z + m[1][3];
		vRet.z = m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z + m[2][3];

		return vRet;
	}

	matrix3x4_t ConcatTransforms(matrix3x4_t in) const {
		auto& m = arrData;
		matrix3x4_t out;
		out[0][0] = m[0][0] * in[0][0] + m[0][1] * in[1][0] + m[0][2] * in[2][0];
		out[0][1] = m[0][0] * in[0][1] + m[0][1] * in[1][1] + m[0][2] * in[2][1];
		out[0][2] = m[0][0] * in[0][2] + m[0][1] * in[1][2] + m[0][2] * in[2][2];
		out[0][3] = m[0][0] * in[0][3] + m[0][1] * in[1][3] + m[0][2] * in[2][3] + m[0][3];
		out[1][0] = m[1][0] * in[0][0] + m[1][1] * in[1][0] + m[1][2] * in[2][0];
		out[1][1] = m[1][0] * in[0][1] + m[1][1] * in[1][1] + m[1][2] * in[2][1];
		out[1][2] = m[1][0] * in[0][2] + m[1][1] * in[1][2] + m[1][2] * in[2][2];
		out[1][3] = m[1][0] * in[0][3] + m[1][1] * in[1][3] + m[1][2] * in[2][3] + m[1][3];
		out[2][0] = m[2][0] * in[0][0] + m[2][1] * in[1][0] + m[2][2] * in[2][0];
		out[2][1] = m[2][0] * in[0][1] + m[2][1] * in[1][1] + m[2][2] * in[2][1];
		out[2][2] = m[2][0] * in[0][2] + m[2][1] * in[1][2] + m[2][2] * in[2][2];
		out[2][3] = m[2][0] * in[0][3] + m[2][1] * in[1][3] + m[2][2] * in[2][3] + m[2][3];
		return out;
	}

	matrix3x4_t operator*(const matrix3x4_t& vm) const {
		return ConcatTransforms(vm);
	}

	matrix3x4_t operator*(const float& other) const {
		matrix3x4_t ret;
		auto& m = arrData;
		for (int i = 0; i < 12; i++) {
			((float*)ret.arrData)[i] = ((float*)m)[i] * other;
		}
		return ret;
	}

	matrix3x4_t operator+(const matrix3x4_t& other) const {
		matrix3x4_t ret;
		auto& m = arrData;
		for (int i = 0; i < 12; i++) {
			((float*)ret.arrData)[i] = ((float*)m)[i] + ((float*)other.arrData)[i];
		}
		return ret;
	}

	float arrData[3][4] = { };
};

__declspec(align(16)) class matrix3x4a_t : public matrix3x4_t
{
public:
	matrix3x4a_t& operator=(const matrix3x4_t& matSource)
	{
		std::copy_n(matSource.Base(), sizeof(float) * 3U * 4U, this->Base());
		return *this;
	}
};

struct ViewMatrix_t
{
	ViewMatrix_t() = default;

	constexpr ViewMatrix_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33)
	{
		arrData[0][0] = m00; arrData[0][1] = m01; arrData[0][2] = m02; arrData[0][3] = m03;
		arrData[1][0] = m10; arrData[1][1] = m11; arrData[1][2] = m12; arrData[1][3] = m13;
		arrData[2][0] = m20; arrData[2][1] = m21; arrData[2][2] = m22; arrData[2][3] = m23;
		arrData[3][0] = m30; arrData[3][1] = m31; arrData[3][2] = m32; arrData[3][3] = m33;
	}

	constexpr ViewMatrix_t(const matrix3x4_t& matFrom, const Vector4D& vecAdditionalColumn = { })
	{
		arrData[0][0] = matFrom.arrData[0][0]; arrData[0][1] = matFrom.arrData[0][1]; arrData[0][2] = matFrom.arrData[0][2]; arrData[0][3] = matFrom.arrData[0][3];
		arrData[1][0] = matFrom.arrData[1][0]; arrData[1][1] = matFrom.arrData[1][1]; arrData[1][2] = matFrom.arrData[1][2]; arrData[1][3] = matFrom.arrData[1][3];
		arrData[2][0] = matFrom.arrData[2][0]; arrData[2][1] = matFrom.arrData[2][1]; arrData[2][2] = matFrom.arrData[2][2]; arrData[2][3] = matFrom.arrData[2][3];
		arrData[3][0] = vecAdditionalColumn.x; arrData[3][1] = vecAdditionalColumn.y; arrData[3][2] = vecAdditionalColumn.z; arrData[3][3] = vecAdditionalColumn.w;
	}

	float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	[[nodiscard]] constexpr Vector4D at(const int nIndex) const
	{
		return Vector4D(arrData[0][nIndex], arrData[1][nIndex], arrData[2][nIndex], arrData[3][nIndex]);
	}

	constexpr ViewMatrix_t& operator+=(const ViewMatrix_t& matAdd)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int n = 0; n < 4; n++)
				this->arrData[i][n] += matAdd[i][n];
		}

		return *this;
	}

	constexpr ViewMatrix_t& operator-=(const ViewMatrix_t& matSubtract)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int n = 0; n < 4; n++)
				this->arrData[i][n] -= matSubtract[i][n];
		}

		return *this;
	}

	ViewMatrix_t operator*(const ViewMatrix_t& matMultiply) const
	{
		return ViewMatrix_t(
			arrData[0][0] * matMultiply.arrData[0][0] + arrData[0][1] * matMultiply.arrData[1][0] + arrData[0][2] * matMultiply.arrData[2][0] + arrData[0][3] * matMultiply.arrData[3][0],
			arrData[0][0] * matMultiply.arrData[0][1] + arrData[0][1] * matMultiply.arrData[1][1] + arrData[0][2] * matMultiply.arrData[2][1] + arrData[0][3] * matMultiply.arrData[3][1],
			arrData[0][0] * matMultiply.arrData[0][2] + arrData[0][1] * matMultiply.arrData[1][2] + arrData[0][2] * matMultiply.arrData[2][2] + arrData[0][3] * matMultiply.arrData[3][2],
			arrData[0][0] * matMultiply.arrData[0][3] + arrData[0][1] * matMultiply.arrData[1][3] + arrData[0][2] * matMultiply.arrData[2][3] + arrData[0][3] * matMultiply.arrData[3][3],

			arrData[1][0] * matMultiply.arrData[0][0] + arrData[1][1] * matMultiply.arrData[1][0] + arrData[1][2] * matMultiply.arrData[2][0] + arrData[1][3] * matMultiply.arrData[3][0],
			arrData[1][0] * matMultiply.arrData[0][1] + arrData[1][1] * matMultiply.arrData[1][1] + arrData[1][2] * matMultiply.arrData[2][1] + arrData[1][3] * matMultiply.arrData[3][1],
			arrData[1][0] * matMultiply.arrData[0][2] + arrData[1][1] * matMultiply.arrData[1][2] + arrData[1][2] * matMultiply.arrData[2][2] + arrData[1][3] * matMultiply.arrData[3][2],
			arrData[1][0] * matMultiply.arrData[0][3] + arrData[1][1] * matMultiply.arrData[1][3] + arrData[1][2] * matMultiply.arrData[2][3] + arrData[1][3] * matMultiply.arrData[3][3],

			arrData[2][0] * matMultiply.arrData[0][0] + arrData[2][1] * matMultiply.arrData[1][0] + arrData[2][2] * matMultiply.arrData[2][0] + arrData[2][3] * matMultiply.arrData[3][0],
			arrData[2][0] * matMultiply.arrData[0][1] + arrData[2][1] * matMultiply.arrData[1][1] + arrData[2][2] * matMultiply.arrData[2][1] + arrData[2][3] * matMultiply.arrData[3][1],
			arrData[2][0] * matMultiply.arrData[0][2] + arrData[2][1] * matMultiply.arrData[1][2] + arrData[2][2] * matMultiply.arrData[2][2] + arrData[2][3] * matMultiply.arrData[3][2],
			arrData[2][0] * matMultiply.arrData[0][3] + arrData[2][1] * matMultiply.arrData[1][3] + arrData[2][2] * matMultiply.arrData[2][3] + arrData[2][3] * matMultiply.arrData[3][3],

			arrData[3][0] * matMultiply.arrData[0][0] + arrData[3][1] * matMultiply.arrData[1][0] + arrData[3][2] * matMultiply.arrData[2][0] + arrData[3][3] * matMultiply.arrData[3][0],
			arrData[3][0] * matMultiply.arrData[0][1] + arrData[3][1] * matMultiply.arrData[1][1] + arrData[3][2] * matMultiply.arrData[2][1] + arrData[3][3] * matMultiply.arrData[3][1],
			arrData[3][0] * matMultiply.arrData[0][2] + arrData[3][1] * matMultiply.arrData[1][2] + arrData[3][2] * matMultiply.arrData[2][2] + arrData[3][3] * matMultiply.arrData[3][2],
			arrData[3][0] * matMultiply.arrData[0][3] + arrData[3][1] * matMultiply.arrData[1][3] + arrData[3][2] * matMultiply.arrData[2][3] + arrData[3][3] * matMultiply.arrData[3][3]);
	}

	constexpr void Identity()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int n = 0; n < 4; n++)
				this->arrData[i][n] = i == n ? 1.0f : 0.0f;
		}
	}

	const matrix3x4_t& As3x4() const
	{
		return *reinterpret_cast<const matrix3x4_t*>(this);
	}

	matrix3x4_t& As3x4()
	{
		return *reinterpret_cast<matrix3x4_t*>(this);
	}

	float arrData[4][4] = { };
};

