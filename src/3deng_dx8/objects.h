#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <string>
#include "3dengine.h"
#include "n3dmath.h"
#include "3dgeom.h"
#include "material.h"
#include "motion.h"

class Object {
protected:
	GraphicsContext *gc;
	RenderParams rendp;

	TriMesh *mesh;
	TriMesh **ShadowVolumes;
	int ShadowCount;
	bool CastShadows;
	
	Matrix4x4 WorldXForm;
//	Matrix4x4 TransMat, RotMat, ScaleMat, GRotMat;
	Matrix4x4 TextureMatrix;
	bool UseTextureMatrix;

	bool AutoSetZWrite;

	void Render2TexUnits();
	void Render4TexUnits();
	void Render8TexUnits();

public:
	std::string name;
	Material material;
	MotionController controller;

	Matrix4x4 TransMat, RotMat, ScaleMat, GRotMat;

	Object(GraphicsContext *gc, byte DetailLevels = 1);
	~Object();

	TriMesh *GetTriMesh();
	
	void ResetTransform();
	void ResetTranslation();
	void ResetRotation();
	void ResetGlobalRotation();
	void ResetScaling();

	void Translate(float tx, float ty, float tz);
	void Rotate(float rx, float ry, float rz);
	void Rotate(const Vector3 &axis, float angle);
	void Rotate(const Matrix4x4 &rot);
	void GlobalRotate(float rx, float ry, float rz);
	void GlobalRotate(const Vector3 &axis, float angle);
	void GlobalRotate(const Matrix4x4 &rot);
	void Scale(float sx, float sy, float sz);

	void SetTranslation(float tx, float ty, float tz);
	void SetRotation(float rx, float ry, float rz);
	void SetRotation(const Vector3 &axis, float angle);
	void SetRotation(const Matrix4x4 &rot);
	void SetGlobalRotation(float rx, float ry, float rz);
	void SetGlobalRotation(const Vector3 &axis, float angle);
	void SetGlobalRotation(const Matrix4x4 &rot);
	void SetScaling(float sx, float sy, float sz);

    const Matrix4x4 GetWorldTransform() const;

	void SetTextureMatrix(Matrix4x4 mat);
	Matrix4x4 GetTextureMatrix() const;

	// set render parameters
	void SetVertexProgram(dword VertexProgram);
	void SetPixelProgram(dword PixelProgram);
	void SetShadingMode(ShadeMode smode);
	void SetWriteZBuffer(bool enable);
	void SetBlendFunc(BlendingFactor src, BlendingFactor dest);
	void GetBlendFunc(BlendingFactor *src, BlendingFactor *dest);

	// about shadows
	void CalculateShadows(const Light **lights, int LightCount);
	TriMesh *GetShadowVolume(int light);
	void SetShadowCasting(bool enable);
	bool GetShadowCasting() const;

	void SetRenderStates();
	void Render();
	void RenderBare();

	// generate geometry
	void CreatePlane(float size, dword subdivisions);
	void CreateCube(float size);
};


#endif	// _OBJECTS_H_