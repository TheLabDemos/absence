#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <string>
#include "switches.h"
#include "n3dmath.h"
#include "curves.h"
#include "typedefs.h"

class Camera {
private:
	Vector3 Pos, LookAt, Up;
	float FOV;
	float NearClip, FarClip;
	// transformation matrices for each vector
	Matrix4x4 PosTranslate, LookTranslate, UpTranslate;
	Matrix4x4 PosRotate, LookRotate, UpRotate;
	Matrix4x4 PosScale, LookScale, UpScale;

	bool FlipRight;

	const Curve *path, *targpath;
	dword StartTime, EndTime;

public:
	std::string name;
	Matrix4x4 CCSmat;

	Camera();
	float GetFOV() const {return FOV;}
	void SetFOV(float FOV) {this->FOV = FOV;}

	void SetClippingPlanes(float NearClip, float FarClip);
	void GetClippingPlanes(float *NearClip, float *FarClip) const;

	void SetCamera(const Vector3 &pos, const Vector3 &lookat, const Vector3 &up);
	Matrix4x4 &GetCameraMatrix();

	// camera controls
	void Move(float x, float y, float z);
	void MoveTo(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Zoom(float factor);

	// haphazard additions
	void SetRightFlipping(bool enable);
	void SetPosition(const Vector3 &pos);
	void SetUpVector(const Vector3 &up);
	void SetTarget(const Vector3 &targ);

	Vector3 GetViewVector() const;
	Vector3 GetPosition() const;
	Vector3 GetUpVector() const;
	Vector3 GetTargetPosition() const;

	Base GetCameraBase() const;
		
	inline void MoveTarget(float x, float y, float z);
	inline void MoveTargetTo(float x, float y, float z);
	inline void RotateTarget(float x, float y, float z);

	void ResetRotation();

	void CreateCameraMatrix();
	void CreateCameraMatrix(Matrix4x4 *matrix, const Vector3 &Pos, const Vector3 &LookAt, const Vector3 &Up) const;
	
	void Spin(float rads);

	void SetCameraPath(const Curve *path, const Curve *tpath, dword StartTime, dword EndTime);
	void FollowPath(dword time, bool Cycle = false);
	void FollowPath(float t);

	dword GetStartTime() const;
	dword GetEndTime() const;
};

#endif	// _CAMERA_H_