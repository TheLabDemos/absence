#include "objects.h"
#include "3dengine.h"
#include "objectgen.h"

Object::Object(GraphicsContext *gc, byte DetailLevels) {
	this->gc = gc;

	mesh = new TriMesh(DetailLevels, gc);

	SetShadingMode(GouraudShading);
	SetVertexProgram(FixedFunction);
	SetPixelProgram(0);
	rendp.Billboarded = false;
	rendp.SourceBlendFactor = BLEND_SRCALPHA;
	rendp.DestBlendFactor = BLEND_INVSRCALPHA;
	rendp.ZWrite = true;
	ShadowCount = 0;
	UseTextureMatrix = false;

	ShadowVolumes = 0;
	CastShadows = false;

	AutoSetZWrite = true;
}

Object::~Object() {
	delete mesh;
	if(rendp.VertexProgram != FixedFunction) gc->DestroyVertexProgram(rendp.VertexProgram);
}

TriMesh *Object::GetTriMesh() {
	return mesh;
}


// Reset Transformation Matrices
void Object::ResetTransform() {
	TransMat.ResetIdentity();
	RotMat.ResetIdentity();
	GRotMat.ResetIdentity();
	ScaleMat.ResetIdentity();
}

void Object::ResetTranslation() { 
	TransMat.ResetIdentity();
}

void Object::ResetRotation() {
	RotMat.ResetIdentity();
}

void Object::ResetGlobalRotation() {
	GRotMat.ResetIdentity();
}

void Object::ResetScaling() {
	ScaleMat.ResetIdentity();
}

// Concatenate additional transformations
void Object::Translate(float tx, float ty, float tz) {
	TransMat.Translate(tx, ty, tz);
}

void Object::Rotate(float rx, float ry, float rz) {
	RotMat.Rotate(rx, ry, rz);
}

void Object::Rotate(const Vector3 &axis, float angle) {
	RotMat.Rotate(axis, angle);
}

void Object::Rotate(const Matrix4x4 &rot) {
	RotMat *= rot;
}

void Object::GlobalRotate(float rx, float ry, float rz) {
	GRotMat.Rotate(rx, ry, rz);
}

void Object::GlobalRotate(const Vector3 &axis, float angle) {
	GRotMat.Rotate(axis, angle);
}

void Object::GlobalRotate(const Matrix4x4 &rot) {
	GRotMat *= rot;
}

void Object::Scale(float sx, float sy, float sz) {
	ScaleMat.Scale(sx, sy, sz);
}

void Object::SetTranslation(float tx, float ty, float tz) {
	TransMat.SetTranslation(tx, ty, tz);
}

void Object::SetRotation(float rx, float ry, float rz) {
	RotMat.Rotate(rx, ry, rz);
}

void Object::SetRotation(const Vector3 &axis, float angle) {
	RotMat.Rotate(axis, angle);
}

void Object::SetRotation(const Matrix4x4 &rot) {
	RotMat = rot;
}

void Object::SetGlobalRotation(float rx, float ry, float rz) {
	GRotMat.Rotate(rx, ry, rz);
}

void Object::SetGlobalRotation(const Vector3 &axis, float angle) {
	GRotMat.Rotate(axis, angle);
}

void Object::SetGlobalRotation(const Matrix4x4 &rot) {
	GRotMat = rot;
}

void Object::SetScaling(float sx, float sy, float sz) {
	ScaleMat.Scale(sx, sy, sz);
}

const Matrix4x4 Object::GetWorldTransform() const {
	return ScaleMat * RotMat * TransMat * GRotMat;
}

void Object::SetTextureMatrix(Matrix4x4 mat) {
	UseTextureMatrix = true;
	TextureMatrix = mat;
}

Matrix4x4 Object::GetTextureMatrix() const {
	return TextureMatrix;
}

void Object::SetVertexProgram(dword VertexProgram) {
	rendp.VertexProgram = VertexProgram;
}

void Object::SetPixelProgram(dword PixelProgram) {
	rendp.PixelProgram = PixelProgram;
}

void Object::SetShadingMode(ShadeMode smode) {
	rendp.Shading = smode;
}

