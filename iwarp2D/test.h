#pragma once 
#include "full.h"


void test(){
}



//struct ValueControlData{
//	HWND hwnd_scroll;
//	static void first_init(){
//	}
//	ValueControlWindow(HWND parent, int x, int y, int w, int h, int id, int value = 0) :x(x), y(y), w(w), h(h){
//		HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, classname, "", WS_CHILD | WS_VISIBLE,
//			x, y, w, h, hwnd, (HMENU)id, hInst, 0);
//		SetWindowLong(hwnd, GWL_USERDATA, (LONG)this);
//	}
//
//	void update_text(){
//		sprintf_s(buf, "r=%d", value);
//		SetWindowText(edit_deform_radius, buf);
//		InvalidateRect(hwnd_app, 0, 0);
//	}
//};
//
//LRESULT CALLBACK WP_ValueControl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//
//	static const char * classname = "ValueControl";
//	ValueControlData * obj = (ValueControlData*)GetWindowLong(hWnd, GWL_USERDATA);
//	switch (message)
//	{
//	case WM_APP:
//	{
//		WNDCLASSEX wcex = {
//			sizeof(WNDCLASSEX),//UINT        cbSize;
//			///* Win 3.x */
//			CS_HREDRAW | CS_VREDRAW | CS_ENABLE,//UINT        style;
//			WP_ValueControl,//WNDPROC     lpfnWndProc;
//			0,//int         cbClsExtra;
//			sizeof(HWND),//int         cbWndExtra;
//			hInst,//HINSTANCE   hInstance;
//			0,//HICON       hIcon;
//			LoadCursor(NULL, IDC_ARROW),//HCURSOR     hCursor;
//			(HBRUSH)(COLOR_BTNFACE + 1),//HBRUSH      hbrBackground;
//			0,//LPCSTR      lpszMenuName;
//			classname,//LPCSTR      lpszClassName;
//			0//HICON       hIconSm;
//		};
//		RegisterClassEx(&wcex);
//	}
//		break;
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hWnd, &ps);
//		EndPaint(hWnd, &ps);
//	}
//	return 0;
//	case WM_CREATE:
//	{
//		HWND parent=GetParent(hWnd);
//		RECT rc;
//		GetClientRect(hWnd, &rc);
//		obj->hwnd_scroll = CreateWindow(
//			TEXT("SCROLLBAR"), TEXT(""),
//			WS_CHILD | WS_VISIBLE | SBS_HORZ,
//			(rc.right - rc.left)*0.25, 0, (rc.right - rc.left), (rc.bottom-rc.top),
//			hWnd, (HMENU)0,
//			((LPCREATESTRUCT)(lParam))->hInstance, NULL
//			);
//		scrdr.cbSize = sizeof(SCROLLINFO);
//		scrdr.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
//		scrdr.nMin = 0;	scrdr.nMax = MAX_DEFORM_AREA_RADIUS;
//		scrdr.nPos = lParam;
//		scrdr.nPage = 1;
//		SetScrollInfo(scroll_deform_radius, SB_CTL, &scrdr, TRUE);
//		scrdr.fMask = SIF_POS;
//	}
//	break;
//	case WM_DESTROY:
//		break;
//	}
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}

