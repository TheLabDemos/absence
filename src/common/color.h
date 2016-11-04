#ifndef _COLORS_H_
#define _COLORS_H_

#include <cmath>
#include "typedefs.h"

#ifdef _MSC_VER
#ifdef DOUBLE_PRECISION_MATH
typedef double float_t;
#else
typedef float float_t;
#endif	// DOUBLE_PRECISION_MATH
#endif	// _MSC_VER

#ifdef NUC3D_VER_DIRECT3D

#include "d3d8.h"

class Color : public D3DCOLORVALUE {
public:

#else

class Color {
public:
	float_t r, g, b, a;

#endif	// NUC3D_VER_DIRECT3D

	Color(float_t intensity = 1.0f);
	Color(float_t r, float_t g, float_t b, float_t a = 1.0f);
	Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255);
	Color(int r, int g, int b, int a = 255);
	Color(dword pcol);
	Color(long pcol);

	Color operator +(const Color &col) const;
	Color operator -(const Color &col) const;
	Color operator *(const Color &col) const;
	Color operator *(float_t scalar) const;

	void operator +=(const Color &col);
	void operator -=(const Color &col);
	void operator *=(const Color &col);
	void operator *=(float_t scalar);

	dword GetPacked32() const;
	word GetPacked16() const;
	word GetPacked15() const;
	byte GetNearest8(const byte **pal) const;
};

Color BlendColors(const Color &c1, const Color &c2, float_t t);

#endif	// _COLORS_H_
