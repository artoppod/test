#ifndef MYUTILH
#define MYUTILH
#include <cmath>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cstdarg>
namespace MyUtil{

#ifndef arraysize
#define arraysize(x) (sizeof(x)/sizeof(x[0]))
#endif
#define array_head(x) (0)
#define array_tail(x) (arraysize(x)-1)
#define PI 3.14159265358979323846

#define IS_IN(x, a, b) (a<=(x)&&(x)<=b)
#define LOOP2(OP) OP OP
#define LOOP3(OP) OP OP OP
#define LOOP4(OP) OP OP OP OP

typedef unsigned char B;
typedef unsigned short B2;
typedef unsigned int  B4;

template<class T, int N> struct V{
	T ary[N];
	size_t size(){ return N; }
	V() { memset(ary, 0, sizeof(ary)); };
	//V(T v0, ...) {
	//	va_list args;
	//	va_start(args, v0);
	//	memset(ary, 0, sizeof(ary));
	//	ary[0] = v0;
	//	for (int i = 1; i < N; ++i) {
	//		ary[i] = va_arg(args, T);
	//	}
	//	va_end(args);
	//}
	V(T  v0, T  v1) {
		assert(N >= 2);
		ary[0] = v0;
		ary[1] = v1;
	}
	V(T  v0, T  v1, T  v2) {
		assert(N >= 3);
		ary[0] = v0;
		ary[1] = v1;
		ary[2] = v2;
	}
	V(T  v0, T  v1, T  v2, T  v3) {
		assert(N >= 4);
		ary[0] = v0;
		ary[1] = v1;
		ary[2] = v2;
		ary[3] = v3;
	}

	T distance(V<T, N> & a){
		T s = 0;
		for (int i = 0; i < N; ++i) s += sqr(ary[i] - a[i]);
		return sqrt(s);
	};
	T & operator [](int i){
		return ary[i];
	}
	const T & operator [](int i) const{
		return ary[i];
	}
	T & operator +=(T & a){
		for (int i = 0; i < N; i++) ary[i] += a[i];
		return *this;
	}
	T & operator *(T & a){
		for (int i = 0; i < N; i++) ary[i] *= a[i];
		return *this;
	}
	T & operator -=(T & a){
		for (int i = 0; i < N; i++) ary[i] -= a[i];
		return *this;
	}
	void set(){
		memset(ary, 0, sizeof(ary));
	}
	void shift(int m=N){
		int i = m - 1;
		T tmp = ary[i];
		while (i>0){
			ary[i] = ary[i-1];
			i--;
		}
		ary[0] = tmp;
	}
};




typedef float matrixfloat;
template<int H, int W> struct M{
	matrixfloat ary[H][W];
	M(){
		memset(ary, 0, sizeof(ary[0][0])*H*W);
	}
	/*virtual M<H, H> operator*(M<W, H> & B) {
		M<H, H> C;
		int i, j, k;
		for (i = 0; i < H; i++)
			for (j = 0; j < H; j++)
				for (k = 0; k < W; k++)
					C(i,j) += ary[i][k] * B(k,j);
		return C;
	}*/
	virtual matrixfloat & operator()(int i, int j) {
		return ary[i][j];
	}
	virtual const matrixfloat & operator()(int i, int j) const {
		return ary[i][j];
	}

	void set(){ memset(ary, 0, sizeof(ary)); };
	//M<W, H> transverse(){
	//	M<W, H> t;
	//	for (int i = 0; i < H; i++){
	//		for (int j = 0; j < W; j++){
	//			t[j][j]=ary[i][j];
	//		}
	//	}
	//	return t;
	//}
};

struct M4x4 : M < 4, 4 > {
	M4x4(matrixfloat a00 = 0, matrixfloat a01 = 0, matrixfloat a02 = 0, matrixfloat a03 = 0,
		matrixfloat a10 = 0, matrixfloat a11 = 0, matrixfloat a12 = 0, matrixfloat a13 = 0,
		matrixfloat a20 = 0, matrixfloat a21 = 0, matrixfloat a22 = 0, matrixfloat a23 = 0,
		matrixfloat a30 = 0, matrixfloat a31 = 0, matrixfloat a32 = 0, matrixfloat a33 = 0) {
		ary[0][0] = a00;	ary[0][1] = a01;	ary[0][2] = a02;	ary[0][3] = a03;
		ary[1][0] = a10;	ary[1][1] = a11;	ary[1][2] = a12;	ary[1][3] = a13;
		ary[2][0] = a20;	ary[2][1] = a21;	ary[2][2] = a22;	ary[2][3] = a23;
		ary[3][0] = a30;	ary[3][1] = a31;	ary[3][2] = a32;	ary[3][3] = a33;
	}
	M4x4 operator*(const M4x4 & B) const;
	V<matrixfloat, 4> transform(V<matrixfloat, 4> & A);

