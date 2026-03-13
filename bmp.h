#include "Windows.h"  
#include "stdio.h"  
#include "string"  
#include "cstring"  
#include "stack"  
#include "malloc.h"  
#include "math.h"  
#include "cstdlib"  
#include "iostream"  
using namespace std;


struct RGBPixel{
	BYTE B;BYTE G;BYTE R;
	RGBPixel(BYTE R=0,BYTE G=0,BYTE B=0):R(R),G(G),B(B){}
	friend bool operator==(const RGBPixel& q,const RGBPixel& h)
	{
		if(q.R!=h.R)return false;
		if(q.G!=h.G)return false;
		if(q.B!=h.B)return false;
		return true;
	}
	friend bool operator!=(const RGBPixel& q,const RGBPixel& h)
	{
		return !(q==h);
	}
}; 
struct HSVPixel{//H 0-360.0   S 0-1.0   V 0-1.0 
	double H;double S;double V;
	HSVPixel(double H=0,double S=1,double V=1):H(H),S(S),V(V){}
}; 
struct RGBAPixel{
	BYTE B;BYTE G;BYTE R;BYTE A;
	RGBAPixel(BYTE R=0,BYTE G=0,BYTE B=0,BYTE A=0):R(R),G(G),B(B),A(A){}
	friend bool operator==(const RGBAPixel& q,const RGBAPixel& h)
	{
		if(q.R!=h.R)return false;
		if(q.G!=h.G)return false;
		if(q.B!=h.B)return false;
		if(q.A!=h.A)return false;
		return true;
	}
	friend bool operator!=(const RGBAPixel& q,const RGBAPixel& h)
	{
		return !(q==h);
	}
};

unsigned char liangdu(RGBPixel a){return (a.R+a.B+a.G+1)/3;}
RGBPixel rgb2grey(RGBPixel a);
double bhd(RGBPixel a);
HSVPixel rgb2hsv(RGBPixel a);
RGBPixel hsv2rgb(HSVPixel a);
double powd(double a,int n);//double pow
int dist(RGBPixel a,RGBPixel b);//distance
bool gre(RGBPixel a);//green

