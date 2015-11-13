#pragma once

#ifndef _DEBUG
#include <zlib.h>
#endif 
#include "full.h"
#define MAX_DEFORM_AREA_RADIUS 250

typedef float iwarpfloat;

enum DeformMode
{
	GROW=0,
	SHRINK,
	MOVE,
	REMOVE,
	SWIRL_CCW,
	SWIRL_CW,
	COLORIZE,
	ZERO_BLANK,
	SAVE,
	LOAD,
	DEFORMMODE_END
} ;

struct iwarp_vals_t
{
	int        deform_area_radius;
	iwarpfloat     deform_amount;
	DeformMode  deform_mode;
	bool    do_bilinear;
};

extern iwarp_vals_t iwarp_vals;
extern V<iwarpfloat,2> deform_area_vectors[MAX_DEFORM_AREA_RADIUS * 2 + 1][MAX_DEFORM_AREA_RADIUS * 2 + 1];
extern iwarpfloat filter[MAX_DEFORM_AREA_RADIUS];

template <int W, int H>
class Iwarp{
	Pixmap *srcimage = 0;
	V<iwarpfloat, 2> deform_vectors[H][W];
	V<short, 4> range;
	void preprocess(){
		static bool b = false;
		if (!b){
			b = true;
			for (int i = 0; i < MAX_DEFORM_AREA_RADIUS; i++){
				filter[i] =
					pow((cos(sqrt((iwarpfloat)i / MAX_DEFORM_AREA_RADIUS) * PI) + 1) *
					0.5, 0.7); /*0.7*/
			}
		}
	}

	//浮動少数点数の座標指定で変形値取得（線形補間）
	void
		iwarp_get_deform_vector(iwarpfloat  x,
		iwarpfloat  y,
		V<iwarpfloat, 2> & v)
	{
		int    xi, yi;
		iwarpfloat dx, dy, my0, my1, mx0, mx1;

		if (is_in_rect(x, y, W - 1, H - 1))
		{
			xi = (int)x;
			yi = (int)y;
			dx = x - xi;
			dy = y - yi;
			mx0 =
				deform_vectors[yi][xi][0] +
				(deform_vectors[yi][xi + 1][0] -
				deform_vectors[yi][xi][0]) * dx;
			mx1 =
				deform_vectors[yi + 1][xi][0] +
				(deform_vectors[yi + 1][xi + 1][0] -
				deform_vectors[yi + 1][xi][0]) * dx;
			my0 =
				deform_vectors[yi][xi][1] +
				dx * (deform_vectors[yi][xi + 1][1] -
				deform_vectors[yi][xi][1]);
			my1 =
				deform_vectors[yi + 1][xi][1] +
				dx * (deform_vectors[yi + 1][xi + 1][1] -
				deform_vectors[yi + 1][xi][1]);
			v[0] = mx0 + dy * (mx1 - mx0);
			v[1] = my0 + dy * (my1 - my0);
		}
		else
		{
			v[0] = v[1] = 0.0;
		}
	}
	
