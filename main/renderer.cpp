#include "../includes.h"
#include "renderer.h"
#include "features/visuals.h"
#include "math.h"
#include "../dependencies/imgui/imgui.h"
#include "../dependencies/imgui/imgui_internal.h"
#include "../dependencies/fonts/weaponicons.h"
#include "../dependencies/imgui/imgui_freetype.h"
#include "../dependencies/fonts/menufonts.h"
#include "../dependencies/fonts/smallestpixel.h"

bool TSurfaceRender::Initialize() {

	TFonts::SmallestPixel = TSurfaceFont{
	CSurfaceFont(TSTRING("Small Fonts"), 10, FW_NORMAL,0),
	 CSurfaceFont(TSTRING("Small Fonts"), 10, FW_NORMAL, FONTFLAG_DROPSHADOW),
	 CSurfaceFont(TSTRING("Small Fonts"), 10, FW_NORMAL, FONTFLAG_OUTLINE)
	};

	TFonts::SmallestPixelSmall = TSurfaceFont{
CSurfaceFont(TSTRING("Small Fonts"), 8, FW_NORMAL,0),
 CSurfaceFont(TSTRING("Small Fonts"), 8, FW_NORMAL, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Small Fonts"), 8, FW_NORMAL, FONTFLAG_OUTLINE)
	};

	TFonts::Verdana = TSurfaceFont{
CSurfaceFont(TSTRING("Verdana"), 12, FW_NORMAL,0),
 CSurfaceFont(TSTRING("Verdana"), 12, FW_NORMAL, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Verdana"), 12, FW_NORMAL, FONTFLAG_OUTLINE)
	};

	TFonts::VerdanaMedium = TSurfaceFont{
CSurfaceFont(TSTRING("Verdana"), 14, FW_NORMAL,0),
 CSurfaceFont(TSTRING("Verdana"),14, FW_NORMAL, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Verdana"), 14, FW_NORMAL, FONTFLAG_OUTLINE)
	};

	TFonts::BigVerdana = TSurfaceFont{
CSurfaceFont(TSTRING("Verdana"), 26, FW_NORMAL,0),
 CSurfaceFont(TSTRING("Verdana"), 26, FW_NORMAL, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Verdana"), 26, FW_NORMAL, FONTFLAG_OUTLINE)
	};

	TFonts::PlayerName = TSurfaceFont{
CSurfaceFont(TSTRING("Sans Serif"), 16, FW_NORMAL,0),
 CSurfaceFont(TSTRING("Sans Serif"),16, FW_NORMAL, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Sans Serif"), 16, FW_NORMAL, FONTFLAG_OUTLINE)
	};

	TFonts::IconsFont = TSurfaceFont{
CSurfaceFont(TSTRING("undefeated"), 13, FW_DONTCARE, FONTFLAG_ANTIALIAS),
 CSurfaceFont(TSTRING("undefeated"),13, FW_DONTCARE, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("undefeated"), 13, FW_DONTCARE, FONTFLAG_OUTLINE)
	};

	TFonts::OtherIconsFont = TSurfaceFont{
CSurfaceFont(TSTRING("Counter-Strike"), 14, 400,FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Counter-Strike"),24, 400, FONTFLAG_DROPSHADOW),
 CSurfaceFont(TSTRING("Counter-Strike"), 24, 400, FONTFLAG_OUTLINE)
	};

	return true;
}

void TDirectXRender::TDirectFonts::InitFonts() {

	if (bInitialized)
		return;

	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig imIconsConfig;
	constexpr ImWchar wIconRanges[] =
	{
		0xE000, 0xF8FF, 
		0
	};

	ImFontConfig MenuConfig;
	MenuConfig.FontBuilderFlags |= ImGuiFreeType::RasterizerFlags::LightHinting;

	ImFontConfig ConfigBold;
	ConfigBold.FontBuilderFlags |= ImGuiFreeType::RasterizerFlags::Bold;

	ImGui::IconFontLogs = io.Fonts->AddFontFromMemoryTTF(IconFontLog, sizeof(IconFontLog), 25, &MenuConfig, io.Fonts->GetGlyphRangesCyrillic());
	ImGui::LexendDecaFont = io.Fonts->AddFontFromMemoryTTF(LexendDeca, sizeof(LexendDeca), 20, &ConfigBold, io.Fonts->GetGlyphRangesCyrillic());
	ImGui::InterMedium = io.Fonts->AddFontFromMemoryTTF(Intermedium, sizeof(Intermedium), 16, &MenuConfig, io.Fonts->GetGlyphRangesCyrillic());
	ImGui::InterSmall = io.Fonts->AddFontFromMemoryTTF(Intermedium, sizeof(Intermedium), 13, &MenuConfig, io.Fonts->GetGlyphRangesCyrillic());

	ImFontConfig cfgVerdanaMedium;
	cfgVerdanaMedium.FontBuilderFlags |= ImGuiFreeType::RasterizerFlags::MonoHinting;
	cfgVerdanaMedium.FontBuilderFlags |= ImGuiFreeType::RasterizerFlags::Monochrome;

	ImFontConfig imIconsConfigSec;
	imIconsConfig.FontBuilderFlags = ImGuiFreeType::LightHinting;

	WeaponIcons = io.Fonts->AddFontFromMemoryCompressedTTF(icons_compressed_data, icons_compressed_size, 14.f, &imIconsConfigSec, wIconRanges);
	WeaponIconsMedium = io.Fonts->AddFontFromMemoryCompressedTTF(icons_compressed_data, icons_compressed_size, 20.f, &imIconsConfigSec, wIconRanges);

	VerdanaSmall = io.Fonts->AddFontFromFileTTF(TSTRING("C:\\Windows\\Fonts\\Verdana.ttf"), 12.f, &cfgVerdanaMedium, io.Fonts->GetGlyphRangesCyrillic());
	VerdanaMedium = io.Fonts->AddFontFromFileTTF(TSTRING("C:\\Windows\\Fonts\\Verdana.ttf"), 15.f, &cfgVerdanaMedium, io.Fonts->GetGlyphRangesCyrillic());

	Tahoma = io.Fonts->AddFontFromFileTTF(TSTRING("C:\\Windows\\Fonts\\Verdana.ttf"), 13.f, &cfgVerdanaMedium, io.Fonts->GetGlyphRangesCyrillic());

	SmallestPixel = io.Fonts->AddFontFromMemoryCompressedTTF(smallest_pixel_compressed_data, smallest_pixel_compressed_size, 10.f, &cfgVerdanaMedium, io.Fonts->GetGlyphRangesCyrillic());
	SmallestPixelMedium = io.Fonts->AddFontFromMemoryCompressedTTF(smallest_pixel_compressed_data, smallest_pixel_compressed_size, 12.f, &cfgVerdanaMedium, io.Fonts->GetGlyphRangesCyrillic());


	io.Fonts->AddFontDefault();

	ImGuiFreeType::BuildFontAtlas(io.Fonts,0u);
	bInitialized = true;
}

void TSurfaceRender::DrawCircle3D(Vector3D vecPosition, float flPoints, float flRadius, Vector4D vecColor, bool bCheckScreen)
{
	float flStep = (float)DirectX::XM_PI * 2.0f / flPoints;

	for (float a = 0; a < (DirectX::XM_PI * 2.0f); a += flStep)
	{
		Vector3D start(flRadius * cosf(a) + vecPosition.x,
			flRadius * sinf(a) + vecPosition.y, vecPosition.z);

		Vector3D end(flRadius * cosf(a + flStep) + vecPosition.x,
			flRadius * sinf(a + flStep) + vecPosition.y, vecPosition.z);

		Vector2D start2d, end2d;
		if (bCheckScreen)
		{
			if (!TVisuals::TUtils::WorldToScreen(start, start2d) ||
				!TVisuals::TUtils::WorldToScreen(end, end2d))
				return;
		}
		else {
			TVisuals::TUtils::WorldToScreen(start, start2d);
			TVisuals::TUtils::WorldToScreen(end, end2d);
		}

		Line(Vector2D(start2d.x, start2d.y), Vector2D(end2d.x, end2d.y), vecColor);
	}
}

void TSurfaceRender::DrawPolyLine(int* x, int* y, const int count, const Vector4D& vecColor)
{
	TInterfaces::Surface->DrawSetColor(vecColor.x, vecColor.y, vecColor.z, vecColor.w);
	TInterfaces::Surface->DrawPolyLine(x, y, count);
}

void TSurfaceRender::DrawPolyLine(const int count, Vertex_t* verts, const Vector4D& color)
{
	auto x = std::make_unique<int[]>(count);
	auto y = std::make_unique<int[]>(count);

	for (auto i : std::views::iota(0, count))
	{
		x[i] = static_cast<int>(verts[i].vecPosition.x);
		y[i] = static_cast<int>(verts[i].vecPosition.y);
	}

	DrawPolyLine(x.get(), y.get(), count, color);
}

void TSurfaceRender::DrawCircle3DFilled(Vector3D vecPosition, float flPoints, float flRadius, Vector4D vecColor, bool bCheckScreen)
{
	/*
	Vector2D orignalW2S = {};

	if (!TVisuals::TUtils::WorldToScreen(vecPosition, orignalW2S))
		return;

	float step = TMath::PI_2 / flPoints;
	for (float angle = 0.0f; angle < TMath::PI_2; angle += step)
	{
		Vector3D worldStart = Vector3D{ flRadius * std::cos(angle) + vecPosition.x, flRadius * std::sin(angle) + vecPosition.y, vecPosition.z };
		Vector3D worldEnd = Vector3D{ flRadius * std::cos(angle + step) + vecPosition.x , flRadius * std::sin(angle + step) + vecPosition.y, vecPosition.z };

		if (Vector2D start, end; TVisuals::TUtils::WorldToScreen(worldStart, start) && TVisuals::TUtils::WorldToScreen(worldEnd, end))
		{
			Line(start, end, vecColor);
			drawTriangle(orignalW2S, start, end, Color(color.r(), color.g(), color.b(), color.a() / 4.0f));
		}
	}

	float flStep = (float)DirectX::XM_PI * 2.0f / flPoints;

	for (float a = 0; a < (DirectX::XM_PI * 2.0f); a += flStep)
	{
		TInterfaces::Surface->DrawPolyLine();

		Vector3D start(flRadius * cosf(a) + vecPosition.x,
			flRadius * sinf(a) + vecPosition.y, vecPosition.z);

		Vector3D end(flRadius * cosf(a + flStep) + vecPosition.x,
			flRadius * sinf(a + flStep) + vecPosition.y, vecPosition.z);

		Vector2D start2d, end2d;
		if (bCheckScreen)
		{
			if (!TVisuals::TUtils::WorldToScreen(start, start2d) ||
				!TVisuals::TUtils::WorldToScreen(end, end2d))
				return;
		}
		else {
			TVisuals::TUtils::WorldToScreen(start, start2d);
			TVisuals::TUtils::WorldToScreen(end, end2d);
		}


		Line(Vector2D(start2d.x, start2d.y), Vector2D(end2d.x, end2d.y), vecColor);
	}
*/
} 


void TSurfaceRender::Text(CSurfaceFont& Font, std::string szRenderText, Vector2D  vecPosition, const Vector4D& vecColor, bool bAlign) {
	Font.DrawString(vecPosition, vecColor, szRenderText, bAlign);
}

void TSurfaceRender::BoxLine3D(Vector3D vecStart, Vector3D vecEnd, Vector4D vecColor, float Width) {

	float flTotalValues = Width / .40f;

	float flCenterValue = flTotalValues / 2.f;
	flCenterValue = flCenterValue / 4.f;

	float flAddAlpha = 1.f / flTotalValues;

	float flCurrentAlpha = 0.0f;

	for (float flWidth = 0.0f; flWidth < Width; flWidth += .40f)
	{
		if (flWidth < flCenterValue)
			flCurrentAlpha += flAddAlpha;

		else
			flCurrentAlpha -= flAddAlpha;

		if (flWidth == 0.0f)
			continue;

		if (flCurrentAlpha <= 0.0f)
			continue;

		Vector3D vecNewStart = Vector3D(vecStart.x, vecStart.y, vecStart.z - flWidth);
		Vector3D vecNewEnd = Vector3D(vecEnd.x, vecEnd.y, vecEnd.z - flWidth);
		Vector4D vecNewColor = Vector4D(vecColor.x, vecColor.y, vecColor.z, flCurrentAlpha * 255.f);

		Vector2D vecWorldStart;
		if (!TVisuals::TUtils::WorldToScreen(vecNewStart, vecWorldStart))
			continue;

		Vector2D vecWorldEnd;
		if (!TVisuals::TUtils::WorldToScreen(vecNewEnd, vecWorldEnd))
			continue;

		TSurfaceRender::Line(vecWorldStart, vecWorldEnd, vecNewColor);
	}

}

void TDirectXRender::Text(ImFont* pFont, std::string szText, Vector2D vecPosition, Vector4D vecColor, bool bOutline, bool bShadow, Vector4D vecOutline) {
	stkText.add_back(TFontObject(pFont, vecPosition, vecColor, vecOutline, szText, bShadow, bOutline));
}

void TDirectXRender::BoxLine3D(Vector3D vecStart, Vector3D vecEnd, Vector4D vecColor, float Width) {

	float flTotalValues = Width / .40f;

	float flCenterValue = flTotalValues / 2.f;
	flCenterValue = flCenterValue / 4.f;

	float flAddAlpha =   1.f / flTotalValues;

	float flCurrentAlpha = 0.0f;

	for (float flWidth = 0.0f; flWidth < Width; flWidth += .40f)
	{
		if (flWidth < flCenterValue)
			flCurrentAlpha += flAddAlpha;
		
		else 
			flCurrentAlpha -= flAddAlpha;
		
		if (flWidth == 0.0f)
			continue;

		if (flCurrentAlpha <= 0.0f)
			continue;

		Vector3D vecNewStart = Vector3D(vecStart.x, vecStart.y, vecStart.z - flWidth);
		Vector3D vecNewEnd = Vector3D(vecEnd.x, vecEnd.y, vecEnd.z - flWidth);
		Vector4D vecNewColor = Vector4D(vecColor.x, vecColor.y, vecColor.z, std::clamp(flCurrentAlpha * 255.f,0.f, vecColor.w ));

		Vector2D vecWorldStart;
		if (!TVisuals::TUtils::WorldToScreen(vecNewStart, vecWorldStart))
			continue;

		Vector2D vecWorldEnd;
		if (!TVisuals::TUtils::WorldToScreen(vecNewEnd, vecWorldEnd))
			continue;

		TDirectXRender::Line(vecWorldStart, vecWorldEnd, vecNewColor,2);
	}
	
}

void TSurfaceRender::Line(Vector2D vecStart, Vector2D vecEnd, Vector4D vecColor) {
	TInterfaces::Surface->DrawSetColor(vecColor.x, vecColor.y, vecColor.z, vecColor.w);
	TInterfaces::Surface->DrawLine(vecStart.x, vecStart.y, vecEnd.x, vecEnd.y);
}

void TSurfaceRender::Rectangle(Vector2D vecMin, Vector2D vecSize, Vector4D vecColor, bool bFilled) {

	TInterfaces::Surface->DrawSetColor(vecColor.x, vecColor.y, vecColor.z, vecColor.w);

	if (bFilled)
		TInterfaces::Surface->DrawFilledRect(vecMin.x, vecMin.y, vecMin.x + vecSize.x, vecMin.y + vecSize.y);
	else
		TInterfaces::Surface->DrawOutlinedRect(vecMin.x, vecMin.y, vecMin.x + vecSize.x, vecMin.y + vecSize.y);
}

void TSurfaceRender::GradientRectangle(Vector2D vecMin, Vector2D vecSize, Vector4D vecColor, Vector4D vecColorSecond, bool bHorizontal) {
	TInterfaces::Surface->DrawSetColor(Color(vecColor.x, vecColor.y, vecColor.z, vecColor.w));
	TInterfaces::Surface->DrawFilledRectFade(vecMin.x, vecMin.y, vecMin.x + vecSize.x, vecMin.y + vecSize.y, vecColor.w, 0, bHorizontal);
	TInterfaces::Surface->DrawSetColor(Color(vecColorSecond.x, vecColorSecond.y, vecColorSecond.z, vecColorSecond.w));
	TInterfaces::Surface->DrawFilledRectFade(vecMin.x, vecMin.y, vecMin.x + vecSize.x, vecMin.y + vecSize.y, 0, vecColorSecond.w, bHorizontal);
}

CSurfaceFont::CSurfaceFont(const std::string& szFontName, int iSize, int iWidth, int uFlags) {
	uFontHandle = TInterfaces::Surface->FontCreate();
	TInterfaces::Surface->SetFontGlyphSet(uFontHandle, szFontName.data(), iSize, iWidth, 0, 0, uFlags);
	pFontSize = GetFontSize(TSTRING("A"));
}

FontSize_t CSurfaceFont::GetFontSize(const std::string& text) {
	std::wstring szWideString = TUtil::MultiByteToWide(text);
	FontSize_t res;
	TInterfaces::Surface->GetTextSize(uFontHandle, szWideString.data(), res.iWidth, res.iHeight);
	return res;
}

void CSurfaceFont::DrawString(const Vector2D& vecPosition, const Vector4D& colColor, const std::string& szText, bool m_bAlign) {

	std::wstring szWideString = TUtil::MultiByteToWide(szText);
	int iWidth, iHeight;

	TInterfaces::Surface->GetTextSize(uFontHandle, szWideString.c_str(), iWidth, iHeight);
	TInterfaces::Surface->DrawSetTextFont(uFontHandle);
	TInterfaces::Surface->DrawSetTextColor(colColor.x, colColor.y, colColor.z, colColor.w);

	TInterfaces::Surface->DrawSetTextPos(m_bAlign ? (vecPosition.x - iWidth / 2) : vecPosition.x, vecPosition.y);
	TInterfaces::Surface->DrawPrintText(szWideString.c_str(), (int)szWideString.size());
}

void TDirectXRender::Circle( Vector2D vecPos, float flRadius, int iPoints, Vector4D vecColor, bool bFilled, int Thickness) {
	stkCircle.add_back(TCircleObject(vecPos, flRadius, iPoints, vecColor, Thickness, bFilled));
}

void TDirectXRender::Circle3DFilled(Vector3D vecOrigin, float flRadius, int iPoints, Vector4D vecColor)
{
	float step = M_PI * 2.0f / iPoints;

	auto pObject = stkWorldCircle.add_back(TCircleObjectWorld(vecOrigin, flRadius, iPoints, Vector4D(int(vecColor.x), int(vecColor.y), int(vecColor.z),
		int(vecColor.w)), Vector4D(int(vecColor.x), int(vecColor.y), int(vecColor.z), int(vecColor.w)), 0, 1.f));

	if (pObject) {

		for (float angle = 0.0f; angle < (M_PI * 2.0f); angle += step)
		{
			Vector3D worldStart = { flRadius * std::cos(angle) + vecOrigin.x, flRadius * std::sin(angle) + vecOrigin.y, vecOrigin.z };

			if (Vector2D screenStart; TVisuals::TUtils::WorldToScreen(worldStart, screenStart))
				pObject->vecPoints.add_back(Vector2D(screenStart.x, screenStart.y));
		}
	}
}

void TDirectXRender::Rectangle(Vector2D vecStart, Vector2D vecSize, Vector4D vecColor, bool Filled, float flRounding, int Thickness)
{
	stkRectangle.add_back(TRectObject(vecStart,
		Vector2D(vecStart.x + vecSize.x, vecStart.y + vecSize.y),
		vecColor,
		flRounding,
		ImDrawCornerFlags_None,
		Thickness,
		Filled));
}

void TDirectXRender::Line(Vector2D vecStart, Vector2D vecEnd, Vector4D vecColor, int Thickness) {
	  stkLine.add_back(TLineObject(vecStart, vecEnd, vecColor, Thickness));
}

void TDirectXRender::LockAndClear() {
	std::unique_lock<std::shared_mutex> lock(drawMutex);
	stkWorldCircle.clear();
	stkExchangeWorldCircle.clear();
	stkLine.clear();
	stkExchangeLine.clear();
	stkRectangle.clear();
	stkExchangeRectangle.clear();
	stkText.clear();
	stkExchangeText.clear();
	stkExchangeCircle.clear();
}

void LockAndClear() {
	
}

void TDirectXRender::SwapDrawData()
{
	std::unique_lock<std::shared_mutex> lock(drawMutex);

	if (!stkWorldCircle.size() && !stkLine.size() && !stkRectangle.size() && !stkText.size() && !stkCircle.size()) {
		stkExchangeWorldCircle.clear();
		stkExchangeLine.clear();
		stkExchangeRectangle.clear();
		stkExchangeText.clear();
		stkExchangeCircle.clear();
		return;
	}

	stkWorldCircle.copy_to(stkExchangeWorldCircle);
	stkWorldCircle.clear();
	
	stkLine.copy_to(stkExchangeLine);
	stkLine.clear();

	stkRectangle.copy_to(stkExchangeRectangle);
	stkRectangle.clear();

	stkText.copy_to(stkExchangeText);
	stkText.clear();

	stkCircle.copy_to(stkExchangeCircle);
	stkCircle.clear();
}
	

void TDirectXRender::OnDrawList(ImDrawList* pDrawList) {

	if (!pDrawList)
		return;

	if (!stkExchangeWorldCircle.size() && !stkExchangeLine.size() && !stkExchangeRectangle.size() && !stkExchangeText.size() && !stkExchangeCircle.size())
		return;

	if (!TDirectFonts::bInitialized)
		return;

	std::unique_lock<std::shared_mutex> lock(drawMutex);

	if (stkExchangeLine.size())
	{
		for (int Current = stkExchangeLine.size() - 1; Current >= 0; Current--)
		{
			auto Object = stkExchangeLine[Current];
			pDrawList->AddLine(ImVec2(Object->vecStart.x, Object->vecStart.y), ImVec2(Object->vecEnd.x, Object->vecEnd.y), ImColor((int)Object->colLine.x, (int)Object->colLine.y, (int)Object->colLine.z, (int)Object->colLine.w), Object->flThickness);
		}

	}

	if (stkExchangeCircle.size())
	{
		for (int Current = stkExchangeCircle.size() - 1; Current >= 0; Current--)
		{
			auto Object = stkExchangeCircle[Current];

			if (Object->bFilled)
				pDrawList->AddCircleFilled(ImVec2(Object->vecPos.x, Object->vecPos.y), Object->flRadius, ImColor((int)Object->vecColor.x, (int)Object->vecColor.y, (int)Object->vecColor.z, (int)Object->vecColor.w), Object->iSegments);
			else
				pDrawList->AddCircle(ImVec2(Object->vecPos.x, Object->vecPos.y), Object->flRadius, ImColor((int)Object->vecColor.x, (int)Object->vecColor.y, (int)Object->vecColor.z, (int)Object->vecColor.w), Object->iSegments, Object->flThickness);

		}
	}

	if (stkExchangeWorldCircle.size())
	{
		for (int Current = stkExchangeWorldCircle.size() - 1; Current >= 0; Current--)
		{
			auto Object = stkExchangeWorldCircle[Current];
			pDrawList->AddConvexPolyFilled((ImVec2*)Object->vecPoints.Base(), Object->vecPoints.size(), ImColor((int)Object->vecColor.x, (int)Object->vecColor.y, (int)Object->vecColor.z, (int)Object->vecColor.w));
			pDrawList->AddPolyline((ImVec2*)Object->vecPoints.Base(), Object->vecPoints.size(), ImColor((int)Object->vecOutlineColor.x, (int)Object->vecOutlineColor.y, (int)Object->vecOutlineColor.z, (int)Object->vecOutlineColor.w) , Object->iFlags, Object->flThickness);
		}

	}

	if (stkExchangeRectangle.size())
	{
		for (int Current = stkExchangeRectangle.size() - 1; Current >= 0; Current--)
		{
			auto Object = stkExchangeRectangle[Current];

			if (Object->bFilled)
			   pDrawList->AddRectFilled(ImVec2(Object->vecMin.x, Object->vecMin.y), ImVec2(Object->vecMax.x, Object->vecMax.y), ImColor((int)Object->colRect.x, (int)Object->colRect.y, (int)Object->colRect.z, (int)Object->colRect.w), Object->flRounding, Object->roundingCorners);
			else
			   pDrawList->AddRect(ImVec2(Object->vecMin.x, Object->vecMin.y), ImVec2(Object->vecMax.x, Object->vecMax.y), ImColor((int)Object->colRect.x, (int)Object->colRect.y, (int)Object->colRect.z, (int)Object->colRect.w), Object->flRounding, Object->roundingCorners, Object->flThickness);
		}

	}

	if (stkExchangeText.size())
	{
		for (int Current = stkExchangeText.size() - 1; Current >= 0; Current--)
		{
			auto Object = stkExchangeText[Current];

			if (Object->pFont) {
				if (Object->bOutline)
					pDrawList->AddTextOutline(Object->pFont, Object->pFont->FontSize,
						ImVec2(Object->vecPosition.x, Object->vecPosition.y),
						ImColor((int)Object->vecColor.x, (int)Object->vecColor.y, (int)Object->vecColor.z, (int)Object->vecColor.w),
						ImColor((int)Object->vecOutline.x, (int)Object->vecOutline.y, (int)Object->vecOutline.z, (int)Object->vecOutline.w), Object->szText.c_str());
				else if (Object->bShadow)
					pDrawList->AddTextShadow(Object->pFont, Object->pFont->FontSize,
						ImVec2(Object->vecPosition.x, Object->vecPosition.y),
						ImColor((int)Object->vecColor.x, (int)Object->vecColor.y, (int)Object->vecColor.z, (int)Object->vecColor.w),
						ImColor((int)Object->vecOutline.x, (int)Object->vecOutline.y, (int)Object->vecOutline.z, (int)Object->vecOutline.w), Object->szText.c_str());
				else
					pDrawList->AddText(Object->pFont, Object->pFont->FontSize, ImVec2(Object->vecPosition.x, Object->vecPosition.y), ImColor((int)Object->vecColor.x, (int)Object->vecColor.y, (int)Object->vecColor.z, (int)Object->vecColor.w), Object->szText.c_str());
			}

		}

	}

}