	M4x4 transverse(){
		return M4x4(
			ary[0][0],ary[1][0],ary[2][0],ary[3][0],
			ary[0][1],ary[1][1],ary[2][1],ary[3][1],
			ary[0][2],ary[1][2],ary[2][2],ary[3][2],
			ary[0][3],ary[1][3],ary[2][3],ary[3][3]);		
	}
};

struct M4x4Identity : M4x4{
	M4x4Identity() {
		ary[0][0] = 1;	ary[0][1] = 0;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = 1;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = 0;	ary[2][2] = 1;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
};

struct M4x4Move : M4x4 {
	M4x4Move(matrixfloat x = 0, matrixfloat y = 0, matrixfloat z = 0) {
		ary[0][0] = 1;	ary[0][1] = 0;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = 1;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = 0;	ary[2][2] = 1;	ary[2][3] = 0;
		ary[3][0] = x;	ary[3][1] = y;	ary[3][2] = z;	ary[3][3] = 1;
	}
	M4x4Move(V<matrixfloat, 4> v) {
		ary[0][0] = 1;	ary[0][1] = 0;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = 1;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = 0;	ary[2][2] = 1;	ary[2][3] = 0;
		ary[3][0] = v[0];	ary[3][1] = v[1];	ary[3][2] = v[2];	ary[3][3] = 1;
	}
};


struct M4x4Scale : M4x4 {
	M4x4Scale(matrixfloat x, matrixfloat y, matrixfloat z) {
		ary[0][0] = x;	ary[0][1] = 0;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = y;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = 0;	ary[2][2] = z;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
	M4x4Scale(matrixfloat x) {
		ary[0][0] = x;	ary[0][1] = 0;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = x;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = 0;	ary[2][2] = x;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
};


struct M4x4Rotate : M4x4 {
	M4x4Rotate(matrixfloat y, matrixfloat p, matrixfloat r) {
		matrixfloat
			cy = (matrixfloat)cos(y), sy = (matrixfloat)sin(y),
			cp = (matrixfloat)cos(p), sp = (matrixfloat)sin(p),
			cr = (matrixfloat)cos(r), sr = (matrixfloat)sin(r);
		ary[0][0] = cy*cr - sy*cp*sr;	ary[0][1] = -cy*sr - sy*cp*cr;	ary[0][2] = sy*sp;	ary[0][3] = 0;
		ary[1][0] = sy*cr + cy*cp*sr;	ary[1][1] = -sy*sr + cy*cp*cr;	ary[1][2] = -cy*sp;	ary[1][3] = 0;
		ary[2][0] = sp*sr;	ary[2][1] = sp*cr;	ary[2][2] = cp;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
};


struct M4x4RotateX : M4x4 {
	M4x4RotateX(matrixfloat rot = 0) {
		matrixfloat c = (matrixfloat)cos(rot), s = (matrixfloat)sin(rot);
		ary[0][0] = 1;	ary[0][1] = 0;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = c;	ary[1][2] = -s;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = s;	ary[2][2] = c;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
};

struct M4x4RotateY : M4x4 {
	M4x4RotateY(matrixfloat rot = 0) {
		matrixfloat c = (matrixfloat)cos(rot), s = (matrixfloat)sin(rot);
		ary[0][0] = c;	ary[0][1] = 0;	ary[0][2] = s;	ary[0][3] = 0;
		ary[1][0] = 0;	ary[1][1] = 1;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = -s;	ary[2][1] = 0;	ary[2][2] = c;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
};

struct M4x4RotateZ : M4x4 {
	M4x4RotateZ(matrixfloat rot = 0) {
		matrixfloat c = (matrixfloat)cos(rot), s = (matrixfloat)sin(rot);
		ary[0][0] = c;	ary[0][1] = -s;	ary[0][2] = 0;	ary[0][3] = 0;
		ary[1][0] = s;	ary[1][1] = c;	ary[1][2] = 0;	ary[1][3] = 0;
		ary[2][0] = 0;	ary[2][1] = 0;	ary[2][2] = 1;	ary[2][3] = 0;
		ary[3][0] = 0;	ary[3][1] = 0;	ary[3][2] = 0;	ary[3][3] = 1;
	}
};

//struct Rect{
//	short x, y, w, h;
//	Rect(short x = 0, short y = 0, short w = 0, short h = 0) :x(x), y(y), w(w), h(h){};
//};
//
//struct Point{
//	short x, y;
//	Point(short x=0, short y=0) :x(x), y(y){};
//};

#define sfree(x) if((x)){assert(nalloc>0);free(x);nalloc--;}
extern B4 nalloc;
void * smalloc(size_t size);
void * srealloc(void * p, size_t size);
void check_memoryleak();


#define ROUND(x) ((int) ((x) + 0.5))
#define ROUND_NEARLY_ZERO(x) 1.0E-15*(long long)((x)*1.0E15)
#define ROUND_NEARLY_ZERO_ARG(x, d) (((long long)((x)*d))/d)

#define is_in_rect(x, y, w, h) (0<=x&&x<w&&0<=y&&y<h)
#define is_in_box(x, y, z, xw, yw, zw) (0<=x&&x<xw&&0<=y&&y<yw&&0<=z&&z<zw)
#define is_in_box2(x, y, z, x0, y0, z0, xw, yw, zw) (x0<=x&&x<=xw&&y0<=y&&y<=yw&&z0<=z&&z<=zw)

int maxvalue(int a, int b);
int minvalue(int a, int b);


double maxvalue_d(double a, double b);
double minvalue_d(double a, double b);

void mysrand(unsigned int a);
double myrand_d(double a, double b);
unsigned int myrand();
int is_in(double x, double m, double M);
int is_in_mod(double x, double min, double max, double m);
float wraparound( float Value, float Begin, float End );
double tri(double x);
double sqr(double x);
double clamp(double x, double min, double max);
int exist_file(char * path);
int center_offset(int child_0, int child_len, int parent_0, int parent_len);
void realloc_ex(void ** p, size_t * n, size_t i, size_t unit);
long long combination(int a, int b);

template<class T, int N>
struct multi_array{
	void **pool;
	T & at(int e0, ...){
		va_list args;
		va_start(args, e0);
		T** p = (T**)pool[e0];
		for (int i = 2; i < N; ++i) {
			p = (T**)p[va_arg(args, int)];
		}
		T* q = (T*)p + va_arg(args, int);
		va_end(args);
		return *q;
	}

	multi_array(int e0, ...){
		int e[N];
		va_list args;
		va_start(args, e0);
		e[0] = e0;
		for (int i = 1; i < N; ++i) {
			e[i] = va_arg(args, int);
		}
		va_end(args);
		{
			int s = sizeof(T) * e[N - 1];
			for (int j = N - 2; j >= 0; --j) {
				s = e[j] * (sizeof(void*) + s);
			}
			//poolrintf("size=%d\N", s);
			pool = (void**)calloc(1, s);
			if (!pool)
				throw;
		}
		int w = e[0];
		void **base = pool;
		for (int i = 1; i < N - 1; i++){
			int k = e[i];
			base[0] = base + w;
			//poolrintf("%d\N", base[0]);
			for (int j = 1; j<w; j++){
				base[j] = (void**)*base + j*k;
				//poolrintf("%d\N", base[j]);
			}
			base += w;
			w *= k;
		}
		{
			int k = e[N - 1] * sizeof(T);
			base[0] = base + w;
			//poolrintf("%d\N", base[0]);
			for (int j = 1; j<w; j++){
				base[j] = (char*)*base + j*k;
				//poolrintf("%d\N", base[j]);
			}
		}
	}
	~multi_array(){
		free(pool);
	}
};

//#define new_multi_array(TYPE, e0, e1, e2, e3, e4, e5) (TYPE*)multi_array(sizeof(TYPE), )


}

#endif
