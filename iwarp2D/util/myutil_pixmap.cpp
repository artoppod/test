#include "myutil_pixmap.h"
#include <cstdlib>


namespace MyUtil{

Color::Color(B r0, B g0, B b0, B a0){
    r=r0; g=g0; b=b0; a=a0;
}
bool Color::operator !=(Color c){
    return all!=c.all;
}


int Color::sum(){
    return r+g+b+a;
}

Color & Pixmap::at(short x, short y){
	static Color dummy(0,0,0);
	if (is_in_rect(x, y, w, h))
		return px[y][x];
	else{
		dummy.all = 0;
		return dummy;
	}
}

//
//V3f Color::sqrdiff(Color & c){
//    return V3f(sqr(r-c.r), sqr(g-c.g), sqr(b-c.b));
//}
//
//V3f Color::absdiff(Color & c){
//    return V3f(abs(r-c.r), abs(g-c.g), abs(b-c.b));
//}


double Color::brightness(){
    //return (double)(r+g+b);
    return ((double)0.299*r+0.587*g+0.114*b)/255;
    //return (double)(r+g+b)/(3*255);
}

#define COLOR_RESULT(cd, cs) ((cs*as+cd*(255-as)*ad/255)/a)
Color Color::alphabrend(Color & src){
    int ad=a, as=src.a;
	if (ad == 0 || as == 0xff){
		return src;
	}
    if(as==0) return *this;
    {
        B a=ad+as-ad*as/255;
        return Color(COLOR_RESULT(r, src.r), COLOR_RESULT(g, src.g), COLOR_RESULT(b, src.b), a);
    }
}

void Color::alphabrend_direct(Color & src){
	int ad = a, as = src.a;
	if (ad == 0 || as == 0xff){
		all = src.all;
	}else if (as != 0){
		B a = ad + as - ad*as / 255;
		r = COLOR_RESULT(r, src.r);
		g = COLOR_RESULT(g, src.g);
		b = COLOR_RESULT(b, src.b);
		this->a = a;
	}
}




void** smalloc2d(size_t size, int x, int y)
{
	void **p;
	char *o;
	int i, d = x*size;
	if (!(p = (void**)smalloc(y*(sizeof(void*) + d))))
		return 0;
	o = (char*)(p + y);
	for (i = 0; i<y; i++)
		p[i] = o + i*d;
	return p;
}

bool Pixmap::set(int w, int h){
	init();
	this->w = w;
	this->h = h;
	this->px = (Color**)smalloc2d(sizeof Color, w, h);
	return true;
}

Pixmap::Pixmap(short w, short h){
	set(w, h);
}

Pixmap::Pixmap(Pixmap * src, int w,int h,int xSrc,int ySrc,
               double sx, double sy, double rot){
    this->w=w;
	this->h = h;
	this->px = (Color**)smalloc2d(sizeof Color, w, h);
	Clear();
    BrendImageRotateScale(src, 0, 0, w, h, xSrc, ySrc, sx, sy, rot);
}

Pixmap::~Pixmap(){
	init();
}


Pixmap::Pixmap(const char * name){
	ReadAsPNG(name, true);
}

void Pixmap::init(){
	sfree(px);
	memset(this, 0, sizeof(*this));
}

void Pixmap::Clear(){
	memset(px+h, 0, sizeof(Color)*w*h);
}


double Pixmap::EncodeWithColorAverage(int skip){
    double r=0, g=0, b=0;
    int i=0;
    for(int xi=0; xi<w; xi+=1+skip){
        for(int yi=0; yi<h; yi+=1+skip){
            Color c=at(xi, yi);
            r+=(double)c.r/255;
            g+=(double)c.g/255;
            b+=(double)c.b/255;
            i++;
        }
    }
    r/=i;
    g/=i;
    b/=i;
    return (r*255.+g)*255.+b;
}

V<float,2> Pixmap::EncodeWithBlightnessPos(int skip){
    MyUtil::V<float,2> p={0,0};
    for(int xi=0; xi<w; xi+=1+skip){
        for(int yi=0; yi<h; yi+=1+skip){
            double b=at(xi, yi).brightness();
            p[0]+=b*xi;
            p[1]+=b*yi;
        }
    }
    //p[0]/=i;p[1]/=i;
    return p;
}

double Pixmap::EncodeRGB(Pixmap * p, int skip){
    double a=0;
    for(int xi=0; xi<w; xi+=1+skip){
        for(int yi=0; yi<h; yi+=1+skip){
            Color c=at(xi, yi), d=p->at(xi, yi);
            a+=abs(c.r-d.r)+abs(c.g-d.g)+abs(c.b-d.b);
        }
    }
    return a;
}

double Pixmap::SSD(Pixmap * p, int skip){
    double a=0;
    for(int xi=0; xi<w; xi+=1+skip){
        for(int yi=0; yi<h; yi+=1+skip){
            a+=sqr(at(xi, yi).brightness()-
                   p->at(xi, yi).brightness());
        }
    }
    return a;
}

double Pixmap::SAD(Pixmap * p, int skip){
    double a=0;
    for(int xi=0; xi<w; xi+=1+skip){
        for(int yi=0; yi<h; yi+=1+skip){
           a+=abs(at(xi, yi).brightness()-p->at(xi, yi).brightness());
        }
    }
    return a;
}


void Pixmap::DrawDot(short x, short y, Color c){
	at(x, y)=c;
}

void Pixmap::DrawRect(Color color, short x, short y, short w, short h){
    int xi, yi;
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi++){
            at(x+xi, y+yi)=color;
        }
    }
}