void Object::SetWriteZBuffer(bool enable) {
	rendp.ZWrite = enable;
	AutoSetZWrite = false;
}

void Object::SetBlendFunc(BlendingFactor src, BlendingFactor dest) {
	rendp.SourceBlendFactor = src;
	rendp.DestBlendFactor = dest;
}

void Object::GetBlendFunc(BlendingFactor *src, BlendingFactor *dest) {
	*src = rendp.SourceBlendFactor;
	*dest = rendp.DestBlendFactor;
}

void Object::CalculateShadows(const Light **lights, int LightCount) {
	if(ShadowVolumes) {
		for(int i=0; i<ShadowCount; i++) {
			delete ShadowVolumes[i];
		}
		delete ShadowVolumes;
	}

	ShadowVolumes = new TriMesh*[LightCount];
	for(int i=0; i<LightCount; i++) {
		ShadowVolumes[i] = CreateShadowVolume(*mesh, lights[i], GetWorldTransform());
	}

	ShadowCount = LightCount;
}

TriMesh *Object::GetShadowVolume(int light) {
	if(light >= ShadowCount) return 0;
	return ShadowVolumes[light];
}

void Object::SetShadowCasting(bool enable) {
	CastShadows = enable;
}

bool Object::GetShadowCasting() const {
	return CastShadows;
}

///////////////////////////

void Object::SetRenderStates() {
	gc->SetWorldMatrix(GetWorldTransform());
	
	gc->SetMaterial(material);
	// RESTORATION: it looks like this was a later feature
	//if(AutoSetZWrite && material.Alpha < 0.991f) rendp.ZWrite = false;
	gc->SetSpecular(material.SpecularEnable);

	gc->SetVertexProgram(rendp.VertexProgram);
	gc->SetPixelProgram(rendp.PixelProgram);
	gc->SetShadingMode(rendp.Shading);
}

void Object::Render() {
	/* RESTORATION:
	 * I know that at the time I was writing this, I never had a card with more
	 * than 2 texture units, so it's best to just disable the untested code
	 * paths completely (Render4TexUnits/Render8TexUnits).
	 */
	/*
	int TexUnits = gc->GetTextureStageNumber();

	if(TexUnits < 4) {
		Render2TexUnits();
	} else if(TexUnits < 8) {
		Render4TexUnits();
	} else {
		Render8TexUnits();
	}
	*/
	Render2TexUnits();
}

