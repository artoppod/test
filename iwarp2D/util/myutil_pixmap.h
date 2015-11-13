
#ifndef MYUTIL_PNG_H
#define MYUTIL_PNG_H
#include <png.h>
#include "myutil.h"
#include <vector>

#define USE_PNG

//#define PLATFORM_IS_LINUX
#define PLATFORM_IS_WINDOWS

#ifdef PLATFORM_IS_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#endif

#ifdef PLATFORM_IS_WINDOWS
#include <Windows.h>
//typedef HWND Display;

#endif

namespace MyUtil{

using namespace std;



union Color{
	struct{
#ifdef PLATFORM_IS_LINUX
        B r, g, b, a;
#endif
#ifdef PLATFORM_IS_WINDOWS
		B b, g, r, a;
#endif
	};
	struct{
		B h, l, s, a;
	};
    B4 all;

    Color(B r0, B g0, B b0, B a0=0xff);
	Color(UINT32 c=0){ all = c; };
	void set(B r, B g, B b, B a){
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	B & operator [](int i){
		switch (i){
		case 0: return b;
		case 1: return g;
		case 2: return r;
		case 3: return a;
		}
		return a;
	}
	
	bool operator !=(Color c);
    int sum();
	double brightness();
	Color alphabrend(Color & src);
	void alphabrend_direct(Color & src);
	B & maxfactor(){
		if (r > g){
			if (r > b)
				return r;
			else
				return b;
		}
		if (g > b)
			return g;
		else
			return b;
	}
	B & minfactor(){
		if (r < g){
			if (r < b)
				return r;
			else
				return b;
		}
		if (g < b)
			return g;
		else
			return b;
	}
 /*   V3f sqrdiff(Color & c);
    V3f absdiff(Color & c);*/

	static inline int hsl_value_int(double n1, double n2, double hue){
		double value;

		if (hue > 255)
			hue -= 255;
		else if (hue < 0)
			hue += 255;

		if (hue < 42.5)
			value = n1 + (n2 - n1) * (hue / 42.5);
		else if (hue < 127.5)
			value = n2;
		else if (hue < 170)
			value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
		else
			value = n1;

		return ROUND(value * 255.0);
	}

	V<B, 3> toHSL(){
		int    r, g, b;
		double h, s, l;
		int    min, max;
		int    delta;

		r = this->r;
		g = this->g;
		b = this->b;

		if (r > g)
		{
			max = maxvalue(r, b);
			min = minvalue(g, b);
		}
		else
		{
			max = maxvalue(g, b);
			min = minvalue(r, b);
		}

		l = (max + min) / 2.0;

		if (max == min)
		{
			s = 0.0;
			h = 0.0;
		}
		else
		{
			delta = (max - min);

			if (l < 128)
				s = 255 * (double)delta / (double)(max + min);
			else
				s = 255 * (double)delta / (double)(511 - max - min);

			if (r == max)
				h = (g - b) / (double)delta;
			else if (g == max)
				h = 2 + (b - r) / (double)delta;
			else
				h = 4 + (r - g) / (double)delta;

			h = h * 42.5;

			if (h < 0)
				h += 255;
			else if (h > 255)
				h -= 255;
		}
		return V<B, 3>(ROUND(h), ROUND(s), ROUND(l));
	}

	void fromHSL(V<B, 3> hsl){
		double h, s, l;

		h = hsl[0];
		s = hsl[1];
		l = hsl[2];

		if (s == 0)
		{
			/*  achromatic case  */
			r = l;
			g = l;
			b = l;
		}
		else
		{
			double m1, m2;

			if (l < 128)
				m2 = (l * (255 + s)) / 65025.0;
			else
				m2 = (l + s - (l * s) / 255.0) / 255.0;

			m1 = (l / 127.5) - m2;

			/*  chromatic case  */
			r = hsl_value_int(m1, m2, h + 85);
			g = hsl_value_int(m1, m2, h);
			b = hsl_value_int(m1, m2, h - 85);
		}
	}

	void darknessbrend_direct(Color & c){
		alphabrend_direct(Color(minvalue(r, c.r), minvalue(g, c.g), minvalue(b, c.b), c.a));
	}
};

struct Pixmap{
    Color ** px=0;
    short w, h;

