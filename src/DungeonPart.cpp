#include "dungeonpart.h"
#include "d3dx8.h"

DungeonPart::DungeonPart(GraphicsContext *gc) {

	this->gc = gc;

	SceneLoader::SetNormalFileSaving(true);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/scene2.3ds", &scene);
	
	// setup camera paths
	CamPath[0] = scene->GetCurve("cpath01");
	CamPath[1] = scene->GetCurve("cpath02");
	CamPath[2] = scene->GetCurve("cpath03");
	TargPath[0] = scene->GetCurve("ctarget01");
    TargPath[1] = scene->GetCurve("ctarget03");

	CamPath[0]->SetArcParametrization(true);
	CamPath[1]->SetArcParametrization(true);
	CamPath[2]->SetArcParametrization(true);
	TargPath[0]->SetArcParametrization(true);
	TargPath[1]->SetArcParametrization(true);


	cam[0] = scene->GetCamera("Camera01");
	cam[1] = scene->GetCamera("Camera02");
	cam[2] = scene->GetCamera("Camera03");
	cam[3] = scene->GetCamera("FreeCam");
	
	cam[0]->SetCameraPath(CamPath[0], TargPath[0], 0, 40000);
	cam[1]->SetCameraPath(CamPath[1], 0, 40000, 45000);
	cam[2]->SetCameraPath(CamPath[2], TargPath[1], 45000, 75000);
	
	// get the lava crust under control
	LavaCrust = scene->GetObject("Plane02");
	scene->RemoveObject(LavaCrust);

	// get the shadow-map walls under control
	ShadowObj[0] = scene->GetObject("TunnelLigh");
	scene->RemoveObject(ShadowObj[0]);
	ShadowObj[1] = scene->GetObject("TunnelLig0");
	scene->RemoveObject(ShadowObj[1]);

	// load the flame textures
	for(int i=0; i<FLAME_TEXTURES; i++) {
		char num[3];
		sprintf(num, "%02d", i);
		string fname = string("data/textures/flame/flame") + string(num) + string(".jpg");
		FlameTex[i] = gc->texman->AddTexture(fname.c_str());
	}

	// get the flame objects and remove them from the scene (to take over the rendering)
	for(int i=0; i<16; i++) {
		char num[3];
		sprintf(num, "%02d", i);
		string name = string("Fire") + string(num);
		
		Flame[i] = scene->GetObject(name.c_str());
		scene->RemoveObject(Flame[i]);
	}

	LightRays = scene->GetObject("LightRays");
	scene->RemoveObject(LightRays);

	Floor[0] = scene->GetObject("floor1");
	Floor[1] = scene->GetObject("floor2");
	Floor[2] = scene->GetObject("floor3");
	for(int i=0; i<3; i++) scene->RemoveObject(Floor[i]);

	Obj = scene->GetObject("DefSphere");
	scene->RemoveObject(Obj);

	mobj = new Object(gc);
	mobj->GetTriMesh()->SetData(Obj->GetTriMesh()->GetVertexArray(), Obj->GetTriMesh()->GetTriangleArray(), Obj->GetTriMesh()->GetVertexCount(), Obj->GetTriMesh()->GetTriangleCount());

	Obj->material.SetTexture(gc->texman->AddTexture("data/textures/rusty01.jpg"), TextureMap);
	Obj->material.SetTexture(gc->texman->AddTexture("data/textures/refmap1.jpg"), EnvironmentMap);

	Obj->material.SetAmbient(Color(0.5f));
	Obj->material.SetDiffuse(Color(0.5f));
	Obj->material.SetSpecular(Color(175.0f / 256.0f, 95.0f / 256.0f, 17.0f / 256.0f));
	Obj->material.SetSpecularPower(90.0f);
	Obj->material.SpecularEnable = true;
	mobj->material = Obj->material;

	Obj->GetTriMesh()->ChangeMode(TriMeshDynamic);
	mobj->GetTriMesh()->ChangeMode(TriMeshDynamic);

	Crystals[0] = scene->GetObject("Box114");
	Crystals[1] = scene->GetObject("Box115");
	Crystals[2] = scene->GetObject("Box116");
	Crystals[3] = scene->GetObject("Box117");
	Crystals[4] = scene->GetObject("Box118");

	for(int i=0; i<5; i++) {
		scene->RemoveObject(Crystals[i]);
	}

	Name = new Object(gc);
	Name->CreatePlane(1.7f, 0);
	Name->Scale(1.0f, 0.3f, 1.0f);

	Fade = new Object(gc);
	Fade->CreatePlane(3.0f, 0);
	Fade->material = Material(0.0f, 0.0f, 0.0f);
	
	// load name textures
	for(int i=0; i<8; i++) {
		char fname[] = "data/textures/Absence/abx.jpg";
		fname[24] = '1' + i;
		NameTex[i] = gc->texman->AddTexture(fname);
	}

	cam[3]->Zoom(-1.0f);
}

