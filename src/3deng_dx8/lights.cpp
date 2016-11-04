#include "lights.h"
#include <cmath>

Light::Light() {
	intensity = 1.0f;
	ambient_color = 0.0f;
	diffuse_color = specular_color = 1.0f;
	CastShadows = false;
}

void Light::SetIntensity(float intensity) {
	this->intensity = intensity;
}

float Light::GetIntensity() const {
	return intensity;
}

void Light::SetColor(const Color &color, PhongComponent comp) {
	if(comp == (PhongComponent)-1) {
        diffuse_color = specular_color = color;
	} else {
		switch(comp) {
		case Ambient:
			ambient_color = color;
			break;
		case Diffuse:
			diffuse_color = color;
			break;
		case Specular:
			specular_color = color;
			break;
		}
	}
}

const Color &Light::GetColor(PhongComponent comp) const {
	switch(comp) {
	case Ambient:
		return ambient_color;
	case Diffuse:
		return diffuse_color;
	case Specular:
		return specular_color;
	}
}

void Light::SetShadowCasting(bool enable) {
	CastShadows = enable;
}

bool Light::GetShadowCasting() const {
	return CastShadows;
}

////////// Directional Light ///////////

// Non-Applicable functions for this type of light
void DirLight::SetPosition(const Vector3 &pos) {}
void DirLight::SetRange(float range) {}
void DirLight::SetAttenuation(float att0, float att1, float att2) {}
Vector3 DirLight::GetPosition() const {return Direction;}
float DirLight::GetRange() const {return 0.0f;}
float DirLight::GetAttenuation(int degree) const {return 0.0f;}
void DirLight::ResetTransform() {}
void DirLight::ResetTranslation() {}
void DirLight::ResetRotation() {}
void DirLight::Transform(const Matrix4x4 &matrix) {}
void DirLight::Translate(float x, float y, float z) {}
void DirLight::Rotate(float x, float y, float z) {}
void DirLight::Rotate(const Vector3 &axis, float angle) {}
void DirLight::SetCone(float InnerCone, float OuterCone) {}
float DirLight::GetInnerCone() const {return 0.0f;}
float DirLight::GetOuterCone() const {return 0.0f;}
void DirLight::SetFalloff(float falloff) {}
float DirLight::GetFalloff() const {return 0.0f;}
void DirLight::ResetTargetTransform() {}
void DirLight::ResetTargetTranslation() {}
void DirLight::ResetTargetRotation() {}
void DirLight::TargetTransform(const Matrix4x4 &matrix) {}
void DirLight::TargetTranslate(float x, float y, float z) {}
void DirLight::TargetRotate(float x, float y, float z) {}
void DirLight::TargetRotate(const Vector3 &axis, float angle) {}



DirLight::DirLight() {
	Direction = Vector3(0, -1, 0);
}

DirLight::DirLight(const Vector3 &dir) {
	Direction = dir;
}


void DirLight::SetDirection(const Vector3 &dir) {
	Direction = dir;
}

Vector3 DirLight::GetDirection() const {
	Vector3 dir = Direction;
	dir.Transform(DirRot);
	return dir;
}

LightType DirLight::GetType() const {
	return LTDir;
}

// direction transformations
void DirLight::ResetDirTransform() {
	DirRot.ResetIdentity();
}

void DirLight::ResetDirRotation() {
	DirRot.ResetIdentity();
}

void DirLight::TransformDir(const Matrix4x4 &matrix) {
	DirRot *= matrix;
}

void DirLight::RotateDir(float x, float y, float z) {
	DirRot.Rotate(x, y, z);
}

void DirLight::RotateDir(const Vector3 &axis, float angle) {
	DirRot.Rotate(axis, angle);
}


void DirLight::SetLight(dword index, GraphicsContext *gc) const {

	Vector3 dir = Direction;
	dir.Transform(DirRot);

	D3DLIGHT8 light;
	memset(&light, 0, sizeof(D3DLIGHT8));
	light.Ambient.r = ambient_color.r * intensity;
	light.Diffuse.r = diffuse_color.r * intensity;
	light.Specular.r = specular_color.r * intensity;
	light.Ambient.g = ambient_color.g * intensity;
	light.Diffuse.g = diffuse_color.g * intensity;
	light.Specular.g = specular_color.g * intensity;
	light.Ambient.b = ambient_color.b * intensity;
	light.Diffuse.b = diffuse_color.b * intensity;
	light.Specular.b = specular_color.b * intensity;
	light.Direction = dir;
	light.Type = (D3DLIGHTTYPE)LTDir;
	
	gc->D3DDevice->SetLight(index, &light);
	gc->D3DDevice->LightEnable(index, true);
}

