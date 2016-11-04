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