void Object::Render2TexUnits() {

	SetRenderStates();

	VertexBuffer *vb = const_cast<VertexBuffer*>(mesh->GetVertexBuffer());
	IndexBuffer *ib = const_cast<IndexBuffer*>(mesh->GetIndexBuffer());

	Material mat = material;

	int TexCount = 0;
	for(int i=0; i<NumberOfTextureTypes; i++) {
		if(mat.Maps[i]) TexCount++;
	}

	if(!rendp.ZWrite) gc->SetZWrite(false);

	if(!TexCount) {
		// render without any texture
		gc->SetTexture(0, 0);
		gc->SetTextureStageColor(0, TexBlendSelectArg1, TexArgDiffuseColor, TexArgTexture);
		if(mat.Alpha < 1.0f) {
			gc->SetTextureFactor(Color(mat.Alpha, mat.Alpha, mat.Alpha, mat.Alpha).GetPacked32());
			gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgFactor, TexArgDiffuseColor);
		} else {
			gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
		}
		gc->DisableTextureStage(1);
		
		gc->SetAlphaBlending(true);
		gc->SetBlendFunc(rendp.SourceBlendFactor, rendp.DestBlendFactor);
		gc->Draw(vb, ib);
		gc->SetAlphaBlending(false);
	} else {
        
		////////// pass 1 (texture & env) ///////////
		int stage = 0;
		if(mat.Maps[TextureMap]) {
			if(UseTextureMatrix) gc->SetTextureMatrix(TextureMatrix, 0);
            gc->SetTexture(stage, mat.Maps[TextureMap]);
			gc->SetTextureStageColor(stage, TexBlendModulate, TexArgCurrent, TexArgTexture);
			if(mat.Alpha < 1.0f) {
				gc->SetTextureFactor(Color(mat.Alpha, mat.Alpha, mat.Alpha, mat.Alpha).GetPacked32());
				gc->SetTextureStageAlpha(stage, TexBlendModulate, TexArgTexture, TexArgFactor);
			} else {
				gc->SetTextureStageAlpha(stage, TexBlendSelectArg2, TexArgCurrent, TexArgTexture);
			}
			gc->SetTextureCoordIndex(stage, 0);
			stage++;
		}

		if(mat.Maps[EnvironmentMap]) {
			gc->SetTexture(stage, mat.Maps[EnvironmentMap]);
			gc->SetTextureStageColor(stage, TexBlendAdd, TexArgCurrent, TexArgTexture);
			gc->SetTextureStageAlpha(stage, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
			Matrix4x4 TexMat = Matrix4x4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f);
            gc->SetTextureMatrix(TexMat, stage);
			gc->D3DDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			gc->D3DDevice->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
			stage++;
		}

		gc->SetTexture(stage, 0);
		gc->DisableTextureStage(stage);

		if(stage > 0) {
			gc->SetAlphaBlending(true);
			gc->SetBlendFunc(rendp.SourceBlendFactor, rendp.DestBlendFactor);
			gc->Draw(vb, ib);
			gc->SetAlphaBlending(false);

			gc->SetTextureMatrix(Matrix4x4(), 0);
			gc->SetTextureMatrix(Matrix4x4(), 1);
		}

		////////// pass 2 (Bump & Lightmap) //////////
		if(stage > 0) {	// did a first pass
			gc->SetAlphaBlending(true);
			gc->SetBlendFunc(BLEND_DESTCOLOR, BLEND_ZERO);		// mult blend
			mat.Emissive = Color(1.0f);	// do not recalculate lighting
		}

		stage = 0;
		if(mat.Maps[LightMap]) {
            gc->SetTexture(stage, mat.Maps[LightMap]);
			gc->SetTextureStageColor(stage, TexBlendSelectArg2, TexArgCurrent, TexArgTexture);
			gc->SetTextureStageAlpha(stage, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
			gc->SetTextureCoordIndex(stage, 1);
			stage++;
		}

		if(mat.Maps[BumpMap]) {
			// re-implementation due
		}

		gc->SetTexture(stage, 0);
		gc->DisableTextureStage(stage);

		if(stage > 0) gc->Draw(vb, ib);

		gc->DisableTextureStage(1);
		gc->SetAlphaBlending(false);
	}

	if(!rendp.ZWrite) gc->SetZWrite(true);
}