void Pixmap::DrawFrame(Color color, short x0, short y0, short x1, short y1){
    V<short, 2> a[2]={V<short, 2>(min(x0, x1), min(y0, y1)),  V<short, 2>(max(x0, x1), max(y0, y1))};
    BrendLine(a[0][0], a[0][1], a[0][0], a[1][1], color);
    BrendLine(a[0][0], a[0][1], a[1][0], a[0][1], color);
    BrendLine(a[0][0], a[1][1], a[1][0], a[1][1], color);
    BrendLine(a[1][0], a[0][1], a[1][0], a[1][1], color);
}

void Pixmap::BrendRect(Color color, short x, short y, short w, short h){
    int xi, yi;
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi++){
            at(xi, yi).alphabrend_direct(color);
        }
    }
}

void Pixmap::BrendImage(Pixmap * src, short srcx, short srcy, short x, short y, short w, short h){
    int xi, yi;
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi++){
			at(x + xi, y + yi).alphabrend_direct(src->at(xi + srcx, yi + srcy));
			//printf("%3d", src->at(xi + srcx, yi + srcy).a);
        }
		//printf("\n");
    }
}



void Pixmap::BrendImageScale(Pixmap * src, short srcx, short srcy, short x, short y, short w, short h, float scale){
    int xi, yi;
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi++){
			at(xi + x, yi + y).alphabrend_direct(src->at((int)(xi / scale + srcx), (int)(yi / scale + srcy)));
        }
    }
}

//時計回りで回転　sx,sy倍して描画。
void Pixmap::BrendImageRotateScale(Pixmap * src, int xDst,int yDst,int w,int h,int xSrc,int ySrc,
		double sx, double sy, double rot)
{
	if(w==0){
		w=src->w;
		h=src->h;
	}
    rot=rot*PI*2;
    double c=ROUND(cos(rot)), s=ROUND(sin(rot));
	double rate=1.0, dx,dy,ddx=1/(rate*sx), ddy=1.0/sy;
	for(dx=0; dx<w; dx+=ddx){
		for(dy=0; dy<h; dy+=ddy){
			int tx=dx*sx, ty=dy*sy, x=(c*tx-s*ty)+xDst, y=(s*tx+c*ty)+yDst;
			if(0<=x&&x<this->w&&0<=y&&y<this->h){
				int xs=dx+xSrc, ys=dy+ySrc;
				if(0<=xs&&xs<src->w&&0<=ys&&ys<src->h){
					at(x, y).alphabrend_direct(src->at(xs, ys));
				}
			}
		}
	}
}


