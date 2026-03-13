#include <windows.h>
#include <iostream>
#include <string>
#include <omp.h>
#include <string.h>
#include <stdio.h> // 用于 sprintf
#include <time.h>  // 用于 srand 和 time
#include <stdlib.h> // 用于 rand
#include <math.h>
#include "bmp.h"
#include "stl.h"
#include "charedit.h"
//#define Pi 3.14159265358979323846264338328

using namespace std;
// 全局变量
HBITMAP g_hDIBBitmap = NULL;
void* g_pDIBPixelData = NULL;
int g_bitmapWidth = 1600;
int g_bitmapHeight = 900;
int g_bitsPerPixel = 24; // 32位 BGRA (或RGBA，取决于系统和DIB_RGB_COLORS的默认解释)
int windowX, windowY;
char g_szClassName[] = "MyDynamicImageViewerClass";
bmp24 lsbmp,b,inimg,loading,bg,old_show,dice_faces[20];
double stan=0.1011,pianyi=0,jiange=19.6138/3;//"lineNumber":19.6138,"obliquity":0.1011,
bool update,dragging=false,got_para;
int *movex;
int *movey;
stlfile dice_ori,dice_pre[40],dice_bak,dice_end;
// 动态变化的参数，例如，用于生成颜色
int g_colorOffset = 0;
const int TIMER_ID = 1; // 计时器ID
RGBPixel rgbp;
HSVPixel hsvp;
// 函数声明
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int g_bitmapStride = 0,tmpy;

// 定义一个结构体来存储找到的显示器信息
typedef struct MonitorInfo {
	HMONITOR hMonitor;
	RECT     rcMonitor;	// 监视器的整个显示区域
    RECT     rcWork;	   // 监视器的工作区域（不包括任务栏等）
    BOOL     bPrimary;     // 是否是主显示器
} MonitorInfo;

// 存储所有显示器的信息
MonitorInfo g_monitors[16]; // 假设最多16个显示器
int g_monitorCount = 0;

// EnumDisplayMonitors 的回调函数
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);

    if (GetMonitorInfo(hMonitor, &monitorInfo)) {
        if (g_monitorCount < sizeof(g_monitors) / sizeof(g_monitors[0])) {
            g_monitors[g_monitorCount].hMonitor = hMonitor;
            g_monitors[g_monitorCount].rcMonitor = monitorInfo.rcMonitor;
            g_monitors[g_monitorCount].rcWork = monitorInfo.rcWork;
            g_monitors[g_monitorCount].bPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
            g_monitorCount++;
        }
    }
    return TRUE; // 继续枚举下一个监视器
}
// 创建 DIB Section
// 修改 CreateDIBSectionBitmap 函数以计算 stride
HBITMAP CreateDIBSectionBitmap(HDC hdc, int width, int height, int bitsPerPixel, void** pBits) {
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // 负高度表示顶到底的DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = bitsPerPixel;
    bmi.bmiHeader.biCompression = BI_RGB; // 无压缩

    // 计算 stride (每行的实际字节数，包括填充)
    // 对于 BI_RGB 压缩，Windows GDI 会自动进行 4 字节对齐
    int bytesPerPixel = bitsPerPixel / 8;
    g_bitmapStride = ((width * bytesPerPixel + 3) / 4) * 4; // 确保是4的倍数

    return CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, pBits, NULL, 0);
}

