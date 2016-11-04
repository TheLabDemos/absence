#include "camera.h"

// camera constructor
Camera::Camera() {
	Pos = Vector3(0.0f, 0.0f, 0.0f);
	LookAt = Vector3(0.0f, 0.0f, 5.0f);
	Up = Vector3(0.0f, 1.0f, 0.0f);
	FOV = QuarterPi;		// 45 deg
	CreateCameraMatrix();

	FlipRight = false;

	path = 0;
	targpath = 0;

	NearClip = 1.0f;
	FarClip = 10000.0f;
}

void Camera::SetCamera(const Vector3 &pos, const Vector3 &lookat, const Vector3 &up) {
	Pos = pos;
	LookAt = lookat;
	Up = up;
}

void Camera::SetClippingPlanes(float NearClip, float FarClip) {
	this->NearClip = NearClip;
	this->FarClip = FarClip;
}

void Camera::GetClippingPlanes(float *NearClip, float *FarClip) const {
	*NearClip = this->NearClip;
	*FarClip = this->FarClip;
}

Matrix4x4 &Camera::GetCameraMatrix() {
	return CCSmat;
}

Vector3 Camera::GetViewVector() const {
	return LookAt - Pos;
}

Vector3 Camera::GetPosition() const {
	return Pos;
}

Vector3 Camera::GetUpVector() const {
	return Up;
}

Vector3 Camera::GetTargetPosition() const {
	return LookAt;
}

Base Camera::GetCameraBase() const {
	Base base;
	base.k = (LookAt - Pos).Normalized();
	base.j = Up.Normalized();
	base.i = CrossProduct(base.j, base.k);
	base.j = CrossProduct(base.i, base.k);

	return base;
}

//////////////////////////////////////////////////////////////
// Create a camera matrix from 3 vectors position/lookat/up //
//////////////////////////////////////////////////////////////
void Camera::CreateCameraMatrix(Matrix4x4 *matrix, const Vector3 &Pos, const Vector3 &LookAt, const Vector3 &Up) const {

	Vector3 n = LookAt - Pos;
	n.Normalize();
	Vector3 u = Up.CrossProduct(n);
	u.Normalize();
	Vector3 v = n.CrossProduct(u);
	float Tx = -(u.DotProduct(Pos));
	float Ty = -(v.DotProduct(Pos));
	float Tz = -(n.DotProduct(Pos));
	
	matrix->ResetIdentity();
	matrix->m[0][0] = u.x;	matrix->m[0][1] = v.x;	matrix->m[0][2] = n.x;
	matrix->m[1][0] = u.y;	matrix->m[1][1] = v.y;	matrix->m[1][2] = n.y;
	matrix->m[2][0] = u.z;	matrix->m[2][1] = v.z;	matrix->m[2][2] = n.z;
	matrix->m[3][0] = Tx;	matrix->m[3][1] = Ty;	matrix->m[3][2] = Tz;
}

void Camera::CreateCameraMatrix() {
	CreateCameraMatrix(&CCSmat, Pos, LookAt, Up);
}

///////////////////////////////////////////////////////
// controling the camera

// moves the camera x/y/z units
void Camera::Move(float x, float y, float z) {
	PosTranslate.Translate(x,y,z);
	LookTranslate.Translate(x,y,z);
	UpTranslate.Translate(x,y,z);
}

// moves the camera TO the new coords
void Camera::MoveTo(float x, float y, float z) {
	Vector3 newpos = Vector3(x,y,z);
	// find the difference between the old and new position
	Vector3 translation = newpos - Pos;
	PosTranslate.Translate(translation.x, translation.y, translation.z);
	LookTranslate.Translate(translation.x, translation.y, translation.z);
	UpTranslate.Translate(translation.x, translation.y, translation.z);
}

void Camera::Rotate(float x, float y, float z) {
	// find the inverted lookat vector
	Vector3 ilook = Pos - LookAt;
	Vector3 newilook = ilook;
	// rotate it
	Matrix4x4 rot;
	rot.ResetIdentity();
	rot.Rotate(x,y,z);
	newilook.Transform(rot);
	// find the translation difference between the two vectors
	Vector3 transl = newilook - ilook;
	//do it
	PosTranslate.ResetIdentity();
	PosTranslate.Translate(transl.x, transl.y, transl.z);
	UpRotate.ResetIdentity();
	UpRotate.Rotate(x, y, z);

	// apply the transformation
	Pos.Transform(PosTranslate);
	Up.Transform(UpRotate);
}
	

////////////////// rotates the camera around the target ///////////////

inline void Camera::RotateTarget(float x, float y, float z) {
	LookRotate.Rotate(x, y, z);
}

inline void Camera::MoveTarget(float x, float y, float z) {
	LookTranslate.Translate(x,y,z);
}

// moves the look at point at the point specified
inline void Camera::MoveTargetTo(float x, float y, float z) {
	Vector3 translation = Vector3(x,y,z) - LookAt;
	LookTranslate.Translate(translation.x, translation.y, translation.z);
}

void Camera::Zoom(float factor) {
	// find the new vector between the camera and the target
	Vector3 offset = (LookAt - Pos) * factor;
	Vector3 diff = offset - LookAt;
	Pos += offset;

	//PosTranslate.ResetIdentity();
	//PosTranslate.Translate(diff.x, diff.y, diff.z);
	//Pos.Transform(PosTranslate);
}

void Camera::Spin(float rads) {
	Up.Rotate((LookAt - Pos).Normalized(), rads);
}

void Camera::SetRightFlipping(bool enable) {
	FlipRight = enable;
}
void Camera::SetPosition(const Vector3 &pos) {
	this->Pos = pos;
}

void Camera::SetUpVector(const Vector3 &up) {
	Up = up;
}

void Camera::SetTarget(const Vector3 &targ) {
	this->LookAt = targ;
}


void Camera::ResetRotation() {
	PosRotate.ResetIdentity();
}


void Camera::SetCameraPath(const Curve *path, const Curve *tpath, dword StartTime, dword EndTime) {
	this->path = path;
	this->targpath = tpath;
	this->StartTime = StartTime;
	this->EndTime = EndTime;
}

void Camera::FollowPath(dword time, bool Cycle) {
	if(Cycle || (!Cycle && time >= StartTime && time < EndTime)) {
		float t = (float)(time - StartTime) / (float)(EndTime - StartTime);
		if(Cycle) {
			t = (float)fmod(t, 1.0f);
		} else {
            if(t < 0.0f) t = 0.0f;
			if(t > 1.0f) t = 1.0f;
		}
		if(path) {
			SetPosition(const_cast<Curve*>(path)->Interpolate(t));
		}
		if(targpath) {
			SetTarget(const_cast<Curve*>(targpath)->Interpolate(t));
		}
	}
}

void Camera::FollowPath(float t) {
	if(t < 0.0f) t = 0.0f;
	if(t > 1.0f) t = 1.0f;
	if(path) {
		SetPosition(const_cast<Curve*>(path)->Interpolate(t));
	}
	if(targpath) {
		SetTarget(const_cast<Curve*>(targpath)->Interpolate(t));
	}
}

dword Camera::GetStartTime() const {
	return StartTime;
}

dword Camera::GetEndTime() const {
	return EndTime;
}