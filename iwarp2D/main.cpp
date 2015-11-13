
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <iostream>
#include <cstdlib>
#include <crtdbg.h>
#include <cstdarg>
#include <tchar.h>
#include <deque>
#include "full.h"
#include "iwarp.h"
#include "test.h"

HINSTANCE hInst;
HWND hwnd_game, hwnd_app;
HANDLE hThread[10];
Color windowcolor = Color(100,100,100,0xff);
DWORD FPS=30;
const short width = 400, height = 400;
static short lastcur[2];
short toolspace = 30;
static HFONT hfont1; 
static HWND combo, edit_deform_amount, edit_deform_radius, scroll_deform_amount, scroll_deform_radius; static int lbtn = 0, rbtn = 0;
enum { ctrl_id_deform_mode = 1, ctrl_id_canvas, ctrl_id_deform_amount, ctrl_id_deform_radius, ctrl_id_deform_amount_ud, ctrl_id_deform_radius_ud };

HANDLE eventRequestRedraw;

static SCROLLINFO scrda, scrdr;
static char buf[256];

RECT rcMain;
MyUtil::PixmapForGDI pxMain;
MyUtil::Pixmap pxBody("picture\\sca1-01.png"), pxSrc(width, height);
SharedVariable<bool> screenlock;
Iwarp<width, height> iwarp;

DWORD progress = 0, lastcount = 0;
void wait_frame(DWORD n, bool init = false){
	DWORD count;
	DWORD progress = 0, lastcount;
	n *= 1000;
	if (lastcount == 0 || init){
		lastcount = timeGetTime();
	}
	progress %= 1000;
	for (;;){
		count = timeGetTime();
		progress += FPS * (count - lastcount);
		lastcount = count;
		if (progress >= n) break;
		Sleep(1);
	}
}



void redraw(){
	SetEvent(eventRequestRedraw);

	/*pxMain.Fill(windowcolor);
	iwarp.redraw(pxMain, true);
	InvalidateRect(hwnd_app, 0, 0);*/
}


LRESULT CALLBACK WndProcCanvas(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int lbtn = 0, rbtn = 0;
	switch (message)
	{
	case WM_KEYDOWN:
	{
		switch (wParam){
		case 'W':
			printf("w\n");
			break;
		}
		break;
	}/*
		  case WM_RBUTTONDOWN:
		  {
		  rbtn++;
		  short x = LOWORD(lParam) *width / (rcMain.right - rcMain.left), y = HIWORD(lParam) * height / (rcMain.bottom - rcMain.top);
		  lastcur[0] = x;
		  lastcur[1] = y;
		  SetCapture(hWnd);

		  }
		  break;
		  case WM_RBUTTONUP:
		  {
		  rbtn = 0;
		  ReleaseCapture();
		  }
		  break;*/
	case WM_LBUTTONDOWN:
	{
		lbtn++;
		short x = LOWORD(lParam) *width / (rcMain.right - rcMain.left), y = HIWORD(lParam) * height / (rcMain.bottom - rcMain.top);
		lastcur[0] = x;
		lastcur[1] = y;
		SetCapture(hWnd);

	}
	break;
	case WM_LBUTTONUP:
	{
		lbtn = 0;
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);
		short x = p.x *width / (rcMain.right - rcMain.left), y = p.y * height / (rcMain.bottom - rcMain.top);

		iwarp.deform(pxSrc, x, y, lastcur[0], lastcur[1]);
		redraw();
		ReleaseCapture();
	}
	break;
	case WM_MOUSEMOVE:
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hWnd, &p);
		short x = p.x *width / (rcMain.right - rcMain.left), y = p.y * height / (rcMain.bottom - rcMain.top);
		if (lbtn){
			iwarp.deform(pxSrc, x, y, lastcur[0], lastcur[1]);
			redraw();
		}
		else{
			InvalidateRect(hWnd, 0, 0);
		}
		lastcur[0] = x;
		lastcur[1] = y;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		screenlock.lockRead();
		if (pxMain.hdc){
			GetClientRect(hWnd, &rcMain);
			StretchBlt(hdc, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top, pxMain.hdc, 0, 0, pxMain.w, pxMain.h, SRCCOPY);
		}
		screenlock.unlockRead();

		short x = lastcur[0] * (rcMain.right - rcMain.left) / width, y = lastcur[1] * (rcMain.bottom - rcMain.top) / height;
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Ellipse(hdc, x - iwarp.values().deform_area_radius, y - iwarp.values().deform_area_radius, x + iwarp.values().deform_area_radius, y + iwarp.values().deform_area_radius);   // 外接矩形の右下隅の y座標

		EndPaint(hWnd, &ps);
	}
	return 0;
	case WM_MOVE:
		GetClientRect(hWnd, &rcMain);
		break;
	case WM_CREATE:
	{
	}
	break;
	case WM_DESTROY:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


