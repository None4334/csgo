#pragma once

enum
{
	COLOR_R = 0,
	COLOR_G = 1,
	COLOR_B = 2,
	COLOR_A = 3
};

struct ColorRGBExp32
{
	std::uint8_t r, g, b;
	char exponent;
};

class TColor {
public:
	float flColors[4] = { 1.f,1.f,1.f,1.f };
};

class Color
{
public:

	Color(float flRed,float flGreen,float flBlue,float flAlpha)
	{
		arrColor[0] = flRed;
		arrColor[1] = flGreen;
		arrColor[2] = flBlue;
		arrColor[3] = flAlpha;
	}

	template <std::size_t N>
	float Base() const
	{
		return arrColor[N] / 255.f;
	}

	std::array<float, 4U> BaseAlpha() const
	{
		std::array<float, 4U> arrBaseColor = { };
		arrBaseColor[COLOR_R] = this->Base<COLOR_R>();
		arrBaseColor[COLOR_G] = this->Base<COLOR_G>();
		arrBaseColor[COLOR_B] = this->Base<COLOR_B>();
		arrBaseColor[COLOR_A] = this->Base<COLOR_A>();
		return arrBaseColor;
	}
private:
	std::array<int, 4U> arrColor;
};