Vertex DirLightVisVertices[] = {
	Vertex(Vector3(0.0f, 0.0f, 0.1f), 0.0f, 0.0f, 0x00ff0000),
	Vertex(Vector3(0.03f, 0.03f, 0.0f), 0.0f, 0.0f, 0x000000ff),
	Vertex(Vector3(-0.03f, 0.03f, 0.0f), 0.0f, 0.0f, 0x000000ff),
	Vertex(Vector3(0.0f, -0.03f, 0.0f), 0.0f, 0.0f, 0x000000ff)
};

Index DirLightVisIndices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 3, 1,
	1, 3, 2
};


void DirLight::Draw(GraphicsContext *gc, float size) {
	
	Material mat(0.9f, 0.8f, 0.3f);
	gc->SetMaterial(mat);
	gc->SetTexture(0, 0);
	gc->SetColorVertex(true);
	gc->SetLighting(false);
	gc->SetZBuffering(false);

	Matrix4x4 WorldMat = (Matrix4x4)Base(Direction).CreateRotationMatrix() * DirRot;
	gc->SetWorldMatrix(WorldMat);

	Matrix4x4 ViewMat = gc->GetViewMatrix();
	Matrix4x4 AugViewMat;

	for(int i=0; i<4; i++) {
        AugViewMat.SetRowVector(ViewMat.GetRowVector(i).Normalized(), i);
	}

	gc->SetViewMatrix(AugViewMat);

	gc->Draw(DirLightVisVertices, DirLightVisIndices, 4, 12);

	gc->SetViewMatrix(ViewMat);

	gc->SetZBuffering(true);
	gc->SetLighting(true);
	gc->SetColorVertex(false);

}

/////////// Point Light //////////////

// Non-Applicable member functions
void PointLight::SetDirection(const Vector3 &dir) {}
Vector3 PointLight::GetDirection() const {return Position;}
void PointLight::ResetDirTransform() {}
void PointLight::ResetDirRotation() {}
void PointLight::TransformDir(const Matrix4x4 &matrix) {}
void PointLight::RotateDir(float x, float y, float z) {}
void PointLight::RotateDir(const Vector3 &axis, float angle) {}
void PointLight::SetCone(float InnerCone, float OuterCone) {}
float PointLight::GetInnerCone() const {return 0.0f;}
float PointLight::GetOuterCone() const {return 0.0f;}
void PointLight::SetFalloff(float falloff) {}
float PointLight::GetFalloff() const {return 0.0f;}
void PointLight::ResetTargetTransform() {}
void PointLight::ResetTargetTranslation() {}
void PointLight::ResetTargetRotation() {}
void PointLight::TargetTransform(const Matrix4x4 &matrix) {}
void PointLight::TargetTranslate(float x, float y, float z) {}
void PointLight::TargetRotate(float x, float y, float z) {}
void PointLight::TargetRotate(const Vector3 &axis, float angle) {}



PointLight::PointLight() {
	Position = Vector3(0, 100, 0);
	Range = 300.0f;
	Attenuation[0] = 1.0f;
	Attenuation[1] = 0.0f;
	Attenuation[2] = 0.0f;
}

PointLight::PointLight(const Vector3 &pos, float range, float att0, float att1, float att2) {
	Position = pos;
	Range = range;
	Attenuation[0] = att0;
	Attenuation[1] = att1;
	Attenuation[2] = att2;
}

Matrix4x4 PointLight::GetTransform() const {
	return PosXForm * PosRot * PosTrans;
}

void PointLight::SetPosition(const Vector3 &pos) {
	Position = pos;
}

void PointLight::SetRange(float range) {
	Range = range;
}

void PointLight::SetAttenuation(float att0, float att1, float att2) {
	Attenuation[0] = att0;
	Attenuation[1] = att1;
	Attenuation[2] = att2;
}

Vector3 PointLight::GetPosition() const {
	Vector3 pos = Position;
	pos.Transform(GetTransform());
	return pos;
}

float PointLight::GetRange() const {
	return Range;
}

float PointLight::GetAttenuation(int degree) const {
	return Attenuation[degree];
}

LightType PointLight::GetType() const {
	return LTPoint;
}

// position transformations
void PointLight::ResetTransform() {
	PosRot.ResetIdentity();
	PosTrans.ResetIdentity();
	PosXForm.ResetIdentity();
}

void PointLight::ResetTranslation() {
	PosTrans.ResetIdentity();
}

void PointLight::ResetRotation() {
	PosRot.ResetIdentity();
}

void PointLight::Transform(const Matrix4x4 &matrix) {
	PosXForm *= matrix;
}

void PointLight::Translate(float x, float y, float z) {
	PosTrans.Translate(x, y, z);
}

void PointLight::Rotate(float x, float y, float z) {
	PosRot.Rotate(x, y, z);
}

void PointLight::Rotate(const Vector3 &axis, float angle) {
	PosRot.Rotate(axis, angle);
}


