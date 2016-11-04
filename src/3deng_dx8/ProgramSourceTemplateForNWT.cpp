#include "nwt/startup.h"
#include "nwt/nucwin.h"
#include "3deng/3deng.h"

int ScreenX;
int ScreenY;

Widget *win;
Engine3D eng3d;
GraphicsContext *gc;

bool Init();
void MainLoop();
void CleanUp();
int KeyHandler(Widget *win, int key);
int CloseHandler(Widget *win, int arg);

int main() {

	win = NWCreateWindow("The Lab Demos", NWT_CENTERX, NWT_CENTERY, 100, 50, 0);
	SetMainLoopFunc(MainLoop);
	SetHandler(HANDLER_KEY, KeyHandler);
	SetHandler(HANDLER_CLOSE, CloseHandler);

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

	return true;
}

void MainLoop() {
}

void CleanUp() {
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