struct bmp24{
	int wid,hei;
	RGBPixel *p;
	bool read(char bmpname[]){
		//delete[] p;
		FILE *fp=fopen(bmpname,"rb");  
		if(fp==0) return false;
		fseek(fp, sizeof(BITMAPFILEHEADER),0);  //FILE *fp=fopen(bmpname,"rb");BITMAPFILEHEADER fileheader;fread(&fileheader, sizeof(BITMAPFILEHEADER), 1,fp);     
		BITMAPINFOHEADER head;
		fread(&head, sizeof(BITMAPINFOHEADER), 1,fp);     
		int bmpWidth = head.biWidth;  
		int bmpHeight = head.biHeight;  
		int biBitCount = head.biBitCount;  
		wid = head.biWidth;  
		hei = head.biHeight;  
		int lineByte=(bmpWidth * biBitCount/8+3)/4*4;  
		int buwei=lineByte-bmpWidth * biBitCount/8;
		if(biBitCount==1)
		{
			//lineByte=((bmpWidth+7)/8+3)/4*4;  
			p=new RGBPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[(bmpWidth * bmpHeight+7)/8];  
			unsigned char loc,w;
			fread(pBmpBuf,1,(bmpWidth * bmpHeight+7)/8,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					w=(i*bmpWidth+j)%8;
					loc=(i*bmpWidth+j)/8;
					if(pBmpBuf[loc]&(0x80>>w))
					{
						p[i*bmpWidth+j].B=255;
						p[i*bmpWidth+j].G=255;
						p[i*bmpWidth+j].R=255;
					}
					else
					{
						p[i*bmpWidth+j].B=0;
						p[i*bmpWidth+j].G=0;
						p[i*bmpWidth+j].R=0;
					}
				}
			}
			delete[] pBmpBuf;
		}  
		if(biBitCount==8)
		{
			RGBQUAD *pColorTable;
			pColorTable=new RGBQUAD[256];  
			fread(pColorTable,sizeof(RGBQUAD),256,fp);  
			p=new RGBPixel[bmpHeight*bmpWidth];
			unsigned char mapp;
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					mapp=pBmpBuf[i*lineByte+j];
					p[i*bmpWidth+j].B=pColorTable[mapp].rgbBlue;
					p[i*bmpWidth+j].G=pColorTable[mapp].rgbGreen;
					p[i*bmpWidth+j].R=pColorTable[mapp].rgbRed;
				}
			}
			delete[] pBmpBuf;
		}  
		if(biBitCount==24)
		{
			p=new RGBPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					p[i*bmpWidth+j].B=pBmpBuf[i*lineByte+j*3];
					p[i*bmpWidth+j].G=pBmpBuf[i*lineByte+j*3+1];
					p[i*bmpWidth+j].R=pBmpBuf[i*lineByte+j*3+2];
				}
			}
			delete[] pBmpBuf;
		}  
		if(biBitCount==32)
		{
			p=new RGBPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					if(pBmpBuf[i*lineByte+j*4+3]!=0)
					{ 
						p[i*bmpWidth+j].B=pBmpBuf[i*lineByte+j*4];
						p[i*bmpWidth+j].G=pBmpBuf[i*lineByte+j*4+1];
						p[i*bmpWidth+j].R=pBmpBuf[i*lineByte+j*4+2];
					}
					else
					{ 
						p[i*bmpWidth+j].B=0;
						p[i*bmpWidth+j].G=0;
						p[i*bmpWidth+j].R=0;
					}
				}
			}
			delete[] pBmpBuf;
		}  
		fclose(fp);  
		return true;
	}
	bool old_read(char bmpname[]){
		//delete[] p;
		FILE *fp=fopen(bmpname,"rb");  
		if(fp==0) return false;
		fseek(fp, sizeof(BITMAPFILEHEADER),0);  //FILE *fp=fopen(bmpname,"rb");BITMAPFILEHEADER fileheader;fread(&fileheader, sizeof(BITMAPFILEHEADER), 1,fp);     
		BITMAPINFOHEADER head;     
		fread(&head, sizeof(BITMAPINFOHEADER), 1,fp);     
		int bmpWidth = head.biWidth;  
		int bmpHeight = head.biHeight;  
		int biBitCount = head.biBitCount;  
		//wid = head.biWidth;  
		//hei = head.biHeight;  
		int lineByte=(bmpWidth * biBitCount/8+3)/4*4;  
		int buwei=lineByte-bmpWidth * biBitCount/8;
		if(biBitCount==24)
		{
			//p=new RGBPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					p[i*bmpWidth+j].B=pBmpBuf[i*lineByte+j*3];
					p[i*bmpWidth+j].G=pBmpBuf[i*lineByte+j*3+1];
					p[i*bmpWidth+j].R=pBmpBuf[i*lineByte+j*3+2];
				}
			}
			delete[] pBmpBuf;
		}
		fclose(fp);  
		return true;
	}
	void save(char bmpName[]){
		int colorTablesize=0;  
		int lineByte=(wid * 3+3)/4*4; 
		int buwei=lineByte-wid*3;
		unsigned char bw=0;
		FILE *fp=fopen(bmpName,"wb");  
		if(fp==0) return;  
		BITMAPFILEHEADER fileHead;  
		fileHead.bfType = 0x4D42;
		fileHead.bfSize= sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+lineByte*hei;  
		fileHead.bfReserved1 = 0;  
		fileHead.bfReserved2 = 0;  
		fileHead.bfOffBits=54;  
		fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);  
		BITMAPINFOHEADER head;    
		head.biBitCount=24;  
		head.biClrImportant=0;  
		head.biClrUsed=0;  
		head.biCompression=0;  
		head.biHeight=hei;  
		head.biPlanes=1;  
		head.biSize=40;  
		head.biSizeImage=lineByte*hei;  
		head.biWidth=wid;  
		head.biXPelsPerMeter=3780;  
		head.biYPelsPerMeter=3780;  
		fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);  
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{
				fwrite(p+i*wid+j,3,1,fp);  
			}
			fwrite(&bw,1,buwei,fp);  
		}
		fclose(fp);  
	}
	void clear(){memset(p, 0, wid*hei * sizeof(RGBPixel));}
	void space_apply(){p=new RGBPixel[wid*hei];clear();}
	void getcolor(double x,double y,double *rr,double *gg,double *bb){
		*rr=0;
		*gg=0;
		*bb=0;
		if(x<=-1||x>=wid||y<=-1||y>=hei)return;
		int xo,yo;
		double xp,yp;
		xo=(int)x;
		yo=(int)y;
		xp=x-xo;
		yp=y-yo;
		if(xo>=0&&yo>=0)
		{
			*rr=*rr+p[yo*wid+xo].R*(1.0-xp)*(1.0-yp);
			*gg=*gg+p[yo*wid+xo].G*(1.0-xp)*(1.0-yp);
			*bb=*bb+p[yo*wid+xo].B*(1.0-xp)*(1.0-yp);
		}
		if(xo>=0&&yo<hei-1)
		{
			*rr=*rr+p[(yo+1)*wid+xo].R*(1-xp)*yp;
			*gg=*gg+p[(yo+1)*wid+xo].G*(1-xp)*yp;
			*bb=*bb+p[(yo+1)*wid+xo].B*(1-xp)*yp;
		}
		if(xo<wid-1&&yo>=0)
		{
			*rr=*rr+p[yo*wid+xo+1].R*xp*(1-yp);
			*gg=*gg+p[yo*wid+xo+1].G*xp*(1-yp);
			*bb=*bb+p[yo*wid+xo+1].B*xp*(1-yp);
		}
		if(xo<wid-1&&yo<hei-1)
		{
			*rr=*rr+p[(yo+1)*wid+xo+1].R*xp*yp;
			*gg=*gg+p[(yo+1)*wid+xo+1].G*xp*yp;
			*bb=*bb+p[(yo+1)*wid+xo+1].B*xp*yp;
		}
		if(*rr<0)*rr=0;
		if(*gg<0)*gg=0;
		if(*bb<0)*bb=0;
	}
	RGBPixel getp(double x,double y){
		RGBPixel ans;
		double rr,gg,bb;
		ans.R=0;
		ans.G=0;
		ans.B=0;
		rr=0;
		gg=0;
		bb=0;
		//if(x<=-1||x>=wid||y<=-1||y>=hei)return ans;
		if(x<0)return getp(0,y);
		if(y<0)return getp(x,0);
		if(x>=wid)return getp(wid-1,y);
		if(y>=hei)return getp(x,hei-1);
		int xo,yo;
		double xp,yp,ass;
		xo=(int)x;
		yo=(int)y;
		xp=x-xo;
		yp=y-yo;
		ass=0;
		if(xo>=0&&yo>=0)
		{
			rr=rr+p[yo*wid+xo].R*(1.0-xp)*(1.0-yp);
			gg=gg+p[yo*wid+xo].G*(1.0-xp)*(1.0-yp);
			bb=bb+p[yo*wid+xo].B*(1.0-xp)*(1.0-yp);
			ass=ass+(1.0-xp)*(1.0-yp);
		}
		if(xo>=0&&yo<hei-1)
		{
			rr=rr+p[(yo+1)*wid+xo].R*(1-xp)*yp;
			gg=gg+p[(yo+1)*wid+xo].G*(1-xp)*yp;
			bb=bb+p[(yo+1)*wid+xo].B*(1-xp)*yp;
			ass=ass+(1-xp)*yp;
		}
		if(xo<wid-1&&yo>=0)
		{
			rr=rr+p[yo*wid+xo+1].R*xp*(1-yp);
			gg=gg+p[yo*wid+xo+1].G*xp*(1-yp);
			bb=bb+p[yo*wid+xo+1].B*xp*(1-yp);
			ass=ass+xp*(1-yp);
		}
		if(xo<wid-1&&yo<hei-1)
		{
			rr=rr+p[(yo+1)*wid+xo+1].R*xp*yp;
			gg=gg+p[(yo+1)*wid+xo+1].G*xp*yp;
			bb=bb+p[(yo+1)*wid+xo+1].B*xp*yp;
			ass=ass+xp*yp;
		}
		rr/=ass;
		gg/=ass;
		bb/=ass;
		if(rr<0)rr=0;
		if(rr>255)rr=255;
		if(gg<0)gg=0;
		if(gg>255)gg=255;
		if(bb<0)bb=0;
		if(bb>255)bb=255;
		ans.R=rr+0.5;
		ans.G=gg+0.5;
		ans.B=bb+0.5;
		return ans;
	}
	BYTE getp_r(double x,double y){
		double rr=0;
		if(x<0)return getp_r(0,y);
		if(y<0)return getp_r(x,0);
		if(x>=wid)return getp_r(wid-1,y);
		if(y>=hei)return getp_r(x,hei-1);
		int xo,yo;
		double xp,yp,ass;
		xo=(int)x;
		yo=(int)y;
		xp=x-xo;
		yp=y-yo;
		ass=0;
		if(xo>=0&&yo>=0)
		{
			rr=rr+p[yo*wid+xo].R*(1.0-xp)*(1.0-yp);
			ass=ass+(1.0-xp)*(1.0-yp);
		}
		if(xo>=0&&yo<hei-1)
		{
			rr=rr+p[(yo+1)*wid+xo].R*(1-xp)*yp;
			ass=ass+(1-xp)*yp;
		}
		if(xo<wid-1&&yo>=0)
		{
			rr=rr+p[yo*wid+xo+1].R*xp*(1-yp);
			ass=ass+xp*(1-yp);
		}
		if(xo<wid-1&&yo<hei-1)
		{
			rr=rr+p[(yo+1)*wid+xo+1].R*xp*yp;
			ass=ass+xp*yp;
		}
		rr=rr/ass;
		if(rr<0)rr=0;
		if(rr>255)rr=255;
		rr+=0.5;
		return (BYTE)rr;
	}
	BYTE getp_g(double x,double y){
		double gg=0;
		if(x<0)return getp_g(0,y);
		if(y<0)return getp_g(x,0);
		if(x>=wid)return getp_g(wid-1,y);
		if(y>=hei)return getp_g(x,hei-1);
		int xo,yo;
		double xp,yp,ass;
		xo=(int)x;
		yo=(int)y;
		xp=x-xo;
		yp=y-yo;
		ass=0;
		if(xo>=0&&yo>=0)
		{
			gg=gg+p[yo*wid+xo].G*(1.0-xp)*(1.0-yp);
			ass=ass+(1.0-xp)*(1.0-yp);
		}
		if(xo>=0&&yo<hei-1)
		{
			gg=gg+p[(yo+1)*wid+xo].G*(1-xp)*yp;
			ass=ass+(1-xp)*yp;
		}
		if(xo<wid-1&&yo>=0)
		{
			gg=gg+p[yo*wid+xo+1].G*xp*(1-yp);
			ass=ass+xp*(1-yp);
		}
		if(xo<wid-1&&yo<hei-1)
		{
			gg=gg+p[(yo+1)*wid+xo+1].G*xp*yp;
			ass=ass+xp*yp;
		}
		gg=gg/ass+0.5;
		if(gg<0)gg=0;
		if(gg>255)gg=255;
		return (unsigned char)gg;
	}
	BYTE getp_b(double x,double y){
		double bb=0;
		if(x<0)return getp_b(0,y);
		if(y<0)return getp_b(x,0);
		if(x>=wid)return getp_b(wid-1,y);
		if(y>=hei)return getp_b(x,hei-1);
		int xo,yo;
		double xp,yp,ass;
		xo=(int)x;
		yo=(int)y;
		xp=x-xo;
		yp=y-yo;
		ass=0;
		if(xo>=0&&yo>=0)
		{
			bb=bb+p[yo*wid+xo].B*(1.0-xp)*(1.0-yp);
			ass=ass+(1.0-xp)*(1.0-yp);
		}
		if(xo>=0&&yo<hei-1)
		{
			bb=bb+p[(yo+1)*wid+xo].B*(1-xp)*yp;
			ass=ass+(1-xp)*yp;
		}
		if(xo<wid-1&&yo>=0)
		{
			bb=bb+p[yo*wid+xo+1].B*xp*(1-yp);
			ass=ass+xp*(1-yp);
		}
		if(xo<wid-1&&yo<hei-1)
		{
			bb=bb+p[(yo+1)*wid+xo+1].B*xp*yp;
			ass=ass+xp*yp;
		}
		bb=bb/ass+0.5;
		if(bb<0)bb=0;
		if(bb>255)bb=255;
		return (unsigned char)bb;
	}
	RGBPixel bigetp(double x,double y){
		RGBPixel ans;
		double rr,gg,bb;
		ans.R=0;
		ans.G=0;
		ans.B=0;
		rr=0;
		gg=0;
		bb=0;
		if(x<=1||x>=wid-2||y<=1||y>=hei-2)return getp(x,y);
		int xo,yo;
		double xp[4],yp[4],s;
		xo=(int)x;
		yo=(int)y;
		s=x+1-xo;
		xp[0]=-0.5*s*s*s+2.5*s*s-4*s+2;
		s=x-xo;
		xp[1]=1.5*s*s*s-2.5*s*s+1;
		s=xo+1.0-x;
		xp[2]=1.5*s*s*s-2.5*s*s+1;
		s=xo+2-x;
		xp[3]=-0.5*s*s*s+2.5*s*s-4*s+2;
		
		s=y+1-yo;
		yp[0]=-0.5*s*s*s+2.5*s*s-4*s+2;
		s=y-yo;
		yp[1]=1.5*s*s*s-2.5*s*s+1;
		s=yo+1.0-y;
		yp[2]=1.5*s*s*s-2.5*s*s+1;
		s=yo+2-y;
		yp[3]=-0.5*s*s*s+2.5*s*s-4*s+2;
		
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<4;j++)
			{
				rr=rr+p[(yo+i-1)*wid+(xo+j-1)].R*xp[j]*yp[i];
				gg=gg+p[(yo+i-1)*wid+(xo+j-1)].G*xp[j]*yp[i];
				bb=bb+p[(yo+i-1)*wid+(xo+j-1)].B*xp[j]*yp[i];
			}
		}
		
		if(rr<0)rr=0;
		if(rr>255)rr=255;
		if(gg<0)gg=0;
		if(gg>255)gg=255;
		if(bb<0)bb=0;
		if(bb>255)bb=255;
		ans.R=rr+0.5;
		ans.G=gg+0.5;
		ans.B=bb+0.5;
		return ans;
	}
	void resize(int w,int h){
		if(wid==w&&hei==h)return;
		bmp24 ls;
		/*
		while(wid*2<w)resize(wid*2,h);
		while(hei*2<h)resize(w,hei*2);
		while(h*2<hei)resize(w,(hei+1)/2);
		while(w*2<wid)resize((wid+1)/2,h);
		*/
		ls.wid=w;
		ls.hei=h;
		ls.space_apply();
		long long rr,gg,bb;
		long long *x;
		long long *y;
		long long *dx;
		long long *dy;
		x=new long long[w];
		y=new long long[h];
		dx=new long long[w];
		dy=new long long[h];
		for(int i=0;i<h;i++)
		{
			y[i]=(i*2*hei+hei)-h;
			if(y[i]<0)y[i]=0;
			if(y[i]>(hei-1)*h*2)y[i]=(hei-1)*h*2;
			dy[i]=y[i]%(2*h);
			y[i]=y[i]/(2*h);
		}
		for(int i=0;i<w;i++)
		{
			x[i]=(i*2*wid+wid)-w;
			if(x[i]<0)x[i]=0;
			if(x[i]>(wid-1)*w*2)x[i]=(wid-1)*w*2;
			dx[i]=x[i]%(2*w);
			x[i]=x[i]/(2*w);
		}
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				rr=p[y[i]*wid+x[j]].R*(2*w-dx[j])*(2*h-dy[i]);
				gg=p[y[i]*wid+x[j]].G*(2*w-dx[j])*(2*h-dy[i]);
				bb=p[y[i]*wid+x[j]].B*(2*w-dx[j])*(2*h-dy[i]);
				if(dx[j]!=0)
				{
					rr=rr+p[y[i]*wid+x[j]+1].R*dx[j]*(2*h-dy[i]);
					gg=gg+p[y[i]*wid+x[j]+1].G*dx[j]*(2*h-dy[i]);
					bb=bb+p[y[i]*wid+x[j]+1].B*dx[j]*(2*h-dy[i]);
				} 
				if(dy[i]!=0)
				{
					rr=rr+p[y[i]*wid+wid+x[j]].R*(2*w-dx[j])*dy[i];
					gg=gg+p[y[i]*wid+wid+x[j]].G*(2*w-dx[j])*dy[i];
					bb=bb+p[y[i]*wid+wid+x[j]].B*(2*w-dx[j])*dy[i];
				}
				if(dx[j]!=0&&dy[i]!=0)
				{
					rr=rr+p[y[i]*wid+wid+x[j]+1].R*dx[j]*dy[i];
					gg=gg+p[y[i]*wid+wid+x[j]+1].G*dx[j]*dy[i];
					bb=bb+p[y[i]*wid+wid+x[j]+1].B*dx[j]*dy[i];
				}
				rr=(rr+2*w*h)/(4*w*h);
				gg=(gg+2*w*h)/(4*w*h);
				bb=(bb+2*w*h)/(4*w*h);
				ls.p[i*w+j].R=rr;
				ls.p[i*w+j].G=gg;
				ls.p[i*w+j].B=bb;
			}
		}
		wid=w;
		hei=h;
		delete[] p;
		space_apply();
		for(int i=w*h-1;i>=0;i--)
		{
			p[i].R=ls.p[i].R;
			p[i].G=ls.p[i].G;
			p[i].B=ls.p[i].B;
		}
		delete[] ls.p;
		delete[] x;
		delete[] y;
		delete[] dx;
		delete[] dy;
		//ls.save("part.bmp");
	}
	void heibaihua(){//黑白化 
		for(long long i=wid*hei-1;i>=0;i--)
		{
			//p[i].R=liangdu(p[i]);
			p[i].R=0.3*p[i].R + 0.59 * p[i].G + 0.11 * p[i].B+0.5;
			p[i].G=p[i].R;
			p[i].B=p[i].R;
		}
	}
	void zftjhh(){//直方图均衡化 
		int n[256],fix[256];
		unsigned char l;
		long long psum=0,nsum=wid*hei;
		memset(n,0,sizeof(n));
		heibaihua();
		for(long long i=nsum-1;i>=0;i--)n[p[i].G]++;
		double pp[255];
		for(int i=0;i<256;i++)
		{
			psum+=n[i];
			pp[i]=(double)psum/nsum;
			fix[i]=(int)(255.0*pp[i]+0.5);
		}
		for(long long i=nsum-1;i>=0;i--)
		{
			l=fix[p[i].R];
			p[i].R=l;
			p[i].G=l;
			p[i].B=l;
		}
	}
	void ezh(){//二值化 
		heibaihua();
		int t=127,oldt=0;
		long long m[2],g[2];
		while(abs(t-oldt)>0)
		{
			g[0]=0;
			g[1]=0;
			m[0]=0;
			m[1]=0;
			oldt=t;
			for(int i=hei*wid-1;i>=0;i--)
			{
				if(p[i].R>t)
				{
					m[0]+=p[i].R;
					g[0]++;
				}
				else
				{
					m[1]+=p[i].R;
					g[1]++;
				}
			}
			t=((double)m[0]/g[0]+(double)m[1]/g[1])/2;
		}
		for(long long i=hei*wid-1;i>=0;i--)
		{
			if(liangdu(p[i])>t)
			{
				p[i].R=255;
				p[i].G=255;
				p[i].B=255;
			}
			else
			{
				p[i].R=0;
				p[i].G=0;
				p[i].B=0;
			}
		}
	}
	void clone(bmp24 o){//克隆 
		hei=o.hei;
		wid=o.wid;
		space_apply();
		memcpy(p,o.p,wid*hei*3);
		/*
		for(long long i=wid*hei-1;i>=0;i--)
		{
			p[i].R=o.p[i].R;
			p[i].G=o.p[i].G;
			p[i].B=o.p[i].B;
		}
		*/
	}
	void cutfrom(bmp24 o,int xstart,int ystart,int xend,int yend){//剪切，xstart <= x < wend 
		if(xstart<0||xend>o.wid||ystart<0||yend>o.hei||xstart>xend||ystart>yend)return;
		hei=yend-ystart;
		wid=xend-xstart;
		space_apply();
		for(int i=ystart;i<yend;i++)
		{
			memcpy(&p[(i-ystart)*wid],&o.p[i*o.wid+xstart],wid*3);
			/*
			for(int j=xstart;j<xend;j++)
			{
				p[(i-ystart)*wid+j-xstart].R=o.p[i*o.wid+j].R;
				p[(i-ystart)*wid+j-xstart].G=o.p[i*o.wid+j].G;
				p[(i-ystart)*wid+j-xstart].B=o.p[i*o.wid+j].B;
			}
			*/
		}
	}
	void otsu(){//otsu二值化 
		heibaihua();
		double the=0,p1=0,maxn=0,mg,pp[256],m=0;
		int maxl;
		long long l=wid*hei,al=0;
		for(int i=0;i<256;i++)pp[i]=0;
		for(long long i=l-1;i>=0;i--)
		{
			pp[p[i].R]+=1;
			al+=p[i].R;
		}
		for(int i=0;i<256;i++)pp[i]=pp[i]/l;
		mg=(double)al/l;
		for(int k=0;k<256;k++)
		{
			p1=p1+pp[k];
			m=m+k*pp[k];
			if(p1<0.000001)continue;
			if(p1>0.999999)break;
			the=(mg*p1-m)*(mg*p1-m)/(p1*(1-p1));
			if(the>maxn)
			{
				maxn=the;
				maxl=k;
			}
		}
		for(long long i=wid*hei-1;i>=0;i--)
		{
			if(p[i].R>maxl)
			{
				p[i].R=255;
				p[i].G=255;
				p[i].B=255;
			}
			else
			{
				p[i].R=0;
				p[i].G=0;
				p[i].B=0;
			}
		}
	}
	void otsu_colorful(){
		double the=0,p1=0,maxn=0,mg,pp[256],m=0;
		int maxl;
		long long l=wid*hei,al=0;
		
		for(int i=0;i<256;i++)pp[i]=0;//R
		for(long long i=l-1;i>=0;i--)
		{
			pp[p[i].R]+=1;
			al+=p[i].R;
		}
		for(int i=0;i<256;i++)pp[i]=pp[i]/l;
		mg=(double)al/l;
		for(int k=0;k<256;k++)
		{
			p1=p1+pp[k];
			m=m+k*pp[k];
			if(p1<0.000001)continue;
			if(p1>0.999999)break;
			the=(mg*p1-m)*(mg*p1-m)/(p1*(1-p1));
			if(the>maxn)
			{
				maxn=the;
				maxl=k;
			}
		}
		for(long long i=wid*hei-1;i>=0;i--)
		{
			if(p[i].R>maxl)
			{
				p[i].R=255;
			}
			else
			{
				p[i].R=0;
			}
		}
		
		for(int i=0;i<256;i++)pp[i]=0;//G
		for(long long i=l-1;i>=0;i--)
		{
			pp[p[i].G]+=1;
			al+=p[i].G;
		}
		for(int i=0;i<256;i++)pp[i]=pp[i]/l;
		mg=(double)al/l;
		for(int k=0;k<256;k++)
		{
			p1=p1+pp[k];
			m=m+k*pp[k];
			if(p1<0.000001)continue;
			if(p1>0.999999)break;
			the=(mg*p1-m)*(mg*p1-m)/(p1*(1-p1));
			if(the>maxn)
			{
				maxn=the;
				maxl=k;
			}
		}
		for(long long i=wid*hei-1;i>=0;i--)
		{
			if(p[i].G>maxl)
			{
				p[i].G=255;
			}
			else
			{
				p[i].G=0;
			}
		}
		
		for(int i=0;i<256;i++)pp[i]=0;//B
		for(long long i=l-1;i>=0;i--)
		{
			pp[p[i].B]+=1;
			al+=p[i].B;
		}
		for(int i=0;i<256;i++)pp[i]=pp[i]/l;
		mg=(double)al/l;
		for(int k=0;k<256;k++)
		{
			p1=p1+pp[k];
			m=m+k*pp[k];
			if(p1<0.000001)continue;
			if(p1>0.999999)break;
			the=(mg*p1-m)*(mg*p1-m)/(p1*(1-p1));
			if(the>maxn)
			{
				maxn=the;
				maxl=k;
			}
		}
		for(long long i=wid*hei-1;i>=0;i--)
		{
			if(p[i].B>maxl)
			{
				p[i].B=255;
			}
			else
			{
				p[i].B=0;
			}
		}
	}
	void limitsize(int w,int h){
		if(wid<=w&&hei<=h)return;
		if(h*wid>w*hei)resize(w,hei*w/wid+0.5);
		else resize(wid*h/hei+0.5,h);
	}
	void hor_flip(){
		int z=0,y=wid-1,locz,locy;
		unsigned char ls;
		while(z<y)
		{
			for(int i=0;i<hei;i++)
			{
				locz=i*wid+z;
				locy=i*wid+y;
				ls=p[locz].R;
				p[locz].R=p[locy].R;
				p[locy].R=ls;
				ls=p[locz].G;
				p[locz].G=p[locy].G;
				p[locy].G=ls;
				ls=p[locz].B;
				p[locz].B=p[locy].B;
				p[locy].B=ls;
			}
			z++;
			y--;
		}
	}
	void ver_flip(){
		int s=hei-1,x=0,locs,locx;
		unsigned char ls;
		while(s>x)
		{
			for(int i=0;i<wid;i++)
			{
				locs=s*wid+i;
				locx=x*wid+i;
				ls=p[locs].R;
				p[locs].R=p[locx].R;
				p[locx].R=ls;
				ls=p[locs].G;
				p[locs].G=p[locx].G;
				p[locx].G=ls;
				ls=p[locs].B;
				p[locs].B=p[locx].B;
				p[locx].B=ls;
			}
			s--;
			x++;
		}
	}
	void clockwise_rotation(){//顺时针90度
		bmp24 ls;
		int loco,locp;
		ls.clone(*this);
		wid=ls.hei;
		hei=ls.wid;
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{
				loco=i*wid+j;
				locp=j*hei+hei-1-i;
				p[loco]=ls.p[locp];
			}
		}
	}
	void counterclockwise_rotation(){
		bmp24 ls;
		int loco,locp;
		ls.clone(*this);
		wid=ls.hei;
		hei=ls.wid;
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{
				loco=i*wid+j;
				locp=(wid-1-j)*hei+i;
				p[loco]=ls.p[locp];
			}
		}
	}
	void doubleresize(int w,int h){
		bmp24 ls;
		ls.wid=w;
		ls.hei=h;
		ls.space_apply();
		double rr,gg,bb;
		
		double *x;
		double *y;
		double *dx;
		double *dy;
		x=new double[w];
		y=new double[h];
		dx=new double[w];
		dy=new double[h];
		for(int i=0;i<h;i++)
		{
			y[i]=(i+0.5)*hei/h-0.5;
			if(y[i]<0)y[i]=0;
			if(y[i]>=hei-1)y[i]=hei-1.00001;
			dy[i]=y[i]-(int)y[i];
			//y[i]=y[i]/(2*h);
		}
		for(int i=0;i<w;i++)
		{
			x[i]=(i+0.5)*wid/w-0.5;
			if(x[i]<0)x[i]=0;
			if(x[i]>=wid-1)x[i]=wid-1.00001;
			dx[i]=x[i]-(int)x[i];
			//x[i]=x[i]/(2*w);
		}
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				/*
				rr=p[((int)y[i])*wid+(int)x[j]].R*(1.0-dx[j])*(1.0-dy[i])
					+p[(1+(int)y[i])*wid+(int)x[j]].R*dx[j]*(1.0-dy[i])
					+p[((int)y[i])*wid+(int)x[j]+1].R*(1.0-dx[j])*dy[i]
					+p[(1+(int)y[i])*wid+(int)x[j]+1].R*dx[j]*dy[i];
				gg=p[((int)y[i])*wid+(int)x[j]].G*(1.0-dx[j])*(1.0-dy[i])
					+p[(1+(int)y[i])*wid+(int)x[j]].G*dx[j]*(1.0-dy[i])
					+p[((int)y[i])*wid+(int)x[j]+1].G*(1.0-dx[j])*dy[i]
					+p[(1+(int)y[i])*wid+(int)x[j]+1].G*dx[j]*dy[i];
				bb=p[((int)y[i])*wid+(int)x[j]].B*(1.0-dx[j])*(1.0-dy[i])
					+p[(1+(int)y[i])*wid+(int)x[j]].B*dx[j]*(1.0-dy[i])
					+p[((int)y[i])*wid+(int)x[j]+1].B*(1.0-dx[j])*dy[i]
					+p[(1+(int)y[i])*wid+(int)x[j]+1].B*dx[j]*dy[i];
				ls.p[i*w+j].R=rr+0.5;
				ls.p[i*w+j].G=gg+0.5;
				ls.p[i*w+j].B=bb+0.5;
				*/
				ls.p[i*w+j]=bigetp(x[j],y[i]);
			}
		}
		wid=w;
		hei=h;
		delete[] p;
		space_apply();
		for(int i=w*h-1;i>=0;i--)
		{
			p[i].R=ls.p[i].R;
			p[i].G=ls.p[i].G;
			p[i].B=ls.p[i].B;
		}
		delete[] ls.p;
		delete[] x;
		delete[] y;
		delete[] dx;
		delete[] dy;
		//ls.save("part.bmp");
	}
	void draw_line(int x1,int y1,int x2,int y2,RGBPixel color){
		int t; 
		int xerr=0,yerr=0,delta_x,delta_y,distance; 
		int incx,incy,uRow,uCol; 
	
		delta_x=x2-x1;
		delta_y=y2-y1; 
		uRow=x1; 
		uCol=y1; 
		if(delta_x>0)incx=1;
		else if(delta_x==0)incx=0;
		else {incx=-1;delta_x=-delta_x;} 
		if(delta_y>0)incy=1; 
		else if(delta_y==0)incy=0;
		else{incy=-1;delta_y=-delta_y;} 
		if(delta_x>delta_y)distance=delta_x;
		else distance=delta_y; 
		for(t=0;t<=distance+1;t++ )
		{
			if(uRow>=0&&uRow<wid&&uCol>=0&&uCol<hei)p[uRow+uCol*wid]=color;
			xerr+=delta_x ; 
			yerr+=delta_y ; 
			if(xerr>distance) 
			{ 
				xerr-=distance; 
				uRow+=incx; 
			} 
			if(yerr>distance) 
			{ 
				yerr-=distance; 
				uCol+=incy; 
			} 
		}  
	}
	void draw_circle(int x,int y,int r,RGBPixel color){
		int rmax=(r+0.5)*(r+0.5),rmin=(r-0.5)*(r-0.5),i,j,d;
		for(i=(int)(r*0.7);i<=r;i++)
		{
			for(j=r-i;j<=r;j++)
			{
				d=i*i+j*j;
				if(d<=rmin)continue;
				if(d<=rmax)
				{
					for(int u=-1;u<=1;u+=2)
					{
						for(int v=-1;v<=1;v+=2)
						{
							int xx,yy;
							xx=x+u*i;
							yy=y+v*j;
							if(xx>=0&&xx<wid&&yy>=0&&yy<hei)p[xx+yy*wid]=color;
							xx=x+v*j;
							yy=y+u*i;
							if(xx>=0&&xx<wid&&yy>=0&&yy<hei)p[xx+yy*wid]=color;
						}
					}
				}
				else break;
			}
		}
	}
	void fill_circle(int x,int y,int r,RGBPixel color){
		int rmax=(r+0.5)*(r+0.5),i,j,d;
		for(i=0;i<=r;i++)
		{
			for(j=r-i;;j++)
			{
				d=i*i+j*j;
				if(d>rmax)
				{
					j--;
					int ymin=max(y-j,0),ymax=min(y+j,hei-1);
					if(ymax<0||ymin>=hei)break;
					if(x+i>=0&&x+i<wid)for(int yy=ymin;yy<=ymax;yy++)p[x+i+yy*wid]=color;
					if(x-i>=0&&x-i<wid)for(int yy=ymin;yy<=ymax;yy++)p[x-i+yy*wid]=color;
					break;
				}
			}
		}
	}
	void fill_triangle(int x0,int y0,int x1,int y1,int x2,int y2,RGBPixel color){
		int a, b, y, last,tmp;
		int dx01, dy01, dx02, dy02, dx12, dy12;
		int sa = 0;
		int sb = 0;
	 	if (y0>y1) 
		{
			tmp=y0;
			y0=y1;
			y1=tmp;
			tmp=x0;
			x0=x1;
			x1=tmp;
	 	}
	 	if (y1 > y2) 
		{
			tmp=y2;
			y2=y1;
			y1=tmp;
			tmp=x2;
			x2=x1;
			x1=tmp;
	 	}
		if (y0 > y1)//y0<=y1<=y2
		{
			tmp=y0;
			y0=y1;
			y1=tmp;
			tmp=x0;
			x0=x1;
			x1=tmp;
		}
		if(y0 == y2) 
		{ 
			a = b = x0;
			if(x1 < a)a = x1;
			else if(x1 > b)b=x1;
			if(x2<a)a = x2;
			else if(x2 > b)b=x2;
			for(int i=a;i<=b;i++)p[y0*wid+i]=color;
			return;
		}
		dx01 = x1 - x0;
		dy01 = y1 - y0;
		dx02 = x2 - x0;
		dy02 = y2 - y0;
		dx12 = x2 - x1;
		dy12 = y2 - y1;
		if(y1 == y2)
		{
			last = y1; 
		}
		else
		{
			last = y1-1; 
		}
		for(y=y0; y<=last; y++) 
		{
			a = x0 + sa / dy01;
			b = x0 + sb / dy02;
			sa += dx01;
    		sb += dx02;
    		if(a > b)
    		{
    			tmp=a;
    			a=b;
    			b=tmp;
			}
			for(int i=a;i<=b;i++)p[y*wid+i]=color;
		}
		sa = dx12 * (y - y1);
		sb = dx02 * (y - y0);
		for(; y<=y2; y++) 
		{
			a = x1 + sa / dy12;
			b = x0 + sb / dy02;
			sa += dx12;
			sb += dx02;
			if(a > b)
			{
    			tmp=a;
    			a=b;
    			b=tmp;
			}
			for(int i=a;i<=b;i++)p[y*wid+i]=color;
		}
	}
	void draw_bezier(int ax,int ay,int bx,int by,int cx,int cy,int dx,int dy,RGBPixel color){
		long long x,y,oldx=dx,oldy=dy;
		for(long long i=0;i<=1024;i++)
		{
			x=i*i*i*ax+3*i*i*(1024-i)*bx+3*i*(1024-i)*(1024-i)*cx+(1024-i)*(1024-i)*(1024-i)*dx;
			y=i*i*i*ay+3*i*i*(1024-i)*by+3*i*(1024-i)*(1024-i)*cy+(1024-i)*(1024-i)*(1024-i)*dy;
			x=(x+536870912)/1073741824;
			y=(y+536870912)/1073741824;
			draw_line(oldx,oldy,x,y,color);
			oldx=x;
			oldy=y;
			//if(x>=0&&x<wid&&y>=0&&y<hei)p[x+y*wid]=color;
		}
	}
	void fill_color(int x,int y,double dis,RGBPixel color){
		int distance=dis*dis,loc=y*wid+x,xx,yy;
		RGBPixel ori_color=p[loc];
		bool *alr;
		alr=new bool[wid*hei];
		for(int i=wid*hei-1;i>=0;i--)alr[i]=0;
		//memset(alr,0,sizeof(alr));
		stack<int> locs;
		locs.push(loc);
		alr[loc]=1;
		while(!locs.empty())
		{
			loc=locs.top();
			xx=loc%wid;
			yy=loc/wid;
			p[loc]=color;
			locs.pop();
			if(xx>0)
			{
				if(alr[loc-1]==0)
				{
					if(dist(ori_color,p[loc-1])<=distance)
					{
						locs.push(loc-1);
					}
					alr[loc-1]=1;
				}
			}
			if(xx<wid-1)
			{
				if(alr[loc+1]==0)
				{
					if(dist(ori_color,p[loc+1])<=distance)
					{
						locs.push(loc+1);
					}
					alr[loc+1]=1;
				}
			}
			if(yy>0)
			{
				if(alr[loc-wid]==0)
				{
					if(dist(ori_color,p[loc-wid])<=distance)
					{
						locs.push(loc-wid);
					}
					alr[loc-wid]=1;
				}
			}
			if(yy<hei-1)
			{
				if(alr[loc+wid]==0)
				{
					if(dist(ori_color,p[loc+wid])<=distance)
					{
						locs.push(loc+wid);
					}
					alr[loc+wid]=1;
				}
			}
		}
	}
	void cover(int x,int y,double dis){
		int distance=dis*dis,loc=y*wid+x,xx,yy,minx=wid,miny=hei,maxx=0,maxy=0;
		RGBPixel color=p[loc];
		char *alr;
		alr=new char[wid*hei];
		for(int i=wid*hei-1;i>=0;i--)alr[i]=0;
		//memset(alr,0,sizeof(alr));
		stack<int> locs;
		locs.push(loc);
		alr[loc]=1;
		while(!locs.empty())
		{
			loc=locs.top();
			xx=loc%wid;
			yy=loc/wid;
			p[loc]=color;
			locs.pop();
			if(xx>maxx)maxx=xx;
			if(yy>maxy)maxy=yy;
			if(xx<minx)minx=xx;
			if(yy<miny)miny=yy;
			if(xx>0)
			{
				if(alr[loc-1]==0)
				{
					if(dist(color,p[loc-1])<=distance)
					{
						locs.push(loc-1);
						alr[loc-1]=1;
					}
				}
			}
			if(xx<wid-1)
			{
				if(alr[loc+1]==0)
				{
					if(dist(color,p[loc+1])<=distance)
					{
						locs.push(loc+1);
						alr[loc+1]=1;
					}
				}
			}
			if(yy>0)
			{
				if(alr[loc-wid]==0)
				{
					if(dist(color,p[loc-wid])<=distance)
					{
						locs.push(loc-wid);
						alr[loc-wid]=1;
					}
				}
			}
			if(yy<hei-1)
			{
				if(alr[loc+wid]==0)
				{
					if(dist(color,p[loc+wid])<=distance)
					{
						locs.push(loc+wid);
						alr[loc+wid]=1;
					}
				}
			}
		}
		if(minx>0)minx--;
		if(miny>0)miny--;
		if(maxx<wid)maxx++;
		if(maxy<hei)maxy++;
		for(int i=minx;i<maxx;i++)
		{
			if(alr[i+miny*wid]==0)
			{
				alr[i+miny*wid]=2;
				locs.push(i+miny*wid);
			}
			if(alr[i+maxy*wid]==0)
			{
				alr[i+maxy*wid]=2;
				locs.push(i+maxy*wid);
			}
		}
		for(int i=miny;i<maxy;i++)
		{
			if(alr[i*wid+minx]==0)
			{
				alr[i*wid+minx]=2;
				locs.push(i*wid+minx);
			}
			if(alr[i*wid+maxx]==0)
			{
				alr[i*wid+maxx]=2;
				locs.push(i*wid+maxx);
			}
		}
		while(!locs.empty())
		{
			loc=locs.top();
			xx=loc%wid;
			yy=loc/wid;
			locs.pop();
			if(xx>minx)
			{
				if(alr[loc-1]==0)
				{
					locs.push(loc-1);
					alr[loc-1]=2;
				}
			}
			if(xx<maxx)
			{
				if(alr[loc+1]==0)
				{
					locs.push(loc+1);
					alr[loc+1]=2;
				}
			}
			if(yy>miny)
			{
				if(alr[loc-wid]==0)
				{
					locs.push(loc-wid);
					alr[loc-wid]=2;
				}
			}
			if(yy<maxy)
			{
				if(alr[loc+wid]==0)
				{
					locs.push(loc+wid);
					alr[loc+wid]=2;
				}
			}
		}
		for(int i=miny+1;i<maxy;i++)
		{
			for(int j=minx+1;j<maxx;j++)
			{
				if(!alr[i*wid+j])p[i*wid+j]=color;
			}
		}
	}
	void turn(double deg){
		double cost,sint;
		sint=sin(deg*3.14159265359/180.0);
		cost=cos(deg*3.14159265359/180.0);
		int xmin=0,xmax=wid-1,ymin=0,ymax=hei-1;
		int xori[4]={0,xmax,xmax,0},yori[4]={0,0,ymax,ymax},xo,yo;
		double xpre[4],ypre[4],xp,yp;
		double rr,gg,bb;
		for(int i=0;i<4;i++)
		{
			xpre[i]=xori[i]*cost-yori[i]*sint;
			ypre[i]=xori[i]*sint+yori[i]*cost;
		}
		xmin=(int)xpre[0];
		xmax=(int)(xpre[0]+0.999999);
		ymin=(int)ypre[0];
		ymax=(int)(ypre[0]+0.999999);
		for(int i=0;i<4;i++)
		{
			if(xmin>xpre[i])xmin=(int)xpre[i];
			if(xmax<xpre[i])xmax=(int)(xpre[i]+0.999999);
			if(ymin>ypre[i])ymin=(int)ypre[i];
			if(ymax<ypre[i])ymax=(int)(ypre[i]+0.999999);
		}
		bmp24 ls;
		ls.wid=xmax-xmin+1;
		ls.hei=ymax-ymin+1;
		ls.space_apply();
		//cout<<"x={"<<xmin<<"~"<<xmax<<"} y={"<<ymin<<"~"<<ymax<<"}\n";
		for(int x=0;x<ls.wid;x++)
		{
			for(int y=0;y<ls.hei;y++)
			{
				xp=(x+xmin)*cost+(y+ymin)*sint;
				yp=0-(x+xmin)*sint+(y+ymin)*cost;
				//printf("x=%d,y=%d,xp=%lf,yp=%lf\n",x,y,xp,yp);
				getcolor(xp,yp,&rr,&gg,&bb);
				ls.p[y*ls.wid+x].R=(int)(rr);
				ls.p[y*ls.wid+x].G=(int)(gg);
				ls.p[y*ls.wid+x].B=(int)(bb);
			}
		}
		clone(ls);
	}
	void selfturn(double deg){
		double cost,sint;
		sint=sin(deg*3.14159265359/180.0);
		cost=cos(deg*3.14159265359/180.0);
		bmp24 ls;
		double xp,yp;
		double rr,gg,bb;
		ls.wid=wid;
		ls.hei=hei;
		ls.space_apply();
		double xmin=(wid-1)/2.0,ymin=(hei-1)/2.0;
		for(int x=0;x<ls.wid;x++)
		{
			for(int y=0;y<ls.hei;y++)
			{
				xp=(x-xmin)*cost+(y-ymin)*sint+xmin;
				yp=0-(x-xmin)*sint+(y-ymin)*cost+ymin;
				//printf("x=%d,y=%d,xp=%lf,yp=%lf\n",x,y,xp,yp);
				getcolor(xp,yp,&rr,&gg,&bb);
				ls.p[y*ls.wid+x].R=(int)(rr);
				ls.p[y*ls.wid+x].G=(int)(gg);
				ls.p[y*ls.wid+x].B=(int)(bb);
			}
		}
		clone(ls);
	}
	void doudong_colorful(){
		int odr=0,odg=0,odb=0;
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{
				odr+=p[i*wid+j].R; 
				odg+=p[i*wid+j].G; 
				odb+=p[i*wid+j].B; 
				if(odr>127)
				{
					p[i*wid+j].R=255;
					odr-=255;
				}
				else
				{
					p[i*wid+j].R=0;
				}
				if(odg>127)
				{
					p[i*wid+j].G=255;
					odg-=255;
				}
				else
				{
					p[i*wid+j].G=0;
				}
				if(odb>127)
				{
					p[i*wid+j].B=255;
					odb-=255;
				}
				else
				{
					p[i*wid+j].B=0;
				}
			}
		}
	}
	void doudong(){
		heibaihua();
		int od=0;
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{
				od+=p[i*wid+j].R;
				if(od>127)
				{
					p[i*wid+j].R=255;
					p[i*wid+j].G=255;
					p[i*wid+j].B=255;
					od-=255;
				}
				else
				{
					p[i*wid+j].R=0;
					p[i*wid+j].G=0;
					p[i*wid+j].B=0;
				}
			}
		}
	}
	void colorprint(){
		wid=1530;
		hei=512;
		space_apply();
		int rr=255,gg=0,bb=0;
		for(int i=0;i<255;i++)
		{
			rr=255;
			gg=i;
			bb=0;
			for(int j=0;j<=255;j++)
			{
				p[j*wid+i].R=rr*j/255.0+0.5;
				p[j*wid+i].G=gg*j/255.0+0.5;
				p[j*wid+i].B=bb*j/255.0+0.5;
			}
			for(int j=0;j<=255;j++)
			{
				p[(256+j)*wid+i].R=(rr*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].G=(gg*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].B=(bb*(255-j)+255*j)/255.0+0.5;
			}
		}
		for(int i=255;i<510;i++)
		{
			rr=510-i;
			gg=255;
			bb=0;
			for(int j=0;j<=255;j++)
			{
				p[j*wid+i].R=rr*j/255.0+0.5;
				p[j*wid+i].G=gg*j/255.0+0.5;
				p[j*wid+i].B=bb*j/255.0+0.5;
			}
			for(int j=0;j<=255;j++)
			{
				p[(256+j)*wid+i].R=(rr*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].G=(gg*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].B=(bb*(255-j)+255*j)/255.0+0.5;
			}
		}
		for(int i=510;i<765;i++)
		{
			rr=0;
			gg=255;
			bb=i-510;
			for(int j=0;j<=255;j++)
			{
				p[j*wid+i].R=rr*j/255.0+0.5;
				p[j*wid+i].G=gg*j/255.0+0.5;
				p[j*wid+i].B=bb*j/255.0+0.5;
			}
			for(int j=0;j<=255;j++)
			{
				p[(256+j)*wid+i].R=(rr*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].G=(gg*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].B=(bb*(255-j)+255*j)/255.0+0.5;
			}
		}
		for(int i=765;i<1020;i++)
		{
			rr=0;
			gg=1020-i;
			bb=255;
			for(int j=0;j<=255;j++)
			{
				p[j*wid+i].R=rr*j/255.0+0.5;
				p[j*wid+i].G=gg*j/255.0+0.5;
				p[j*wid+i].B=bb*j/255.0+0.5;
			}
			for(int j=0;j<=255;j++)
			{
				p[(256+j)*wid+i].R=(rr*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].G=(gg*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].B=(bb*(255-j)+255*j)/255.0+0.5;
			}
		}
		for(int i=1020;i<1275;i++)
		{
			rr=i-1020;
			gg=0;
			bb=255;
			for(int j=0;j<=255;j++)
			{
				p[j*wid+i].R=rr*j/255.0+0.5;
				p[j*wid+i].G=gg*j/255.0+0.5;
				p[j*wid+i].B=bb*j/255.0+0.5;
			}
			for(int j=0;j<=255;j++)
			{
				p[(256+j)*wid+i].R=(rr*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].G=(gg*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].B=(bb*(255-j)+255*j)/255.0+0.5;
			}
		}
		for(int i=1275;i<1530;i++)
		{
			rr=255;
			gg=0;
			bb=1530-i;
			for(int j=0;j<=255;j++)
			{
				p[j*wid+i].R=rr*j/255.0+0.5;
				p[j*wid+i].G=gg*j/255.0+0.5;
				p[j*wid+i].B=bb*j/255.0+0.5;
			}
			for(int j=0;j<=255;j++)
			{
				p[(256+j)*wid+i].R=(rr*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].G=(gg*(255-j)+255*j)/255.0+0.5;
				p[(256+j)*wid+i].B=(bb*(255-j)+255*j)/255.0+0.5;
			}
		}
	}
	void fushe(int w,int h,int rr,double angle){
		double ang,r,Ran;
		HSVPixel ls;
		wid=w;
		hei=h;
		Ran=rr;
		space_apply();
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{	
				r=sqrt((i-hei/2)*(i-hei/2)+(j-wid/2)*(j-wid/2));
				if(r>Ran)r=Ran;
				ang=atan2(i-hei/2,j-wid/2);
				ls.H=ang*180.0/3.1415926535897932+angle;
				ls.S=r/Ran;
				ls.V=1;
				p[i*wid+j]=hsv2rgb(ls);
			}
		}
	}
};

