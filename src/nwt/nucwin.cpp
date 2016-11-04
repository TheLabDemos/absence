#include "nucwin.h"
#include <windows.h>
#include <winuser.h>

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif	// WM_MOUSEWHEEL

MainLoopMode loopmode;
char *Arguments;

int (*KeyHandlerFunc)(Widget*, int) = 0;
int (*CloseHandlerFunc)(Widget*, int) = 0;
int (*MouseHandlerFunc)(Widget*, int, int, bool, bool, bool) = 0;
int (*MouseUpHandlerFunc)(Widget*, int, int, bool, bool, bool) = 0;
int (*MouseWheelHandlerFunc)(Widget*, int, int, int) = 0;
int (*WinMoveHandlerFunc)(Widget*, int, int) = 0;
int (*PaintHandlerFunc)(Widget*) = 0;

void (*RealTimeLoopFunc)() = 0;

void SetHandler(HandlerType htype, int (*Handler)(Widget*, int)) {
	switch(htype) {
	case HANDLER_KEY:
		KeyHandlerFunc = Handler;
		break;

	case HANDLER_CLOSE:
		CloseHandlerFunc = Handler;
		break;

	case HANDLER_MOUSE:
		MouseHandlerFunc = (int (*)(Widget*, int, int, bool, bool, bool))Handler;
		break;

	case HANDLER_MOUSEUP:
		MouseUpHandlerFunc = (int (*)(Widget*, int, int, bool, bool, bool))Handler;
		break;

	case HANDLER_WHEEL:
		MouseWheelHandlerFunc = (int (*)(Widget*, int, int, int))Handler;
		break;

	case HANDLER_WINMOVE:
		WinMoveHandlerFunc = (int (*)(Widget*, int, int))Handler;
		break;

	case HANDLER_PAINT:
		PaintHandlerFunc = (int (*)(Widget*))Handler;
		break;

	default: break;
	}
}

void SetMainLoopFunc(void (*func)()) {
	RealTimeLoopFunc = func;
}

int NWMainLoop(MainLoopMode mode) {

	MSG msg;
	loopmode = mode;

	if(mode == RealTimeLoop) {
		while(1) {
			if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				if(msg.message == WM_QUIT) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				if(RealTimeLoopFunc) RealTimeLoopFunc();
			}
		}
	} else {
		while(GetMessage(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

bool NWCheckForMessages() {
	MSG msg;
	return (bool)PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
}


LRESULT CALLBACK MainHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
	case WM_KEYDOWN:
		if(KeyHandlerFunc) return KeyHandlerFunc(hWnd, (int)wParam);
		break;

	case WM_CLOSE:
		if(CloseHandlerFunc) CloseHandlerFunc(0, 0);
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		return 0;
		break;

	case WM_MOUSEMOVE:
		if(MouseHandlerFunc) MouseHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), (bool)(wParam & MK_LBUTTON), (bool)(wParam & MK_MBUTTON), (bool)(wParam & MK_RBUTTON));
		break;

	case WM_LBUTTONDOWN:
		if(MouseHandlerFunc) MouseHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), true, false, false);
		break;

	case WM_MBUTTONDOWN:
		if(MouseHandlerFunc) MouseHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), false, true, false);
		break;

	case WM_RBUTTONDOWN:
		if(MouseHandlerFunc) MouseHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), false, false, true);
		break;

	case WM_LBUTTONUP:
		if(MouseUpHandlerFunc) MouseUpHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), true, false, false);
		break;

	case WM_MBUTTONUP:
		if(MouseUpHandlerFunc) MouseUpHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), false, true, false);
		break;

	case WM_RBUTTONUP:
		if(MouseUpHandlerFunc) MouseUpHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), false, false, true);
		break;

	case WM_MOUSEWHEEL:
		if(MouseWheelHandlerFunc) MouseWheelHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam), (short)HIWORD(wParam));
		break;

	case WM_MOVE:
		if(WinMoveHandlerFunc) WinMoveHandlerFunc(hWnd, LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_PAINT:
		if(PaintHandlerFunc) PaintHandlerFunc(hWnd);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Point NWGetScreenSize() {
	return Point(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

void NWCloseWindow(Widget *win) {
	PostMessage(win, WM_CLOSE, 0, 0);
}

std::string NWFileSaveChooser(Widget *win, const char *title, const char *filetypes, const char *defext) {
	
	char fname[512];
	fname[0] = 0;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = win;
	ofn.lpstrFilter = filetypes;
	ofn.lpstrFile = fname;
	ofn.nMaxFile = 512;
	ofn.lpstrTitle = title;
	ofn.lpstrDefExt = defext;
	ofn.Flags = OFN_PATHMUSTEXIST;

	if(!GetSaveFileName(&ofn)) return "";

	return fname;
}