void updateParameter(void * param){
	if (param == &iwarp.values().deform_area_radius){
		//iwarp.values().deform_area_radius = x;
		sprintf_s(buf, "r=%d", iwarp.values().deform_area_radius);
		SetWindowText(edit_deform_radius, buf);
		InvalidateRect(hwnd_app, 0, 0);
	}
	else if (param == &iwarp.values().deform_amount){
		sprintf_s(buf, "a=%.3lf", iwarp.values().deform_amount);
		SetWindowText(edit_deform_amount, buf);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_COMMAND:
	{
		switch (LOWORD(wParam)){
		case ctrl_id_deform_mode:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				DeformMode i = (DeformMode)SendMessage(combo, CB_GETCURSEL, 0, 0);
				switch (i){
				case ZERO_BLANK:
					iwarp.compress();
					redraw();
					MessageBox(hWnd, "zeroed value of blank pixel.", "", MB_ICONINFORMATION);
					SendMessage(combo, CB_SETCURSEL, iwarp.values().deform_mode, 0);
					break;
				case SAVE:
				{
					char name[MAX_PATH] = { 0 };
					OPENFILENAME ofn = { 0 };
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFilter = TEXT("Image Warping files {*.iwp}\0*.iwp\0")
						TEXT("All files {*.*}\0*.*\0\0");
					ofn.lpstrFile = name;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
					ofn.lpstrDefExt = "iwp";
					if (GetSaveFileName(&ofn)){
						iwarp.save(name);
						MessageBox(hWnd, "saved.", "", MB_ICONINFORMATION);
						SetWindowText(hWnd, name);
					}
					else{
						MessageBox(hWnd, "not saved.", "", MB_ICONWARNING);
					}
					SendMessage(combo, CB_SETCURSEL, iwarp.values().deform_mode, 0);					
					break;
				}

				case LOAD:
				{
					char name[MAX_PATH] = { 0 };
					OPENFILENAME ofn = { 0 };
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hWnd;
					ofn.lpstrFilter = TEXT("Image Warping files {*.iwp}\0*.iwp\0")
						TEXT("All files {*.*}\0*.*\0\0");
					ofn.lpstrFile = name;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
					ofn.lpstrDefExt = "iwp";
					if (GetOpenFileName(&ofn)){
						iwarp.load(name);
						SetWindowText(hWnd, name);
					}
					else{
					}
					SendMessage(combo, CB_SETCURSEL, iwarp.values().deform_mode, 0);
					break;
				}

				default:
					iwarp.values().deform_mode = i;
					break;
				}
			}
			break;
		}
		break;
	}
	case WM_HSCROLL:
	{
		SCROLLINFO & scr = ((HWND)lParam == scroll_deform_radius ? scrdr : scrda);
		switch (LOWORD(wParam)) {
		case SB_LEFT:
			scr.nPos = scr.nMin;
			break;
		case SB_RIGHT:
			scr.nPos = scr.nMax;
			break;
		case SB_LINELEFT:
			if (scr.nPos) scr.nPos--;
			break;
		case SB_LINERIGHT:
			if (scr.nPos < scr.nMax) scr.nPos++;
			break;
		case SB_PAGELEFT:
			scr.nPos -= scr.nMax / 10;
			break;
		case SB_PAGERIGHT:
			scr.nPos += scr.nMax / 10;
			break;
		case SB_THUMBTRACK:
			//case SB_THUMBPOSITION:
			scr.nPos = HIWORD(wParam);
			break;
		default:
			break;
		}
		SetScrollInfo((HWND)lParam, SB_CTL, &scr, TRUE);
		if ((HWND)lParam == scroll_deform_radius){
			iwarp.values().deform_area_radius = scr.nPos;
			updateParameter(&iwarp.values().deform_area_radius);
		}
		else{
			iwarp.values().deform_amount = (double)scr.nPos*0.001;
			updateParameter(&iwarp.values().deform_amount);
		}
		return 0;
	}
	break;
	case WM_CREATE:
	{
		hfont1 = CreateFont(12, 0, 0, 0,
			FW_NORMAL, FALSE, FALSE, 0,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック");
		{
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW | CS_ENABLE;
			wcex.lpfnWndProc = WndProcCanvas;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInst;
			wcex.hIcon = 0;
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			wcex.lpszMenuName = 0;
			wcex.lpszClassName = "Canvas";
			wcex.hIconSm = 0;
			if (!RegisterClassEx(&wcex)) throw - 1;
			hwnd_app = CreateWindowEx(WS_EX_CLIENTEDGE, "Canvas", "MainCanvas", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
				0, 24, 400, 400, hWnd, (HMENU)ctrl_id_canvas, hInst, 0);
			SetFocus(hwnd_app);
		}

		combo = CreateWindow(
			TEXT("COMBOBOX"), NULL,
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			0, 0, 100, 200, hWnd, (HMENU)ctrl_id_deform_mode,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
			);
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Grow");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Shrink");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Move");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Remove");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Swirl CCW");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Swirl CW");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Colorise");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Zero Blank");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Save");
		SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"Load");
		SendMessage(combo, CB_SETCURSEL, iwarp.values().deform_mode, 0);

		SendMessage(combo, WM_SETFONT, (WPARAM)hfont1, MAKELPARAM(FALSE, 0));
		RECT rc;
		GetWindowRect(combo, &rc);
		int toolheight = rc.bottom - rc.top;

		edit_deform_radius = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			TEXT("STATIC"), NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			100, 0, 50, toolheight, hWnd, (HMENU)ctrl_id_deform_radius,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
			);
		scroll_deform_radius = CreateWindow(
			TEXT("SCROLLBAR"), TEXT(""),
			WS_CHILD | WS_VISIBLE | SBS_HORZ,
			150, 0, 100, toolheight,
			hWnd, (HMENU)ctrl_id_deform_radius,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
			);
		scrdr.cbSize = sizeof(SCROLLINFO);
		scrdr.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		scrdr.nMin = 0;	scrdr.nMax = MAX_DEFORM_AREA_RADIUS;
		scrdr.nPos = iwarp.values().deform_area_radius;
		scrdr.nPage = 1;
		SetScrollInfo(scroll_deform_radius, SB_CTL, &scrdr, TRUE);
		scrdr.fMask = SIF_POS;


		edit_deform_amount = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			TEXT("STATIC"), NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			250, 0, 50, toolheight, hWnd, (HMENU)ctrl_id_deform_amount,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
			);
		scroll_deform_amount = CreateWindow(
			TEXT("SCROLLBAR"), TEXT(""),
			WS_CHILD | WS_VISIBLE | SBS_HORZ,
			300, 0, 100, toolheight,
			hWnd, (HMENU)ctrl_id_deform_amount,
			((LPCREATESTRUCT)(lParam))->hInstance, NULL
			);
		scrda.cbSize = sizeof(SCROLLINFO);
		scrda.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		scrda.nMin = 0;	scrda.nMax = 1000;
		scrda.nPos = iwarp.values().deform_amount*1000;
		scrda.nPage = 1;
		SetScrollInfo(scroll_deform_amount, SB_CTL, &scrda, TRUE);
		scrda.fMask = SIF_POS;


		SendMessage(edit_deform_amount, WM_SETFONT, (WPARAM)hfont1, MAKELPARAM(FALSE, 0));
		SendMessage(edit_deform_radius, WM_SETFONT, (WPARAM)hfont1, MAKELPARAM(FALSE, 0));
		updateParameter(&iwarp.values().deform_area_radius);
		updateParameter(&iwarp.values().deform_amount);
		break;
	}
	case WM_DESTROY:
		DeleteObject(hfont1);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}