void PointLight::SetLight(dword index, GraphicsContext *gc) const {

	Vector3 pos = Position;
	pos.Transform(GetTransform());

	D3DLIGHT8 light;
	memset(&light, 0, sizeof(D3DLIGHT8));
	light.Ambient.r = ambient_color.r * intensity;
	light.Diffuse.r = diffuse_color.r * intensity;
	light.Specular.r = specular_color.r * intensity;
	light.Ambient.g = ambient_color.g * intensity;
	light.Diffuse.g = diffuse_color.g * intensity;
	light.Specular.g = specular_color.g * intensity;
	light.Ambient.b = ambient_color.b * intensity;
	light.Diffuse.b = diffuse_color.b * intensity;
	light.Specular.b = specular_color.b * intensity;
	light.Position = pos;
	light.Range = Range;
	light.Attenuation0 = Attenuation[0];
	light.Attenuation1 = Attenuation[1];
	light.Attenuation2 = Attenuation[2];
	light.Type = (D3DLIGHTTYPE)LTPoint;

	gc->D3DDevice->SetLight(index, &light);
	gc->D3DDevice->LightEnable(index, true);
}



Index PointLightVisIndices[] = {0, 2, 1, 0, 3, 2};

void PointLight::Draw(GraphicsContext *gc, float size) {

	dword color = (diffuse_color * intensity).GetPacked32();

	float PtHalfSz = size / 2.0f;
	Vertex PointLightVisVertices[] = {
		Vertex(Vector3(-PtHalfSz, PtHalfSz, 0.0f), 0.0f, 0.0f, color),
		Vertex(Vector3(PtHalfSz, PtHalfSz, 0.0f), 1.0f, 0.0f, color),
		Vertex(Vector3(PtHalfSz, -PtHalfSz, 0.0f), 1.0f, 1.0f, color),
		Vertex(Vector3(-PtHalfSz, -PtHalfSz, 0.0f), 0.0f, 1.0f, color)
	};

	Texture *tex = gc->texman->LoadTexture("STOCKTEX_BLOB");
	gc->SetTexture(0, tex);
	gc->SetTextureStageColor(0, TexBlendModulate, TexArgTexture, TexArgCurrent);
	gc->SetTexture(1, 0);
	
	gc->SetAlphaBlending(true);
	gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);
	
	gc->SetColorVertex(true);
	gc->SetLighting(false);

	Matrix4x4 InvPos;
	InvPos.Translate(Position.x, Position.y, Position.z);
	gc->SetWorldMatrix(InvPos * GetTransform());

	gc->SetZWrite(false);
	gc->SetBillboarding(true);
	gc->Draw(PointLightVisVertices, PointLightVisIndices, 4, 6);
	gc->SetBillboarding(false);
	gc->SetZWrite(true);

	gc->SetAlphaBlending(false);	
	gc->SetLighting(true);
	gc->SetColorVertex(false);
	gc->SetTexture(0, 0);
}


////////////// Spot Light ////////////////
void SpotLight::ResetTargetTransform() {}
void SpotLight::ResetTargetTranslation() {}
void SpotLight::ResetTargetRotation() {}
void SpotLight::TargetTransform(const Matrix4x4 &matrix) {}
void SpotLight::TargetTranslate(float x, float y, float z) {}
void SpotLight::TargetRotate(float x, float y, float z) {}
void SpotLight::TargetRotate(const Vector3 &axis, float angle) {}

SpotLight::SpotLight() {
	Position = Vector3(0, 100, 0);
	Direction = Vector3(0, -1, 0);
	Falloff = 1.0f;
	Phi = QuarterPi;
	Theta = Phi - (Phi / 9.0f);
}

SpotLight::SpotLight(const Vector3 &pos, const Vector3 &dir, float InnerCone, float OuterCone, float range, float att0, float att1, float att2) {
	Position = pos;
	Direction = dir;
	SetCone(InnerCone, OuterCone);
	Range = range;
	Attenuation[0] = att0;
	Attenuation[1] = att1;
	Attenuation[2] = att2;
	Falloff = 1.0f;
}

void SpotLight::SetDirection(const Vector3 &dir) {
	Direction = dir;
}

Vector3 SpotLight::GetDirection() const {
	Vector3 dir = Direction;
	dir.Transform(DirRot);
	return dir;
}

// direction transformations
void SpotLight::ResetDirTransform() {
	DirRot.ResetIdentity();
}

void SpotLight::ResetDirRotation() {
	DirRot.ResetIdentity();
}

void SpotLight::TransformDir(const Matrix4x4 &matrix) {
	DirRot *= matrix;
}

void SpotLight::RotateDir(float x, float y, float z) {
	DirRot.Rotate(x, y, z);
}

void SpotLight::RotateDir(const Vector3 &axis, float angle) {
	DirRot.Rotate(axis, angle);
}