struct bmp32{
	int wid,hei;
	RGBAPixel *p;
	bool read(char bmpname[])
	{
		//delete[] p;
		FILE *fp=fopen(bmpname,"rb");  
		if(fp==0) return false;
		fseek(fp, sizeof(BITMAPFILEHEADER),0);  //FILE *fp=fopen(bmpname,"rb");BITMAPFILEHEADER fileheader;fread(&fileheader, sizeof(BITMAPFILEHEADER), 1,fp);     
		BITMAPINFOHEADER head;     
		fread(&head, sizeof(BITMAPINFOHEADER), 1,fp);     
		int bmpWidth = head.biWidth;  
		int bmpHeight = head.biHeight;  
		int biBitCount = head.biBitCount;  
		wid = head.biWidth;  
		hei = head.biHeight;  
		int lineByte=(bmpWidth * biBitCount/8+3)/4*4;  
		int buwei=lineByte-bmpWidth * biBitCount/8;
		if(biBitCount==1)
		{
			//lineByte=((bmpWidth+7)/8+3)/4*4;  
			p=new RGBAPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[(bmpWidth * bmpHeight+7)/8];  
			unsigned char loc,w;
			fread(pBmpBuf,1,(bmpWidth * bmpHeight+7)/8,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					w=(i*bmpWidth+j)%8;
					loc=(i*bmpWidth+j)/8;
					if(pBmpBuf[loc]&(0x80>>w))
					{
						p[i*bmpWidth+j].B=255;
						p[i*bmpWidth+j].G=255;
						p[i*bmpWidth+j].R=255;
						p[i*bmpWidth+j].A=255;
					}
					else
					{
						p[i*bmpWidth+j].B=0;
						p[i*bmpWidth+j].G=0;
						p[i*bmpWidth+j].R=0;
						p[i*bmpWidth+j].A=255;
					}
				}
			}
			delete[] pBmpBuf;
		}  
		if(biBitCount==8)
		{
			RGBQUAD *pColorTable;
			pColorTable=new RGBQUAD[256];  
			fread(pColorTable,sizeof(RGBQUAD),256,fp);  
			p=new RGBAPixel[bmpHeight*bmpWidth];
			unsigned char mapp;
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					mapp=pBmpBuf[i*lineByte+j];
					p[i*bmpWidth+j].B=pColorTable[mapp].rgbBlue;
					p[i*bmpWidth+j].G=pColorTable[mapp].rgbGreen;
					p[i*bmpWidth+j].R=pColorTable[mapp].rgbRed;
					p[i*bmpWidth+j].A=255;
				}
			}
			delete[] pBmpBuf;
		}  
		if(biBitCount==24)
		{
			p=new RGBAPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					p[i*bmpWidth+j].B=pBmpBuf[i*lineByte+j*3];
					p[i*bmpWidth+j].G=pBmpBuf[i*lineByte+j*3+1];
					p[i*bmpWidth+j].R=pBmpBuf[i*lineByte+j*3+2];
					p[i*bmpWidth+j].A=255;
				}
			}
			delete[] pBmpBuf;
		}  
		if(biBitCount==32)
		{
			p=new RGBAPixel[bmpHeight*bmpWidth];
			unsigned char *pBmpBuf=new unsigned char[lineByte * bmpHeight];  
			fread(pBmpBuf,1,lineByte * bmpHeight,fp);  
			for(int i=0;i<bmpHeight;i++)
			{
				for(int j=0;j<bmpWidth;j++)
				{
					p[i*bmpWidth+j].B=pBmpBuf[i*lineByte+j*4];
					p[i*bmpWidth+j].G=pBmpBuf[i*lineByte+j*4+1];
					p[i*bmpWidth+j].R=pBmpBuf[i*lineByte+j*4+2];
					p[i*bmpWidth+j].A=pBmpBuf[i*lineByte+j*4+3];
				}
			}
			delete[] pBmpBuf;
		}  
		fclose(fp);  
		return true;
	}
	void save(char bmpName[])
	{
		int colorTablesize=0;  
		int lineByte=(wid * 4+3)/4*4; 
		int buwei=lineByte-wid*3;
		unsigned char bw=0;
		FILE *fp=fopen(bmpName,"wb");  
		if(fp==0) return;  
		BITMAPFILEHEADER fileHead;  
		fileHead.bfType = 0x4D42;
		fileHead.bfSize= sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+lineByte*hei;  
		fileHead.bfReserved1 = 0;  
		fileHead.bfReserved2 = 0;  
		fileHead.bfOffBits=54;  
		fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);  
		BITMAPINFOHEADER head;    
		head.biBitCount=32;  
		head.biClrImportant=0;  
		head.biClrUsed=0;  
		head.biCompression=0;  
		head.biHeight=hei;  
		head.biPlanes=1;  
		head.biSize=40;  
		head.biSizeImage=lineByte*hei;  
		head.biWidth=wid;  
		head.biXPelsPerMeter=3780;  
		head.biYPelsPerMeter=3780;  
		fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);  
		for(int i=0;i<hei;i++)
		{
			for(int j=0;j<wid;j++)
			{
				fwrite(p+i*wid+j,4,1,fp);  
			}
			fwrite(&bw,1,buwei,fp);  
		}
		fclose(fp);  
	}
	void space_apply()
	{
		p=new RGBAPixel[wid*hei];
		for(int i=wid*hei-1;i>=0;i--)
		{
			p[i].R=0;
			p[i].G=0;
			p[i].B=0;
			p[i].A=0;
		}
	}
};

