#ifndef _STARTUP_H_
#define _STARTUP_H_

#include <windows.h>
#include "nucwin.h"

int main();
void RegisterWindowClasses();

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR CmdLine, int ShowCmd) {
	Arguments = CmdLine;
	RegisterWindowClasses();
	return main();
}

void RegisterWindowClasses() {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hInstance = GetModuleHandle(0);
	wc.lpfnWndProc = MainHandler;
	wc.lpszClassName = "NucWin";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);
}

#endif	// _STARTUP_H_