bool isDrawableTiming(){
	DWORD count;
	count = timeGetTime();
	if (FPS * (count - lastcount) >= 1000){
		lastcount = count;
		screenlock.lockWrite();
		pxMain.Fill(windowcolor);
		iwarp.redraw(pxMain, true);
		screenlock.unlockWrite();
		InvalidateRect(hwnd_app, 0, 0);
		return true;
	}
}


DWORD WINAPI MainThread(LPVOID vdParam) {
	HANDLE eventRequest[1] = {
		OpenEvent(EVENT_ALL_ACCESS, FALSE, "request redraw"), };
	for (;;){
		switch (WaitForMultipleObjects(1, eventRequest, false, INFINITE) - WAIT_OBJECT_0){
		case 0:
			isDrawableTiming();
			//ResetEvent(eventRequest[0]);
			break;
		}
	}
	return 0;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::AllocConsole();               // コマンドプロンプトが表示される
	//freopen("CON", "r", stdin);     // 標準入力の割り当て
	FILE * fp;
	freopen_s(&fp, "CON", "w", stdout);    // 標準出力の割り当て
	test();
	{
		HWND hWnd;
		hInst = hInstance;
		{
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hInstance;
			wcex.hIcon = 0;
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
			wcex.lpszMenuName = 0;
			wcex.lpszClassName = "IwarpWindow";
			wcex.hIconSm = 0;
			if (!RegisterClassEx(&wcex)) return FALSE;
		}
		RECT    rect = { 0, 0, width, height };
		::AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);
		hWnd = CreateWindow("IwarpWindow", "", WS_OVERLAPPEDWINDOW,
			0, 0, rect.right - rect.left, rect.bottom - rect.top+toolspace, NULL, NULL, hInstance, NULL);
		if (!hWnd)
			return FALSE;

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
		//SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		DWORD thid;
		hThread[0] = CreateThread(NULL, 0, MainThread, (LPVOID)hWnd, 0, &thid);
		CreateEvent(NULL, !TRUE, FALSE, "request redraw");
		eventRequestRedraw = OpenEvent(EVENT_ALL_ACCESS, FALSE, "request redraw");
		pxMain.set(width, height);
		pxSrc.BrendImage(&pxBody, 0, 0, center_offset(0, pxBody.w, 0, pxSrc.w), center_offset(0, pxBody.h, 0, pxSrc.h), pxBody.w, pxBody.h);
		SetWindowText(hWnd, "last.iwp");
		iwarp.load("last.iwp");
		iwarp.set_srcimage(pxSrc);
		redraw();
	}
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	for (int i = 0; i < arraysize(hThread); i++){
		if (hThread[i]){
			TerminateThread(hThread[i], 0);
			CloseHandle(hThread[i]);
		}
	}
	::FreeConsole();
	return (int)msg.wParam;
}