int ox=360,oy=920,xe=360,ye=920,dx,dy,ticks=1,field_x=719,field_y=1839,dice_size=360,nordep=7;
vec3d rotation_axis,target_axis,real_position,real_right,wanted_position=vec3d(0,0,1),wanted_axis;
POINT mouse_start;
float squeeze_x[40];
RGBPixel face_color[40];
//0.0000703125
#define shicha 0.00028125
//face 0.7946544450  len 0.3420201433  3:7
float pingmian=0.7946544450;
double ax[6],ay[6],az[6];
#define EPS 1e-6
void showdice(float dis,int x,int y,float bs){ 
	float ux,uy,vx,vy,det,px,py,u,v;
	int maxx,maxy,minx,miny,loctmp;
	bool calced;
    for(int i=0;i<40;i++)memcpy(dice_pre[i].f,dice_ori.f,dice_ori.number*sizeof(facet));
    // 设置线程数为12
    omp_set_num_threads(12);
    
    // 简单的并行化：只对最外层i循环并行
    #pragma omp parallel for private(ux, uy, vx, vy, det, minx, maxx, miny, maxy, loctmp, px, py, u, v, calced) schedule(dynamic)
    for(int i=0;i<40;i++)
    {
        for(int j=0;j<dice_pre[i].number;j++)
        {
        	for(int k=0;k<3;k++)
        	{
				dice_pre[i].f[j].vertex[k].z=dice_pre[i].f[j].vertex[k].z-pingmian;
				dice_pre[i].f[j].vertex[k].x=(dice_pre[i].f[j].vertex[k].x+dice_pre[i].f[j].vertex[k].z*(squeeze_x[i]+shicha*(x-720)))*dis/(dis-dice_pre[i].f[j].vertex[k].z)*bs;
				dice_pre[i].f[j].vertex[k].y=(dice_pre[i].f[j].vertex[k].y+dice_pre[i].f[j].vertex[k].z*(shicha*(y-1280)))*dis/(dis-dice_pre[i].f[j].vertex[k].z)*bs;
				//dice_pre[i].f[j].vertex[k].z=dice_pre[i].f[j].vertex[k].z*bs;
			}
			dice_pre[i].f[j].calc();
        }
        for(int j=0;j<dice_pre[i].number;j++)
        {
            if(dice_pre[i].f[j].normal.z<=0) continue;
            
            ux=dice_pre[i].f[j].vertex[1].x-dice_pre[i].f[j].vertex[0].x;
            uy=dice_pre[i].f[j].vertex[1].y-dice_pre[i].f[j].vertex[0].y;
            vx=dice_pre[i].f[j].vertex[2].x-dice_pre[i].f[j].vertex[0].x;
            vy=dice_pre[i].f[j].vertex[2].y-dice_pre[i].f[j].vertex[0].y;
			det=ux*vy-vx*uy;
			if (fabsf(det) < 1e-6f) continue;
			double y_min = fmin(dice_pre[i].f[j].vertex[0].y, fmin(dice_pre[i].f[j].vertex[1].y, dice_pre[i].f[j].vertex[2].y))+y;
			double y_max = fmax(dice_pre[i].f[j].vertex[0].y, fmax(dice_pre[i].f[j].vertex[1].y, dice_pre[i].f[j].vertex[2].y))+y;
			int y_from = max((int)floor(y_min),0);
			int y_to   = min((int)ceil(y_max),lsbmp.hei-1);
			if(y_from>=lsbmp.hei||y_to<0)continue; 
			// 对每个整数 y 扫描
			for (int yy = y_from; yy <= y_to; ++yy) {
				double x_left = INFINITY;   // 左边界初始化为正无穷
				double x_right = -INFINITY; // 右边界初始化为负无穷
				// 处理三条边
				for (int ii = 0; ii < 3; ++ii) {
					double xa = dice_pre[i].f[j].vertex[ii].x+x, ya = dice_pre[i].f[j].vertex[ii].y+y;
					double xb = dice_pre[i].f[j].vertex[(ii + 1) % 3].x+x, yb = dice_pre[i].f[j].vertex[(ii + 1) % 3].y+y;
					// 保证 ya <= yb 以便判断范围
					if (ya > yb) {
						double tmp;
						tmp = xa; xa = xb; xb = tmp;
						tmp = ya; ya = yb; yb = tmp;
					}
					// 检查当前 y 是否在边的纵坐标范围内（含端点）
					if (yy < ya - EPS || yy > yb + EPS)
						continue;
					// 处理水平边
					if (fabs(ya - yb) < EPS) {
						if (fabs(yy - ya) < EPS) { // y 恰好等于边的纵坐标
							double xmin = fmin(xa, xb);
							double xmax = fmax(xa, xb);
							if (xmin < x_left)  x_left = xmin;
							if (xmax > x_right) x_right = xmax;
						}
					}
					// 普通边，线性插值求交点
					else {
						double tt = (yy - ya) / (yb - ya); // t 在 [0,1] 内
						double x_inst = xa + tt * (xb - xa);
						if (x_inst < x_left)  x_left = x_inst;
						if (x_inst > x_right) x_right = x_inst;
					}
				}
				if (lsbmp.wid <= x_left) continue;
				if (0 > x_right) continue;
				if(x_right>=lsbmp.wid)x_right=lsbmp.wid-1;
				if(x_left<0)x_left=0;
				// 根据左右边界输出整数点
				if (x_left <= x_right + EPS) { // 有效区间
					int ix_min = (int)ceil(x_left - EPS);
					int ix_max = (int)floor(x_right + EPS);
					for (int xx = ix_min; xx <= ix_max; ++xx) 
					{
	                    calced=false;
	                    if(b.p[yy*lsbmp.wid+xx].R==i)
	                    {
	                        px=xx-x-dice_pre[i].f[j].vertex[0].x;
	                        py=yy-y-dice_pre[i].f[j].vertex[0].y;
	                        u=(px*vy-py*vx)/det;
	                        v=(py*ux-px*uy)/det;
	                        calced=true;
	                        if(u<0||v<0||u+v>1) continue;
	                        loctmp=min(((int)(v*255)),255)*256+min((int)(u*255),255);
	                        //lsbmp.p[xx+yy*lsbmp.wid].R=face_color[j].R;
	                        lsbmp.p[xx+yy*lsbmp.wid].R=dice_faces[j].p[loctmp].R;
	                    }
	                    if(b.p[yy*lsbmp.wid+xx].G==i)
	                    {
	                        if(!calced)
	                        {
	                            px=xx-x-dice_pre[i].f[j].vertex[0].x;
	                            py=yy-y-dice_pre[i].f[j].vertex[0].y;
	                            u=(px*vy-py*vx)/det;
	                            v=(py*ux-px*uy)/det;
	                            calced=true;
	                            if(u<0||v<0||u+v>1) continue;
								loctmp=min(((int)(v*255)),255)*256+min((int)(u*255),255);
	                        }
	                        //lsbmp.p[xx+yy*lsbmp.wid].G=face_color[j].G;
	                        lsbmp.p[xx+yy*lsbmp.wid].G=dice_faces[j].p[loctmp].G;
	                    }
	                    if(b.p[yy*lsbmp.wid+xx].B==i)
	                    {
	                        if(!calced)
	                        {
	                            px=xx-x-dice_pre[i].f[j].vertex[0].x;
	                            py=yy-y-dice_pre[i].f[j].vertex[0].y;
	                            u=(px*vy-py*vx)/det;
	                            v=(py*ux-px*uy)/det;
	                            calced=true;
	                            if(u<0||v<0||u+v>1) continue;
								loctmp=min(((int)(v*255)),255)*256+min((int)(u*255),255);
	                        }
	                        //lsbmp.p[xx+yy*lsbmp.wid].B=face_color[j].B;
	                        lsbmp.p[xx+yy*lsbmp.wid].B=dice_faces[j].p[loctmp].B;
	                    }
					}
				}
			}
			/*************************************************************/
        }
    }
}
void showdice_bak(float dis,int x,int y,float bs){ 
	float ux,uy,vx,vy,det,px,py,u,v;
	int maxx,maxy,minx,miny,loctmp;
	bool calced;
    for(int i=0;i<40;i++)memcpy(dice_pre[i].f,dice_ori.f,dice_ori.number*sizeof(facet));
    // 设置线程数为12
    omp_set_num_threads(12);
    
    // 简单的并行化：只对最外层i循环并行
    #pragma omp parallel for private(ux, uy, vx, vy, det, minx, maxx, miny, maxy, loctmp, px, py, u, v, calced) schedule(dynamic)
    for(int i=0;i<40;i++)
    {
        for(int j=0;j<dice_pre[i].number;j++)
        {
        	for(int k=0;k<3;k++)
        	{
				dice_pre[i].f[j].vertex[k].z=dice_pre[i].f[j].vertex[k].z-pingmian;
				dice_pre[i].f[j].vertex[k].x=(dice_pre[i].f[j].vertex[k].x+dice_pre[i].f[j].vertex[k].z*(squeeze_x[i]+shicha*(x-720)))*dis/(dis-dice_pre[i].f[j].vertex[k].z)*bs;
				dice_pre[i].f[j].vertex[k].y=(dice_pre[i].f[j].vertex[k].y+dice_pre[i].f[j].vertex[k].z*(shicha*(y-1280)))*dis/(dis-dice_pre[i].f[j].vertex[k].z)*bs;
				//dice_pre[i].f[j].vertex[k].z=dice_pre[i].f[j].vertex[k].z*bs;
			}
			dice_pre[i].f[j].calc();
        }
        for(int j=0;j<dice_pre[i].number;j++)
        {
            if(dice_pre[i].f[j].normal.z<=0) continue;
            
            ux=dice_pre[i].f[j].vertex[1].x-dice_pre[i].f[j].vertex[0].x;
            uy=dice_pre[i].f[j].vertex[1].y-dice_pre[i].f[j].vertex[0].y;
            vx=dice_pre[i].f[j].vertex[2].x-dice_pre[i].f[j].vertex[0].x;
            vy=dice_pre[i].f[j].vertex[2].y-dice_pre[i].f[j].vertex[0].y;
            det=ux*vy-vx*uy;
            if (fabsf(det) < 1e-6f) continue;
            
            minx=max((int)floorf(fminf(fminf(dice_pre[i].f[j].vertex[0].x,dice_pre[i].f[j].vertex[1].x),dice_pre[i].f[j].vertex[2].x))+x,0);
            maxx=min((int) ceilf(fmaxf(fmaxf(dice_pre[i].f[j].vertex[0].x,dice_pre[i].f[j].vertex[1].x),dice_pre[i].f[j].vertex[2].x))+x,lsbmp.wid-1);
            miny=max((int)floorf(fminf(fminf(dice_pre[i].f[j].vertex[0].y,dice_pre[i].f[j].vertex[1].y),dice_pre[i].f[j].vertex[2].y))+y,0);
            maxy=min((int) ceilf(fmaxf(fmaxf(dice_pre[i].f[j].vertex[0].y,dice_pre[i].f[j].vertex[1].y),dice_pre[i].f[j].vertex[2].y))+y,lsbmp.hei-1);
            
            // 内部循环不并行化，避免过度开销
            for(int yy=miny; yy<=maxy; yy++)
            {
                for(int xx=minx; xx<=maxx; xx++)
                {
                    calced=false;
                    if(b.p[yy*lsbmp.wid+xx].R==i)
                    {
                        px=xx-x-dice_pre[i].f[j].vertex[0].x;
                        py=yy-y-dice_pre[i].f[j].vertex[0].y;
                        u=(px*vy-py*vx)/det;
                        v=(py*ux-px*uy)/det;
                        calced=true;
                        if(u<0||v<0||u+v>1) continue;
                        loctmp=((int)(v*255))*256+(int)(u*255);
                        //lsbmp.p[xx+yy*lsbmp.wid].R=face_color[j].R;
                        lsbmp.p[xx+yy*lsbmp.wid].R=dice_faces[j].p[loctmp].R;
                    }
                    if(b.p[yy*lsbmp.wid+xx].G==i)
                    {
                        if(!calced)
                        {
                            px=xx-x-dice_pre[i].f[j].vertex[0].x;
                            py=yy-y-dice_pre[i].f[j].vertex[0].y;
                            u=(px*vy-py*vx)/det;
                            v=(py*ux-px*uy)/det;
                            calced=true;
                            if(u<0||v<0||u+v>1) continue;
                        	loctmp=((int)(v*255))*256+(int)(u*255);
                        }
                        //lsbmp.p[xx+yy*lsbmp.wid].G=face_color[j].G;
                        lsbmp.p[xx+yy*lsbmp.wid].G=dice_faces[j].p[loctmp].G;
                    }
                    if(b.p[yy*lsbmp.wid+xx].B==i)
                    {
                        if(!calced)
                        {
                            px=xx-x-dice_pre[i].f[j].vertex[0].x;
                            py=yy-y-dice_pre[i].f[j].vertex[0].y;
                            u=(px*vy-py*vx)/det;
                            v=(py*ux-px*uy)/det;
                            calced=true;
                            if(u<0||v<0||u+v>1) continue;
                        	loctmp=((int)(v*255))*256+(int)(u*255);
                        }
                        //lsbmp.p[xx+yy*lsbmp.wid].B=face_color[j].B;
                        lsbmp.p[xx+yy*lsbmp.wid].B=dice_faces[j].p[loctmp].B;
                    }
                }
            }
        }
    }
}
void get_rotate_para(double *a, float ome, float phi){
	a[0]=0;
	a[1]=ome;
	a[2]=0;
	a[3]=(10*phi-6*ome*20)/(20*20*20);
	a[4]=(-15*phi+8*20*ome)/(20*20*20*20);
	a[5]=(6*phi-3*20*ome)/(20*20*20*20*20);
}
void get_wanted_axis(vec3d R1, vec3d R2, vec3d R3){
	double the=acos((R1.x+R2.y+R3.z-1)/2);
	wanted_axis.x=R2.z-R3.y;
	wanted_axis.y=R3.x-R1.z;
	wanted_axis.z=R1.y-R2.x;
	if(wanted_axis.length()<1e-6)wanted_axis=vec3d(0,0,1);
	wanted_axis.guiyihua();
	wanted_axis=wanted_axis*the;
	get_rotate_para(ax,rotation_axis.x,wanted_axis.x);
	get_rotate_para(ay,rotation_axis.y,wanted_axis.y);
	get_rotate_para(az,rotation_axis.z,wanted_axis.z);
}
void UpdateBitmapPixels() {
    if (g_pDIBPixelData == NULL) return;
    unsigned char* pixels = (unsigned char*)g_pDIBPixelData;
    int bytesPerPixel = g_bitsPerPixel / 8; // 例如，对于 24 位，bytesPerPixel = 3
    if(got_para==false){
		GetCursorPos(&mouse_start);
		tmpy = 2559 - mouse_start.y + windowY;
		memcpy(lsbmp.p,old_show.p,lsbmp.wid*lsbmp.hei*3);
		if(tmpy<280)tmpy=280;
		if(tmpy>=2280)tmpy=2279;
		for(int i=0;i<75;i++)
		{
			int starty=tmpy+i;
			if(starty>=2280)starty-=2000;
			for(int j=0;j<680;j++)if(lsbmp.p[starty*lsbmp.wid+j].G<151-2*i)lsbmp.p[starty*lsbmp.wid+j].G=150-2*i;
			for(int j=760;j<1440;j++)if(lsbmp.p[starty*lsbmp.wid+j].G<151-2*i)lsbmp.p[starty*lsbmp.wid+j].G=150-2*i;
			starty=tmpy-i;
			if(starty<280)starty+=2000;
			for(int j=0;j<680;j++)if(lsbmp.p[starty*lsbmp.wid+j].G<151-2*i)lsbmp.p[starty*lsbmp.wid+j].G=150-2*i;
			for(int j=760;j<1440;j++)if(lsbmp.p[starty*lsbmp.wid+j].G<151-2*i)lsbmp.p[starty*lsbmp.wid+j].G=150-2*i;
		}
    }
	else if (dragging){
		memcpy(lsbmp.p,old_show.p,old_show.wid*old_show.hei*3);
		lsbmp.draw_line(mouse_start.x, mouse_start.y, mouse_start.x + dx, mouse_start.y + dy, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x + 1, mouse_start.y, mouse_start.x + dx + 1, mouse_start.y + dy, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x - 1, mouse_start.y, mouse_start.x + dx - 1, mouse_start.y + dy, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x, mouse_start.y + 1, mouse_start.x + dx, mouse_start.y + dy + 1, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x, mouse_start.y - 1, mouse_start.x + dx, mouse_start.y + dy - 1, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x + 1, mouse_start.y + 1, mouse_start.x + dx + 1, mouse_start.y + dy + 1, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x - 1, mouse_start.y + 1, mouse_start.x + dx - 1, mouse_start.y + dy + 1, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x + 1, mouse_start.y - 1, mouse_start.x + dx + 1, mouse_start.y + dy - 1, RGBPixel(0,255,0));
		lsbmp.draw_line(mouse_start.x - 1, mouse_start.y - 1, mouse_start.x + dx - 1, mouse_start.y + dy - 1, RGBPixel(0,255,0));
	}
	else
	{
		ticks--;//min 22->21
		memcpy(lsbmp.p,bg.p,bg.wid*bg.hei*3);
		xe=ox+movex[ticks];
		ye=oy+movey[ticks];
		target_axis.x=1;
		target_axis.y=1;
		int vx,vy,vx2,vy2,xe2,ye2;
		float tx,ty;
		vx=xe<0 ? (xe-field_x+1)/field_x : xe/field_x;
		if(vx%2==0)xe=xe-vx*field_x;
		else
		{
			xe=vx*field_x+field_x-xe;
			target_axis.y=-1;
		}
		vy=ye<0 ? (ye-field_y+1)/field_y : ye/field_y;
		if(vy%2==0)ye=ye-vy*field_y;
		else
		{
			ye=vy*field_y+field_y-ye;
			target_axis.x=-1;
		}
		showdice(14400.0/dice_size,xe+(1440-field_x)/2,ye+(2560-field_y)/2,dice_size);
		//printf("ticks =%6d, (%d,%d)\n",ticks,xe+(1440-field_x)/2,ye+(2560-field_y)/2);
		if(ticks<=0)
		{
			update=false;
			rotation_axis=vec3d(0,0,0);
			ticks=0;
			memcpy(old_show.p,lsbmp.p,lsbmp.wid*lsbmp.hei*3);
		}
		else
		{
			if(ticks>20){
				target_axis.z=0;
				tx=2;
				ty=2;
				xe2=ox+movex[ticks-1];
				vx2= xe2<0 ? (xe2-field_x+1)/field_x : xe2/field_x;
				if(vx!=vx2)
				{
					int x_last= xe2<0 ? (xe2+1)/field_x : xe2/field_x;
					tx=(float)(xe2-x_last*field_x)/(movex[ticks-1]-movex[ticks]);
					ye2=oy+movey[ticks-1]+tx*(movey[ticks]-movey[ticks-1]);
					vy2= ye2<0 ? (ye2-field_y+1)/field_y : ye2/field_y;
					if((vy2-x_last)%2==0)target_axis.z=(float)(movey[ticks-1]-movey[ticks])/dice_size;
					else target_axis.z=(float)(movey[ticks]-movey[ticks-1])/dice_size;
				}
				ye2=oy+movey[ticks-1];
				vy2= ye2<0 ? (ye2-field_y+1)/field_y : ye2/field_y;
				if(vy!=vy2)
				{
					int y_last= ye2<0 ? (ye2+1)/field_y : ye2/field_y;
					ty=(float)(ye2-y_last*field_y)/(movey[ticks-1]-movey[ticks]);
					if(ty<tx)
					{
						xe2=ox+movex[ticks-1]+ty*(movex[ticks]-movex[ticks-1]);
						vx2= xe2<0 ? (xe2-field_x+1)/field_x : xe2/field_x;
						if((vx2-y_last)%2==0)target_axis.z=(float)(movex[ticks]-movex[ticks-1])/dice_size;
						else target_axis.z=(float)(movex[ticks-1]-movex[ticks])/dice_size;
					//printf("z=%f\n",target_axis.z);
					}
				}
				target_axis.y=target_axis.y*(movex[ticks-1]-movex[ticks])/dice_size;
				target_axis.x=target_axis.x*(movey[ticks]-movey[ticks-1])/dice_size;
				target_axis.z*=10;
				//rotation_axis.z=0;
				target_axis=target_axis-rotation_axis;
				target_axis.x*=0.2;
				target_axis.y*=0.2;
				target_axis.z*=0.1;
				rotation_axis=rotation_axis+target_axis;
				dice_ori.rotate(rotation_axis,rotation_axis.length());
			}
			else
			{
				if(ticks==20)//1 4 9 16 25 36 49 64 81 100 119 136 151 164 175 184 191 196 199 200
				{
					memcpy(dice_end.f,dice_ori.f,dice_ori.number*sizeof(facet));
					//dice_end.rotate(rotation_axis,rotation_axis.length()*20);
					int dice_ans;
					float maxfs=0;
					for(int i=0;i<20;i++)
					{
						if(dice_end.f[i].normal.z>maxfs)
						{
							dice_ans=i;
							maxfs=dice_end.f[i].normal.z;
						}
					}
					real_position=dice_ori.f[dice_ans].normal;
					real_position.guiyihua();
					real_right=dice_ori.f[dice_ans].vertex[1]-dice_ori.f[dice_ans].vertex[0];
					real_right.guiyihua();
					dice_end.f[0].vertex[0]=vec3d(1,0,0);
					dice_end.f[0].vertex[1]=vec3d(0,1,0);
					dice_end.f[0].vertex[2]=vec3d(0,0,1);
					dice_end.transform(real_position, real_right, vec3d(0,0,0), vec3d(0,0,1), vec3d(1,0,0), vec3d(0,1,0));
					get_wanted_axis(dice_end.f[0].vertex[0], dice_end.f[0].vertex[1], dice_end.f[0].vertex[2]);
					memcpy(dice_end.f,dice_ori.f,dice_ori.number*sizeof(facet));
				}
				int tt=21-ticks;
				target_axis.x=ax[1]*tt+ax[3]*tt*tt*tt+ax[4]*tt*tt*tt*tt+ax[5]*tt*tt*tt*tt*tt;
				target_axis.y=ay[1]*tt+ay[3]*tt*tt*tt+ay[4]*tt*tt*tt*tt+ay[5]*tt*tt*tt*tt*tt;
				target_axis.z=az[1]*tt+az[3]*tt*tt*tt+az[4]*tt*tt*tt*tt+az[5]*tt*tt*tt*tt*tt;
				memcpy(dice_ori.f,dice_end.f,dice_ori.number*sizeof(facet));
				dice_ori.rotate(target_axis,target_axis.length());
			}
		}
	}
    for (int y = 0; y < g_bitmapHeight; ++y) {
        int lineStart = y * g_bitmapStride;

        for (int x = 0; x < g_bitmapWidth; ++x) {
            int index = lineStart + x * bytesPerPixel;
            int loc=(g_bitmapHeight-1-y)*g_bitmapWidth+x;
            pixels[index + 0] = lsbmp.p[loc].B;
            pixels[index + 1] = lsbmp.p[loc].G;
            pixels[index + 2] = lsbmp.p[loc].R;
        }
    }
}
HBITMAP copybmp(HDC hdc, bmp24 input){
	g_bitmapWidth = input.wid;
	g_bitmapHeight = input.hei;
	g_hDIBBitmap = CreateDIBSectionBitmap(hdc, g_bitmapWidth, g_bitmapHeight, 24, &g_pDIBPixelData);
    unsigned char* pixels = (unsigned char*)g_pDIBPixelData;
    for (int y = 0; y < g_bitmapHeight; ++y) {
        int lineStart = y * g_bitmapStride;
        for (int x = 0; x < g_bitmapWidth; ++x) {
            int index = lineStart + x * 3; // 在当前行内的偏移量
            int loc=(g_bitmapHeight-1-y)*g_bitmapWidth+x;
            pixels[index + 0] = input.p[loc].B;
            pixels[index + 1] = input.p[loc].G;
            pixels[index + 2] = input.p[loc].R;
        }
    }
    return g_hDIBBitmap;
}