RGBPixel rgb2grey(RGBPixel a)
{
	RGBPixel b;
	b.R=b.G=b.B=(a.B+a.G+a.R)/3;
	return b;
}
double bhd(RGBPixel a) 
{
	unsigned char cmax,cmin;
	double s;
	cmin=min(a.R,min(a.G,a.B));
	cmax=max(a.R,min(a.G,a.B));
	if(cmax==0)return 0;
	else return (double)(cmax-cmin)/(double)cmax;
}
HSVPixel rgb2hsv(RGBPixel a)
{
	HSVPixel K;
	double r,g,b,cmax,cmin,dt;
	r=(double)a.R/255.0;
	g=(double)a.G/255.0;
	b=(double)a.B/255.0;
	cmin=min(r,g);
	cmin=min(cmin,b);
	cmax=max(r,max(g,b));
	dt=cmax-cmin;
	K.V=cmax;
	if(cmax==0)
	{
		K.S=0;
		K.H=0;
	}
	else K.S=dt/cmax;
	if(dt==0)K.H=0;
	else 
	{
		if(cmax==r)
		{
			double ls=(g-b)/dt;
			while(ls<0)ls+=6.0;
			while(ls>=6)ls-=6.0;
			K.H=60*ls;
		}
		else if(cmax==g)K.H=60*((b-r)/dt+2);
		else K.H=60*((r-g)/dt+4);
	}
	return K;
}

