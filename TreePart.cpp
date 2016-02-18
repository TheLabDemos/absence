#include "treepart.h"

TreePart::TreePart(GraphicsContext *gc) {
	this->gc = gc;

	scene = new Scene(gc);
/*
	LeavesParticle = gc->texman->AddTexture("data/textures/leaf.png");

	leaves = new ParticleSystem(gc);
	leaves->SetPosition(Vector3(0.0f, 800.0f, 0.0f));
	leaves->SetGravitualForce(0.8f);
	leaves->SetFriction(0.05f);
	leaves->SetInitialColor(0.6f, 0.5f, 0.5f);
	leaves->SetDeathColor(0.6f, 0.5f, 0.5f);
	leaves->SetParticleSize(6.0f);
	leaves->SetParticleLife(1000);
	leaves->SetSpawnRadius(1500.0f);
	leaves->SetSpawnRate(20.0f);
	leaves->SetTexture(LeavesParticle);
	leaves->SetShootDirection(Vector3(0.0f, -10.0f, 0.0f));
	leaves->SetMaxDispersionAngle(QuarterPi / 2.0f);
	leaves->SetBlendingMode(BLEND_SRCALPHA, BLEND_INVSRCALPHA);
*/
	WispParticle = gc->texman->AddTexture("data/textures/psys02.jpg");
    
	for(int i=0; i<4; i++) {
        WispParticles[i] = new ParticleSystem(gc);
		WispParticles[i]->SetGravitualForce(0.8f);
		WispParticles[i]->SetFriction(0.06f);
		WispParticles[i]->SetInitialColor(0.7f, 0.8f, 1.0f);
		WispParticles[i]->SetDeathColor(0.2f, 0.2f, 0.5f);
		WispParticles[i]->SetParticleSize(7.5f);
		WispParticles[i]->SetParticleLife(15);
		WispParticles[i]->SetSpawnRadius(0.2f);
		WispParticles[i]->SetSpawnRate(4.0f);
		WispParticles[i]->SetTexture(WispParticle);
		WispParticles[i]->SetShootDirection(Vector3(0.0f, 0.0f, 0.0f));
		WispParticles[i]->SetMaxDispersionAngle(QuarterPi / 2.0f);
		WispParticles[i]->SetBlendingMode(BLEND_ONE, BLEND_ONE);
		WispParticles[i]->SetSpawningDifferenceDispersion(1.0f);
	}
	
	SceneLoader::SetNormalFileSaving(true);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/tree2.3ds", &scene);

	Trees = scene->GetObject("Trees");
	scene->RemoveObject(Trees);

	for(int i=0; i<3; i++) {
		dummy[i] = new Camera;
	}

	Object *graves = scene->GetObject("Graves");
	//graves->SetShadowCasting(true);
    
	WispPath[0] = scene->GetCurve("wisp1");
	WispPath[1] = scene->GetCurve("wisp2");
	WispPath[2] = scene->GetCurve("wisp3");
	
	WispPath[0]->SetArcParametrization(true);
	WispPath[1]->SetArcParametrization(true);
	WispPath[2]->SetArcParametrization(true);

	dummy[0]->SetCameraPath(WispPath[0], 0, 0, 8000);
	dummy[1]->SetCameraPath(WispPath[1], 0, 0, 7000);
	dummy[2]->SetCameraPath(WispPath[2], 0, 0, 10000);
	
	lights[0] = scene->GetLight("wispl1");
	lights[1] = scene->GetLight("wispl2");
	lights[2] = scene->GetLight("wispl3");
	lights[3] = scene->GetLight("wispl04");

	for(int i=0; i<3; i++) {
		lights[i]->SetRange(250.0f);
		lights[i]->SetColor(Color(0.6f, 0.6f, 0.9f));
	}
	lights[3]->SetColor(Color(0.6f, 0.6f, 0.9f));

	CamPath = scene->GetCurve("cpath01");
	TargPath = scene->GetCurve("ctarget01");
	
	CamPath->SetArcParametrization(true);
	TargPath->SetArcParametrization(true);

	cam = scene->GetCamera("Camera02");
	cam->SetClippingPlanes(1.0f, 80000.0f);
	cam->SetCameraPath(CamPath, TargPath, 0, 40000);
	scene->SetActiveCamera(cam);

	//scene->SetShadows(true);
	//lights[3]->SetShadowCasting(true);

	Moon = scene->GetObject("Moon");
	scene->RemoveObject(Moon);

	Stars = scene->GetObject("StarDome");
	scene->RemoveObject(Stars);

}

TreePart::~TreePart() {
	delete scene;
	delete leaves;
}

void TreePart::MainLoop() {
	dword msec = timer.GetMilliSec();
	float t = msec / 1000.0f;

	float start = 10.0f;
	float end = 10500.0f;
	
	gc->D3DDevice->SetRenderState(D3DRS_FOGENABLE, true);
	gc->D3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	gc->D3DDevice->SetRenderState(D3DRS_FOGCOLOR, 0);
    gc->D3DDevice->SetRenderState(D3DRS_FOGSTART, *(dword*)(&start));
	gc->D3DDevice->SetRenderState(D3DRS_FOGEND, *(dword*)(&end));

	cam->FollowPath(msec);
	for(int i=0; i<3; i++) {
		dummy[i]->FollowPath(msec, true);
		lights[i]->SetPosition(dummy[i]->GetPosition());
	}
	lights[3]->SetPosition(cam->GetTargetPosition());

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	scene->Render();
	
	gc->SetZWrite(false);
	Trees->Render();
	gc->SetZWrite(true);

	//for(int i=0; i<3; i++) {
	//	lights[i]->Draw(gc, 20.0f);
	//}
	//lights[3]->Draw(gc, 30.0f);

	gc->D3DDevice->SetRenderState(D3DRS_FOGENABLE, false);

	//leaves->Update(t);
	//leaves->Render();

	Stars->Render();
	Moon->Render();

	for(int i=0; i<4; i++) {
        WispParticles[i]->SetPosition(Vector3(lights[i]->GetPosition()));
		WispParticles[i]->Update(t);
		WispParticles[i]->Render();
	}
}

