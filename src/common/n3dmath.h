#ifndef _N3DMATH_H_
#define _N3DMATH_H_

//	- n3dmath.h - 
//	Nuc3D 2 Header File
//	written by: John Tsiombikas (10 Sep 2002)
//	Last Modification: 14 April 2002
//	---------------------------
//	Mathematical stuff

#include <iostream>
#include <cmath>

#ifdef NUC3D_VER_DIRECT3D
#include "d3d8.h"	// Direct3D type definitions (D3DMATRIX)
#endif	//NUC3D_VER_DIRECT3D

// forward declarations
class Matrix4x4;
class Matrix3x3;
class Vector3;
class Vector2;
class Vector4;
class Quaternion;

// mathematical constants
const float Pi = 3.1415926535897932f;
const float TwoPi = 6.2831853071795865f;// Pi * 2.0f;
const float HalfPi = 1.5707963267948965f; //Pi * 0.5f;
const float QuarterPi = 0.7853981633974483f; //Pi * 0.25f;
const float ThreeQuartersPi = 2.3561944901923450f; //QuarterPi * 3.0f;

const float XSmallNumber = 1.e-8f;
const float SmallNumber = 1.e-4f;

// basis vectors
#define VECTOR3_I	(Vector3(1.0f, 0.0f, 0.0f))
#define VECTOR3_J	(Vector3(0.0f, 1.0f, 0.0f))
#define VECTOR3_K	(Vector3(0.0f, 0.0f, 1.0f))

#define VECTOR2_I	(Vector2(1.0f, 0.0f))
#define VECTOR2_J	(Vector2(0.0f, 1.0f))

// angle conversion
#define DEGTORAD(a)	(Pi * (a) / 180)
#define RADTODEG(a) ((a) * 180 / Pi)



// ------------- Vector3 class -------------

#ifdef NUC3D_VER_DIRECT3D	// if we are using Direct3D version

class Vector3 : public D3DVECTOR {
public:

#else	// not D3D

class Vector3 {
public:
	float x, y, z;

#endif	//NUC3D_VER_DIRECT3D

	Vector3();
	Vector3(float x, float y, float z);

	inline float DotProduct(const Vector3 &vec) const;
	inline Vector3 CrossProduct(const Vector3 &vec) const;

	inline Vector3 operator +(const Vector3 &vec) const;
	inline Vector3 operator -(const Vector3 &vec) const;
	inline Vector3 operator *(float scalar) const;
	inline Vector3 operator /(float scalar) const;
	inline void operator +=(const Vector3 &vec);
	inline void operator -=(const Vector3 &vec);
	inline void operator *=(float scalar);
	inline void operator /=(float scalar);
	inline Vector3 operator -() const;	// unary minus for inversion

	inline bool operator >(const Vector3 &vec) const;
	inline bool operator <(const Vector3 &vec) const;
	inline bool operator >(float len) const;
	inline bool operator <(float len) const;
	inline bool operator ==(const Vector3 &vec) const;
	inline bool operator ==(float len) const;

	inline operator Vector2() const;
	inline operator Vector4() const;

	inline float Length() const;
	inline float LengthSq() const;
	
	inline void Normalize();
	inline Vector3 Normalized() const;

	inline Vector3 Reflection(const Vector3 &normal) const;
	Vector3 Refraction(const Vector3 &normal, float FromIOR, float ToIOR) const;
	
	void Transform(const Matrix4x4 &mat);	// transform a vector using a transformation matrix
	void Transform(const Quaternion &quat);	// transform by a quaternion

	// Direct transformations on the vector
	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3 &axis, float angle);
	void Scale(float x, float y, float z);

	float &operator [](int index);

	friend std::ostream &operator <<(std::ostream &out, const Vector3 &vec);
};

inline float DotProduct(const Vector3 &vec1, const Vector3 &vec2);
inline Vector3 CrossProduct(const Vector3 &vec1, const Vector3 &vec2);

////////////////////// 4-dimensional vectors ////////////////////////////
class Vector4 {
public:
	float x, y, z, w;

	Vector4();
	Vector4(const Vector4 &vec);
	Vector4(const Vector3 &vec);	// create from a 3 dimensional vector setting w=1
	Vector4(float x, float y, float z, float w);