	void
		iwarp_deform(
		int    x,
		int     y,
		iwarpfloat vx,
		iwarpfloat vy)
	{
		int    xi, yi, x0, x1, y0, y1, radius2, length2;
		iwarpfloat deform_value, nvx = 0, nvy = 0, emh, em, edge_width;
		V<iwarpfloat, 2> v;
		x0 = (x < iwarp_vals.deform_area_radius) ?
			-x : -iwarp_vals.deform_area_radius;

		x1 = (x + iwarp_vals.deform_area_radius >= W) ?
			W - x - 1 : iwarp_vals.deform_area_radius;

		y0 = (y < iwarp_vals.deform_area_radius) ?
			-y : -iwarp_vals.deform_area_radius;

		y1 = (y + iwarp_vals.deform_area_radius >= H) ?
			H - y - 1 : iwarp_vals.deform_area_radius;
		range[0] = maxvalue(W/2, minvalue(range[0], x + x0));
		range[1] = maxvalue(range[1], x+x1);
		range[2] = minvalue(range[2], y+y0);
		range[3] = maxvalue(range[3], y+y1);

		radius2 = sqr(iwarp_vals.deform_area_radius);

		for (yi = y0; yi <= y1; yi++)
			for (xi = x0; xi <= x1; xi++)
			{
				length2 = (xi * xi + yi * yi) * MAX_DEFORM_AREA_RADIUS / radius2;
				if (length2 < MAX_DEFORM_AREA_RADIUS)
				{
					switch (iwarp_vals.deform_mode)
					{
					case GROW:
						deform_value = filter[length2] * 0.1 * iwarp_vals.deform_amount;
						nvx = -deform_value * xi;
						nvy = -deform_value * yi;
						break;

					case SHRINK:
						deform_value = filter[length2] * 0.1 * iwarp_vals.deform_amount;
						nvx = deform_value * xi;
						nvy = deform_value * yi;
						break;

					case SWIRL_CW:
						deform_value = filter[length2] * iwarp_vals.deform_amount * 0.5;
						nvx = deform_value * yi;
						nvy = -deform_value * xi;
						break;

					case SWIRL_CCW:
						deform_value = filter[length2] * iwarp_vals.deform_amount * 0.5;
						nvx = -deform_value * yi;
						nvy = deform_value  * xi;
						break;

					case MOVE:
						deform_value = filter[length2] * iwarp_vals.deform_amount;
						nvx = deform_value * vx;
						nvy = deform_value * vy;
						break;

					default:
						break;
					}

					if (iwarp_vals.deform_mode == REMOVE)
					{
						deform_value =
							1.0 - 0.5 * iwarp_vals.deform_amount * filter[length2];
						deform_area_vectors[yi + iwarp_vals.deform_area_radius][xi + iwarp_vals.deform_area_radius][0] =
							deform_value * deform_vectors[y + yi][x + xi][0];
						deform_area_vectors[yi + iwarp_vals.deform_area_radius][xi + iwarp_vals.deform_area_radius][1] =
							deform_value * deform_vectors[y + yi][x + xi][1];
					}
					else
					{
						edge_width = 0.2 *  iwarp_vals.deform_area_radius;
						emh = em = 1.0;
						if (x + xi < edge_width)
							em = (iwarpfloat)(x + xi) / edge_width;
						if (y + yi < edge_width)
							emh = (iwarpfloat)(y + yi) / edge_width;
						if (emh <em)
							em = emh;
						if (W - x - xi - 1 < edge_width)
							emh = (iwarpfloat)(W - x - xi - 1) / edge_width;
						if (emh < em)
							em = emh;
						if (H - y - yi - 1 < edge_width)
							emh = (iwarpfloat)(H - y - yi - 1) / edge_width;
						if (emh <em)
							em = emh;

						nvx = nvx * em;
						nvy = nvy * em;

						iwarp_get_deform_vector(nvx + x + xi, nvy + y + yi, v);
						v[0] += nvx;
						v[1] += nvy;
						//clamp
						if (v[0] + x + xi <0.0)
							v[0] = -x - xi;
						else if (v[0] + x + xi >(W - 1))
							v[0] = W - x - xi - 1;
						if (v[1] + y + yi < 0.0)
							v[1] = -y - yi;
						else if (v[1] + y + yi >(H - 1))
							v[1] = H - y - yi - 1;
						deform_area_vectors[yi + iwarp_vals.deform_area_radius][xi + iwarp_vals.deform_area_radius] = v;
					}
				}
			}

		for (yi = y0; yi <= y1; yi++)
			for (xi = x0; xi <= x1; xi++)
			{
				length2 = (xi*xi + yi*yi) * MAX_DEFORM_AREA_RADIUS / radius2;
				if (length2 < MAX_DEFORM_AREA_RADIUS)
				{
					deform_vectors[y + yi][x + xi] = deform_area_vectors[yi + iwarp_vals.deform_area_radius][xi + iwarp_vals.deform_area_radius];
				}
			}
		//iwarp_changed = true;	iwarp_update_preview(x + x0, y + y0, x + x1 + 1, y + y1 + 1);
	}