	Pixmap(){ memset(this, 0, sizeof(*this)); };
	Pixmap(short w, short h);
	Pixmap(const char * name);
    Pixmap(Pixmap * src, int w,int h,int xSrc,int ySrc, double sx, double sy, double rot);
	virtual ~Pixmap();
	virtual bool set(int w, int h);
	virtual void init();
    Color & at(short x, short y);
    void DrawDot(short x, short y, Color c);
    void DrawRect(Color color, short x, short y, short w, short h);
    void DrawFrame(Color color, short x0, short y0, short x1, short y1);
    void BrendRect(Color color, short x, short y, short w, short h);
    void BrendImage(Pixmap * src, short srcx, short srcy, short x, short y, short w, short h);
    void BrendImageScale(Pixmap * src, short srcx, short srcy, short x, short y, short w, short h, float scale);
	void BrendImageRotateScale(Pixmap * src, int xDst,int yDst,int w,int h,int xSrc,int ySrc,
		double sx, double sy, double rot);
	void Fill(Color c);
	void Clear();
    void BrendDot(short x, short y, Color c);
	void linedrawfunc(Pixmap * p, int x, int y);
    void BrendLine(short x0, short y0, short x, short y, Color c);
	void CallLine(int x0, int y0, int x, int y, void (*drawfunc)(Pixmap *, int, int));
    void CutGradation(int lv);
    double SSD(Pixmap * p, int skip=0);
    double SAD(Pixmap * p, int skip=0);
    V<float, 2> EncodeWithBlightnessPos(int skip=0);
    double EncodeWithColorAverage(int skip=0);
    double EncodeRGB(Pixmap * p, int skip=0);
#ifdef PLATFORM_IS_LINUX
    void CopyScreen(Display * display, short x0=0, short y0=0, short width=-1, short height=-1);
#endif
    bool ReadAsPNG(const char * name, bool binit);
    bool WriteAsPNG(const char * name, short x0=0, short y0=0, short x1=-1, short y1=-1);
};

#ifdef PLATFORM_IS_WINDOWS
struct PixmapForGDI : public Pixmap{
	HDC hdc;
	HBITMAP hbmp;
	PixmapForGDI() : Pixmap(){};
	PixmapForGDI(int w, int h){
		if(!set(w, h)) throw -1;
	};

	PixmapForGDI(const char * name){
		ReadAsPNG(name, true);
	};

	virtual ~PixmapForGDI(){
		init();//dataをnullにすることでPixmapのデストラクタでdataを解放することを防いでる
	}

	virtual void init(){
		if (hbmp) DeleteObject(hbmp);
		if (hdc) DeleteDC(hdc);
		if (px) sfree(px);
		memset(this, 0, sizeof(*this));
	}

	virtual bool set(int w, int h){
		BITMAPINFO bmi = { 0 };
		init();
		HDC hdc0 = GetDC(0);
		// メモリデバイスDC作成	
		if (!(hdc = CreateCompatibleDC(hdc0)))
		{
			ReleaseDC(0, hdc0);
			goto $ABORT;
		}
		void *pb;
		// DIBSection構築
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = w;
		bmi.bmiHeader.biHeight = h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = 0;
		bmi.bmiHeader.biXPelsPerMeter = 0;
		bmi.bmiHeader.biYPelsPerMeter = 0;
		bmi.bmiHeader.biClrUsed = 0;
		bmi.bmiHeader.biClrImportant = 0;
		hbmp = CreateDIBSection(hdc0, &bmi, DIB_RGB_COLORS, (void**)&pb, NULL, 0);
		ReleaseDC(0, hdc0);
		if (!hbmp)
		{
			goto $ABORT;
		}
		{
			int i;
			px = (Color**)smalloc(sizeof(Color*) * h);
			for (i = 0; i < h; i++)
			{
				px[i] = (Color*)pb + (h - 1 - i) * w;
			}
		}
		SelectObject(hdc, hbmp);
		this->w = w;
		this->h = h;
		return true;
	$ABORT:
		init();
		return false;
	}
};
#endif

}

#endif