DungeonPart::~DungeonPart() {
	delete scene;
}

#define INRANGE(a, b) (msec >= a && msec < b)

void DungeonPart::MainLoop() {
	dword msec = timer.GetMilliSec();
	float t = (float)msec / 1000.0f;

    // set the active camera
	for(int i=0; i<3; i++) {
		cam[i]->FollowPath(msec);
	}
	Vector3 Cam3Pos = cam[3]->GetPosition();
	
	if(msec >= 0 && msec < 40000) {
		scene->SetActiveCamera(cam[0]);
	}
	if(msec >= 39990 && msec < 45000) {
		scene->SetActiveCamera(cam[1]);
	}
	if(msec >= 45000 && msec < 75000) {
		scene->SetActiveCamera(cam[2]);
	}

	if(INRANGE(17000, 18000) || INRANGE(22000, 23000)) {
		if(INRANGE(22000, 23000)) {
            cam[3]->Rotate(0.0f, t, 0.0f);
		} else {
			cam[3]->Rotate(0.0f, -t, 0.0f);
		}
		scene->SetActiveCamera(cam[3]);
	}

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);


	scene->Render();

	// Render flames
	gc->SetLighting(false);
	gc->SetZWrite(false);
	int ftexnum = (int)((float)msec / 33.35f) % FLAME_TEXTURES;
	gc->SetAlphaBlending(true);
	gc->SetBackfaceCulling(false);
	gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);
	gc->SetTextureStageColor(0, TexBlendModulate, TexArgTexture, TexArgCurrent);
	gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgTexture, TexArgTexture);
	gc->SetMaterial(Flame[0]->material);
	gc->SetTexture(1, 0);
	for(int i=0; i<16; i++) {		
		gc->SetTexture(0, FlameTex[ftexnum]);
		gc->SetWorldMatrix(Flame[i]->GetWorldTransform());
		Flame[i]->RenderBare();
	}
	gc->SetBackfaceCulling(true);
	gc->SetAlphaBlending(false);
	gc->SetZWrite(true);
	gc->SetLighting(true);

	// render the shadow walls
	gc->SetAlphaBlending(true);
	gc->SetBlendFunc(BLEND_DESTCOLOR, BLEND_ZERO);
	gc->SetTexture(0, ShadowObj[0]->material.Maps[0]);
	gc->SetTextureStageColor(0, TexBlendModulate, TexArgTexture, TexArgDiffuseColor);
	gc->SetMaterial(ShadowObj[0]->material);
	gc->SetWorldMatrix(ShadowObj[0]->GetWorldTransform());
	ShadowObj[0]->RenderBare();
	gc->SetWorldMatrix(ShadowObj[1]->GetWorldTransform());
	ShadowObj[1]->RenderBare();
	gc->SetAlphaBlending(false);

	// render volumetric light
	gc->SetAlphaBlending(true);
	gc->SetZWrite(false);


	LightRays->ResetScaling();
	
	LightRays->material.Alpha = 0.02f;
	LightRays->material.SetEmissive(1.0f, 1.0f, 1.0f);
    for(int i=0; i<14; i++) {
		LightRays->Scale(0.975f, 1.0f, 0.975f);
        LightRays->Render();
	}
	gc->SetZWrite(true);
	gc->SetAlphaBlending(false);


	// The Morphing Object

	Obj->GetTriMesh()->SetData(mobj->GetTriMesh()->GetVertexArray(), mobj->GetTriMesh()->GetTriangleArray(), mobj->GetTriMesh()->GetVertexCount(), mobj->GetTriMesh()->GetTriangleCount());
	
	dword VertCount = Obj->GetTriMesh()->GetVertexCount();
	Vertex *verts = Obj->GetTriMesh()->GetModVertexArray();
	for(dword i=0; i<VertCount; i++) {
		float uangle = DotProduct(verts[i].pos.Normalized(), VECTOR3_I);
		float vangle = DotProduct(verts[i].pos.Normalized(), VECTOR3_J);

		float sfact = 0.3f * (sinf(uangle * cosf(t) * 5.0f) + sinf(vangle * 7.0f * sinf(t)) + cosf(uangle * cosf(t*2.0f) * 3.0f) * 1.5f);
		verts[i].pos += verts[i].pos * sfact;
	}
	

	Obj->GetTriMesh()->CalculateNormals();
	Obj->Render();
    

	// make the lava move
	Matrix3x3 TexMat;
	TexMat.m[2][0] = t/50.0f;
	TexMat.m[2][1] = t/70.0f;
	gc->SetTextureMatrix(TexMat);
	LavaCrust->Render();
	gc->SetTextureMatrix(Matrix4x4());

	/////// Render the Crystals ///////////
