#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <windows.h>
#include "typedefs.h"

struct Point;
struct Rect;

#define NWT_CENTERX	-0xdead
#define NWT_CENTERY -0xbeef

typedef HWND__ Widget;

#define NWT_WIN_TOPMOST	1

Widget *NWCreateWindow(const char *name, int x, int y, int xsz, int ysz, uint16 flags);
void NWResize(Widget *wdg, int nx, int ny);
void NWResizeClientArea(Widget *wdg, uint32 WinStyle);

void NWSetWindowPos(Widget *wdg, const Point &pos);

Point NWGetWindowPos(Widget *wdg);
Point NWGetWindowSize(Widget *wdg);

#endif	// _WIDGET_H_