	void colorize(iwarpfloat x, iwarpfloat y){
		int    xi, yi, x0, x1, y0, y1, radius2, length2;
		int r=iwarp_vals.deform_area_radius /4;
		x0 = (x < r) ?
			-x : -r;

		x1 = (x + r >= W) ?
			W - x - 1 : r;

		y0 = (y < r) ?
			-y : -r;

		y1 = (y + r >= H) ?
			H - y - 1 : r;
		range[0] = maxvalue(W / 2, minvalue(range[0], x + x0));
		range[1] = maxvalue(range[1], x + x1);
		range[2] = minvalue(range[2], y + y0);
		range[3] = maxvalue(range[3], y + y1);

		radius2 = sqr(r);
		V<B, 3> hsl;
		Color c2(255,255,40);
		for (yi = y0; yi <= y1; yi++)
			for (xi = x0; xi <= x1; xi++){
				length2 = (xi * xi + yi * yi) * MAX_DEFORM_AREA_RADIUS / radius2;
				if (length2 < MAX_DEFORM_AREA_RADIUS){
					Color & c = srcimage->at(deform_vectors[yi + (int)y][(int)x + xi][0] + (int)x + xi,
						deform_vectors[(int)y + yi][(int)x + xi][1] + (int)y + yi);
					if (c.a != 0){
						//c.r = minvalue(c2.r, c.r);c.g = minvalue(c2.g, c.g);c.b = minvalue(c2.b, c.b);
						//c.alphabrend_direct(Color(c2.r*c.r/255.0, c2.g*c.g/255.0, c2.b*c.b/255.0, 2));
						c.alphabrend_direct(Color(minvalue(c2.r, c.r), minvalue(c2.g, c.g), minvalue(c2.b, c.b), 125));
						/*
						hsl = c.toHSL();
						hsl[0]++;
						c.fromHSL(hsl);*/
					/*	c.r = minvalue(255, c.r + 1);
						c.g = maxvalue(0, c.g - 1);
						c.b = maxvalue(0, c.b - 1);*/
					}
				}
			}
	}


public:

	Iwarp(){
		preprocess();
		range[0] = W;
		range[1] = 0;
		range[2] = H;
		range[3] = 0;
	}

	void
		deform(
		Pixmap & src,
		int x,
		int y,
		int xx,
		int yy)
	{
		srcimage = &src;
		switch(iwarp_vals.deform_mode){
		case MOVE:
		{
			iwarpfloat l, dx, dy, xf, yf;
			int    num, i, x0, y0;
			dx = x - xx;
			dy = y - yy;
			l = sqrt(dx * dx + dy * dy);
			num = (int)(l * 2 / iwarp_vals.deform_area_radius) + 1;
			dx /= num;
			dy /= num;
			xf = xx + dx; yf = yy + dy;

			for (i = 0; i< num; i++)
			{
				x0 = (int)xf;
				y0 = (int)yf;

				iwarp_deform(x0, y0, -dx, -dy);

				xf += dx;
				yf += dy;
			}
			break;
		}
		case COLORIZE:
		{
			iwarpfloat l, dx, dy, xf, yf;
			int    num, i, x0, y0;
			dx = x - xx;
			dy = y - yy;
			l = sqrt(dx * dx + dy * dy);
			num = (int)(l * 2 / iwarp_vals.deform_area_radius*4) + 1;
			dx /= num;
			dy /= num;
			xf = xx + dx; yf = yy + dy;
			for (i = 0; i< num; i++)
			{
				x0 = (int)xf;
				y0 = (int)yf;
				colorize(x, y);
				xf += dx;
				yf += dy;
			}
			break;
		}
			break;
		default:
			iwarp_deform(x, y, 0, 0);
		}
	}

	void redraw(Pixmap & dstimage, bool fulldraw=false, int x=0, int y=0){
		int x0 = range[0], x1 = range[1], y0 = range[2], y1 = range[3];
		//if (fulldraw)
		{
			x0 = W / 2;
			x1 = W - 1;
			y0 = 0;
			y1 = H - 1;
		}
		for (int yi = y0; yi <= y1; yi++){
			for (int xi = x0; xi <= x1; xi++){
				Color & c = srcimage->at(deform_vectors[yi][xi][0] + xi, deform_vectors[yi][xi][1] + yi);
				dstimage.at(x+xi, y+yi).darknessbrend_direct(c);
				dstimage.at(x + W - xi - 1, y + yi).darknessbrend_direct(c);
				/*	if (iwarp_vals.do_bilinear){
				int     xi, yi, j;
				iwarpfloat  dx, dy, m0, m1;
				xi = (int)x;
				yi = (int)y;
				dx = x - xi;
				dy = y - yi;
				Color &p0 = srcimage->at(xi, yi);
				Color &p1 = srcimage->at(xi + 1, yi);
				Color &p2 = srcimage->at(xi, yi + 1);
				Color &p3 = srcimage->at(xi + 1, yi + 1);
				for (j = 0; j < 4; j++){
				m0 = p0[j] + dx * (p1[j] - p0[j]);
				m1 = p2[j] + dx * (p3[j] - p2[j]);
				color[j] = (unsigned char)(m0 + dy * (m1 - m0));
				}
				}else*/
			}
		}
	}

