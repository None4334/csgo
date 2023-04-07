

struct FontSize_t {
	int iWidth;
	int iHeight;
};

class CSurfaceFont {
public:
	HFont      uFontHandle;
	FontSize_t pFontSize;
public:
	CSurfaceFont() : uFontHandle{}, pFontSize{} {};

	CSurfaceFont(const std::string& szFontName, int iSize, int iWidth, int uFlags);
	FontSize_t GetFontSize(const std::string& text);
	void DrawString(const Vector2D& vecPosition, const Vector4D& colColor, const std::string& szText, bool m_bAlign = false);
};

struct TSurfaceFont {
	CSurfaceFont Default;
	CSurfaceFont Shadow;
	CSurfaceFont Outline;
};

struct TCircleObjectWorld
{
	TCircleObjectWorld()
	{

	}

	TCircleObjectWorld(const Vector3D& centre, float radius, int segments, Vector4D color, float thickness)
		: vecCenter{ centre }, flRadius{ radius }, iSegments{ segments }, vecColor{ color }, flThickness{ thickness }
	{}
	TCircleObjectWorld(const Vector3D& centre, float radius, int segments, Vector4D color)
		: vecCenter{ centre }, flRadius{ radius }, iSegments{ segments }, vecColor{ color }
	{}

	TCircleObjectWorld(const Vector3D& pos, float radius, int segments, Vector4D color, int flags, float thickness)
		: vecPos{ pos }, flRadius{ radius }, iSegments{ segments }, vecColor{ color }, iFlags{ flags }, flThickness{ thickness }
	{}

	TCircleObjectWorld(const Vector3D& pos, float radius, int segments, Vector4D color, Vector4D outline, int flags, float thickness)
		: vecPos{ pos }, flRadius{ radius }, iSegments{ segments }, vecColor{ color }, vecOutlineColor{ outline }, iFlags{ flags }, flThickness{ thickness }
	{}

	Vector3D vecCenter;
	Vector3D vecPos;
    TDynamicStack<Vector2D, 500> vecPoints;
	float flRadius;
	Vector4D vecColor;
	Vector4D vecOutlineColor;
	int iSegments;
	int iFlags;
	float flThickness;
};

struct TCircleObject
{
	TCircleObject()
	{

	}

	TCircleObject(const Vector2D& _vecPos, float _flRadius, int _iSegments, Vector4D _vecColor, float _flThickness, bool _Filled)
	{
		 vecPos=_vecPos;
		 flRadius= _flRadius;
		 vecColor= _vecColor;
		 iSegments= _iSegments;
		 flThickness= _flThickness;
		 bFilled = _Filled;
	}

	Vector2D vecPos = Vector2D();
	float flRadius = 0.f;
	Vector4D vecColor = Vector4D();
	int iSegments = 0.f;
	float flThickness = 1.f;
	bool bFilled = false;
};

struct TLineObject
{
	TLineObject()
	{

	}
	TLineObject(Vector2D vecStartOther,
		Vector2D vecEndOther,
		Vector4D colLineOther,
		float flThicknessOther)
	{
		vecStart = vecStartOther;
		vecEnd = vecEndOther;
		colLine = colLineOther;
		flThickness = flThicknessOther;
	}

	Vector2D vecStart = { };
	Vector2D vecEnd = { };
	Vector4D colLine = 0x0;
	float flThickness = 0.f;
};

struct TRectObject
{
	TRectObject() {

	}

	TRectObject(Vector2D vecMinOther,
		Vector2D vecMaxOther,
		Vector4D colRectOther,
		float flRoundingOther,
		int roundingCornersOther,
		float flThicknessOther,
		bool bFilledOther) {
		vecMin = vecMinOther;
		vecMax = vecMaxOther;
		colRect = colRectOther;
		flRounding = flRoundingOther;
		roundingCorners = roundingCornersOther;
		flThickness = flThicknessOther;
		bFilled = bFilledOther;
	}

	Vector2D vecMin = { };
	Vector2D vecMax = { };
	Vector4D colRect = 0x0;
	float flRounding = 0.f;
	int roundingCorners = 0;
	float flThickness = 0.f;
	bool bFilled = false;
};

struct ImFont;
enum EOutlineMode {
	TEXTO_NONE=0,
	TEXTO_SHADOW ,
	TEXTO_OUTLINE
};

struct TFontObject {