void Object::Render4TexUnits() {
	SetRenderStates();

	VertexBuffer *vb = const_cast<VertexBuffer*>(mesh->GetVertexBuffer());
	IndexBuffer *ib = const_cast<IndexBuffer*>(mesh->GetIndexBuffer());

	Material mat = material;

	int TexCount = 0;
	for(int i=0; i<NumberOfTextureTypes; i++) {
		if(mat.Maps[i]) TexCount++;
	}

	if(!rendp.ZWrite) gc->SetZWrite(false);

	if(!TexCount) {
		// render without any texture
		gc->SetTexture(0, 0);
		gc->SetTextureStageColor(0, TexBlendSelectArg1, TexArgDiffuseColor, TexArgTexture);
		if(mat.Alpha < 1.0f) {
			gc->SetTextureFactor(Color(mat.Alpha, mat.Alpha, mat.Alpha, mat.Alpha).GetPacked32());
			gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgFactor, TexArgDiffuseColor);
		} else {
			gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
		}
		gc->DisableTextureStage(1);
		
		gc->SetAlphaBlending(true);
		gc->SetBlendFunc(rendp.SourceBlendFactor, rendp.DestBlendFactor);
		gc->Draw(vb, ib);
		gc->SetAlphaBlending(false);
	} else {
        
		////////// pass 1 (texture, detail, env, bump) ///////////
		int stage = 0;
		if(mat.Maps[TextureMap]) {
			if(UseTextureMatrix) gc->SetTextureMatrix(TextureMatrix, 0);
            gc->SetTexture(stage, mat.Maps[TextureMap]);
			gc->SetTextureStageColor(stage, TexBlendModulate, TexArgCurrent, TexArgTexture);
			if(mat.Alpha < 1.0f) {
				gc->SetTextureFactor(Color(mat.Alpha, mat.Alpha, mat.Alpha, mat.Alpha).GetPacked32());
				gc->SetTextureStageAlpha(stage, TexBlendModulate, TexArgTexture, TexArgFactor);
			} else {
				gc->SetTextureStageAlpha(stage, TexBlendSelectArg2, TexArgCurrent, TexArgTexture);
			}
			gc->SetTextureCoordIndex(stage, 0);
			stage++;
		}

		if(mat.Maps[DetailMap]) {
            gc->SetTexture(stage, mat.Maps[DetailMap]);
			gc->SetTextureStageColor(stage, TexBlendAdd, TexArgCurrent, TexArgTexture);
			gc->SetTextureStageAlpha(stage, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
			gc->SetTextureCoordIndex(stage, 1);
			stage++;
		}

		if(mat.Maps[EnvironmentMap]) {
			gc->SetTexture(stage, mat.Maps[EnvironmentMap]);
			gc->SetTextureStageColor(stage, TexBlendAdd, TexArgCurrent, TexArgTexture);
			gc->SetTextureStageAlpha(stage, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
			Matrix4x4 TexMat = Matrix4x4(0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f);
            gc->SetTextureMatrix(TexMat, stage);
			gc->D3DDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			gc->D3DDevice->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);
			stage++;
		}

		if(mat.Maps[BumpMap]) {
			// re-implementation due
		}

		gc->SetTexture(stage, 0);
		gc->DisableTextureStage(stage);

		if(stage > 0) {
			gc->SetAlphaBlending(true);
			gc->SetBlendFunc(rendp.SourceBlendFactor, rendp.DestBlendFactor);
			gc->Draw(vb, ib);
			gc->SetAlphaBlending(false);

			gc->SetTextureMatrix(Matrix4x4(), 0);
			gc->SetTextureMatrix(Matrix4x4(), 1);
			gc->SetTextureMatrix(Matrix4x4(), 2);
		}

		////////// pass 2 (Bump & Lightmap) //////////
		if(stage > 0) {	// did a first pass
			gc->SetAlphaBlending(true);
			gc->SetBlendFunc(BLEND_DESTCOLOR, BLEND_ZERO);		// mult blend
			mat.Emissive = Color(1.0f);	// do not recalculate lighting
		}

		stage = 0;
		if(mat.Maps[LightMap]) {
            gc->SetTexture(stage, mat.Maps[LightMap]);
			gc->SetTextureStageColor(stage, TexBlendSelectArg2, TexArgCurrent, TexArgTexture);
			gc->SetTextureStageAlpha(stage, TexBlendSelectArg1, TexArgCurrent, TexArgTexture);
			gc->SetTextureCoordIndex(stage, 1);
			stage++;
		}

		gc->SetTexture(stage, 0);
		gc->DisableTextureStage(stage);

		if(stage > 0) gc->Draw(vb, ib);

		gc->SetAlphaBlending(false);
	}

	if(!rendp.ZWrite) gc->SetZWrite(true);
}


void Object::Render8TexUnits() {Render4TexUnits();}