	float DotProduct(const Vector4 &vec) const;
	Vector4 CrossProduct(const Vector4 &vec1, const Vector4 &vec2) const;

	Vector4 operator +(const Vector4 &vec) const;
	Vector4 operator -(const Vector4 &vec) const;
	Vector4 operator *(float scalar) const;
	Vector4 operator /(float scalar) const;
	void operator +=(const Vector4 &vec);
	void operator -=(const Vector4 &vec);
	void operator *=(float scalar);
	void operator /=(float scalar);
	Vector4 operator -() const;	// unary minus for inversion

	bool operator >(const Vector4 &vec) const;
	bool operator <(const Vector4 &vec) const;
	bool operator >(float len) const;
	bool operator <(float len) const;
	bool operator ==(const Vector4 &vec) const;
	bool operator ==(float len) const;

	operator Vector3() const;

	float Length() const;
	float LengthSq() const;
	
	void Normalize();
	Vector4 Normalized() const;

	void Transform(const Matrix4x4 &mat);	// transform a vector using a transformation matrix

	// Direct transformations on the vector
	void Translate(float x, float y, float z, float w);
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3 &axis, float angle);
	void Scale(float x, float y, float z, float w);

	float &operator [](int index);

	friend std::ostream &operator <<(std::ostream &out, const Vector3 &vec);
};

float DotProduct(const Vector4 &vec1, const Vector4 &vec2);
Vector4 CrossProduct(const Vector4 &vec1, const Vector4 &vec2, const Vector4 &vec3);

///////////////////////////

class Vector2 {
public:
	float x, y;

	Vector2();
	Vector2(const Vector2 &vec);
	Vector2(float x, float y);

	float DotProduct(const Vector2 &vec) const;
	//Vector2 CrossProduct(const Vector2 &vec) const;

	Vector2 operator +(const Vector2 &vec) const;
	Vector2 operator -(const Vector2 &vec) const;
	Vector2 operator *(float scalar) const;
	Vector2 operator /(float scalar) const;
	void operator +=(const Vector2 &vec);
	void operator -=(const Vector2 &vec);
	void operator *=(float scalar);
	void operator /=(float scalar);
	Vector2 operator -() const;	// unary minus for inversion

	bool operator >(const Vector2 &vec) const;
	bool operator <(const Vector2 &vec) const;
	bool operator >(float len) const;
	bool operator <(float len) const;
	bool operator ==(const Vector2 &vec) const;
	bool operator ==(float len) const;

	operator Vector3() const;
	    
	float Length() const;
	float LengthSq() const;
	
	void Normalize();
	Vector2 Normalized() const;

	Vector2 Reflection(const Vector2 &normal) const;
	Vector2 Refraction(const Vector2 &normal, float FromIOR, float ToIOR) const;
	
	void Transform(const Matrix3x3 &mat);	// transform a vector using a transformation matrix

	// Direct transformations on the vector
	void Translate(float x, float y);
	void Rotate(float angle);
	void Scale(float x, float y);

	float &operator [](int index);

	friend std::ostream &operator <<(std::ostream &out, const Vector2 &vec);
};

float DotProduct(const Vector3 &vec1, const Vector3 &vec2);


struct Vector2i {
	int x, y;

	Vector2i(int x=0, int y=0) {this->x = x; this->y = y;}
};


////////////////// Quaternion ///////////////////////

class Quaternion {
public:
	float s;
	Vector3 v;

	Quaternion();
	Quaternion(float s, const Vector3 &v);
	Quaternion(float s, float x, float y, float z);	

	Quaternion operator +(const Quaternion &quat) const;
	Quaternion operator -(const Quaternion &quat) const;
	Quaternion operator -() const;
	Quaternion operator *(const Quaternion &quat) const;
	
	void operator +=(const Quaternion &quat);
	void operator -=(const Quaternion &quat);
	void operator *=(const Quaternion &quat);

	void ResetIdentity();

	Quaternion Conjugate() const;

	float Length() const;
	float LengthSq() const;
	
	void Normalize();
	Quaternion Normalized() const;

	Quaternion Inverse() const;

	void SetRotation(const Vector3 &axis, float angle);
	void Rotate(const Vector3 &axis, float angle);

	Matrix3x3 GetRotationMatrix() const;
};

	

////////////////////////// Matrices //////////////////////////////

