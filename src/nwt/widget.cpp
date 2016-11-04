#include "widget.h"
#include "nucwin.h"

Widget *NWCreateWindow(const char *name, int x, int y, int xsz, int ysz, uint16 flags) {
	if(x == NWT_CENTERX) {
		x = (GetSystemMetrics(SM_CXSCREEN) - xsz) >> 1;
	}
	if(y == NWT_CENTERY) {
		y = (GetSystemMetrics(SM_CYSCREEN) - ysz) >> 1;
	}

	HINSTANCE AppInstance = GetModuleHandle(0);
	unsigned long ex_style = flags & NWT_WIN_TOPMOST ? WS_EX_TOPMOST : 0;
	return CreateWindowEx(ex_style, "NucWin", name, WS_OVERLAPPEDWINDOW | WS_VISIBLE, x, y, xsz, ysz, 0, 0, AppInstance, 0);
}


void NWResize(Widget *wdg, int nx, int ny) {
	int xpos = (GetSystemMetrics(SM_CXSCREEN) - nx) >> 1;
	int ypos = (GetSystemMetrics(SM_CYSCREEN) - ny) >> 1;
	MoveWindow(wdg, xpos, ypos, nx, ny, true);
}

void NWResizeClientArea(Widget *wdg, uint32 WinStyle) {
    RECT rect;
	GetWindowRect(wdg, &rect);
	AdjustWindowRectEx(&rect, WinStyle, false, 0);
	MoveWindow(wdg, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
}

void NWSetWindowPos(Widget *wdg, const Point &pos) {
	Point sz = NWGetWindowSize(wdg);
	MoveWindow(wdg, pos.x, pos.y, sz.x, sz.y, true);
}

Point NWGetWindowPos(Widget *wdg) {
	Rect rect;
	GetWindowRect(wdg, &rect);

	return Point(rect.left, rect.top);
}

Point NWGetWindowSize(Widget *wdg) {
	Rect rect;
	GetWindowRect(wdg, &rect);

	return Point(rect.right - rect.left, rect.bottom - rect.top);
}
	