	void compress(){

		for (int yi = 0; yi < H; yi++){
			for (int xi = 0; xi < W; xi++){
				Color & c = srcimage->at(deform_vectors[yi][xi][0] + xi, deform_vectors[yi][xi][1] + yi);
				if (c.a == 0){
					deform_vectors[yi][xi][0] = 0;
					deform_vectors[yi][xi][1] = 0;
				}
			}
		}

	}

#define COMPRESSMODE "9R"
	void save(const char * name){
#ifndef _DEBUG
		gzFile zf = gzopen(name, "wb"COMPRESSMODE);
		/*for (V<iwarpfloat, 2> * p = (V<iwarpfloat, 2> *)&deform_vectors[H - 1][W - 1]; p != (V<iwarpfloat, 2> *)deform_vectors; p--){
			(*p)[0] = ROUND_NEARLY_ZERO_ARG((*p)[0], 1.0E2);
			(*p)[1] = ROUND_NEARLY_ZERO_ARG((*p)[1], 1.0E2);
		}*/
		char * filetype = "IWARP001";
		gzwrite(zf, filetype, sizeof(char) * 8);
		if (memcmp(filetype, "IWARP000", 8) == 0){
			gzwrite(zf, range.ary, sizeof(range.ary));
			for (int yi = range[2]; yi <= range[3]; yi++){
				for (int xi = range[0]; xi <= range[1]; xi++){
					gzwrite(zf, deform_vectors[yi][xi].ary, sizeof(iwarpfloat) * 2);
				}
			}
		}
		else if (memcmp(filetype, "IWARP001", 8) == 0){
			gzwrite(zf, deform_vectors, sizeof(deform_vectors));
		}
		gzclose(zf);
#endif
	}

	bool load(const char * name){

#ifndef _DEBUG
		gzFile zf = gzopen(name, "rb"COMPRESSMODE);
		char filetype[8];
		gzread(zf, filetype, sizeof(char) * 8);
		if (memcmp(filetype, "IWARP000", 8) == 0){
			gzread(zf, range.ary, sizeof(range.ary));
			memset(deform_vectors, 0, sizeof(deform_vectors));
			for (int yi = range[2]; yi <= range[3]; yi++){
				for (int xi = range[0]; xi <= range[1]; xi++){
					gzread(zf, deform_vectors[yi][xi].ary, sizeof(iwarpfloat)*2);
				}
			}			
		}
		else if (memcmp(filetype, "IWARP001", 8) == 0){
			gzread(zf, &deform_vectors[0][0], sizeof(deform_vectors));	
		}
		else{
			gzclose(zf);
			return false;
		}
		gzclose(zf);
#endif
		return true;
		//FILE * fp;
		//fopen_s(&fp, name, "rb");
		//if (!fp) return false;
		//char filetype[8];
		//fread(filetype, sizeof(char), 8, fp);
		//if (memcmp(filetype, "IWARP000", 8) == 0){
		//	fread(range.ary, sizeof(range.ary[0]), 4, fp);
		//	memset(deform_vectors, 0, sizeof(deform_vectors));
		//	for (int yi = range[2]; yi <= range[3]; yi++){
		//		for (int xi = range[0]; xi <= range[1]; xi++){
		//			fread(deform_vectors[yi][xi].ary, sizeof(iwarpfloat), 2, fp);
		//		}
		//	}
		//	//printf("%d,%d,%d,%d\n", range[0],range[1],range[2],range[3]);
		//}
		//else{
		//	fclose(fp);
		//	return false;
		//}
		//fclose(fp);
		//return true;
	}

	void set_srcimage(Pixmap & src){
		srcimage = &src;
	}

	iwarp_vals_t & values(){
		return iwarp_vals;
	}
};
