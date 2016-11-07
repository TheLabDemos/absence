#include <cassert>
#include "hellpart.h"

HellPart::HellPart(GraphicsContext *gc) {
	this->gc = gc;

	SceneLoader::SetNormalFileSaving(true);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/hell.3ds", &scene);

	CamPath = scene->GetCurve("CamPath");
	cam = const_cast<Camera*>(scene->GetActiveCamera());
	cam->SetCameraPath(CamPath, 0, 0, 30000);
	cam->SetClippingPlanes(1.0f, 100000.0f);

	cam2 = scene->GetCamera("Camera02");
	cam3 = scene->GetCamera("Camera03");
	cam2->SetClippingPlanes(1.0f, 100000.0f);
	cam3->SetClippingPlanes(1.0f, 100000.0f);

	Thunder = scene->GetLight("Omni01");
	Thunder->SetIntensity(0.0f);

	ThundEnv[0] = scene->GetObject("b1");
	ThundEnv[1] = scene->GetObject("b2");
	ThundEnv[2] = scene->GetObject("b3");
	ThundEnv[3] = scene->GetObject("b4");
	ThundEnv[4] = scene->GetObject("b5");
	for(int i=0; i<5; i++) {
		scene->RemoveObject(ThundEnv[i]);
	}

	Stones[0] = scene->GetObject("Box01");
	Stones[1] = scene->GetObject("Box02");
	Stones[2] = scene->GetObject("Box03");
	Stones[3] = scene->GetObject("Box04");
	Stones[4] = scene->GetObject("Box05");
	for(int i=0; i<5; i++) {
		scene->RemoveObject(Stones[i]);
		Stones[i]->SetShadingMode(FlatShading);
	}

	Rings[0] = scene->GetObject("Tube01");
	Rings[1] = scene->GetObject("Tube02");
	Rings[2] = scene->GetObject("Tube03");
	Rings[3] = scene->GetObject("Tube04");
	Rings[4] = scene->GetObject("Tube05");
	for(int i=0; i<5; i++) {
		scene->RemoveObject(Rings[i]);
	}

	// Load Credits
	dbgtex = gc->texman->AddTexture("data/textures/psys02.jpg");
	CredNuc[0] = gc->texman->AddTexture("data/textures/credits/nuc1.jpg");
	CredNuc[1] = gc->texman->AddTexture("data/textures/credits/nuc2.jpg");
	CredNuc[2] = gc->texman->AddTexture("data/textures/credits/nuc3.jpg");
	CredNuc[3] = gc->texman->AddTexture("data/textures/credits/nuc4.jpg");
	CredNuc[4] = gc->texman->AddTexture("data/textures/credits/nuc5.jpg");
	CredNuc[5] = gc->texman->AddTexture("data/textures/credits/nuc6.jpg");
	CredNuc[6] = gc->texman->AddTexture("data/textures/credits/nuc7.jpg");

	CredRaw[0] = gc->texman->AddTexture("data/textures/credits/raw1.jpg");
	CredRaw[1] = gc->texman->AddTexture("data/textures/credits/raw2.jpg");
	CredRaw[2] = gc->texman->AddTexture("data/textures/credits/raw3.jpg");
	CredRaw[3] = gc->texman->AddTexture("data/textures/credits/raw4.jpg");
	CredRaw[4] = gc->texman->AddTexture("data/textures/credits/raw5.jpg");
	CredRaw[5] = gc->texman->AddTexture("data/textures/credits/raw6.jpg");
	CredRaw[6] = gc->texman->AddTexture("data/textures/credits/raw7.jpg");

	CredAmi[0] = gc->texman->AddTexture("data/textures/credits/am1.jpg");
	CredAmi[1] = gc->texman->AddTexture("data/textures/credits/am2.jpg");
	CredAmi[2] = gc->texman->AddTexture("data/textures/credits/am3.jpg");
	CredAmi[3] = gc->texman->AddTexture("data/textures/credits/am4.jpg");
	CredAmi[4] = gc->texman->AddTexture("data/textures/credits/am5.jpg");
	CredAmi[5] = gc->texman->AddTexture("data/textures/credits/am6.jpg");
	CredAmi[6] = gc->texman->AddTexture("data/textures/credits/am7.jpg");

	CredAmv[0] = gc->texman->AddTexture("data/textures/credits/amv1.jpg");
	CredAmv[1] = gc->texman->AddTexture("data/textures/credits/amv2.jpg");
	CredAmv[2] = gc->texman->AddTexture("data/textures/credits/amv3.jpg");
	CredAmv[3] = gc->texman->AddTexture("data/textures/credits/amv4.jpg");
	CredAmv[4] = gc->texman->AddTexture("data/textures/credits/amv5.jpg");
	CredAmv[5] = gc->texman->AddTexture("data/textures/credits/amv6.jpg");
	CredAmv[6] = gc->texman->AddTexture("data/textures/credits/amv7.jpg");

	Credits = new Object(gc);
	Credits->CreatePlane(1.7f, 0);
	Credits->Scale(1.0f, 0.3f, 1.0f);

	Blood = scene->GetObject("Blood");
	scene->RemoveObject(Blood);
	Grail = scene->GetObject("Object13");
	//scene->RemoveObject(Grail);
}

