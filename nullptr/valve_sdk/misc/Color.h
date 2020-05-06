#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Color
{
public:
	Color();
	Color(int _r, int _g, int _b);
	Color(int _r, int _g, int _b, int _a);
	Color(float _r, float _g, float _b) : Color(_r, _g, _b, 1.0f) {}
	Color(float _r, float _g, float _b, float _a)
		: Color(
			static_cast<int>(_r * 255.0f),
			static_cast<int>(_g * 255.0f),
			static_cast<int>(_b * 255.0f),
			static_cast<int>(_a * 255.0f))
	{
	}

	Color(Color cl, int a)
	{
		SetColor(cl.r(), cl.g(), cl.b(), a);
	}

	Color(int* cl, int a)
	{
		SetColor(cl[0], cl[1], cl[3], a);
	}

	Color(float* rgb, int a)
	{
		SetColor(rgb[0], rgb[1], rgb[2], a);
	}

	explicit Color(float* rgb) : Color(rgb[0], rgb[1], rgb[2], 1.0f) {}
	explicit Color(unsigned long argb)
	{
		_CColor[2] = (unsigned char)((argb & 0x000000FF) >> (0 * 8));
		_CColor[1] = (unsigned char)((argb & 0x0000FF00) >> (1 * 8));
		_CColor[0] = (unsigned char)((argb & 0x00FF0000) >> (2 * 8));
		_CColor[3] = (unsigned char)((argb & 0xFF000000) >> (3 * 8));
	}

	void    SetRawColor(int color32);
	int     GetRawColor() const;
	void    SetColor(int _r, int _g, int _b, int _a = 0);
	void    SetColor(float _r, float _g, float _b, int _a = 0);
	void    SetColor(float _r, float _g, float _b, float _a = 0);
	void    GetColor(int& _r, int& _g, int& _b, int& _a) const;

	std::string GetNormalnijHexColor() const;

	int r() const { return _CColor[0]; }
	int g() const { return _CColor[1]; }
	int b() const { return _CColor[2]; }
	int a() const { return _CColor[3]; }

	unsigned char& operator[](int index)
	{
		return _CColor[index];
	}
	const unsigned char& operator[](int index) const
	{
		return _CColor[index];
	}

	bool operator==(const Color& rhs) const;
	bool operator!=(const Color& rhs) const;
	Color& operator=(const Color& rhs);
	
	Color& FromHSV(float h, float s, float v)
	{
		float colOut[3]{ };
		if (s == 0.0f)
		{
			_CColor[0] = _CColor[1] = _CColor[2] = static_cast<int>(v * 255);
			return *this;
		}

		h = std::fmodf(h, 1.0f) / (60.0f / 360.0f);
		int   i = static_cast<int>(h);
		float f = h - static_cast<float>(i);
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0:
			colOut[0] = v;
			colOut[1] = t;
			colOut[2] = p;
			break;
		case 1:
			colOut[0] = q;
			colOut[1] = v;
			colOut[2] = p;
			break;
		case 2:
			colOut[0] = p;
			colOut[1] = v;
			colOut[2] = t;
			break;
		case 3:
			colOut[0] = p;
			colOut[1] = q;
			colOut[2] = v;
			break;
		case 4:
			colOut[0] = t;
			colOut[1] = p;
			colOut[2] = v;
			break;
		case 5: default:
			colOut[0] = v;
			colOut[1] = p;
			colOut[2] = q;
			break;
		}

		_CColor[0] = static_cast<int>(colOut[0] * 255);
		_CColor[1] = static_cast<int>(colOut[1] * 255);
		_CColor[2] = static_cast<int>(colOut[2] * 255);
		return *this;
	}

	static Color FromHSB(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1)
		{
			return Color(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 2)
		{
			return Color(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 3)
		{
			return Color(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		else if (h < 4)
		{
			return Color(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else if (h < 5)
		{
			return Color(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else
		{
			return Color(
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255),
				(unsigned char)(q * 255)
			);
		}
	}

	static Color Black;
	static Color White;
	static Color Red;
	static Color Green;
	static Color Blue;

	unsigned char _CColor[4];

	float save_color[4];
};