/*
	float StartRise = 4;//53.0f;
	float EndRise = 8;//57.0f;
	float yoffset = -355.0f;

	if(t >= StartRise && t < EndRise) {
		for(int i=0; i<5; i++) Crystals[i]->SetTranslation(0.0f, (t - StartRise) / (EndRise - StartRise), 0.0f);
	}
*/
	gc->SetShadingMode(FlatShading);
	for(int i=0; i<5; i++) {
		Crystals[i]->Render();
	}
	gc->SetShadingMode(GouraudShading);


	//////////////// render the mirroring marble floor ///////////////

	// burn the stencil baby
	gc->SetZWrite(false);
	gc->SetColorWrite(false, false, false, false);
	gc->SetStencilBuffering(true);
	gc->SetStencilFunc(CMP_ALWAYS);
	gc->SetStencilOp(SOP_KEEP, SOP_KEEP, SOP_INC);
	
	for(int i=0; i<3; i++) Floor[i]->Render();
	gc->SetZWrite(true);
	gc->SetColorWrite(true, true, true, true);
	gc->SetStencilOp(SOP_KEEP, SOP_KEEP, SOP_KEEP);

	// render the rest of the floor normally
	Floor[0]->ResetRotation();
	Floor[0]->Rotate(0.0f, t/2.0f, 0.0f);
	Floor[1]->ResetRotation();
	Floor[1]->Rotate(0.0f, -t/4.0f, 0.0f);

	for(int i=0; i<3; i++) Floor[i]->Render();

	// render the reflection where stencil > 0
	gc->SetStencilFunc(CMP_EQUAL);
	gc->SetStencilReference(1);
	gc->ClearZBuffer(1.0f);

	gc->SetFrontFace(CounterClockwise);
	
	std::list<Object*> *objlist = scene->GetObjectsList();
	std::list<Object*>::iterator iter = objlist->begin();
	while(iter != objlist->end()) {
		(*iter)->Scale(1.0f, -1.0f, 1.0f);
		float alpha = (*iter)->material.Alpha;
		(*iter)->material.Alpha = 0.25f;
		(*iter)->Render();
		(*iter)->material.Alpha = alpha;
		(*iter++)->Scale(1.0f, -1.0f, 1.0f);
	}

	gc->SetFrontFace(Clockwise);
	gc->SetStencilBuffering(false);

	if(t < 6.0f) {
        gc->SetAlphaBlending(true);
		gc->SetLighting(false);
		gc->SetZBuffering(false);
		gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);
		gc->SetMaterial(Material(1.0f, 1.0f, 1.0f));
		gc->SetTexture(0, NameTex[(msec >> 6) % 8]);
		gc->SetTextureStageColor(0, TexBlendSelectArg1, TexArgTexture, TexArgTexture);
		gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgTexture, TexArgTexture);
        gc->SetWorldMatrix(Name->GetWorldTransform());
		Matrix4x4 ViewMat;
		ViewMat.Translate(0.0f, 0.0f, 1.0f);
		gc->SetViewMatrix(ViewMat);
		Name->RenderBare();
		gc->SetLighting(true);
	
		// fade in
		Fade->material.Alpha = min(max(0.0f, (2.0f - t)), 1.0f);
		Fade->Render();

		gc->SetZBuffering(true);
		gc->SetAlphaBlending(false);
	}

	cam[3]->SetPosition(Cam3Pos);
}