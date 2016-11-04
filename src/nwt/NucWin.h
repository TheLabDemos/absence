#ifndef _NUCWIN_H_
#define _NUCWIN_H_

#include <string>
#include "widget.h"

struct Point {
	int x, y;
	inline Point(int x=0, int y=0) {this->x = x; this->y = y;}
};

struct Rect : public RECT {
	inline Rect() {}
	inline Rect(int l, int t, int r, int b) {left=l; top=t; right=r; bottom=b;}
};


#define HandlerFunctionPtr int (*)(Widget*, int)

extern char *Arguments;

enum HandlerType {
	HANDLER_KEY, 
	HANDLER_CLOSE, 
	HANDLER_MOUSE, 
	HANDLER_WHEEL, 
	HANDLER_WINMOVE,
	HANDLER_MOUSEUP,
	HANDLER_PAINT
};
enum MainLoopMode {EventLoop, RealTimeLoop};

void SetHandler(HandlerType htype, int (*Handler)(Widget*, int));

void SetMainLoopFunc(void (*func)());

Point NWGetScreenSize();

int NWMainLoop(MainLoopMode mode = EventLoop);
bool NWCheckForMessages();
LRESULT CALLBACK MainHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void NWCloseWindow(Widget *win);

std::string NWFileSaveChooser(Widget *win, const char *title, const char *filetypes, const char *defext);

#endif	// _NUCWIN_H_