void Pixmap::Fill(Color c){
	DrawRect(c, 0, 0, this->w, this->h);
}

void Pixmap::BrendDot(short x, short y, Color c){
    at(x, y).alphabrend_direct(c);
}


void Pixmap::BrendLine(short x0, short y0, short x, short y, Color c){
    short cnt=0, w=x-x0, h=y-y0, dsa=(w>0)?1:-1, dsb=(h>0)?1:-1, da, db, *penx, *peny, *px;
	if(dsa*w>=dsb*h){
		px=&x;
		penx=&x0;
		peny=&y0;
		da=dsa*dsb*h;
		db=w;
	}else{
        short swap=dsa;
		dsa=dsb;
		dsb=swap;
		px=&y;
		penx=&y0;
		peny=&x0;
		da=dsa*dsb*w;
		db=h;
	}
	for(;*penx!=*px; (*penx)+=dsa){
		BrendDot(x0, y0, c);
		cnt+=da;
		if((dsa<0)^(cnt>=db)){//(dsa<0)!=(cnt>=db)
			cnt-=db;
			(*peny)+=dsb;
		}
	}
}

void Pixmap::CallLine(int x0, int y0, int x, int y, void (*drawfunc)(Pixmap *, int, int)){
	int cnt=0, w=x-x0, h=y-y0, dsa=(w>0)?1:-1, dsb=(h>0)?1:-1, da, db, *penx, *peny, *px;
	if(dsa*w>=dsb*h){
		px=&x;
		penx=&x0;
		peny=&y0;
		da=dsa*dsb*h;
		db=w;
	}else{
		int swap=dsa;
		dsa=dsb;
		dsb=swap;
		px=&y;
		penx=&y0;
		peny=&x0;
		da=dsa*dsb*w;
		db=h;
	}
	for(;*penx!=*px; (*penx)+=dsa){
		drawfunc(this, x0, y0);
		cnt+=da;
		if((dsa<0)^(cnt>=db)){//(dsa<0)!=(cnt>=db)
			cnt-=db;
			(*peny)+=dsb;
		}
	}
}

//unsupported index color png
bool Pixmap::ReadAsPNG(const char * name, bool binit=false){
#ifdef PLATFORM_IS_LINUX
    FILE            *fp;
    png_structp     png_ptr;
    png_infop       info_ptr;
	png_uint_32 width, height;
    int             bit_depth, color_type, interlace_type;
    unsigned char   **image;
    int             i;

    fopen_s(&fp, name, "rb");
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);             // info_ptr構造体を確保・初期化します
    png_init_io(png_ptr, fp);                               // libpngにfpを知らせます
    png_read_info(png_ptr, info_ptr);                       // PNGファイルのヘッダを読み込みます
	png_get_IHDR(png_ptr, info_ptr, &width, &height,        // IHDRチャンク情報を取得します
                    &bit_depth, &color_type, &interlace_type,
                    NULL, NULL);
    image = (png_bytepp)malloc(height * sizeof(png_bytep)); // 以下３行は２次元配列を確保します
    for (i = 0; i < height; i++)
            image[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
    png_read_image(png_ptr, image);                         // 画像データを読み込みます

    for (i = 0; i < height; i++){
        for(int x=min(width, w)-1; x>=0; x--){
            at(x, i).b=image[i][x*4+0];
            at(x, i).g=image[i][x*4+1];
            at(x, i).r=image[i][x*4+2];
            at(x, i).a=image[i][x*4+3];
        }
        free(image[i]);
    }
    free(image);
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    fclose(fp);      
#endif

#ifdef PLATFORM_IS_WINDOWS
	unsigned int sig_read = 0;
	int bit_depth, color_type, interlace_type;
	FILE* fp;
	png_structp png_ptr;
	png_infop info_ptr;

	// ファイルを開く
	fopen_s(&fp, name, "rb");
	if (!fp) throw - 1;
	// png_structの初期化
	png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(fp); throw - 1;
	}
	png_init_io(png_ptr, fp);

	// png_infoの初期化
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, 0, 0);
		throw - 1;
	}
	// 画像情報の取得
	unsigned long wi, he;
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&wi, (png_uint_32*)&he,
		&bit_depth, &color_type, &interlace_type, NULL, NULL);

	png_set_expand(png_ptr);
	if (bit_depth == 16) png_set_strip_16(png_ptr);
	png_set_bgr(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	png_read_update_info(png_ptr, info_ptr);
	
	png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);//行データのバイト数
	int channels = (int)png_get_channels(png_ptr, info_ptr); //チャンネル数

	if (binit) set(wi, he);
	// 画像の読み込み
	png_bytepp image = (png_bytepp)smalloc2d(sizeof(png_byte), rowbytes, he); // 以下３行は２次元配列を確保します
	png_read_image(png_ptr, image);                         // 画像データを読み込みます

	for (int i = min(he, h) - 1; i >= 0; i--){
		for (int x = min(wi, w) - 1; x >= 0; x--){
			at(x, i).b = image[i][x * channels + 0];
			at(x, i).g = image[i][x * channels + 1];
			at(x, i).r = image[i][x * channels + 2];
			at(x, i).a = (channels>3?image[i][x * channels + 3]:0xff);
		}
	}
	sfree(image);
	// 後片づけ
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	fclose(fp);
#endif

    return true;
}

