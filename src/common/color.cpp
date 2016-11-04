#include <cmath>
#include "color.h"

template <class T>
const T &clamp(const T &val, const T &low, const T &high) {
	if(val > high) return high;
	if(val < low) return low;
	return val;
}

Color::Color(float_t intensity) {
	r = g = b = clamp<float_t>(intensity, 0.0f, 1.0f);
	a = 1.0f;
}

Color::Color(float_t r, float_t g, float_t b, float_t a) {
	this->r = clamp<float_t>(r, 0.0f, 1.0f);
	this->g = clamp<float_t>(g, 0.0f, 1.0f);
	this->b = clamp<float_t>(b, 0.0f, 1.0f);
	this->a = clamp<float_t>(a, 0.0f, 1.0f);
}

Color::Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
	this->r = (float_t)clamp<unsigned int>(r, 0, 255) / 255.0f;
	this->g = (float_t)clamp<unsigned int>(g, 0, 255) / 255.0f;
	this->b = (float_t)clamp<unsigned int>(b, 0, 255) / 255.0f;
	this->a = (float_t)clamp<unsigned int>(a, 0, 255) / 255.0f;
}

Color::Color(int r, int g, int b, int a) {
	this->r = (float_t)clamp<int>(r, 0, 255) / 255.0f;
	this->g = (float_t)clamp<int>(g, 0, 255) / 255.0f;
	this->b = (float_t)clamp<int>(b, 0, 255) / 255.0f;
	this->a = (float_t)clamp<int>(a, 0, 255) / 255.0f;
}

Color::Color(long pcol) {
	a = (float_t)(((dword)pcol >> 24) & 0xff) / 255.0f;
	r = (float_t)(((dword)pcol >> 16) & 0xff) / 255.0f;
	g = (float_t)(((dword)pcol >> 8) & 0xff) / 255.0f;
	b = (float_t)((dword)pcol & 0xff) / 255.0f;
}

Color::Color(dword pcol) {
	a = (float_t)((pcol >> 24) & 0xff) / 255.0f;
	r = (float_t)((pcol >> 16) & 0xff) / 255.0f;
	g = (float_t)((pcol >> 8) & 0xff) / 255.0f;
	b = (float_t)(pcol & 0xff) / 255.0f;
}

dword Color::GetPacked32() const {
	return	(((dword)(a * 255.0f) << 24) & 0xff000000) | 
			(((dword)(r * 255.0f) << 16) & 0x00ff0000) | 
			(((dword)(g * 255.0f) << 8) & 0x0000ff00) | 
			((dword)(b * 255.0f) & 0x000000ff);
}

word Color::GetPacked16() const {
	return (word)(r * 32.0f) << 11 | (word)(g * 64.0f) << 5 | (word)(b * 32.0f);
}

word Color::GetPacked15() const {
	return (word)a << 15 | (word)(r * 32.0f) << 10 | (word)(g * 32.0f) << 5 | (word)(b * 32.0f);
}

#ifndef HalfPi
const float_t HalfPi = 1.5707963268f;
#endif	// HalfPi

byte Color::GetNearest8(const byte **pal) const {
	float_t Score[256];
	for(int i=0; i<256; i++) {
		Color palcol = Color(pal[i][0], pal[i][1], pal[i][2]);
		float_t NearR = (float_t)cos(fabs(r - palcol.r) * HalfPi);
		float_t NearG = (float_t)cos(fabs(g - palcol.g) * HalfPi);
		float_t NearB = (float_t)cos(fabs(b - palcol.b) * HalfPi);
		Score[i] = NearR + NearG + NearB;
	}

	int nearest = 0;
	for(int i=0; i<256; i++) {
		if(Score[i] > Score[nearest]) nearest = i;
	}
	return nearest;
}

Color Color::operator +(const Color &col) const {
	return Color(r + col.r, g + col.g, b + col.b, a + col.a);
}

Color Color::operator -(const Color &col) const {
	return Color(r - col.r, g - col.g, b - col.b, a - col.a);
}

Color Color::operator *(const Color &col) const {
	return Color(r * col.r, g * col.g, b * col.b, a * col.a);
}
	
Color Color::operator *(float_t scalar) const {
	return Color(r * scalar, g * scalar, b * scalar, a);
}


void Color::operator +=(const Color &col) {
	*this = Color(r + col.r, g + col.g, b + col.b, a + col.a);
}

void Color::operator -=(const Color &col) {
	*this = Color(r - col.r, g - col.g, b - col.b, a - col.a);
}

void Color::operator *=(const Color &col) {
	*this = Color(r * col.r, g * col.g, b * col.b, a * col.a);
}

void Color::operator *=(float_t scalar) {
	*this = Color(r * scalar, g * scalar, b * scalar, a);
}

Color BlendColors(const Color &c1, const Color &c2, float_t t) {
	float_t r = c1.r + (c2.r - c1.r) * t;
	float_t g = c1.g + (c2.g - c1.g) * t;
	float_t b = c1.b + (c2.b - c1.b) * t;
	return Color(r, g, b);
}
