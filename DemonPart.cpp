#include "demonpart.h"
#include "nwt/widget.h"
#include "fmod.h"

extern Widget *win;
extern FMUSIC_MODULE *mod;

DemonPart::DemonPart(GraphicsContext *gc) {
	this->gc = gc;

	//SceneLoader::SetNormalFileSaving(true);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/demon.3ds", &scene);

	curve = scene->GetCurve("Line01");
	curve->SetArcParametrization(true);
	cam = scene->GetCamera("Camera01");
	cam->SetCameraPath(curve, 0, 0, 1500);
}

DemonPart::~DemonPart() {
	delete scene;
}

void DemonPart::MainLoop() {
	dword msec = timer.GetMilliSec();
	float t = msec / 1000.0f;

	FMUSIC_SetMasterVolume(mod, (1500 - msec) / 6);

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	if(t > 1.5f) {
		PostMessage(win, WM_CLOSE, 0, 0);
	}

	cam->FollowPath(msec);
	scene->Render();
}