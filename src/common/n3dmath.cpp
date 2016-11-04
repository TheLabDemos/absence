#include "n3dmath.h"

#define fsin (float)sin
#define fcos (float)cos

float frand(float range) {
	return ((float)rand() / (float)RAND_MAX) * range;
}

Vector3::Vector3() {
	x = y = z = 0.0f;
}

Vector3::Vector3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}
/*  inlined
float Vector3::DotProduct(const Vector3 &vec) const {
	return x * vec.x + y * vec.y + z * vec.z;
}

float DotProduct(const Vector3 &vec1, const Vector3 &vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

Vector3 Vector3::CrossProduct(const Vector3 &vec) const {
	return Vector3(y * vec.z - z * vec.y,  z * vec.x - x * vec.z,  x * vec.y - y * vec.x);
}

Vector3 CrossProduct(const Vector3 &vec1, const Vector3 &vec2) {
	return Vector3(vec1.y * vec2.z - vec1.z * vec2.y,  vec1.z * vec2.x - vec1.x * vec2.z,  vec1.x * vec2.y - vec1.y * vec2.x);
}

Vector3 Vector3::operator +(const Vector3 &vec) const {
	return Vector3(x + vec.x, y + vec.y, z + vec.z);
}

Vector3 Vector3::operator -(const Vector3 &vec) const {
	return Vector3(x - vec.x, y - vec.y, z - vec.z);
}

Vector3 Vector3::operator *(float scalar) const {
	return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator /(float scalar) const {
	return Vector3(x / scalar, y / scalar, z / scalar);
}

void Vector3::operator +=(const Vector3 &vec) {
	x += vec.x;
	y += vec.y;
	z += vec.z;
}

void Vector3::operator -=(const Vector3 &vec) {
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
}

void Vector3::operator *=(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

void Vector3::operator /=(float scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
}

Vector3 Vector3::operator -() const {
	return Vector3(-x, -y, -z);
}

bool Vector3::operator >(const Vector3 &vec) const {
	return LengthSq() > vec.LengthSq();
}

bool Vector3::operator <(const Vector3 &vec) const {
	return LengthSq() < vec.LengthSq();
}

bool Vector3::operator >(float len) const {
	return LengthSq() > len;
}

bool Vector3::operator <(float len) const {
	return LengthSq() < len;
}

bool Vector3::operator ==(const Vector3 &vec) const {
	return ((*this - vec).Length() < XSmallNumber);
}

bool Vector3::operator ==(float len) const {
	return ((this->Length() - len) < XSmallNumber);
}

Vector3::operator Vector2() const {
	return Vector2(x, y);
}

Vector3::operator Vector4() const {
	return Vector4(x, y, z, 1.0f);
}


float Vector3::Length() const {
	return (float)sqrt(x*x + y*y + z*z);
}

float Vector3::LengthSq() const {
	return x*x + y*y + z*z;
}

void Vector3::Normalize() {
	float len = (float)sqrt(x*x + y*y + z*z);
	x /= len;
	y /= len;
	z /= len;
}

Vector3 Vector3::Normalized() const {
	float len = (float)sqrt(x*x + y*y + z*z);
	return Vector3(x / len, y / len, z / len);
}

Vector3 Vector3::Reflection(const Vector3 &normal) const {
	return normal * this->DotProduct(normal) * 2.0f - *this;
}
*/
Vector3 Vector3::Refraction(const Vector3 &normal, float FromIOR, float ToIOR) const {
	float m = FromIOR / ToIOR;
	Vector3 dir = *this;
	dir.Normalize();
	float CosAngleIncoming = dir.DotProduct(normal);
	float CosAngleRefr = (1.0f / (m*m)) * (float)sqrt(1.0f - m*m * (1 - CosAngleIncoming * CosAngleIncoming));

	return dir * m - normal * (CosAngleRefr + m * CosAngleIncoming);
}

void Vector3::Transform(const Matrix4x4 &mat) {
	// assume row vectors
	float nx = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + mat.m[3][0];
	float ny = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + mat.m[3][1];
	z  = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + mat.m[3][2];
	x = nx;
	y = ny;
}

void Vector3::Transform(const Quaternion &quat) {
	Quaternion vq(0.0f, *this);
	vq = quat * vq * quat.Inverse();
	*this = vq.v;
}

// direct transformations

void Vector3::Translate(float x, float y, float z) {
	this->x += x;
	this->y += y;
	this->z += z;
}

void Vector3::Rotate(float x, float y, float z) {
	
	Matrix4x4 xform;
	xform.SetRotation(x, y, z);
	
	Transform(xform);
}

void Vector3::Rotate(const Vector3 &axis, float angle) {

	Matrix4x4 xform;
	xform.SetRotation(axis, angle);

	Transform(xform);
}