class NamedPipeClient {
private:
    HANDLE hPipe;
    std::string pipeName;

public:
    NamedPipeClient(const std::string& name) : pipeName(name), hPipe(INVALID_HANDLE_VALUE) {}
    ~NamedPipeClient(){disconnect();}
    bool connect() {
        hPipe = CreateFileA(
            pipeName.c_str(),           // 管道名称
            GENERIC_READ | GENERIC_WRITE, // 读写权限
            FILE_SHARE_READ | FILE_SHARE_WRITE, // 共享模式
            NULL,                       // 默认安全属性
            OPEN_EXISTING,              // 打开已存在的管道
            FILE_ATTRIBUTE_NORMAL,      // 文件属性
            NULL                        // 无模板文件
        );
        
        if (hPipe == INVALID_HANDLE_VALUE) 
		{
            DWORD error = GetLastError();
            /*
            std::cerr << "无法连接到管道: " << pipeName << std::endl;
            std::cerr << "错误代码: " << error << std::endl;
            if (error == ERROR_PIPE_BUSY){std::cerr << "管道正忙，请稍后重试" << std::endl;}
			else if (error == ERROR_FILE_NOT_FOUND) {std::cerr << "管道不存在，请确保服务器正在运行" << std::endl;}
			else if (error == ERROR_ACCESS_DENIED) {std::cerr << "访问被拒绝" << std::endl;}
			*/
            return false;
        }
        
        return true;
    }
    void disconnect() {
        if (hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }
    }
    bool sendCommand(const std::string& command, std::string& response) {
        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "未连接到管道" << std::endl;
            return false;
        }
        DWORD bytesWritten;
        BOOL writeResult = WriteFile(
            hPipe,                      // 管道句柄
            command.c_str(),            // 要发送的数据
            static_cast<DWORD>(command.length()), // 数据长度
            &bytesWritten,              // 实际写入的字节数
            NULL                        // 非重叠I/O
        );
        if (!writeResult) {
            std::cerr << "发送命令失败，错误代码: " << GetLastError() << std::endl;
            return false;
        }
        char buffer[2048];
        DWORD bytesRead;
        BOOL readResult = ReadFile(
            hPipe,                      // 管道句柄
            buffer,                     // 接收缓冲区
            sizeof(buffer) - 1,         // 缓冲区大小
            &bytesRead,                 // 实际读取的字节数
            NULL                        // 非重叠I/O
        );
        if (readResult && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response = std::string(buffer, bytesRead);
            return true;
        } else {
            DWORD error = GetLastError();
            std::cerr << "读取响应失败，错误代码: " << error << std::endl;
            return false;
        }
    }
};