LightType SpotLight::GetType() const {
	return LTSpot;
}

void SpotLight::SetCone(float InnerCone, float OuterCone) {
	Theta = InnerCone;
	Phi = OuterCone;
}

float SpotLight::GetInnerCone() const {
	return Theta;
}

float SpotLight::GetOuterCone() const {
	return Phi;
}

void SpotLight::SetFalloff(float falloff) {
	Falloff = falloff;
}

float SpotLight::GetFalloff() const {
	return Falloff;
}


void SpotLight::SetLight(dword index, GraphicsContext *gc) const {
	
	Vector3 pos = Position;
	pos.Transform(GetTransform());

	Vector3 dir = Direction;
	dir.Transform(DirRot);

	D3DLIGHT8 light;
	memset(&light, 0, sizeof(D3DLIGHT8));
	light.Ambient.r = ambient_color.r * intensity;
	light.Diffuse.r = diffuse_color.r * intensity;
	light.Specular.r = specular_color.r * intensity;
	light.Ambient.g = ambient_color.g * intensity;
	light.Diffuse.g = diffuse_color.g * intensity;
	light.Specular.g = specular_color.g * intensity;
	light.Ambient.b = ambient_color.b * intensity;
	light.Diffuse.b = diffuse_color.b * intensity;
	light.Specular.b = specular_color.b * intensity;
	light.Position = pos;
	light.Direction = dir;
	light.Range = Range;
	light.Attenuation0 = Attenuation[0];
	light.Attenuation1 = Attenuation[1];
	light.Attenuation2 = Attenuation[2];
	light.Falloff = Falloff;
	light.Theta = Theta;
	light.Phi = Phi;
	light.Type = (D3DLIGHTTYPE)LTSpot;

	gc->D3DDevice->SetLight(index, &light);
	gc->D3DDevice->LightEnable(index, true);
}

void SpotLight::Draw(GraphicsContext *gc, float size) {
	PointLight::Draw(gc, size);
}

//////////////// Target Spot Light ////////////////////
void TargetSpotLight::SetDirection(const Vector3 &dir) {}
Vector3 TargetSpotLight::GetDirection() const {return Vector3();}
void TargetSpotLight::ResetDirTransform() {}
void TargetSpotLight::ResetDirRotation() {}
void TargetSpotLight::TransformDir(const Matrix4x4 &matrix) {}
void TargetSpotLight::RotateDir(float x, float y, float z) {}
void TargetSpotLight::RotateDir(const Vector3 &axis, float angle) {}

TargetSpotLight::TargetSpotLight() {
	Target = Vector3(0.0f, 0.0f, 0.0f);
}

TargetSpotLight::TargetSpotLight(const Vector3 &pos, const Vector3 &target, float InnerCone, float OuterCone, float range, float att0, float att1, float att2) {
	Position = pos;
	Target = target;
	SetCone(InnerCone, OuterCone);
	Range = range;
	Attenuation[0] = att0;
	Attenuation[1] = att1;
	Attenuation[2] = att2;
	Falloff = 1.0f;
}

Matrix4x4 TargetSpotLight::GetTargetTransform() const {
	return TargXForm * TargRot * TargTrans;
}

void TargetSpotLight::ResetTargetTransform() {
	TargRot.ResetIdentity();
	TargTrans.ResetIdentity();
	TargXForm.ResetIdentity();
}

void TargetSpotLight::ResetTargetTranslation() {
	TargTrans.ResetIdentity();
}

void TargetSpotLight::ResetTargetRotation() {
	TargRot.ResetIdentity();
}

void TargetSpotLight::TargetTransform(const Matrix4x4 &matrix) {
	TargXForm *= matrix;
}

void TargetSpotLight::TargetTranslate(float x, float y, float z) {
	TargTrans.Translate(x, y, z);
}

void TargetSpotLight::TargetRotate(float x, float y, float z) {
	TargRot.Rotate(x, y, z);
}

void TargetSpotLight::TargetRotate(const Vector3 &axis, float angle) {
	TargRot.Rotate(axis, angle);
}


void TargetSpotLight::SetLight(dword index, GraphicsContext *gc) const {
	Vector3 targ = Target;
	targ.Transform(GetTargetTransform());

	Vector3 pos = Position;
	pos.Transform(GetTransform());

	Vector3 *dir = const_cast<Vector3*>(&Direction);
	*dir = (targ - pos).Normalized();

	SpotLight::SetLight(index, gc);
}

void TargetSpotLight::Draw(GraphicsContext *gc, float size) {
	Vector3 targ = Target;
	targ.Transform(GetTargetTransform());

	Vector3 pos = Position;
	pos.Transform(GetTransform());

	Direction = (targ - pos).Normalized();
	SpotLight::Draw(gc, size);
}