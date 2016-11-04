#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <string>
#include "switches.h"
#include "n3dmath.h"
#include "3dengine.h"

enum LightType {
	LTDir = D3DLIGHT_DIRECTIONAL, 
	LTSpot = D3DLIGHT_SPOT, 
	LTPoint = D3DLIGHT_POINT
};

/////////////////////////////////////////////////////////////////////
//                 -- Abstract Base class Light --                 //
// pure virtual functions for every possible light parameter,      //
// implement non-applicable functions as private in each subclass. //
/////////////////////////////////////////////////////////////////////

class Light {
protected:
	float intensity;
	Color ambient_color, diffuse_color, specular_color;
	bool CastShadows;

public:
	std::string name;

	Light();

	// set parameters
	virtual void SetPosition(const Vector3 &pos) = 0;
	virtual void SetDirection(const Vector3 &dir) = 0;
	virtual void SetColor(const Color &color, PhongComponent comp = (PhongComponent)-1);
	virtual void SetIntensity(float intensity);
	virtual void SetRange(float range) = 0;
	virtual void SetAttenuation(float att0, float att1, float att2) = 0;
	virtual void SetShadowCasting(bool enable);

	// get parameters
	virtual Vector3 GetPosition() const = 0;
	virtual Vector3 GetDirection() const = 0;
	virtual const Color &GetColor(PhongComponent comp = Diffuse) const;
	virtual float GetIntensity() const;
	virtual float GetRange() const = 0;
	virtual float GetAttenuation(int degree) const = 0;
	virtual bool GetShadowCasting() const;
	
	virtual LightType GetType() const = 0;

	virtual void SetCone(float InnerCone, float OuterCone) = 0;
	virtual float GetInnerCone() const = 0;
	virtual float GetOuterCone() const = 0;

	virtual void SetFalloff(float falloff) = 0;
	virtual float GetFalloff() const = 0;
	
	// position transformations
	virtual void ResetTransform() = 0;
	virtual void ResetTranslation() = 0;
	virtual void ResetRotation() = 0;

	virtual void Transform(const Matrix4x4 &matrix) = 0;
	virtual void Translate(float x, float y, float z) = 0;
	virtual void Rotate(float x, float y, float z) = 0;
	virtual void Rotate(const Vector3 &axis, float angle) = 0;

	// direction transformations
	virtual void ResetDirTransform() = 0;
	virtual void ResetDirRotation() = 0;

	virtual void TransformDir(const Matrix4x4 &matrix) = 0;
	virtual void RotateDir(float x, float y, float z) = 0;
	virtual void RotateDir(const Vector3 &axis, float angle) = 0;

	// target transformations
	virtual void ResetTargetTransform() = 0;
	virtual void ResetTargetTranslation() = 0;
	virtual void ResetTargetRotation() = 0;
	virtual void TargetTransform(const Matrix4x4 &matrix) = 0;
	virtual void TargetTranslate(float x, float y, float z) = 0;
	virtual void TargetRotate(float x, float y, float z) = 0;
	virtual void TargetRotate(const Vector3 &axis, float angle) = 0;

	virtual void SetLight(dword index, GraphicsContext *gc) const = 0;

	virtual void Draw(GraphicsContext *gc, float size = 1.0f) = 0;
};

// Directional Light
class DirLight : public Light {
private:
	// Non-Applicable functions for this type of light
	virtual void SetPosition(const Vector3 &pos);
	virtual void SetRange(float range);
	virtual void SetAttenuation(float att0, float att1, float att2);
	virtual Vector3 GetPosition() const;
	virtual float GetRange() const;
	virtual float GetAttenuation(int degree) const;
	virtual void ResetTransform();
	virtual void ResetTranslation();
	virtual void ResetRotation();
	virtual void Transform(const Matrix4x4 &matrix);
	virtual void Translate(float x, float y, float z);
	virtual void Rotate(float x, float y, float z);
	virtual void Rotate(const Vector3 &axis, float angle);
	virtual void ResetTargetTransform();
	virtual void ResetTargetTranslation();
	virtual void ResetTargetRotation();
	virtual void TargetTransform(const Matrix4x4 &matrix);
	virtual void TargetTranslate(float x, float y, float z);
	virtual void TargetRotate(float x, float y, float z);
	virtual void TargetRotate(const Vector3 &axis, float angle);
	virtual void SetCone(float InnerCone, float OuterCone);
	virtual float GetInnerCone() const;
	virtual float GetOuterCone() const;
	virtual void SetFalloff(float falloff);
	virtual float GetFalloff() const;


protected:
	Vector3 Direction;
	Matrix4x4 DirRot;
	
public:
	DirLight();
	DirLight(const Vector3 &dir);

	// accessors
	virtual void SetDirection(const Vector3 &dir);
	virtual Vector3 GetDirection() const;
	
	virtual LightType GetType() const;
	
	// direction transformations
	virtual void ResetDirTransform();
	virtual void ResetDirRotation();

	virtual void TransformDir(const Matrix4x4 &matrix);
	virtual void RotateDir(float x, float y, float z);
	virtual void RotateDir(const Vector3 &axis, float angle);

	virtual void SetLight(dword index, GraphicsContext *gc) const;

	virtual void Draw(GraphicsContext *gc, float size = 1.0f);
};