unsigned char fonts[6][71*7]={//56*71
0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFC,0X00,0X00,0X00,0X01,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X01,0XFF,0XFF,0XC7,0XFF,0XFF,0X00,0X03,0XFF,0XFE,0X00,0XFF,0XFF,0X80,0X03,0XFF,0XF8,0X00,0X7F,0XFF,0X80,0X07,0XFF,0XF0,0X00,0X1F,0XFF,0XC0,0X0F,0XFF,0XE0,0X00,0X1F,0XFF,0XC0,0X0F,0XFF,0XE0,0X00,0X0F,0XFF,0XE0,0X1F,0XFF,0XC0,0X00,0X07,0XFF,0XE0,0X1F,0XFF,0X80,0X00,0X07,0XFF,0XF0,0X1F,0XFF,0X80,0X00,0X03,0XFF,0XF0,0X3F,0XFF,0X80,0X00,0X03,0XFF,0XF0,0X3F,0XFF,0X00,0X00,0X01,0XFF,0XF8,0X7F,0XFF,0X00,0X00,0X01,0XFF,0XF8,0X7F,0XFE,0X00,0X00,0X01,0XFF,0XF8,0X7F,0XFE,0X00,0X00,0X01,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFE,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFE,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X00,0X7F,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFE,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFC,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFE,0X00,0X00,0X00,0XFF,0XFC,0XFF,0XFE,0X00,0X00,0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X00,0XFF,0XFC,0X7F,0XFE,0X00,0X00,0X01,0XFF,0XF8,0X7F,0XFF,0X00,0X00,0X01,0XFF,0XF8,0X3F,0XFF,0X00,0X00,0X01,0XFF,0XF8,0X3F,0XFF,0X00,0X00,0X01,0XFF,0XF8,0X1F,0XFF,0X80,0X00,0X03,0XFF,0XF0,0X1F,0XFF,0X80,0X00,0X03,0XFF,0XF0,0X1F,0XFF,0XC0,0X00,0X07,0XFF,0XE0,0X0F,0XFF,0XC0,0X00,0X0F,0XFF,0XE0,0X0F,0XFF,0XE0,0X00,0X0F,0XFF,0XC0,0X07,0XFF,0XF0,0X00,0X1F,0XFF,0XC0,0X07,0XFF,0XF8,0X00,0X3F,0XFF,0X80,0X03,0XFF,0XFC,0X00,0X7F,0XFF,0X80,0X01,0XFF,0XFF,0X01,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X03,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X00,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X3F,0XFF,0XF8,0X00,0X00,0X00,0X00,0X00,0XFF,0X00,0X00,0X00,
0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0XE0,0X00,0X03,0XFF,0XF0,0X00,0X00,0XFE,0X00,0X03,0XFF,0XF0,0X00,0X00,0XFF,0XC0,0X03,0XFF,0XF0,0X00,0X00,0XFF,0XF0,0X03,0XFF,0XF0,0X00,0X00,0XFF,0XFE,0X03,0XFF,0XF0,0X00,0X00,0XFF,0XFF,0X83,0XFF,0XF0,0X00,0X00,0XFF,0XFF,0XC3,0XFF,0XF0,0X00,0X00,0X7F,0XFF,0XF3,0XFF,0XF0,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X03,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X7F,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X1F,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X0F,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X07,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X01,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X00,0XFF,0XFF,0XF0,0X00,0X00,0X00,0X00,0X7F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,0X7F,0XFF,0X00,0X00,0X00,0X00,0X00,0X3F,0XFF,0X80,0X00,0X00,0X00,0X00,0X3F,0XFF,0XC0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XE0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X07,0XFF,0XFC,0X00,0X00,0X00,0X00,0X03,0XFF,0XFE,0X00,0X00,0X00,0X00,0X01,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0X7F,0XFF,0XC0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XE0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XF8,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFC,0X00,0X00,0X00,0X00,0X07,0XFF,0XFE,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0XFF,0XFF,0XE0,0X00,0X00,0X00,0X00,0X7F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFC,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFE,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0X80,0X00,0X00,0X00,0X03,0XFF,0XFF,0XC0,0X00,0X00,0X00,0X00,0XFF,0XFF,0XE0,0X00,0X00,0X00,0X00,0X7F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XF8,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFC,0X00,0X00,0X00,0X00,0X07,0XFF,0XFE,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0X80,0X00,0X00,0X00,0X00,0X7F,0XFF,0XC0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XC0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XE0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XE0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X01,0XFF,0XF8,0X00,0X00,0X00,0X00,0X01,0XFF,0XF8,0XFF,0XF8,0X00,0X00,0X01,0XFF,0XF8,0XFF,0XFC,0X00,0X00,0X01,0XFF,0XF8,0XFF,0XFC,0X00,0X00,0X01,0XFF,0XF8,0X7F,0XFE,0X00,0X00,0X01,0XFF,0XF8,0X7F,0XFE,0X00,0X00,0X01,0XFF,0XF8,0X3F,0XFF,0X00,0X00,0X03,0XFF,0XF8,0X3F,0XFF,0X00,0X00,0X03,0XFF,0XF8,0X3F,0XFF,0X80,0X00,0X03,0XFF,0XF0,0X1F,0XFF,0XC0,0X00,0X07,0XFF,0XF0,0X1F,0XFF,0XE0,0X00,0X0F,0XFF,0XF0,0X0F,0XFF,0XF0,0X00,0X1F,0XFF,0XE0,0X07,0XFF,0XFC,0X00,0X3F,0XFF,0XE0,0X03,0XFF,0XFF,0X81,0XFF,0XFF,0XC0,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X01,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X00,0X1F,0XFF,0XF8,0X00,0X00,
0X00,0X00,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFE,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X7F,0XFF,0XFE,0X07,0XFF,0XFF,0XF0,0X7F,0XFF,0XC0,0X00,0X7F,0XFF,0XF8,0X7F,0XFE,0X00,0X00,0X1F,0XFF,0XF8,0X7F,0XF8,0X00,0X00,0X07,0XFF,0XF8,0X7F,0XE0,0X00,0X00,0X03,0XFF,0XFC,0X7F,0X80,0X00,0X00,0X03,0XFF,0XFC,0X7E,0X00,0X00,0X00,0X01,0XFF,0XFC,0X7C,0X00,0X00,0X00,0X01,0XFF,0XFC,0X78,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XE0,0X00,0X00,0X00,0X01,0XFF,0XFF,0XC0,0X00,0X00,0X00,0X3F,0XFF,0XFF,0XC0,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X00,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XC0,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFE,0X00,0X00,0X00,0X00,0X01,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X7F,0XFF,0X80,0X00,0X00,0X00,0X00,0X3F,0XFF,0XC0,0X00,0X00,0X00,0X00,0X1F,0XFF,0XC0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XE0,0X00,0X00,0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X00,0X00,0X07,0XFF,0XE0,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0X00,0X00,0X00,0X00,0X03,0XFF,0XF0,0XFF,0XF8,0X00,0X00,0X03,0XFF,0XF0,0XFF,0XF8,0X00,0X00,0X03,0XFF,0XF0,0XFF,0XFC,0X00,0X00,0X03,0XFF,0XF0,0X7F,0XFC,0X00,0X00,0X07,0XFF,0XF0,0X7F,0XFE,0X00,0X00,0X07,0XFF,0XF0,0X7F,0XFE,0X00,0X00,0X07,0XFF,0XE0,0X3F,0XFF,0X00,0X00,0X0F,0XFF,0XE0,0X3F,0XFF,0X80,0X00,0X1F,0XFF,0XE0,0X1F,0XFF,0XC0,0X00,0X3F,0XFF,0XC0,0X1F,0XFF,0XF0,0X00,0X7F,0XFF,0XC0,0X0F,0XFF,0XFE,0X03,0XFF,0XFF,0X80,0X07,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XC0,0X00,0X00,0X03,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0X7F,0XFF,0XF8,0X00,0X00,
0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X1F,0XFF,0X00,0X7F,0XFE,0X00,0X00,0X1F,0XFF,0X00,0X3F,0XFE,0X00,0X00,0X1F,0XFF,0X00,0X1F,0XFF,0X00,0X00,0X1F,0XFF,0X00,0X1F,0XFF,0X80,0X00,0X1F,0XFF,0X00,0X0F,0XFF,0XC0,0X00,0X1F,0XFF,0X00,0X07,0XFF,0XC0,0X00,0X1F,0XFF,0X00,0X03,0XFF,0XE0,0X00,0X1F,0XFF,0X00,0X03,0XFF,0XF0,0X00,0X1F,0XFF,0X00,0X01,0XFF,0XF8,0X00,0X1F,0XFF,0X00,0X00,0XFF,0XF8,0X00,0X1F,0XFF,0X00,0X00,0X7F,0XFC,0X00,0X1F,0XFF,0X00,0X00,0X7F,0XFE,0X00,0X1F,0XFF,0X00,0X00,0X3F,0XFF,0X00,0X1F,0XFF,0X00,0X00,0X1F,0XFF,0X00,0X1F,0XFF,0X00,0X00,0X0F,0XFF,0X80,0X1F,0XFF,0X00,0X00,0X0F,0XFF,0XC0,0X1F,0XFF,0X00,0X00,0X07,0XFF,0XE0,0X1F,0XFF,0X00,0X00,0X03,0XFF,0XE0,0X1F,0XFF,0X00,0X00,0X01,0XFF,0XF0,0X1F,0XFF,0X00,0X00,0X01,0XFF,0XF8,0X1F,0XFF,0X00,0X00,0X00,0XFF,0XFC,0X1F,0XFF,0X00,0X00,0X00,0X7F,0XFE,0X1F,0XFF,0X00,0X00,0X00,0X3F,0XFE,0X1F,0XFF,0X00,0X00,0X00,0X3F,0XFF,0X1F,0XFF,0X00,0X00,0X00,0X1F,0XFF,0X9F,0XFF,0X00,0X00,0X00,0X0F,0XFF,0XDF,0XFF,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X01,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,0X00,0X00,0X00,0X00,0X3F,0XFF,0XFF,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFF,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0X00,0X00,0X00,0X00,0X03,0XFF,0XFF,0X00,0X00,0X00,0X00,0X01,0XFF,0XFF,0X00,0X00,0X00,0X00,0X01,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X7F,0XFF,0X00,0X00,0X00,0X00,0X00,0X3F,0XFF,0X00,0X00,0X00,0X00,0X00,0X3F,0XFF,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0X00,
0X00,0X00,0X0F,0XFF,0XC0,0X00,0X00,0X00,0X01,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X01,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X07,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XC0,0X7F,0XFF,0XFC,0X1F,0XFF,0XFF,0XE0,0X7F,0XFF,0X80,0X01,0XFF,0XFF,0XE0,0X7F,0XFC,0X00,0X00,0X3F,0XFF,0XF0,0X7F,0XF0,0X00,0X00,0X1F,0XFF,0XF0,0X7F,0XC0,0X00,0X00,0X0F,0XFF,0XF8,0X7F,0X00,0X00,0X00,0X07,0XFF,0XF8,0X7E,0X00,0X00,0X00,0X03,0XFF,0XF8,0X78,0X00,0X00,0X00,0X03,0XFF,0XF8,0X70,0X00,0X00,0X00,0X01,0XFF,0XF8,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XFC,0X00,0X00,0X00,0X00,0X01,0XFF,0XF8,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X03,0XFF,0XF8,0X00,0X00,0X00,0X00,0X07,0XFF,0XF8,0X00,0X00,0X00,0X00,0X07,0XFF,0XF0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XF0,0X00,0X00,0X00,0X00,0X3F,0XFF,0XE0,0X00,0X00,0X00,0X00,0X7F,0XFF,0XE0,0X00,0X00,0X00,0X03,0XFF,0XFF,0XC0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XC0,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0XF0};