bool Pixmap::WriteAsPNG(const char * name, short x0, short y0, short width, short height){
#ifndef USE_PNG
	return false;
#endif
	FILE		*fp;
	png_structp	png_ptr;
	png_infop	info_ptr;
	if(width==-1) width=w;
	if(height==-1) height=h;
	width=MyUtil::clamp(width, 0, w);
	height=MyUtil::clamp(height, 0, h);
	fopen_s(&fp, name, "wb");
	if (fp == NULL) return false;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return false;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		fclose(fp);
		return false;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr,  &info_ptr);
		fclose(fp);
		return false;
	}
	png_init_io(png_ptr, fp);
	png_set_write_status_fn(png_ptr, 0);
	png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
	png_set_compression_level(png_ptr, PNG_TEXT_Z_DEFAULT_COMPRESSION);
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	//png_set_gAMA(png_ptr, info_ptr, 1.0);
	if(false) png_set_invert_alpha(png_ptr);
	if(true) png_set_bgr(png_ptr);
	png_write_info(png_ptr, info_ptr);
	{
		png_byte ** row_pointers=(png_byte **)malloc(height * sizeof(void*));
		int row;
		for(row=0; row<height; row++)
			row_pointers[row]=(png_bytep)((int*)&at(x0, y0+row));
		png_write_image(png_ptr, row_pointers);
		png_write_end(png_ptr, info_ptr);
		free(row_pointers);
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return true;
}

#ifdef PLATFORM_IS_LINUX
void Pixmap::CopyScreen(Display * display, short x0, short y0, short width, short height){
	if(width==-1) width=w;
	if(height==-1) height=h;
	width=MyUtil::clamp(width, 0, w);
	height=MyUtil::clamp(height, 0, h);
    XImage * img=XGetImage(display, DefaultRootWindow(display), x0, y0, width, height, 0xffffffff, ZPixmap);
	int xi, yi;
    for(yi=0; yi<height; yi++){
        Color * p=data+(yi+0)*w, * q=(Color*)(img->data+(yi)*img->bytes_per_line);
        for(xi=0; xi<width; xi++){
            p[xi+0]=q[xi];
            p[xi+0].a=0xff;//アルファ値を不透明で固定する
        }
    }
    XFree(img);
}
#endif


//N値化する
void Pixmap::CutGradation(int n){
	n = 256 / n;
	for (int x=0; x<w; x++){
		for (int y = 0; y < h; y++){
			at(x, y).r = (at(x, y).r / n)*n;
			at(x, y).g = (at(x, y).g / n)*n;
			at(x, y).b = (at(x, y).b / n)*n;
		}
    }
}

}