HellPart::~HellPart() {
	delete scene;
}

void HellPart::MainLoop() {
	dword msec = timer.GetMilliSec();
	float t = msec / 1000.0f;

	// deform blood surface
	Vertex *BloodVerts = Blood->GetTriMesh()->GetModVertexArray();
	dword VertexCount = Blood->GetTriMesh()->GetVertexCount();

	for(dword i=0; i<VertexCount; i++) {
		Vector2 pos2d(BloodVerts[i].pos.x, BloodVerts[i].pos.z);
		float dist = pos2d.Length();
		BloodVerts[i].pos.y = (1.5f * sinf(dist - t*3.0f)) * (dist > 0.0f ? (1.0f / (dist*1.5f)) : 1.0f);
	}
	Blood->GetTriMesh()->CalculateNormals();

	
	////////////////////////

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	scene->SetActiveCamera(cam);

	if(t >= 18.0f && t < 22.0f) {
		scene->SetActiveCamera(cam2);
		Credits->material.SetTexture(CredNuc[(int)(t * 20.0f) % 7], TextureMap);
	}
	if(t >= 24.0f && t < 28.0f) {
		scene->SetActiveCamera(cam3);
		Credits->material.SetTexture(CredRaw[(int)(t * 20.0f) % 7], TextureMap);
	}
	if(t >= 30.0f && t < 34.0f) {
		scene->SetActiveCamera(cam2);
		Credits->material.SetTexture(CredAmi[(int)(t * 20.0f) % 7], TextureMap);
	}
	if(t >= 36.0f && t < 40.0f) {
		scene->SetActiveCamera(cam3);
		Credits->material.SetTexture(CredAmv[(int)(t * 20.0f) % 7], TextureMap);
	}

	cam->FollowPath(msec);

	gc->SetAmbientLight(Color(0.5f));

	scene->Render();

	for(int i=0; i<5; i++) {
		Matrix4x4 tmat = Stones[i]->TransMat, rmat = Stones[i]->GRotMat;
		Stones[i]->Translate(0.0f, sinf(t) * 54.0f, 0.0f);
		Stones[i]->GlobalRotate(0.0f, t/4.0f, 0.0f);
		
		Stones[i]->Render();
		
		Stones[i]->TransMat = tmat;
		Stones[i]->GRotMat = rmat;
	}

	float Rot = t / 2.0f;

	for(int i=0; i<5; i++) {
		Rings[i]->ResetRotation();
	}

	Rings[0]->Rotate(Rot, 0.0f, 0.0f);

	Rings[1]->Rotate(0.0f, 0.0f, Rot);

	Vector3 axis(1.0f, 0.0f, 0.0f);
	axis.Rotate(0.0f, 0.0f, QuarterPi);
	Rings[2]->Rotate(0.0f, 0.0f, QuarterPi);
	Rings[2]->Rotate(axis, Rot);

	axis = Vector3(0.0f, 0.0f, 1.0f);
	axis.Rotate(QuarterPi, 0.0f, 0.0f);
	Rings[3]->Rotate(QuarterPi, 0.0f, 0.0f);
	Rings[3]->Rotate(axis, Rot);

	Rings[4]->Rotate(Rot, 0.0f, Rot);

	gc->SetZWrite(false);
	for(int i=0; i<5; i++) {
		Rings[i]->Render();
	}
	gc->SetZWrite(true);

	Light *light = scene->GetLight("Spot01");
	light->SetRange(100000.0f);
	light->SetIntensity(0.5f);
	light->SetLight(0, gc);
	Blood->Render();
	light->SetRange(100.0f);
	light->SetLight(0, gc);

	gc->SetAmbientLight(Color(0.0f));

	if((t >= 18.0f && t < 22.0f) || (t >= 24.0f && t < 28.0f) || (t >= 30.0f && t < 34.0f) || (t >= 36.0f && t < 40.0f)) {
		gc->SetAlphaBlending(true);
		gc->SetLighting(false);
		gc->SetZBuffering(false);
		gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);
		gc->SetMaterial(Material(1.0f, 1.0f, 1.0f));
		gc->SetTexture(0, Credits->material.Maps[TextureMap]);
		gc->SetTexture(1, 0);
		gc->SetTextureCoordIndex(0, 0);

		gc->SetTextureStageColor(0, TexBlendSelectArg1, TexArgTexture, TexArgCurrent);
		gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgTexture, TexArgCurrent);
		Matrix4x4 WorldMat;
		WorldMat.Scale(1.0f, 0.3f, 1.0f);
		gc->SetWorldMatrix(WorldMat);
		Matrix4x4 ViewMat;
		ViewMat.Translate(0.0f, 0.0f, 2.0f);
		gc->SetViewMatrix(ViewMat);
		Matrix4x4 ProjMat, OldProj;
		OldProj = gc->GetProjectionMatrix();
		float aspect = (float)gc->ContextParams.x / (float)gc->ContextParams.y;
		CreateProjectionMatrix(&ProjMat, QuarterPi, aspect, 1.0f, 10.0f);
		gc->SetProjectionMatrix(ProjMat);

        Credits->RenderBare();

		gc->SetProjectionMatrix(OldProj);
		gc->SetLighting(true);
		gc->SetZBuffering(true);
		gc->SetAlphaBlending(false);
	}
}