#ifdef NUC3D_VER_DIRECT3D	// if we are using Direct3D version

class Matrix4x4 : public D3DMATRIX {
public:

#else	// Software or OpenGL version

class Matrix4x4 {
public:
	float m[4][4];

#endif	//NUC3D_VER_DIRECT3D

	Matrix4x4();
	Matrix4x4(const Matrix4x4 &mat);
	Matrix4x4(const Matrix3x3 &mat);
	Matrix4x4(	float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33 );

	// basic operations on matrices
	Matrix4x4 operator +(const Matrix4x4 &mat) const;
	Matrix4x4 operator -(const Matrix4x4 &mat) const;
	Matrix4x4 operator *(const Matrix4x4 &mat) const;
	Matrix4x4 operator *(float scalar) const;
	void operator +=(const Matrix4x4 &mat);
	void operator -=(const Matrix4x4 &mat);
	void operator *=(const Matrix4x4 &mat);
	void operator *=(float scalar);

	void ResetIdentity();

	// concatenate various transformation matrices with our current matrix
	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3 &axis, float rads);
	void Scale(float x, float y, float z);

	// set the matrix to a specific transformation matrix
	void SetTranslation(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotation(const Vector3 &axis, float angle);
	void SetScaling(float x, float y, float z);

	// row and column accessors
	void SetColumnVector(const Vector4 &vec, int columnindex);
	void SetRowVector(const Vector4 &vec, int rowindex);
	Vector4 GetColumnVector(int columnindex) const;
	Vector4 GetRowVector(int rowindex) const;

	// other operations on matrices
	void Transpose();
	Matrix4x4 Transposed() const;

	float Determinant() const;
	Matrix4x4 Adjoint() const;
	Matrix4x4 Inverse() const;
};


////////////////// Matrix3x3 //////////////////
class Matrix3x3 {
public:
	float m[3][3];

	Matrix3x3();
	Matrix3x3(const Matrix3x3 &mat);
	Matrix3x3(	float m00, float m01, float m02,
				float m10, float m11, float m12,
				float m20, float m21, float m22 );

	// basic operations on matrices
	Matrix3x3 operator +(const Matrix3x3 &mat) const;
	Matrix3x3 operator -(const Matrix3x3 &mat) const;
	Matrix3x3 operator *(const Matrix3x3 &mat) const;
	Matrix3x3 operator *(float scalar) const;
	void operator +=(const Matrix3x3 &mat);
	void operator -=(const Matrix3x3 &mat);
	void operator *=(const Matrix3x3 &mat);
	void operator *=(float scalar);

	void ResetIdentity();

	// concatenate various transformation matrices with our current matrix
	void Translate(float x, float y);
	void Rotate(float angle);
	void Scale(float x, float y);
	
	// set the matrix to a specific transformation matrix
	void SetTranslation(float x, float y);
	void SetRotation(float angle);
	void SetScaling(float x, float y);

	// row and column accessors
	void SetColumnVector(const Vector3 &vec, int columnindex);
	void SetRowVector(const Vector3 &vec, int rowindex);
	Vector3 GetColumnVector(int columnindex) const;
	Vector3 GetRowVector(int rowindex) const;

	// other operations on matrices
	void Transpose();
	Matrix3x3 Transposed() const;

	void OrthoNormalize();
	Matrix3x3 OrthoNormalized();

	//float Determinant() const; // NYI
	//Matrix3x3 Adjoint() const;
	//Matrix3x3 Inverse() const;
};

///////////////// ray /////////////////

class Ray {
public:
	Vector3 Origin;
	Vector3 Direction;
	float Energy;
	float CurrentIOR;

	Ray();
	Ray(const Vector3 &origin, const Vector3 &direction);
};

// a coordinate system basis
class Base {
public:
	Vector3 i, j, k;

	Base();
	Base(const Vector3 &i, const Vector3 &j, const Vector3 &k);
	Base(const Vector3 &dir, bool LeftHanded=true);

	void Rotate(float x, float y, float z);
	void Rotate(const Vector3 &axis, float angle);
	void Rotate(const Matrix4x4 &mat);
	void Rotate(const Quaternion &quat);

	Matrix3x3 CreateRotationMatrix() const;
};



float frand(float range);


#include "n3dmath.inl"

#endif	// _N3DMATH_H_