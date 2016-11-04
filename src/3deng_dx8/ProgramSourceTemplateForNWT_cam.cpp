#include "nwt/startup.h"
#include "nwt/nucwin.h"
#include "3deng_dx8/3deng.h"

int ScreenX;
int ScreenY;

Widget *win;
Engine3D eng3d;
GraphicsContext *gc;

Scene *scene;

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

	SceneLoader::SetGraphicsContext(gc);
	SceneLoader::SetDataPath("data/");
	SceneLoader::LoadScene("data/test_scene.3ds", &scene);

	return true;
}

void MainLoop() {
	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	scene->Render();

	gc->Flip();
}

void CleanUp() {
	delete scene;
}

///////// handlers /////////
int KeyHandler(Widget *win, int key) {

	switch(key) {
	case VK_ESCAPE:
		NWCloseWindow(win);
		break;

	case 'a':
	case 'A':
		if(scene) {
			Camera *cam;
			if((cam = scene->GetActiveCamera())) {
				cam->Zoom(0.05f);
			}
		}
		break;

	case 'z':
	case 'Z':
		if(scene) {
			Camera *cam;
			if((cam = scene->GetActiveCamera())) {
				cam->Zoom(-0.05f);
			}
		}
		break;

	}
	return 0;
}

int CloseHandler(Widget *win, int arg) {
	CleanUp();
	return 0;
}

int WheelHandler(Widget *win, int x, int y, int rot) {
	float tr = rot > 0 ? 0.05f : -0.05f;

	Camera *cam;
	if(scene) {
		if((cam = scene->GetActiveCamera())) {
            cam->Zoom(tr);
		}
	}
	
	return 0;
}

int MouseHandler(Widget *win, int x, int y, bool left, bool middle, bool right) {
	static POINT PrevPos;
	POINT MoveDiff;

	if(left) {
	}
	
	if(right) {
		MoveDiff.x = x - PrevPos.x;
		MoveDiff.y = y - PrevPos.y;
		float xangle = (float)MoveDiff.x / 50.0f;
		float dy = (float)MoveDiff.y;

		Camera *cam;
		if(scene) {
			if((cam = scene->GetActiveCamera())) {
                cam->Rotate(0.0f, xangle, 0.0f);
				cam->SetPosition(cam->GetPosition() + Vector3(0.0f, dy, 0.0f));
			}
		}
	}

	if(middle) {
	}

	PrevPos.x = x;
	PrevPos.y = y;

	return 0;
}