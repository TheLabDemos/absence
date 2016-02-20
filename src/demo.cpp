#include "nwt/startup.h"
#include "nwt/nucwin.h"
#include "3deng_dx8/3deng.h"
#include "demosystem/demosys.h"
#include "fmod.h"

// parts
#include "beginpart.h"
#include "dungeonpart.h"
#include "treepart.h"
#include "tunnelpart.h"
#include "hellpart.h"
#include "greetspart.h"
#include "demonpart.h"

int ScreenX;
int ScreenY;

Widget *win;
Engine3D eng3d;
GraphicsContext *gc;
DemoSystem *demo;
FMUSIC_MODULE *mod;


// parts
DungeonPart *dungeonpart;
BeginPart *beginpart;
TreePart *treepart;
TunnelPart *tunnelpart;
HellPart *hellpart;
GreetsPart *greetspart;
DemonPart *demonpart;

bool Init();
void MainLoop();
void CleanUp();
int KeyHandler(Widget *win, int key);
int CloseHandler(Widget *win, int arg);
int MouseHandler(Widget *win, int x, int y, bool left, bool middle, bool right);
int WheelHandler(Widget *win, int x, int y, int rot);


int main() {

	win = NWCreateWindow("The Lab Demos", NWT_CENTERX, NWT_CENTERY, 100, 50, 0);
	SetMainLoopFunc(MainLoop);
	SetHandler(HANDLER_KEY, KeyHandler);
	SetHandler(HANDLER_CLOSE, CloseHandler);
	
	SetHandler(HANDLER_MOUSE, (HandlerFunctionPtr)MouseHandler);
	SetHandler(HANDLER_WHEEL, (HandlerFunctionPtr)WheelHandler);


	if(!Init()) return 0;

	return NWMainLoop(RealTimeLoop);
}

