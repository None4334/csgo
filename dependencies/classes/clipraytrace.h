
class RayTracer {
public:
	struct Ray {
		Ray(const Vector3D& direction);
		Ray(const Vector3D& startPoint, const Vector3D& endPoint);
		Vector3D m_startPoint;
		Vector3D m_direction;
		float m_length;
	};

	struct Hitbox {
		Hitbox();
		Hitbox(const Vector3D& mins, const Vector3D& maxs, const float radius);
		Hitbox(const std::tuple<Vector3D, Vector3D, float>& initTuple);
		Vector3D m_mins;
		Vector3D m_maxs;
		float m_radius;
	};

	struct Trace {
		Trace();
		bool m_hit;
		float m_fraction;
		Vector3D m_traceEnd;
		Vector3D m_traceOffset;
	};

	enum Flags {
		Flags_NONE = 0,
		Flags_RETURNEND = (1 << 0),
		Flags_RETURNOFFSET = (1 << 1)
	};

	static void TraceFromCenter(const Ray& ray, const Hitbox& hitbox, Trace& trace, int flags = 0);
	static void TraceHitbox(const Ray& ray, const Hitbox& hitbox, Trace& trace, int flags = 0);

	static void ClipRayToHitbox(Vector3D vecStart,Vector3D vecEnd,Vector3D vecMin,Vector3D vecMax,float flRadius,Trace& EngineTrace) {
		Hitbox box(vecMin, vecMax, flRadius);
		Ray ray = RayTracer::Ray(vecStart, vecEnd);
		TraceFromCenter(ray, box, EngineTrace, RayTracer::Flags_NONE);
	}
};