void make_test(){
	for(int i=0;i<2560;i++)for(int j=716;j<724;j++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
	for(int i=280;i<2280;i++)
	{
		for(int j=680;j<690;j++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
		for(int j=750;j<760;j++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
	}
	for(int j=680;j<760;j++)
	{
		for(int i=270;i<280;i++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
		for(int i=2280;i<2290;i++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
	}
	for(int kd=0;kd<=40;kd++)
	{
		for(int i=50*kd-5+280;i<50*kd+5+280;i++)
		{
			for(int j=((kd%5)==0)?630:660;j<690;j++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
			for(int j=((kd%5)==0)?810:780;j>=750;j--)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
		}
	}
	for(int i=280;i<2280;i++)
	{
		for(int j=690;j<750;j++)
		{
			double xw;
			
			xw=i*stan-j+1440.0;
			while(xw>=jiange)xw-=jiange;
			if(((xw/jiange*2000.0+280)>=i-20)&&((xw/jiange*2000.0+280)<i+20))lsbmp.p[i*lsbmp.wid+j].G=255;
			else lsbmp.p[i*lsbmp.wid+j].G=0;
			///xw=i/stan-j+1.0/3+1440.0;
			xw=xw+1.0/3;
			while(xw>=jiange)xw-=jiange;
			if(((xw/jiange*2000.0+280)>=i-20)&&((xw/jiange*2000.0+280)<i+20))lsbmp.p[i*lsbmp.wid+j].R=255;
			else lsbmp.p[i*lsbmp.wid+j].R=0;
			
			//xw=i/stan-j-1.0/3+1440.0;
			xw=xw+jiange-2.0/3;
			while(xw>=jiange)xw-=jiange;
			if(((xw/jiange*2000.0+280)>=i-20)&&((xw/jiange*2000.0+280)<i+20))lsbmp.p[i*lsbmp.wid+j].B=255;
			else lsbmp.p[i*lsbmp.wid+j].B=0;
		}
	}
	for(int kd=0;kd<=40;kd+=5)
	{
		int sty=50*kd-5+280-36,stx0,stx1,n0=kd/10,n1=kd%10,sp=10;
		stx0=810+sp;
		stx1=620-56-56-sp;
		for(int i=0;i<497;i++)
		{
			int st=(i/7+sty)*lsbmp.wid+(i%7)*8+stx0;
			for(int j=0;j<8;j++)if(fonts[n0][i]&(0x80>>j))lsbmp.p[st+j]=RGBPixel(255,255,255);
			st=(i/7+sty)*lsbmp.wid+(i%7)*8+stx0+sp+56;
			for(int j=0;j<8;j++)if(fonts[n1][i]&(0x80>>j))lsbmp.p[st+j]=RGBPixel(255,255,255);
			st=(i/7+sty)*lsbmp.wid+(i%7)*8+stx1;
			for(int j=0;j<8;j++)if(fonts[n0][i]&(0x80>>j))lsbmp.p[st+j]=RGBPixel(255,255,255);
			st=(i/7+sty)*lsbmp.wid+(i%7)*8+stx1+sp+56;
			for(int j=0;j<8;j++)if(fonts[n1][i]&(0x80>>j))lsbmp.p[st+j]=RGBPixel(255,255,255);
		}
		for(int i=50*kd-5+280;i<50*kd+5+280;i++)
		{
			for(int j=((kd%5)==0)?630:660;j<690;j++)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
			for(int j=((kd%5)==0)?810:780;j>=750;j--)lsbmp.p[i*lsbmp.wid+j]=RGBPixel(255,255,255);
		}
	}
}
int pipes(){
    std::string pipeName = "\\\\.\\pipe\\OpenstageAI_server_pipe";
    std::string command = "getDeivice"; 
    NamedPipeClient client(pipeName);
    if (!client.connect()) {
        //std::cerr << "连接失败，请确保服务器端正在运行" << std::endl;
        return 1;
    }
    std::string response;
    if (client.sendCommand(command, response)) 
	{
        //std::cout << response << std::endl;
		int loc=-1;
		char params[1000];
		do
		{
			loc++;
			params[loc]=response[loc];
		}while(params[loc]!='}');
		//cout<<params;
		loc=charfind(params,"lineNumber");
		sscanf(&params[loc+12],"%lf",&jiange);
		jiange=jiange/3.0;
		loc=charfind(params,"obliquity");
		sscanf(&params[loc+11],"%lf",&stan);
    } else {
        //std::cerr << "命令执行失败" << std::endl;
        return 1;
    }
    return 0;
}
bool get_para(){
	FILE *fpt=fopen("data","r");
	if(fpt!=NULL)
	{
		fscanf(fpt,"%lf\n%lf\n%lf\n",&stan,&jiange,&pianyi);
		fclose(fpt);
		return true;
	}
	else
		return false;
}
void save_data(){
	FILE *fpt=fopen("data","w");
	fprintf(fpt,"%lf\n%lf\n%lf\n",stan,jiange,pianyi);
	fclose(fpt);
}
void make_mask(){
	b.wid=1440;
	b.hei=2560;
	//b.space_apply();
	for(int i=0;i<b.hei;i++)
	{
		for(int j=0;j<b.wid;j++)
		{
			double xw;
			xw=i*stan-j+1440.0+pianyi;
			xw=fmod(xw,jiange);
			b.p[i*b.wid+j].G=(int)(xw/jiange*40.0);
			xw=xw+1.0/3.0;
			xw=fmod(xw,jiange);
			b.p[i*b.wid+j].R=(int)(xw/jiange*40.0);
			xw=xw+jiange-2.0/3.0;
			xw=fmod(xw,jiange);
			b.p[i*b.wid+j].B=(int)(xw/jiange*40.0);
		}
	}
}
void make_dice(){
	//dice_ori.create_cube(2);
	//dice_ori.create_regular_polyhedron(20,2);
	dice_ori.number=20;
	dice_ori.space_apply();
	double le=1;
	double dd=(sqrt(5)+1)/2*le;
	vec3d points[12];
	points[0]=vec3d(0,dd,le);
	points[1]=vec3d(0,dd,-le);
	points[2]=vec3d(0,-dd,le);
	points[3]=vec3d(0,-dd,-le);
	points[4]=vec3d(dd,le,0);
	points[5]=vec3d(dd,-le,0);
	points[6]=vec3d(-dd,le,0);
	points[7]=vec3d(-dd,-le,0);
	points[8]=vec3d(le,0,dd);
	points[9]=vec3d(-le,0,dd);
	points[10]=vec3d(le,0,-dd);
	points[11]=vec3d(-le,0,-dd);
	for(int i=0;i<12;i++) points[i].guiyihua();
	int faces[20][3]={
		1,0,4,//1
		3,7,11,//2
		4,8,5,//3
		7,9,6,//4
		6,1,11,//5
		9,2,8,//6
		1,4,10,//7
		2,3,5,//8
		8,0,9,//9
		5,3,10,//10
		0,6,9,//11
		3,11,10,//12
		0,1,6,//13
		7,2,9,//14
		1,10,11,//15
		2,5,8,//16
		10,4,5,//17
		11,7,6,//18
		4,0,8,//19
		3,2,7//20
	};
	for(int i=0;i<dice_ori.number;i++)
	{
		dice_ori.f[i].vertex[0]=points[faces[i][0]];
		dice_ori.f[i].vertex[1]=points[faces[i][1]];
		dice_ori.f[i].vertex[2]=points[faces[i][2]];
		dice_ori.f[i].calc();
	}
	//dice_ori.face_to_ground(19);
	dice_ori.transform(dice_ori.f[19].vertex[1]-dice_ori.f[19].vertex[0],dice_ori.f[19].normal,vec3d(0,0,0),vec3d(1,0,0),vec3d(0,0,1),vec3d(0,0,0));
	for(int i=0;i<40;i++)
	{
		dice_pre[i].copyfrom(dice_ori);
		squeeze_x[i]=tan(20*Pi/180)*(19.5-i)/19.5;
		face_color[i]=hsv2rgb(HSVPixel(rand()%360,0.5,1));
	}
	bmp24 dice_surface;
	bool sfd=false;
	if(dice_surface.read("dice_surface\\dice.bmp"))
	{
		POINT fp[20][3]={
			10,2, 8,2, 9,1,
			3,1, 4,2, 2,2,//2
			9,1, 7,1, 8,0,
			4,2, 6,2, 5,3,//4
			1,3, 0,2, 2,2,
			6,2, 5,1, 7,1,//6
			10,2, 9,1, 11,1,
			5,1, 3,1, 4,0,//8
			7,1, 8,2, 6,2,
			2,0, 3,1, 1,1,//10
			8,2, 7,3, 6,2,
			3,1, 2,2, 1,1,//12
			8,2, 10,2, 9,3,
			4,2, 5,1, 6,2,//14
			0,2, 1,1, 2,2,
			5,1, 6,0, 7,1,//16
			11,1, 9,1, 10,0,
			2,2, 4,2, 3,3,//18
			9,1, 8,2, 7,1,
			3,1, 5,1, 4,2//20
		};
		for(int i=0;i<20;i++)
		{
			dice_faces[i].wid=256;
			dice_faces[i].hei=256;
			dice_faces[i].space_apply();
			for(int j=0;j<3;j++)
			{
				fp[i][j].x=dice_surface.wid*fp[i][j].x/11;
				fp[i][j].y=dice_surface.hei*fp[i][j].y/3;
			}
			POINT u,v;
			u.x=fp[i][1].x-fp[i][0].x;
			u.y=fp[i][1].y-fp[i][0].y;
			v.x=fp[i][2].x-fp[i][0].x;
			v.y=fp[i][2].y-fp[i][0].y;
			for(int y=0;y<256;y++)
			{
				for(int x=255-y;x>=0;x--)
				{
					double xx,yy;
					xx=x*u.x/255.0+y*v.x/255.0+fp[i][0].x;
					yy=x*u.y/255.0+y*v.y/255.0+fp[i][0].y;
					dice_faces[i].p[y*256+x]=dice_surface.getp(xx,yy);
				}
			}
		}
		sfd=true;
	}
	else
	{
		char face_name[100];
		for(int i=0;i<20;i++)
		{
			sprintf(face_name,"dice_surface\\%02d.bmp",i+1);
			if(!dice_faces[i].read(face_name))break;
			if(i==19)sfd=true;
		}
		if(sfd)
		{
			for(int i=0;i<20;i++)
			{
				dice_faces[i].resize(512,256);
				for(int j=1;j<256;j++)
				{
					for(int k=0;k<512-2*j;k++)
					{
						dice_faces[i].p[j*dice_faces[i].wid+k]=dice_faces[i].p[j*dice_faces[i].wid+k+j];
					}
				}
				dice_faces[i].resize(256,256);
			}
		}
		else
		{
			for(int i=0;i<20;i++)
			{
				dice_faces[i].wid=256;
				dice_faces[i].hei=256;
				dice_faces[i].space_apply();
				for(int j=dice_faces[i].wid*dice_faces[i].hei-1;j>=0;j--)
				{
					dice_faces[i].p[j]=face_color[i];
				}
			}
		}
	}
	dice_bak.copyfrom(dice_ori);
	dice_end.copyfrom(dice_ori);
}
void get_bg(){
	if(!bg.read("background.bmp"))
	{
		bg.wid=1440;
		bg.hei=2560;
		bg.space_apply();
	}
}
void reset_para(){
	delete[] dice_ori.f;
	dice_ori.copyfrom(dice_bak);
	dice_size=360;
	field_x=1439-dice_size*2;
	field_y=2559-dice_size*2;
	nordep=7;
	pingmian=0.7946544450;
	ox=field_x/2;
	oy=field_y/2;
	movex[0]=0;
	movey[0]=0;
	rotation_axis=vec3d(0,0,0);
	target_axis=vec3d(0,0,0);
	ticks=1;
	update = true;
}
void reset_bias(){
	lsbmp.clear();
	pipes();
	make_test();
	got_para=false;
	memcpy(old_show.p,lsbmp.p,lsbmp.wid*lsbmp.hei*3);
}
void init(){
	lsbmp.wid=1440;
	lsbmp.hei=2560;
	old_show.wid=1440;
	old_show.hei=2560;
	b.wid=1440;
	b.hei=2560;
	b.space_apply();
	lsbmp.space_apply();
	if(get_para())
	{
		make_mask();
		got_para=true;
		get_bg();
		old_show.clone(bg);
	}
	else
	{
		old_show.space_apply();
		reset_bias();
	}
	make_dice();
	movex = new int[1];
	movey = new int[1];
	movex[0]=0;
	movey[0]=0;
	dragging=false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    srand((unsigned int)time(NULL)); // 初始化随机数生成器
    
    update = false;
    init();
    
	g_bitmapWidth=lsbmp.wid;
	g_bitmapHeight=lsbmp.hei;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 可以不使用，因为会被位图覆盖
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // --- 枚举所有显示器并找到目标显示器 ---
    g_monitorCount = 0; // 重置计数器
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

    MonitorInfo* targetMonitor = NULL;
    
    for (int i = 0; i < g_monitorCount; ++i) {
        if ((g_monitors[i].rcMonitor.right-g_monitors[i].rcMonitor.left==1440)&&(g_monitors[i].rcMonitor.bottom-g_monitors[i].rcMonitor.top==2560)) {
            targetMonitor = &g_monitors[i];
            break;
        }
    }

    // 如果没有找到，则使用主显示器
    if (targetMonitor == NULL && g_monitorCount > 0) {
        targetMonitor = &g_monitors[0];
    } else if (targetMonitor == NULL && g_monitorCount == 0) {
        MessageBox(NULL, "No display monitors found!", "Error", MB_ICONERROR);
        return 1;
    }
    windowX = targetMonitor->rcWork.left;
    windowY = targetMonitor->rcWork.top;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW | WS_EX_TOPMOST,
        g_szClassName,
        "Dynamic Image Viewer",
        WS_POPUP, // 无边框
        windowX, windowY,
        g_bitmapWidth, g_bitmapHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if(hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 获取屏幕DC来创建DIB Section
    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen == NULL) {
        MessageBox(hwnd, "Failed to get screen DC!", "Error", MB_ICONERROR);
        DestroyWindow(hwnd);
        return 1;
    }
    
    //g_hDIBBitmap = CreateDIBSectionBitmap(hdcScreen, g_bitmapWidth, g_bitmapHeight, g_bitsPerPixel, &g_pDIBPixelData);
    //g_hDIBBitmap = CreateDIBSectionBitmap(hdcScreen, lsbmp.wid, lsbmp.hei, 24, &g_pDIBPixelData);
	g_hDIBBitmap = copybmp(hdcScreen, lsbmp);
	SetWindowPos(hwnd, NULL, windowX, windowY, g_bitmapWidth, g_bitmapHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
    ReleaseDC(NULL, hdcScreen); // 释放屏幕DC
    if (g_hDIBBitmap == NULL || g_pDIBPixelData == NULL) {
        MessageBox(hwnd, "Failed to create DIB Section!", "Error", MB_ICONERROR);
        DestroyWindow(hwnd);
        return 1;
    }
    // 初始更新一次像素数据
    UpdateBitmapPixels();
	SetWindowPos(hwnd, NULL, windowX, windowY, g_bitmapWidth, g_bitmapHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
    // 设置一个计时器，每30毫秒触发一次
    SetTimer(hwnd, TIMER_ID, 1 , NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
	
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    

    // 清理资源
    if (g_hDIBBitmap != NULL) {
        DeleteObject(g_hDIBBitmap);
    }

    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static POINT ptMouseOffset;
    static BOOL bDragging = FALSE;

    switch(msg) {
        case WM_TIMER: {
            if (wParam == TIMER_ID) if(update==true||got_para==false){
                // 修改位图数据
                UpdateBitmapPixels();
                // 触发窗口重绘，InvalidateRect会使整个客户区无效
                InvalidateRect(hwnd, NULL, FALSE); // FALSE表示背景不擦除，减少闪烁
                //update=false;
            }
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (g_hDIBBitmap != NULL) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                HGDIOBJ oldBitmap = SelectObject(hdcMem, g_hDIBBitmap);
                BitBlt(hdc, 0, 0, g_bitmapWidth, g_bitmapHeight, hdcMem, 0, 0, SRCCOPY);
                SelectObject(hdcMem, oldBitmap);
                DeleteDC(hdcMem);
            }
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_LBUTTONDOWN: {
        	if(got_para==false)
        	{
				pianyi=60-(tmpy-280)/50.0;
				pianyi=pianyi*(jiange/40.0);
				save_data();
				make_mask();
				get_bg();
				old_show.clone(bg);
				reset_para();
				got_para=true;
				/*
				UpdateBitmapPixels();
				UpdateWindow(hwnd);
				*/
			}
			else
        	if(ticks==0)
        	{
				bDragging = TRUE;
				dragging = true;
				update=true;
				SetCapture(hwnd);
				GetCursorPos(&ptMouseOffset);
				RECT rect;
				GetWindowRect(hwnd, &rect);
				dx=0;
				dy=0;
				mouse_start.x = ptMouseOffset.x - rect.left;
				mouse_start.y = 2559 - ptMouseOffset.y + rect.top;
        	}
            //ptMouseOffset.x = ptMouseOffset.x - rect.left;
            //ptMouseOffset.y = ptMouseOffset.y - rect.top;
            break;
        }

        case WM_MOUSEMOVE: {
            if (bDragging) {
                POINT ptCurrentMousePos;
                GetCursorPos(&ptCurrentMousePos);
                dx = ptCurrentMousePos.x - ptMouseOffset.x;
                dy = ptMouseOffset.y - ptCurrentMousePos.y;
                //SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
            break;
        }

	    case WM_LBUTTONUP: {
			if(bDragging)
			{
			    bDragging = FALSE;
			    dragging = false;
			    ReleaseCapture();
			    while(dx>=-3&&dy>=-3&&dx<=3&&dy<=3)
			    {
			    	dx=(rand()%720)-360;
			    	dy=(rand()%1840)-920;
			    }
			    if(dx<-30000)dx=-30000;
			    if(dx>30000)dx=30000;
			    if(dy<-30000)dy=-30000;
			    if(dy>30000)dy=30000;
			    ticks=sqrt(sqrt(20*sqrt(dx*dx+dy*dy)/3680))*100+1;//3678-50t  min 22
			    dx*=20;
			    dy*=20;
				delete[] movex;
				delete[] movey;
			    movex = new int[ticks+2];
			    movey = new int[ticks+2];
				movex[0] = dx;
				movey[0] = dy;
			    double t2=ticks*ticks*ticks*ticks;
			    int biasj=1;
			    for(int i=1;i<=ticks;i++)
				{
					double tmpbs=((double)i*i*i*i)/t2;
					movex[biasj] = dx * tmpbs;
					movey[biasj] = dy * tmpbs;
					if(movex[biasj]<3&&movey[biasj]<3&&movex[biasj]>-3&&movey[biasj]>-3)continue;
					movex[biasj] = dx - movex[biasj];
					movey[biasj] = dy - movey[biasj];
					biasj++;
				}
				biasj--;
				ticks=biasj;
				movex[ticks]=0;
				movey[ticks]=0;
			    if(dx<0)dx=(dx-field_x/2)/field_x*field_x-dx;
			    else dx=(dx+field_x/2)/field_x*field_x-dx;
			    if(dy<0)dy=(dy-field_y/2)/field_y*field_y-dy;
			    else dy=(dy+field_y/2)/field_y*field_y-dy;
				movex[0] += dx;
				movey[0] += dy;
				for(int i=biasj-1;i>0;i--)
				{
					t2=0.5+0.5*cos(i*Pi/biasj);
					movex[i] += t2*dx;
					movey[i] += t2*dy;
				}
				rotation_axis.x=(float)(movey[ticks]-movey[ticks-1])/dice_size;
				rotation_axis.y=(float)(movex[ticks-1]-movex[ticks])/dice_size;
				rotation_axis.z=0;
				target_axis=vec3d(0,0,0);
			}
            break;
        }

        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            else if (wParam == 'R') {
            	reset_para();
            }
            else if (wParam == 'S') {
            	if(ticks==0&&!bDragging){
            		nordep=max(-7,nordep-1);
            		pingmian=0.7946544450*nordep/7;
					ticks=1;
					update = true;
				}
            }
            else if (wParam == 'W') {
            	if(ticks==0&&!bDragging){
            		nordep=min(7,nordep+1);
            		pingmian=0.7946544450*nordep/7;
					ticks=1;
					update = true;
				}
			}
			else if(wParam == 'P'){
				reset_bias();
            }
			else if(wParam == 'P'){
				reset_bias();
            }
			else if(wParam == VK_SPACE){
				if(got_para==true&&ticks==0&&bDragging==false)
				{
			    dx=(rand()%720)-360;
			    dy=(rand()%1840)-920;
			    ticks=sqrt(sqrt(20*sqrt(dx*dx+dy*dy)/3680))*100+1;//3678-50t  min 22
			    dx*=20;
			    dy*=20;
				delete[] movex;
				delete[] movey;
			    movex = new int[ticks+2];
			    movey = new int[ticks+2];
				movex[0] = dx;
				movey[0] = dy;
			    double t2=ticks*ticks*ticks*ticks;
			    int biasj=1;
			    for(int i=1;i<=ticks;i++)
				{
					double tmpbs=((double)i*i*i*i)/t2;
					movex[biasj] = dx * tmpbs;
					movey[biasj] = dy * tmpbs;
					if(movex[biasj]<3&&movey[biasj]<3&&movex[biasj]>-3&&movey[biasj]>-3)continue;
					movex[biasj] = dx - movex[biasj];
					movey[biasj] = dy - movey[biasj];
					biasj++;
				}
				biasj--;
				ticks=biasj;
				movex[ticks]=0;
				movey[ticks]=0;
			    if(dx<0)dx=(dx-field_x/2)/field_x*field_x-dx;
			    else dx=(dx+field_x/2)/field_x*field_x-dx;
			    if(dy<0)dy=(dy-field_y/2)/field_y*field_y-dy;
			    else dy=(dy+field_y/2)/field_y*field_y-dy;
				movex[0] += dx;
				movey[0] += dy;
				for(int i=biasj-1;i>0;i--)
				{
					t2=0.5+0.5*cos(i*Pi/biasj);
					movex[i] += t2*dx;
					movey[i] += t2*dy;
				}
				rotation_axis.x=(float)(movey[ticks]-movey[ticks-1])/dice_size;
				rotation_axis.y=(float)(movex[ticks-1]-movex[ticks])/dice_size;
				rotation_axis.z=0;
				target_axis=vec3d(0,0,0);
				update=true;
				}
            }
            /*
            else if (wParam == VK_LEFT) {xe=max(0,xe-1);update=true;}
            else if (wParam == VK_RIGHT){xe=min(xe+1,lsbmp.wid-1);update=true;}
            else if (wParam == VK_UP)   {ye=min(ye+1,lsbmp.hei-1);update=true;}
            else if (wParam == VK_DOWN) {ye=max(0,ye-1);update=true;}
            */
            break;
        }
		
		case WM_MOUSEWHEEL: {
			if(ticks==0&&!bDragging)
			{
				if(((short)HIWORD(wParam))>0)
				{
					dice_size=min(dice_size+20,560);
				}
				else
				{
					dice_size=max(dice_size-20,200);
				}
				ticks=1;
				field_x=1439-dice_size*2;
				field_y=2559-dice_size*2;
				ox=field_x/2;
				oy=field_y/2;
				movex[0]=0;
				movey[0]=0;
				update = true;
			}
            break;
        }

        case WM_DESTROY: {
            KillTimer(hwnd, TIMER_ID); // 销毁计时器
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