RGBPixel hsv2rgb(HSVPixel a)
{
	RGBPixel k;
	while(a.H>=360.0)a.H-=360;
	while(a.H<0)a.H+=360;
	if(a.S<0)a.S=0;
	if(a.S>1)a.S=1;
	if(a.V<0)a.V=0;
	if(a.V>1)a.V=1;
	double c=a.S*a.V;
	double m=a.V-c;
	double x,ls=a.H/60.0;
	while(ls>2.0)ls-=2.0;
	ls=ls-1.0;
	if(ls<0)ls=0-ls;
	x=c*(1-ls);
	double r,g,b;
	if(a.H<60)
	{
		r=c;
		g=x;
		b=0.0;
	}
	else if(a.H<120)
	{
		r=x;
		g=c;
		b=0.0;
	}
	else if(a.H<180)
	{
		r=0.0;
		g=c;
		b=x;
	}
	else if(a.H<240)
	{
		r=0.0;
		g=x;
		b=c;
	}
	else if(a.H<300)
	{
		r=x;
		g=0.0;
		b=c;
	}
	else
	{
		r=c;
		g=0.0;
		b=x;
	}
	r+=m;
	b+=m;
	g+=m;
	r*=255.0;
	g*=255.0;
	b*=255.0;
	k.R=(unsigned char)r;
	k.G=(unsigned char)g;
	k.B=(unsigned char)b;
	return k;
}

