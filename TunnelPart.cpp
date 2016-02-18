#include "tunnelpart.h"

TunnelPart::TunnelPart(GraphicsContext *gc) {
	this->gc = gc;

	SceneLoader::SetNormalFileSaving(true);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/tunnel.3ds", &scene);

	CamPath = scene->GetCurve("Line01");
	TargPath = scene->GetCurve("Line02");

	CamPath->SetArcParametrization(true);
	TargPath->SetArcParametrization(true);
    
	cam = scene->GetCamera("Camera02");
	cam->SetCameraPath(CamPath, TargPath, 0, 30000);
}

TunnelPart::~TunnelPart() {
	delete scene;
}

void TunnelPart::MainLoop() {
	dword msec = timer.GetMilliSec();
	float t = msec / 1000.0f;

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	cam->FollowPath(msec);

	scene->Render();
}