void Vector3::Scale(float x, float y, float z) {
	this->x *= x;
	this->y *= y;
	this->z *= z;
}

float &Vector3::operator [](int index) {
	return !index ? x : index == 1 ? y : z;
}

std::ostream &operator <<(std::ostream &out, const Vector3 &vec) {
	out << vec.x << ", " << vec.y << ", " << vec.z;
	return out;
}

// ------------- Vector4 implementation ---------------

Vector4::Vector4() {
	x = y = z = 0.0f;
}

Vector4::Vector4(const Vector4 &vec) {
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
}

Vector4::Vector4(const Vector3 &vec) {
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = 1.0f;
}

Vector4::Vector4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

float Vector4::DotProduct(const Vector4 &vec) const {
	return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

float DotProduct(const Vector4 &vec1, const Vector4 &vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
}

Vector4 Vector4::CrossProduct(const Vector4 &vec1, const Vector4 &vec2) const {
    float  A, B, C, D, E, F;       // Intermediate Values
    Vector4 result;

    // Calculate intermediate values.
    A = (vec1.x * vec2.y) - (vec1.y * vec2.x);
    B = (vec1.x * vec2.z) - (vec1.z * vec2.x);
    C = (vec1.x * vec2.w) - (vec1.w * vec2.x);
    D = (vec1.y * vec2.z) - (vec1.z * vec2.y);
    E = (vec1.y * vec2.w) - (vec1.w * vec2.y);
    F = (vec1.z * vec2.w) - (vec1.w * vec2.z);

    // Calculate the result-vector components.
    result.x =   (y * F) - (z * E) + (w * D);
    result.y = - (x * F) + (z * C) - (w * B);
    result.z =   (x * E) - (y * C) + (w * A);
    result.w = - (x * D) + (y * B) - (z * A);
    return result;
}

Vector4 CrossProduct(const Vector4 &vec1, const Vector4 &vec2, const Vector4 &vec3) {
    float  A, B, C, D, E, F;       // Intermediate Values
    Vector4 result;

    // Calculate intermediate values.
    A = (vec2.x * vec3.y) - (vec2.y * vec3.x);
    B = (vec2.x * vec3.z) - (vec2.z * vec3.x);
    C = (vec2.x * vec3.w) - (vec2.w * vec3.x);
    D = (vec2.y * vec3.z) - (vec2.z * vec3.y);
    E = (vec2.y * vec3.w) - (vec2.w * vec3.y);
    F = (vec2.z * vec3.w) - (vec2.w * vec3.z);

    // Calculate the result-vector components.
    result.x =   (vec1.y * F) - (vec1.z * E) + (vec1.w * D);
    result.y = - (vec1.x * F) + (vec1.z * C) - (vec1.w * B);
    result.z =   (vec1.x * E) - (vec1.y * C) + (vec1.w * A);
    result.w = - (vec1.x * D) + (vec1.y * B) - (vec1.z * A);
    return result;
}

Vector4 Vector4::operator +(const Vector4 &vec) const {
	return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
}

Vector4 Vector4::operator -(const Vector4 &vec) const {
	return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
}

Vector4 Vector4::operator *(float scalar) const {
	return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4 Vector4::operator /(float scalar) const {
	return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

void Vector4::operator +=(const Vector4 &vec) {
	x += vec.x;
	y += vec.y;
	z += vec.z;
	w += vec.w;
}

void Vector4::operator -=(const Vector4 &vec) {
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	w -= vec.w;
}

void Vector4::operator *=(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}

void Vector4::operator /=(float scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
}

Vector4 Vector4::operator -() const {
	return Vector4(-x, -y, -z, -w);
}


bool Vector4::operator >(const Vector4 &vec) const {
	return LengthSq() > vec.LengthSq();
}

bool Vector4::operator <(const Vector4 &vec) const {
	return LengthSq() < vec.LengthSq();
}

bool Vector4::operator >(float len) const {
	return LengthSq() > len;
}

bool Vector4::operator <(float len) const {
	return LengthSq() < len;
}

bool Vector4::operator ==(const Vector4 &vec) const {
	return ((*this - vec).Length() < XSmallNumber);
}

bool Vector4::operator ==(float len) const {
	return ((this->Length() - len) < XSmallNumber);
}

Vector4::operator Vector3() const {
	return Vector3(x, y, z);
}


float Vector4::Length() const {
	return (float)sqrt(x*x + y*y + z*z + w*w);
}

float Vector4::LengthSq() const {
	return x*x + y*y + z*z + w*w;
}

void Vector4::Normalize() {
	float len = (float)sqrt(x*x + y*y + z*z + w*w);
	x /= len;
	y /= len;
	z /= len;
	w /= len;
}

Vector4 Vector4::Normalized() const {
	float len = (float)sqrt(x*x + y*y + z*z + w*w);
	return Vector4(x / len, y / len, z / len, w / len);
}

void Vector4::Transform(const Matrix4x4 &mat) {
	// assume row vectors
	float nx = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + w * mat.m[3][0];
	float ny = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + w * mat.m[3][1];
	float nz = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + w * mat.m[3][2];
	w = x * mat.m[0][3] + y * mat.m[1][3] + z * mat.m[2][3] + w * mat.m[3][3];
	x = nx;
	y = ny;
	z = nz;
}


// Direct transformations on the vector
void Vector4::Translate(float x, float y, float z, float w) {
	x += x;
	y += y;
	z += z;
	w += w;
}

void Vector4::Rotate(float x, float y, float z) {
	Matrix4x4 xform;
	xform.SetRotation(x, y, z);
	Transform(xform);
}

void Vector4::Rotate(const Vector3 &axis, float angle) {
	Matrix4x4 xform;
	xform.SetRotation(axis, angle);
	Transform(xform);
}

void Vector4::Scale(float x, float y, float z, float w) {
	this->x *= x;
	this->y *= y;
	this->z *= z;
	this->w *= w;
}

float &Vector4::operator [](int index) {
	return !index ? x : index == 1 ? y : index == 2 ? z : w;
}

std::ostream &operator <<(std::ostream &out, const Vector4 &vec) {
	out << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
	return out;
}

// ------------- Vector2 implementation ---------------

Vector2::Vector2() {
	x = y = 0.0f;
}

Vector2::Vector2(const Vector2 &vec) {
	x = vec.x;
	y = vec.y;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

float Vector2::DotProduct(const Vector2 &vec) const {
	return x * vec.x + y * vec.y;
}

float DotProduct(const Vector2 &vec1, const Vector2 &vec2) {
	return vec1.x * vec2.x + vec1.y + vec2.y;
}

Vector2 Vector2::operator +(const Vector2 &vec) const {
	return Vector2(x + vec.x, y + vec.y);
}

Vector2 Vector2::operator -(const Vector2 &vec) const {
	return Vector2(x - vec.x, y - vec.y);
}

Vector2 Vector2::operator *(float scalar) const {
	return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator /(float scalar) const {
	return Vector2(x / scalar, y / scalar);
}

void Vector2::operator +=(const Vector2 &vec) {
	x += vec.x;
	y += vec.y;
}

void Vector2::operator -=(const Vector2 &vec) {
	x -= vec.x;
	y -= vec.y;
}

void Vector2::operator *=(float scalar) {
	x *= scalar;
	y *= scalar;
}

void Vector2::operator /=(float scalar) {
	x /= scalar;
	y /= scalar;
}

Vector2 Vector2::operator -() const {
	return Vector2(-x, -y);
}

bool Vector2::operator >(const Vector2 &vec) const {
	return LengthSq() > vec.LengthSq();
}

bool Vector2::operator <(const Vector2 &vec) const {
	return LengthSq() < vec.LengthSq();
}

bool Vector2::operator >(float len) const {
	return LengthSq() > len;
}

bool Vector2::operator <(float len) const {
	return LengthSq() < len;
}

bool Vector2::operator ==(const Vector2 &vec) const {
	return ((*this - vec).Length() < XSmallNumber);
}

bool Vector2::operator ==(float len) const {
	return ((this->Length() - len) < XSmallNumber);
}

Vector2::operator Vector3() const {
	return Vector3(x, y, 1.0f);
}

float Vector2::Length() const {
	return (float)sqrt(x * x + y * y);
}

float Vector2::LengthSq() const {
	return x * x + y * y;
}
	
void Vector2::Normalize() {
	float len = (float)sqrt(x * x + y * y);
	x /= len;
	y /= len;
}

Vector2 Vector2::Normalized() const {
	float len = (float)sqrt(x * x + y * y);
	return Vector2(x / len, y / len);
}

//Vector2 Vector2::Reflection(const Vector2 &normal) const;
//Vector2 Vector2::Refraction(const Vector2 &normal, float FromIOR, float ToIOR) const;
	
void Vector2::Transform(const Matrix3x3 &mat) {
	float nx = x * mat.m[0][0] + y * mat.m[1][0] + mat.m[2][0];
	y = x * mat.m[0][1] + y * mat.m[1][1] + mat.m[2][1];
	x = nx;
}

void Vector2::Translate(float x, float y) {
	this->x += x;
	this->y += y;
}

void Vector2::Rotate(float angle) {
	Matrix3x3 xform;
	xform.SetRotation(angle);

	Transform(xform);
}

void Vector2::Scale(float x, float y) {
	this->x *= x;
	this->y *= y;
}

float &Vector2::operator [](int index) {
	return !index ? x : y;
}

std::ostream &operator <<(std::ostream &out, const Vector2 &vec) {
	out << vec.x << ", " << vec.y;
	return out;
}


// --------------- Quaternion implementation ---------------

Quaternion::Quaternion() {
	s = 1.0f;
	v.x = v.y = v.z = 0.0f;
}

Quaternion::Quaternion(float s, float x, float y, float z) {
	v.x = x;
	v.y = y;
	v.z = z;
	this->s = s;
}

Quaternion::Quaternion(float s, const Vector3 &v) {
	this->s = s;
	this->v = v;
}

Quaternion Quaternion::operator +(const Quaternion &quat) const {
	return Quaternion(s + quat.s, v + quat.v);
}

Quaternion Quaternion::operator -(const Quaternion &quat) const {
	return Quaternion(s - quat.s, v - quat.v);
}

Quaternion Quaternion::operator -() const {
	return Quaternion(-s, -v);
}

// Quaternion Multiplication:
// Q1*Q2 = [s1*s2 - v1.v2,  s1*v2 + s2*v1 + v1(x)v2]
Quaternion Quaternion::operator *(const Quaternion &quat) const {
	Quaternion newq;	
	newq.s = s * quat.s - DotProduct(v, quat.v);
	newq.v = quat.v * s + v * quat.s + CrossProduct(v, quat.v);	
	return newq;
}

void Quaternion::operator +=(const Quaternion &quat) {
	*this = Quaternion(s + quat.s, v + quat.v);
}

void Quaternion::operator -=(const Quaternion &quat) {
	*this = Quaternion(s - quat.s, v - quat.v);
}

void Quaternion::operator *=(const Quaternion &quat) {
	*this = *this * quat;
}

void Quaternion::ResetIdentity() {
	s = 1.0f;
	v.x = v.y = v.z = 0.0f;
}

Quaternion Quaternion::Conjugate() const {
	return Quaternion(s, -v);
}

float Quaternion::Length() const {
	return (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z + s*s);
}

// Q * ~Q = ||Q||^2
float Quaternion::LengthSq() const {
	return v.x*v.x + v.y*v.y + v.z*v.z + s*s;
}

void Quaternion::Normalize() {
	float len = (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z + s*s);
	v.x /= len;
	v.y /= len;
	v.z /= len;
	s /= len;
}

Quaternion Quaternion::Normalized() const {
	Quaternion nq = *this;
	float len = (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z + s*s);
	nq.v.x /= len;
	nq.v.y /= len;
	nq.v.z /= len;
	nq.s /= len;
	return nq;
}

// Quaternion Inversion: Q^-1 = ~Q / ||Q||^2
Quaternion Quaternion::Inverse() const {
	Quaternion inv = Conjugate();
	float lensq = LengthSq();
	inv.v /= lensq;
	inv.s /= lensq;

	return inv;
}


void Quaternion::SetRotation(const Vector3 &axis, float angle) {
	float HalfAngle = angle / 2.0f;
	s = cosf(HalfAngle);
	v = axis * sinf(HalfAngle);
}

void Quaternion::Rotate(const Vector3 &axis, float angle) {
	Quaternion q;
	float HalfAngle = angle / 2.0f;
	q.s = cosf(HalfAngle);
	q.v = axis * sinf(HalfAngle);

	*this *= q;
}


Matrix3x3 Quaternion::GetRotationMatrix() const {
	return Matrix3x3(1.0f - 2.0f * v.y*v.y - 2.0f * v.z*v.z,		2.0f * v.x * v.y + 2.0f * s * v.z,			2.0f * v.z * v.x - 2.0f * s * v.y,
						2.0f * v.x * v.y - 2.0f * s * v.z,		1.0f - 2.0f * v.x*v.x - 2.0f * v.z*v.z,			2.0f * v.y * v.z + 2.0f * s * v.x,
						2.0f * v.z * v.x + 2.0f * s * v.y,			2.0f * v.y * v.z - 2.0f * s * v.x,		1.0f - 2.0f * v.x*v.x - 2.0f * v.y*v.y);
}


// ------------- Matrix implementation ---------------

Matrix4x4::Matrix4x4() {
	memset(m, 0, 16*sizeof(float));
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

Matrix4x4::Matrix4x4(const Matrix4x4 &mat) {
	memcpy(m, mat.m, 16*sizeof(float));
}

Matrix4x4::Matrix4x4(const Matrix3x3 &mat) {
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			m[i][j] = mat.m[i][j];
		}
	}
	m[3][0] = m[3][1] = m[3][2] = m[0][3] = m[1][3] = m[2][3] = 0.0f;
	m[3][3] = 1.0f;
}

Matrix4x4::Matrix4x4(	float m00, float m01, float m02, float m03,
						float m10, float m11, float m12, float m13,
						float m20, float m21, float m22, float m23,
						float m30, float m31, float m32, float m33 ) {

	memcpy(m, &m00, 16*sizeof(float));	// arguments are adjacent in stack
}

Matrix4x4 Matrix4x4::operator +(const Matrix4x4 &mat) const {

	Matrix4x4 tmp;

	const float *op1 = (float*)m;
	const float *op2 = (float*)mat.m;
	float *dst = (float*)tmp.m;

	for(int i=0; i<16; i++) *dst++ = *op1++ + *op2++;

	return tmp;
}

Matrix4x4 Matrix4x4::operator -(const Matrix4x4 &mat) const {

	Matrix4x4 tmp;

	const float *op1 = (float*)m;
	const float *op2 = (float*)mat.m;
	float *dst = (float*)tmp.m;

	for(int i=0; i<16; i++) *dst++ = *op1++ - *op2++;

	return tmp;
}

Matrix4x4 Matrix4x4::operator *(float scalar) const {

	Matrix4x4 tmp;

	const float *op1 = (float*)m;
	float *dst = (float*)tmp.m;

	for(int i=0; i<16; i++) *dst++ = *op1++ * scalar;

	return tmp;
}

Matrix4x4 Matrix4x4::operator *(const Matrix4x4 &mat) const {
	Matrix4x4 tmp;

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			tmp.m[i][j] = m[i][0]*mat.m[0][j] + m[i][1]*mat.m[1][j] + m[i][2]*mat.m[2][j] + m[i][3]*mat.m[3][j];
		}
	}

	return tmp;
}

void Matrix4x4::operator +=(const Matrix4x4 &mat) {

	const float *op2 = (float*)mat.m;
	float *dst = (float*)m;

	for(int i=0; i<16; i++) *dst++ += *op2++;
}

void Matrix4x4::operator -=(const Matrix4x4 &mat) {

	const float *op2 = (float*)mat.m;
	float *dst = (float*)m;

	for(int i=0; i<16; i++) *dst++ -= *op2++;
}

void Matrix4x4::operator *=(float scalar) {

	float *dst = (float*)m;

	for(int i=0; i<16; i++) *dst++ *= scalar;
}

void Matrix4x4::operator *=(const Matrix4x4 &mat) {
	Matrix4x4 tmp;

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			tmp.m[i][j] = m[i][0]*mat.m[0][j] + m[i][1]*mat.m[1][j] + m[i][2]*mat.m[2][j] + m[i][3]*mat.m[3][j];
		}
	}

	memcpy(m, tmp.m, 16*sizeof(float));
}