double powd(double a,int n)//double pow
{
	double ans=1.0;
	for(int i=0;i<n;i++)ans*=a;
	return ans;
}
int dist(RGBPixel a,RGBPixel b)//distance
{
	int o=0;
	o=o+(a.R-b.R)*(a.R-b.R);
	o=o+(a.G-b.G)*(a.G-b.G);
	o=o+(a.B-b.B)*(a.B-b.B);
	return o;
}
void xiangsufenge(bmp24 a,bmp24 b)
{
	b.wid=a.wid;
	b.hei=a.hei;
	b.space_apply();
	RGBPixel Green;
	Green.R=Green.B=0;
	Green.G=255;
	int yz=10000;
	for(int i=0;i<b.hei;i++)
		for(int j=0;j<b.wid;j++)
			b.p[i*b.wid+j]=a.p[i*b.wid+j];
	for(int j=0;j<b.wid;j++)b.p[j]=Green;
	for(int i=2;i<b.hei;i++)
	{
		int t=0; 
		for(int j=0;j<b.wid;j++)
		{
			if(dist(a.p[i*b.wid+j],a.p[(i-1)*b.wid+j])>yz)t++;
		}
		if(t>4)
		{
			for(int j=0;j<b.wid;j++)b.p[i*b.wid+j]=Green;
			i++;
		}
	}
	for(int i=0;i<b.hei;i++)b.p[i*b.wid]=Green;
	for(int j=2;j<b.wid;j++)
	{
		int t=0; 
		for(int i=0;i<b.hei;i++)
		{
			if(dist(a.p[i*b.wid+j],a.p[i*b.wid+j-1])>yz)t++;
		}
		if(t>4)
		{
			for(int i=0;i<b.hei;i++)b.p[i*b.wid+j]=Green;
			j++;
		}
	}
}
bool gre(RGBPixel a)//green
{
	if(a.G!=255)return false;
	if(a.B!=0)return false;
	if(a.R!=0)return false;
	return true;
}