/*
void Object::Render() {

	SetRenderStates();
	
	VertexBuffer *vb = const_cast<VertexBuffer*>(mesh->GetVertexBuffer());
	IndexBuffer *ib = const_cast<IndexBuffer*>(mesh->GetIndexBuffer());

	Material mat = material;
	int MapsCount = 0, ActiveTex = 0;
	for(int i=0; i<NumberOfTextureTypes; i++) {
		if(mat.Maps[i]) MapsCount++;
	}

	if(!MapsCount) {
		gc->SetTexture(0, 0);
		gc->DisableTextureStage(0);
		gc->Draw(vb, ib);
	} else {
		int pass = 1;
		TextureType PassFirstTexture;
		while(MapsCount) {
			
			if(mat.Maps[BumpMap]) {
				gc->SetTexture(ActiveTex, mat.Maps[BumpMap]);
				//gc->SetTextureFactor() TODO
				gc->SetTextureStageColor(ActiveTex, TexBlendDotProduct, TexArgFactor, TexArgTexture);
				mat.Maps[BumpMap] = 0;
				if(!ActiveTex) PassFirstTexture = BumpMap;
			} else if(mat.Maps[TextureMap]) {
				gc->SetTexture(ActiveTex, mat.Maps[TextureMap]);
				gc->SetTextureStageColor(ActiveTex, TexBlendModulate, TexArgCurrent, TexArgTexture);
				//gc->SetTextureStageColor(ActiveTex, TexBlendSelectArg2, TexArgCurrent, TexArgTexture);
				gc->SetTextureCoordIndex(ActiveTex, 0);
				mat.Maps[TextureMap] = 0;
				if(!ActiveTex) PassFirstTexture = TextureMap;
			} else if(mat.Maps[DetailMap]) {
				gc->SetTexture(ActiveTex, mat.Maps[DetailMap]);
				gc->SetTextureStageColor(ActiveTex, TexBlendAdd, TexArgCurrent, TexArgTexture);
				gc->SetTextureCoordIndex(ActiveTex, 1);
				mat.Maps[DetailMap] = 0;
				if(!ActiveTex) PassFirstTexture = DetailMap;
			} else if(mat.Maps[EnvironmentMap]) {
				gc->SetTexture(ActiveTex, mat.Maps[EnvironmentMap]);
				gc->SetTextureStageColor(ActiveTex, TexBlendAdd, TexArgCurrent, TexArgTexture);

				//gc->SetTextureCoordGenerator(TexGenSpherical); TODO
				Matrix4x4 TexMat = Matrix4x4(0.5f,	0.0f,	0.0f,	0.0f,
											0.0f,	-0.5f,	0.0f,	0.0f,
											0.0f,	0.0f,	1.0f,	0.0f,
											0.5f,	0.5f,	0.0f,	1.0f ); 
				gc->SetTextureMatrix(TexMat, ActiveTex);
				gc->D3DDevice->SetTextureStageState(ActiveTex, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
				gc->D3DDevice->SetTextureStageState(ActiveTex, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL);

				mat.Maps[EnvironmentMap] = 0;
				if(!ActiveTex) PassFirstTexture = EnvironmentMap;
			} else if(mat.Maps[LightMap]) {
				gc->SetTexture(ActiveTex, mat.Maps[LightMap]);
				gc->SetTextureStageColor(ActiveTex, TexBlendModulate, TexArgCurrent, TexArgTexture);
				gc->SetTextureCoordIndex(ActiveTex, 2);
				mat.Maps[LightMap] = 0;
				if(!ActiveTex) PassFirstTexture = LightMap;
			}

			MapsCount--;
			ActiveTex++;

			if(!MapsCount) {
				gc->SetTextureStageColor(ActiveTex, TexBlendModulate, TexArgCurrent, TexArgDiffuseColor);
			}

			if(ActiveTex >= gc->GetTextureStageNumber() || !MapsCount) {
				if(pass++ > 1) {
					gc->SetAlphaBlending(true);
					if(PassFirstTexture == DetailMap || PassFirstTexture == EnvironmentMap) {
						gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);
					} else {
                        gc->SetBlendFunc(BLEND_DESTCOLOR, BLEND_ZERO);
					}
                    gc->Draw(vb, ib);
					gc->SetAlphaBlending(false);
				} else {
					gc->Draw(vb, ib);
				}
				ActiveTex = 0;
				for(int i=0; i<gc->GetTextureStageNumber(); i++) {
					gc->SetTextureMatrix(Matrix4x4(), i);
				}
			}
		}
	}

	if(material.SpecularEnable) gc->SetSpecular(false);
}
*/



void Object::RenderBare() {
	VertexBuffer *vb = const_cast<VertexBuffer*>(mesh->GetVertexBuffer());
	IndexBuffer *ib = const_cast<IndexBuffer*>(mesh->GetIndexBuffer());
	gc->Draw(vb, ib);
}


// generate geometry
void Object::CreatePlane(float size, dword subdivisions) {
	ObjGen::CreatePlane(gc, size, subdivisions, &mesh, mesh->GetLevelCount());
}

void Object::CreateCube(float size) {
	ObjGen::CreateCube(gc, size, &mesh, mesh->GetLevelCount());
}