// Point Light (omni)
class PointLight : public Light {
private:
	// Non-Applicable member functions
	virtual void SetDirection(const Vector3 &dir);
	virtual Vector3 GetDirection() const;
	virtual void ResetDirTransform();
	virtual void ResetDirRotation();
	virtual void TransformDir(const Matrix4x4 &matrix);
	virtual void RotateDir(float x, float y, float z);
	virtual void RotateDir(const Vector3 &axis, float angle);
	virtual void ResetTargetTransform();
	virtual void ResetTargetTranslation();
	virtual void ResetTargetRotation();
	virtual void TargetTransform(const Matrix4x4 &matrix);
	virtual void TargetTranslate(float x, float y, float z);
	virtual void TargetRotate(float x, float y, float z);
	virtual void TargetRotate(const Vector3 &axis, float angle);
	virtual void SetCone(float InnerCone, float OuterCone);
	virtual float GetInnerCone() const;
	virtual float GetOuterCone() const;
	virtual void SetFalloff(float falloff);
	virtual float GetFalloff() const;

protected:
	Vector3 Position;
	float Range;
	float Attenuation[3];

	Matrix4x4 PosRot, PosTrans, PosXForm;

	virtual Matrix4x4 GetTransform() const;

public:
	PointLight();
	PointLight(const Vector3 &pos, float range = 1000.0f, float att0 = 1.0f, float att1 = 0.0f, float att2 = 0.0f);

	// accessors
	virtual void SetPosition(const Vector3 &pos);
	virtual void SetRange(float range);
	virtual void SetAttenuation(float att0, float att1, float att2);

	virtual Vector3 GetPosition() const;
	virtual float GetRange() const;
	virtual float GetAttenuation(int degree) const;
	
	virtual LightType GetType() const;
	
	// position transformations
	virtual void ResetTransform();
	virtual void ResetTranslation();
	virtual void ResetRotation();

	virtual void Transform(const Matrix4x4 &matrix);
	virtual void Translate(float x, float y, float z);
	virtual void Rotate(float x, float y, float z);
	virtual void Rotate(const Vector3 &axis, float angle);

	virtual void SetLight(dword index, GraphicsContext *gc) const;

	virtual void Draw(GraphicsContext *gc, float size = 100.0f);
};

// Spotlight
class SpotLight : public PointLight {
private:
	virtual void ResetTargetTransform();
	virtual void ResetTargetTranslation();
	virtual void ResetTargetRotation();
	virtual void TargetTransform(const Matrix4x4 &matrix);
	virtual void TargetTranslate(float x, float y, float z);
	virtual void TargetRotate(float x, float y, float z);
	virtual void TargetRotate(const Vector3 &axis, float angle);

protected:
	Vector3 Direction;
	Matrix4x4 DirRot;

	float Falloff;
	float Theta, Phi;

public:
	SpotLight();
	SpotLight(const Vector3 &pos, const Vector3 &dir, float InnerCone, float OuterCone, float range = 1000.0f, float att0 = 1.0f, float att1 = 0.0f, float att2 = 0.0f);

	// accessors
	virtual void SetDirection(const Vector3 &dir);
	virtual Vector3 GetDirection() const;
	
	// direction transformations
	virtual void ResetDirTransform();
	virtual void ResetDirRotation();

	virtual void TransformDir(const Matrix4x4 &matrix);
	virtual void RotateDir(float x, float y, float z);
	virtual void RotateDir(const Vector3 &axis, float angle);

	virtual LightType GetType() const;

	virtual void SetCone(float InnerCone, float OuterCone);
	virtual float GetInnerCone() const;
	virtual float GetOuterCone() const;

	virtual void SetFalloff(float falloff);
	virtual float GetFalloff() const;

	virtual void SetLight(dword index, GraphicsContext *gc) const;

	virtual void Draw(GraphicsContext *gc, float size = 100.0f);
};

class TargetSpotLight : public SpotLight {
private:
	// Non-Applicable shit
	virtual void SetDirection(const Vector3 &dir);
	virtual Vector3 GetDirection() const;
	virtual void ResetDirTransform();
	virtual void ResetDirRotation();
	virtual void TransformDir(const Matrix4x4 &matrix);
	virtual void RotateDir(float x, float y, float z);
	virtual void RotateDir(const Vector3 &axis, float angle);

protected:
	Vector3 Target;
	Matrix4x4 TargRot, TargTrans, TargXForm;

	virtual Matrix4x4 GetTargetTransform() const;

public:

	TargetSpotLight();
	TargetSpotLight(const Vector3 &pos, const Vector3 &target, float InnerCone, float OuterCone, float range = 1000.0f, float att0 = 1.0f, float att1 = 0.0f, float att2 = 0.0f);

	virtual void ResetTargetTransform();
	virtual void ResetTargetTranslation();
	virtual void ResetTargetRotation();

	virtual void TargetTransform(const Matrix4x4 &matrix);
	virtual void TargetTranslate(float x, float y, float z);
	virtual void TargetRotate(float x, float y, float z);
	virtual void TargetRotate(const Vector3 &axis, float angle);

	virtual void SetLight(dword index, GraphicsContext *gc) const;

	virtual void Draw(GraphicsContext *gc, float size = 100.0f);
};

#endif	// _LIGHTS_H_