bool Init() {

	ContextInitParameters cip;
	try {
		cip = eng3d.LoadContextParamsConfigFile("n3dinit.conf");
		gc = eng3d.CreateGraphicsContext(win, 0, &cip);
	}
	catch(EngineInitException except) {
		MessageBox(win, except.GetReason().c_str(), "Fatal Error", MB_OK | MB_ICONSTOP);
		return false;
	}
	ScreenX = cip.x;
	ScreenY = cip.y;
	NWResize(win, ScreenX, ScreenY);
	NWResizeClientArea(win, WS_OVERLAPPEDWINDOW);

	ShowCursor(false);

	// Loading pics....
	Texture *loading[9];
	loading[0] = gc->texman->AddTexture("data/textures/Loading/loading0.jpg");
	loading[1] = gc->texman->AddTexture("data/textures/Loading/loading1.jpg");
	loading[2] = gc->texman->AddTexture("data/textures/Loading/loading2.jpg");
	loading[3] = gc->texman->AddTexture("data/textures/Loading/loading3.jpg");
	loading[4] = gc->texman->AddTexture("data/textures/Loading/loading4.jpg");
	loading[5] = gc->texman->AddTexture("data/textures/Loading/loading5.jpg");
	loading[6] = gc->texman->AddTexture("data/textures/Loading/loading6.jpg");
	loading[7] = gc->texman->AddTexture("data/textures/Loading/loading7.jpg");
	loading[8] = gc->texman->AddTexture("data/textures/Loading/loading8.jpg");

	demo = new DemoSystem(gc);
	SceneLoader::SetGraphicsContext(gc);

	Object *quad = new Object(gc);
	quad->CreatePlane(4.0f, 0);
	quad->Scale(1.3333f, 1.0f, 1.0f);
	quad->material = Material(1.0f, 1.0f, 1.0f);

	Matrix4x4 ViewMat;
	ViewMat.Translate(0.0f, 0.0f, 3.6f);
	gc->SetViewMatrix(ViewMat);
	
	gc->SetZBuffering(false);
	gc->SetLighting(false);

	quad->material.SetTexture(loading[0], TextureMap);
	quad->Render();
	gc->Flip();
	beginpart = new BeginPart(gc);
	beginpart->SetTimingRel(0, 11000);
	demo->AddPart(beginpart);

	quad->material.SetTexture(loading[1], TextureMap);
	quad->Render();
	gc->Flip();
	dungeonpart = new DungeonPart(gc);
	dungeonpart->SetTimingRel(11000, 75000);
	demo->AddPart(dungeonpart);

	quad->material.SetTexture(loading[2], TextureMap);
	quad->Render();
	gc->Flip();
	treepart = new TreePart(gc);
	treepart->SetTimingRel(85900, 40000);
    demo->AddPart(treepart);

	quad->material.SetTexture(loading[3], TextureMap);
	quad->Render();
	gc->Flip();
	tunnelpart = new TunnelPart(gc);
	tunnelpart->SetTimingRel(125900, 30000);
	demo->AddPart(tunnelpart);

	quad->material.SetTexture(loading[4], TextureMap);
	quad->Render();
	gc->Flip();
	hellpart = new HellPart(gc);
	hellpart->SetTimingRel(155900, 40000);
	demo->AddPart(hellpart);

	quad->material.SetTexture(loading[5], TextureMap);
	quad->Render();
	gc->Flip();
	greetspart = new GreetsPart(gc);
	greetspart->SetTimingRel(196000, 12000);
	demo->AddPart(greetspart);

	quad->material.SetTexture(loading[6], TextureMap);
	quad->Render();
	gc->Flip();
	demonpart = new DemonPart(gc);
	demonpart->SetTimingRel(208000, 2000);
	demo->AddPart(demonpart);

	quad->material.SetTexture(loading[7], TextureMap);
	quad->Render();
	gc->Flip();
	FSOUND_SetHWND(win);
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
	FSOUND_SetBufferSize(200);
	FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS);
	mod = FMUSIC_LoadSong("data/GOTH03.XM");
	FMUSIC_SetMasterVolume(mod, 250);

	quad->material.SetTexture(loading[8], TextureMap);
	quad->Render();
	gc->Flip();
	gc->SetBackfaceCulling(true);
	gc->SetZBuffering(true);
	gc->SetLighting(true);

	Sleep(800);
	
	demo->Run();

	FMUSIC_PlaySong(mod);

	return true;
}

void MainLoop() {
	demo->Update();
	gc->Flip();
}

void CleanUp() {
	ShowCursor(true);
	FMUSIC_FreeSong(mod);
	FSOUND_Close();
	delete demo;
}

///////// handlers /////////
int KeyHandler(Widget *win, int key) {
	switch(key) {
	case VK_ESCAPE:
		NWCloseWindow(win);
		break;

	}
	return 0;
}

int CloseHandler(Widget *win, int arg) {
	CleanUp();
	return 0;
}

int WheelHandler(Widget *win, int x, int y, int rot) {
/*	float tr = rot > 0 ? 0.05f : -0.05f;

	Camera *cam = const_cast<Camera*>(demo->GetActivePath()->GetScene()->GetActiveCamera());
	cam->SetCameraPath(0, 0, 0, 0);

	cam->Zoom(tr);
*/	
	return 0;
}


int MouseHandler(Widget *win, int x, int y, bool left, bool middle, bool right) {
/*	static POINT PrevPos;
	POINT MoveDiff;

	if(right) {
		MoveDiff.x = x - PrevPos.x;
		MoveDiff.y = y - PrevPos.y;
		float xangle = (float)MoveDiff.x / 50.0f;
		float dy = (float)MoveDiff.y;

		Camera *cam = const_cast<Camera*>(demo->GetActivePath()->GetScene()->GetActiveCamera());
		cam->SetCameraPath(0, 0, 0, 0);

		cam->Rotate(0.0f, xangle, 0.0f);
		cam->SetPosition(cam->GetPosition() + Vector3(0.0f, dy, 0.0f));
	}


	PrevPos.x = x;
	PrevPos.y = y;
*/
	return 0;
}