void Matrix4x4::ResetIdentity() {
	memset(m, 0, 16*sizeof(float));
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}


// Transformations (assuming column vectors)

void Matrix4x4::Translate(float x, float y, float z) {
	
	Matrix4x4 tmp(	1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					x, y, z, 1 );
	*this *= tmp;
}

void Matrix4x4::Rotate(float x, float y, float z) {

	*this *= Matrix4x4(	1,	0,			0,			0,
						0,	fcos(x),	fsin(x),	0,
						0,	-fsin(x),	fcos(x),	0,
						0,	0,			0,			1 );
	
	*this *= Matrix4x4(	fcos(y),	0,	-fsin(y),	0,
						0,			1,	0,			0,
						fsin(y),	0,	fcos(y),	0,
						0,			0,	0,			1 );

	*this *= Matrix4x4(	fcos(z),	fsin(z),	0,	0,
						-fsin(z),	fcos(z),	0,	0,
						0,			0,			1,	0,
						0,			0,			0,	1 );
}

void Matrix4x4::Rotate(const Vector3 &axis, float angle) {

	float sina = fsin(angle);
	float cosa = fcos(angle);
	float invcosa = 1-cosa;
	float nxsq = axis.x * axis.x;
	float nysq = axis.y * axis.y;
	float nzsq = axis.z * axis.z;

	Matrix4x4 xform;
	xform.m[0][0] = nxsq + (1-nxsq) * cosa;
	xform.m[0][1] = axis.x * axis.y * invcosa - axis.z * sina;
	xform.m[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	xform.m[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	xform.m[1][1] = nysq + (1-nysq) * cosa;
	xform.m[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	xform.m[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	xform.m[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	xform.m[2][2] = nzsq + (1-nzsq) * cosa;

	*this *= xform;
}

void Matrix4x4::Scale(float x, float y, float z) {
	
	Matrix4x4 xform(x, 0, 0, 0,
					0, y, 0, 0,
					0, 0, z, 0,
					0, 0, 0, 1 );
	*this *= xform;
}


//////////////////////////////

void Matrix4x4::SetTranslation(float x, float y, float z) {
	
	*this = Matrix4x4(	1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						x, y, z, 1 );
}

void Matrix4x4::SetRotation(float x, float y, float z) {

	*this = Matrix4x4(	1,	0,			0,			0,
						0,	fcos(x),	fsin(x),	0,
						0,	-fsin(x),	fcos(x),	0,
						0,	0,			0,			1 );
	
	*this *= Matrix4x4(	fcos(y),	0,	-fsin(y),	0,
						0,			1,	0,			0,
						fsin(y),	0,	fcos(y),	0,
						0,			0,	0,			1 );

	*this *= Matrix4x4(	fcos(z),	fsin(z),	0,	0,
						-fsin(z),	fcos(z),	0,	0,
						0,			0,			1,	0,
						0,			0,			0,	1 );
}

void Matrix4x4::SetRotation(const Vector3 &axis, float angle) {

	// caching of multiply used function results (opt)
	float sina = fsin(angle);
	float cosa = fcos(angle);
	float invcosa = 1-cosa;
	float nxsq = axis.x * axis.x;
	float nysq = axis.y * axis.y;
	float nzsq = axis.z * axis.z;

	Matrix4x4 xform;
	xform.m[0][0] = nxsq + (1-nxsq) * cosa;
	xform.m[0][1] = axis.x * axis.y * invcosa - axis.z * sina;
	xform.m[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	xform.m[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	xform.m[1][1] = nysq + (1-nysq) * cosa;
	xform.m[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	xform.m[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	xform.m[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	xform.m[2][2] = nzsq + (1-nzsq) * cosa;

	*this = xform;
}

void Matrix4x4::SetScaling(float x, float y, float z) {
	
	Matrix4x4 xform(x, 0, 0, 0,
					0, y, 0, 0,
					0, 0, z, 0,
					0, 0, 0, 1 );
	*this = xform;
}

void Matrix4x4::SetColumnVector(const Vector4 &vec, int columnindex) {
	
	m[0][columnindex] = vec.x;
	m[1][columnindex] = vec.y;
	m[2][columnindex] = vec.z;
	m[3][columnindex] = vec.w;
}

void Matrix4x4::SetRowVector(const Vector4 &vec, int rowindex) {

	m[rowindex][0] = vec.x;
	m[rowindex][1] = vec.y;
	m[rowindex][2] = vec.z;
	m[rowindex][3] = vec.w;
}

Vector4 Matrix4x4::GetColumnVector(int columnindex) const {

	return Vector4(m[0][columnindex], m[1][columnindex], m[2][columnindex], m[3][columnindex]);
}

Vector4 Matrix4x4::GetRowVector(int rowindex) const {

	return Vector4(m[rowindex][0], m[rowindex][1], m[rowindex][2], m[rowindex][3]);
}

// other operations on matrices

void Matrix4x4::Transpose() {
	Matrix4x4 mat = *this;

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			m[i][j] = mat.m[j][i];
		}
	}
}

Matrix4x4 Matrix4x4::Transposed() const {
	Matrix4x4 mat = *this;

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			mat.m[i][j] = m[j][i];
		}
	}

	return mat;
}


float Matrix4x4::Determinant() const {

	float det11 =	(m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));

	float det12 =	(m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));

	float det13 =	(m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	float det14 =	(m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	return m[0][0] * det11 - m[0][1] * det12 + m[0][2] * det13 - m[0][3] * det14;
}


Matrix4x4 Matrix4x4::Adjoint() const {

	Matrix4x4 coef;

	coef.m[0][0] =	(m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	coef.m[0][1] =	(m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
	coef.m[0][2] =	(m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
	coef.m[0][3] =	(m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	coef.m[1][0] =	(m[0][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[0][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[0][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	coef.m[1][1] =	(m[0][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[0][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[0][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
	coef.m[1][2] =	(m[0][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[0][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[0][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
	coef.m[1][3] =	(m[0][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[0][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[0][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	coef.m[2][0] =	(m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3])) -
					(m[0][2] * (m[1][1] * m[3][3] - m[3][1] * m[1][3])) +
					(m[0][3] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]));
	coef.m[2][1] =	(m[0][0] * (m[1][2] * m[3][3] - m[3][2] * m[1][3])) -
					(m[0][2] * (m[1][0] * m[3][3] - m[3][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[3][2] - m[3][0] * m[1][2]));
	coef.m[2][2] =	(m[0][0] * (m[1][1] * m[3][3] - m[3][1] * m[1][3])) -
					(m[0][1] * (m[1][0] * m[3][3] - m[3][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]));
	coef.m[2][3] =	(m[0][0] * (m[1][1] * m[3][2] - m[3][1] * m[1][2])) -
					(m[0][1] * (m[1][0] * m[3][2] - m[3][0] * m[1][2])) +
					(m[0][2] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]));

	coef.m[3][0] =	(m[0][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3])) -
					(m[0][2] * (m[1][1] * m[2][3] - m[2][1] * m[1][3])) +
					(m[0][3] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]));
	coef.m[3][1] =	(m[0][0] * (m[1][2] * m[2][3] - m[2][2] * m[1][3])) -
					(m[0][2] * (m[1][0] * m[2][3] - m[2][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]));
	coef.m[3][2] =	(m[0][0] * (m[1][1] * m[2][3] - m[2][1] * m[1][3])) -
					(m[0][1] * (m[1][0] * m[2][3] - m[2][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));
	coef.m[3][3] =	(m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])) -
					(m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2])) +
					(m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));

	coef.Transpose();

	float *elem = (float*)coef.m;
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			coef.m[i][j] = j%2 ? -coef.m[i][j] : coef.m[i][j];
			if(i%2) coef.m[i][j] = -coef.m[i][j];
		}
	}

	return coef;
}

Matrix4x4 Matrix4x4::Inverse() const {

	Matrix4x4 AdjMat = Adjoint();

	return AdjMat * (1.0f / Determinant());
}


// --------- 3 by 3 matrices implementation --------------

Matrix3x3::Matrix3x3() {
	memset(m, 0, 9 * sizeof(float));
	m[0][0] = m[1][1] = m[2][2] = 1.0f;
}

Matrix3x3::Matrix3x3(const Matrix3x3 &mat) {
	memcpy(m, mat.m, 9 * sizeof(float));
}

Matrix3x3::Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
	memcpy(m, &m00, 9*sizeof(float));	// arguments are adjacent in stack
}

Matrix3x3 Matrix3x3::operator +(const Matrix3x3 &mat) const {
	Matrix3x3 tmp;

	const float *op1 = (float*)m;
	const float *op2 = (float*)mat.m;
	float *dst = (float*)tmp.m;

	for(int i=0; i<9; i++) *dst++ = *op1++ + *op2++;

	return tmp;
}

Matrix3x3 Matrix3x3::operator -(const Matrix3x3 &mat) const {
	Matrix3x3 tmp;

	const float *op1 = (float*)m;
	const float *op2 = (float*)mat.m;
	float *dst = (float*)tmp.m;

	for(int i=0; i<9; i++) *dst++ = *op1++ - *op2++;

	return tmp;
}

Matrix3x3 Matrix3x3::operator *(const Matrix3x3 &mat) const {
	Matrix3x3 tmp;

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			tmp.m[i][j] = m[i][0]*mat.m[0][j] + m[i][1]*mat.m[1][j] + m[i][2]*mat.m[2][j];
		}
	}

	return tmp;
}

Matrix3x3 Matrix3x3::operator *(float scalar) const {
	Matrix3x3 tmp;

	const float *op1 = (float*)m;
	float *dst = (float*)tmp.m;

	for(int i=0; i<9; i++) *dst++ = *op1++ * scalar;

	return tmp;
}

void Matrix3x3::operator +=(const Matrix3x3 &mat) {
	const float *op = (float*)mat.m;
	float *dst = (float*)m;

	for(int i=0; i<9; i++) *dst++ += *op++;
}

void Matrix3x3::operator -=(const Matrix3x3 &mat) {
	const float *op = (float*)mat.m;
	float *dst = (float*)m;

	for(int i=0; i<9; i++) *dst++ -= *op++;
}

void Matrix3x3::operator *=(const Matrix3x3 &mat) {
	Matrix4x4 tmp;

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			tmp.m[i][j] = m[i][0]*mat.m[0][j] + m[i][1]*mat.m[1][j] + m[i][2]*mat.m[2][j];
		}
	}

	memcpy(m, tmp.m, 9*sizeof(float));
}

void Matrix3x3::operator *=(float scalar) {
	float *dst = (float*)m;

	for(int i=0; i<9; i++) *dst++ *= scalar;
}


void Matrix3x3::ResetIdentity() {
	memset(m, 0, 9 * sizeof(float));
	m[0][0] = m[1][1] = m[2][2] = 1.0f;
}

void Matrix3x3::Translate(float x, float y) {
	Matrix3x3 tmp(	1, 0, 0,
					0, 1, 0,
					x, y, 1 );
	*this *= tmp;	
}

void Matrix3x3::Rotate(float angle) {
	Matrix3x3 tmp(	fcos(angle), fsin(angle),	0,
					-fsin(angle), fcos(angle),	0,
						0,				0,		1 );
	*this *= tmp;
}

void Matrix3x3::Scale(float x, float y) {
	Matrix3x3 tmp(	x, 0, 0,
					0, y, 0,
					0, 0, 1);

	*this *= tmp;
}

void Matrix3x3::SetTranslation(float x, float y) {
	Matrix3x3(	1, 0, 0,
				0, 1, 0,
				x, y, 1 );
}

void Matrix3x3::SetRotation(float angle) {
	Matrix3x3(	fcos(angle),	fsin(angle),	0,
				-fsin(angle),	fcos(angle),	0,
					0,				0,			1 );
}

void Matrix3x3::SetScaling(float x, float y) {
	Matrix3x3(	x, 0, 0,
				0, y, 0,
				0, 0, 1 );
}

void Matrix3x3::SetColumnVector(const Vector3 &vec, int columnindex) {
	m[columnindex][0] = vec.x;
	m[columnindex][1] = vec.y;
	m[columnindex][2] = vec.z;
}

void Matrix3x3::SetRowVector(const Vector3 &vec, int rowindex) {
	m[0][rowindex] = vec.x;
	m[1][rowindex] = vec.y;
	m[2][rowindex] = vec.z;
}

Vector3 Matrix3x3::GetColumnVector(int columnindex) const {
	return Vector3(m[columnindex][0], m[columnindex][1], m[columnindex][2]);
}

Vector3 Matrix3x3::GetRowVector(int rowindex) const {
	return Vector3(m[0][rowindex], m[1][rowindex], m[2][rowindex]);
}

void Matrix3x3::Transpose() {
	Matrix3x3 mat = *this;

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			m[i][j] = mat.m[j][i];
		}
	}	
}

Matrix3x3 Matrix3x3::Transposed() const {
	Matrix3x3 mat;

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			mat.m[i][j] = m[j][i];
		}
	}

	return mat;
}


void Matrix3x3::OrthoNormalize() {
	Vector3 i, j, k;
	i = GetRowVector(0);
	j = GetRowVector(1);
	k = GetRowVector(2);

	i = CrossProduct(j, k);
	j = CrossProduct(k, i);
	k = CrossProduct(i, j);

	SetRowVector(i, 0);
	SetRowVector(j, 1);
	SetRowVector(k, 2);
}

Matrix3x3 Matrix3x3::OrthoNormalized() {
	Vector3 i, j, k;
	i = GetRowVector(0);
	j = GetRowVector(1);
	k = GetRowVector(2);

	i = CrossProduct(j, k);
	j = CrossProduct(k, i);
	k = CrossProduct(i, j);

	Matrix3x3 newmat;
	newmat.SetRowVector(i, 0);
	newmat.SetRowVector(j, 1);
	newmat.SetRowVector(k, 2);

	return newmat;
}



// ----------- Ray implementation --------------
Ray::Ray() {
	Origin = Vector3(0.0f, 0.0f, 0.0f);
	Direction = Vector3(0.0f, 0.0f, 1.0f);
	Energy = 1.0f;
	CurrentIOR = 1.0f;
}

Ray::Ray(const Vector3 &origin, const Vector3 &direction) {
	Origin = origin;
	Direction = direction;
}

// ----------- Base implementation --------------
Base::Base() {
	i = Vector3(1, 0, 0);
	j = Vector3(0, 1, 0);
	k = Vector3(0, 0, 1);
}

Base::Base(const Vector3 &i, const Vector3 &j, const Vector3 &k) {
	this->i = i;
	this->j = j;
	this->k = k;
}

Base::Base(const Vector3 &dir, bool LeftHanded) {
	k = dir;
	j = VECTOR3_J;
	i = CrossProduct(j, k);
	j = CrossProduct(k, i);
}


void Base::Rotate(float x, float y, float z) {
	Matrix4x4 RotMat;
	RotMat.SetRotation(x, y, z);
	i.Transform(RotMat);
	j.Transform(RotMat);
	k.Transform(RotMat);
}

void Base::Rotate(const Vector3 &axis, float angle) {
	Quaternion q;
	q.SetRotation(axis, angle);
	i.Transform(q);
	j.Transform(q);
	k.Transform(q);
}

void Base::Rotate(const Matrix4x4 &mat) {
	i.Transform(mat);
	j.Transform(mat);
	k.Transform(mat);
}

void Base::Rotate(const Quaternion &quat) {
	i.Transform(quat);
	j.Transform(quat);
	k.Transform(quat);
}

Matrix3x3 Base::CreateRotationMatrix() const {
	return Matrix3x3(	i.x, i.y, i.z,
						j.x, j.y, j.z,
						k.x, k.y, k.z);
}