	TFontObject(ImFont* _pFont,Vector2D _vecPosition, Vector4D _vecColor, Vector4D _vecOutline, std::string _szText,bool _bShadow = false,bool _bOutline = false) {
		pFont = _pFont;
		vecPosition = _vecPosition;
		vecColor = _vecColor;
		vecOutline = _vecOutline;
		szText = _szText;
		bShadow = _bShadow;
		bOutline = _bOutline;
	}

	TFontObject() {

	}

	ImFont* pFont = nullptr;
	Vector2D vecPosition= Vector2D();
	Vector4D vecColor= Vector4D();
	Vector4D vecOutline = Vector4D();
	bool bShadow = false;
	bool bOutline = false;
	std::string szText;
};

class ImDrawList;
namespace TDirectXRender {

	inline TDynamicStack<TCircleObjectWorld, 10000> stkWorldCircle;
	inline TDynamicStack<TCircleObjectWorld, 10000> stkExchangeWorldCircle;
	
	inline TDynamicStack<TLineObject, 10000> stkLine;
	inline TDynamicStack<TLineObject, 10000> stkExchangeLine;

	inline TDynamicStack<TRectObject, 10000> stkRectangle;
	inline TDynamicStack<TRectObject, 10000> stkExchangeRectangle;

	inline TDynamicStack<TFontObject, 10000> stkText;
	inline TDynamicStack<TFontObject, 10000> stkExchangeText;

	inline TDynamicStack<TCircleObject, 10000> stkCircle;
	inline TDynamicStack<TCircleObject, 10000> stkExchangeCircle;

	namespace TDirectFonts {
		inline ImFont* WeaponIcons;
		inline ImFont* WeaponIconsMedium;
		inline ImFont* VerdanaSmall;
		inline ImFont* VerdanaMedium;
		inline ImFont* Tahoma;
		inline ImFont* SmallestPixel;
		inline ImFont* SmallestPixelMedium;
		inline bool bInitialized = false;
		void InitFonts();
	}

	void LockAndClear();

	inline std::shared_mutex drawMutex = { };

	void Circle3DFilled(Vector3D vecOrigin, float flRadius, int iPoints, Vector4D vecColor);
	void Line(Vector2D vecStart, Vector2D vecEnd, Vector4D vecColor , int Thickness);
	void Rectangle(Vector2D vecStart, Vector2D vecSize, Vector4D vecColor,bool Filled = false,float flRounding = 0.0f, int Thickness = 1.f);
	void BoxLine3D(Vector3D vecStart, Vector3D vecEnd, Vector4D vecColor, float Width);

	void Circle(Vector2D vecPos, float flRadius, int iPoints, Vector4D vecColor, bool bFilled = false, int Thickness = 1.f);
	void Text(ImFont* pFont , std::string szText, Vector2D vecPosition, Vector4D vecColor,bool bOutline = false , bool bShadow = false, Vector4D vecOutline = Vector4D());
	void OnDrawList(ImDrawList* DrawList);
	void SwapDrawData();

}

namespace TSurfaceRender {

	namespace TFonts {
		inline TSurfaceFont SmallestPixel;
		inline TSurfaceFont SmallestPixelSmall;
		inline TSurfaceFont Verdana;
		inline TSurfaceFont VerdanaMedium;
		inline TSurfaceFont BigVerdana;
		inline TSurfaceFont PlayerName;
		inline TSurfaceFont IconsFont;
		inline TSurfaceFont OtherIconsFont;
	}


	void Text(CSurfaceFont& Font,std::string szRenderText, Vector2D  vecPosition, const Vector4D& vecColor, bool bAlign);
	void Line(Vector2D vecStart, Vector2D vecEnd, Vector4D vecColor);
	void BoxLine3D(Vector3D vecStart, Vector3D vecEnd, Vector4D vecColor,float Width);
	void Rectangle(Vector2D vecMin, Vector2D vecSize, Vector4D vecColor, bool bFilled);
	void GradientRectangle(Vector2D vecMin, Vector2D vecSize, Vector4D vecColor, Vector4D vecColorSecond, bool bHorizontal);
	void DrawCircle3D(Vector3D vecPosition, float flPoints, float flRadius, Vector4D vecColor, bool bCheckScreen = true);
	void DrawCircle3DFilled(Vector3D vecPosition, float flPoints, float flRadius, Vector4D vecColor, bool bCheckScreen = true);
	void DrawPolyLine(int* x, int* y, const int count, const Vector4D& vecColor);
	void DrawPolyLine(const int count, Vertex_t* verts, const Vector4D& color);
	bool Initialize();
}