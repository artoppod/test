
#include "myutil.h"

namespace MyUtil{

B4 nalloc=0;


int maxvalue(int a, int b){
	if (a>b) return a;
	return b;
};

int minvalue(int a, int b){
	if (a<b) return a;
	return b;
};


double maxvalue_d(double a, double b){
	if (a>b) return a;
	return b;
};

double minvalue_d(double a, double b){
	if (a<b) return a;
	return b;
};


void * smalloc(size_t size){
	nalloc++;
	return malloc(size);
}

void * srealloc(void * p, size_t size){
	if(p==NULL) nalloc++;
	return realloc(p, size);
}

void check_memoryleak(){
	if(nalloc){
		fprintf(stderr, "memoryleak was found.:%d\n", nalloc);
	}
}

static unsigned int x=4783102,y=362436069,z=521288629,w=88675123; 
unsigned int myrand(){
    unsigned int t=(x^(x<<11));
    x=y;y=z;z=w;
    return (w=(w^(w>>19))^(t^(t>>8)));
} 

//[a, b)
double myrand_d(double a, double b){
	return a+(double)myrand()*(b-a)/(unsigned int)~0;
}

void mysrand(unsigned int a){
	x=4783102^a;
}


double norm1(double x){
	if(x<0) return-x;
	return x;
}

double dire1(double x){
	return x/norm1(x);
}

int is_in(double x, double m, double M){
	return m<=x&&x<M;
}



float wraparound( float Value, float Begin, float End ){
    float dist = End - Begin;
    Value -= Begin;
    Value -= (int)( Value / dist ) * dist;
    long ofs = ( *(long*)&Value >> 31 ) & *(long*)&dist;
    return Value + *(float*)&ofs + Begin;
}


double tri(double x){
	int y;
	x=wraparound(x, 0, 2*PI);
	y=(int)(x/PI)%2;
	x=1.0-(x-y*PI)*2/PI;
	if(y){
		return -x; 
	}
	return x;
}

//square
double sqr(double x){
	return x*x;
}

double clamp(double x, double min, double max){
	if(x<min) return min;
	if(max<x) return max;
	return x; 
}

int is_in_mod(double x, double min, double max, double m){
	/*x=wraparound(x, 0,m);
	min=wraparound(min, 0,m);
	max=wraparound(max, 0,m);*/
	if(min<max){
		return min<=x&&x<=max;
	}else{
		return x<=min||max<=x;		
	}
}


int exist_file(char * path){
	FILE *fp;
	fopen_s(&fp, path, "r");
	if (fp){
		fclose(fp);
		return 1;
	}
	return 0;
}

void id_to_codename(const char * name_symbol, char * buf, int id){
	int radix=strlen(name_symbol);
	for(; id; id/=radix, buf++){
		*buf=name_symbol[id%radix];
	}
}


int codename_to_id(const char * name_symbol, const char * cn){
	int radix=strlen(name_symbol);
	int id=0, i=strlen(cn)-1;
	for(; i>=0; i--){
		id*=radix;
		id+=strchr(name_symbol, cn[i])-name_symbol;
	}
	return id;
}

int center_offset(int child_0, int child_len, int parent_0, int parent_len)
{
	return parent_0-child_0+((parent_len-child_len)>>1);
}



void realloc_ex(void ** p, size_t * n, size_t i, size_t unit){
	if(*n<=i){
		if(*n==0){
			*n=1;
		}
		while(*n<=i) *n*=2;
		*p=srealloc(*p, *n*unit);
		if(!p){
			perror("realloc");
			exit(EXIT_FAILURE);
		}
	}
}


long long combination(int a, int b){
	long long x = 1;
	assert(a>=b);
	for(; b>=1; a--){
		x*=(long long)a/b;
		b--;
	}
	return x;
}




V<float, 2> V2fRotate(V<float, 2> pos, float rot){
	float c=ROUND(cos(rot)), s=ROUND(sin(rot));
	V<float, 2> p(c*pos[0]-s*pos[1], s*pos[0]+c*pos[1]);
	return p;
}

V<float, 2> V2fAdd(V<float, 2> v, V<float, 2> w){
	V<float, 2> a(v[0]+w[0], v[1]+w[1]);
	return a;
}

V<float, 2> V2fSub(V<float, 2> v, V<float, 2> w){
	V<float, 2> a(v[0]-w[0], v[1]-w[1]);
	return a;
}



M4x4 M4x4::operator*(const M4x4 & B) const {
	M4x4 C;
	int i, j, k;
	i = 0; LOOP4(j = 0; LOOP4(k = 0; LOOP4(C.ary[i][j] += ary[i][k] * B.ary[k][j]; k++;)j++;)i++;)
		return C;
}

V<matrixfloat, 4> M4x4::transform(V<matrixfloat, 4> & A) {
	V<matrixfloat, 4> C;
	int i, j;
	i = 0; LOOP4(j = 0; LOOP4(C[i] += A[j] * ary[j][i]; j++;)i++;)
		return C;
}


}

