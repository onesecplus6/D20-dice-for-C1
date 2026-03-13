#include "Windows.h"  
#include "stdio.h"  
#include "string"  
#include "cstring"  
#include "stack"  
#include "queue"  
#include "malloc.h"  
#include "math.h"  
#include "cstdlib"  
#include "iostream"  
#define Pi 3.1415926535897932384626
using namespace std;


struct vec3d
{
	float x;float y;float z;
	
	vec3d(float x=0,float y=0,float z=0):x(x),y(y),z(z){}
	double length()
	{
		return sqrt(x*x+y*y+z*z);
	}
	void guiyihua()
	{
		double le=length();
		if(le==0)return;
		x/=le;
		y/=le;
		z/=le;
	}
	friend bool operator==(const vec3d &qs,const vec3d &hs)
	{
		if(qs.x!=hs.x)return false;
		if(qs.y!=hs.y)return false;
		if(qs.z!=hs.z)return false;
		return true;
	}
	friend bool operator!=(const vec3d &qs,const vec3d &hs)
	{
		return !(qs==hs);
	}
    friend ostream& operator<<(ostream&out,const vec3d &p)//重载输出流
    {
    	out<<"("<<p.x<<","<<p.y<<","<<p.z<<")";
        return out;
    }
	friend vec3d operator+(const vec3d &qs,const vec3d &hs)
	{
		vec3d ans;
    	ans.x=qs.x+hs.x;
    	ans.y=qs.y+hs.y;
    	ans.z=qs.z+hs.z;
    	return ans;
	}
	friend vec3d operator-(const vec3d &qs,const vec3d &hs)
	{
		vec3d ans;
    	ans.x=qs.x-hs.x;
    	ans.y=qs.y-hs.y;
    	ans.z=qs.z-hs.z;
    	return ans;
    }
    friend vec3d operator*(const vec3d &qs,const vec3d &hs)//叉乘 
    {
    	vec3d ans;
    	ans.x=qs.y*hs.z-qs.z*hs.y;
    	ans.y=qs.z*hs.x-qs.x*hs.z;
    	ans.z=qs.x*hs.y-qs.y*hs.x;
    	return ans;
    }
    friend float operator/(const vec3d &qs,const vec3d &hs)//`*点乘 
    {
    	float ans=0;
    	ans=qs.x*hs.x+qs.y*hs.y+qs.z*hs.z;
    	return ans;
    }
    friend vec3d operator/(const vec3d &qs,const int &hs)
    {
    	vec3d ans;
    	ans.x=qs.x/hs;
    	ans.y=qs.y/hs;
    	ans.z=qs.z/hs;
    	return ans;
    }
    friend vec3d operator/(const vec3d &qs,const float &hs)
    {
    	vec3d ans;
    	ans.x=qs.x/hs;
    	ans.y=qs.y/hs;
    	ans.z=qs.z/hs;
    	return ans;
    }
    friend vec3d operator/(const vec3d &qs,const double &hs)
    {
    	vec3d ans;
    	ans.x=qs.x/hs;
    	ans.y=qs.y/hs;
    	ans.z=qs.z/hs;
    	return ans;
    }
    friend vec3d operator*(const int &qs,const vec3d &hs)
    {
    	vec3d ans;
    	ans.x=qs*hs.x;
    	ans.y=qs*hs.y;
    	ans.z=qs*hs.z;
    	return ans;
    }
    friend vec3d operator*(const float &qs,const vec3d &hs)
    {
    	vec3d ans;
    	ans.x=qs*hs.x;
    	ans.y=qs*hs.y;
    	ans.z=qs*hs.z;
    	return ans;
    }
    friend vec3d operator*(const double &qs,const vec3d &hs)
    {
    	vec3d ans;
    	ans.x=qs*hs.x;
    	ans.y=qs*hs.y;
    	ans.z=qs*hs.z;
    	return ans;
    }
    friend vec3d operator*(const vec3d &hs,const int &qs)
    {
    	return qs*hs;
    }
    friend vec3d operator*(const vec3d &hs,const float &qs)
    {
    	return qs*hs;
    }
    friend vec3d operator*(const vec3d &hs,const double &qs)
    {
    	return qs*hs;
    }
	void rotate(vec3d zh,double the){
		if(zh.length()<1e-6)return;
		vec3d Rx,Ry,Rz,tmp;
		double si=sin(the),co=cos(the);
		zh.guiyihua();
		Rx.x=co+(1-co)*zh.x*zh.x;
		Rx.y=(1-co)*zh.x*zh.y-si*zh.z;
		Rx.z=(1-co)*zh.x*zh.z+si*zh.y;
		Ry.x=(1-co)*zh.x*zh.y+si*zh.z;
		Ry.y=co+(1-co)*zh.y*zh.y;
		Ry.z=(1-co)*zh.z*zh.y-si*zh.x;
		Rz.x=(1-co)*zh.x*zh.z-si*zh.y;
		Rz.y=(1-co)*zh.z*zh.y+si*zh.x;
		Rz.z=co+(1-co)*zh.z*zh.z;
		tmp.x=*this/Rx;
		tmp.y=*this/Ry;
		tmp.z=*this/Rz;
		*this=tmp;
	}
	double angle(vec3d a)
	{
		double ans = (*this / this->length()) / (a/a.length());
		return acos(ans);
	}
	void rotate_to(vec3d a, double ratio)
	{
		vec3d axis=*this * a;
		axis.guiyihua();
		this->rotate(axis,this->angle(a)*ratio);
	}
}; 
struct facet
{
	vec3d normal,vertex[3];
	void calc()
	{
		normal=(vertex[1]-vertex[0])*(vertex[2]-vertex[0]);
	}
	void reverse()//0不变 12互换 计算法向量 
	{
		vec3d ls;
		ls=vertex[1];
		vertex[1]=vertex[2];
		vertex[2]=ls;
		calc();
	}
	int direction(vec3d a,vec3d b)
	{
		int k=-1;
		for(int i=0;i<3;i++)if(vertex[i]==a){k=i;break;}
		if(k==-1)return -1;
		if(vertex[(k+1)%3]==b)return 1;
		if(vertex[(k+2)%3]==b)return 0;
		return -1;
	}
};
struct stlfile
{
	unsigned int number;
	facet *f;
	char text[80];
	bool read(char stlname[]){
		FILE *fp=fopen(stlname,"rb");
		if(fp==0) return false;
		fread(text,80,1,fp);
		fread(&number,sizeof(number),1,fp);
		f=new facet[number];
		for(unsigned int i=0;i<number;i++)
		{
			fread(&f[i].normal.x,sizeof(float),1,fp);
			fread(&f[i].normal.y,sizeof(float),1,fp);
			fread(&f[i].normal.z,sizeof(float),1,fp);
			
			fread(&f[i].vertex[0].x,sizeof(float),1,fp);
			fread(&f[i].vertex[0].y,sizeof(float),1,fp);
			fread(&f[i].vertex[0].z,sizeof(float),1,fp);
			
			fread(&f[i].vertex[1].x,sizeof(float),1,fp);
			fread(&f[i].vertex[1].y,sizeof(float),1,fp);
			fread(&f[i].vertex[1].z,sizeof(float),1,fp);
			
			fread(&f[i].vertex[2].x,sizeof(float),1,fp);
			fread(&f[i].vertex[2].y,sizeof(float),1,fp);
			fread(&f[i].vertex[2].z,sizeof(float),1,fp);
			
			fseek(fp,2,SEEK_CUR);
		}
		fclose(fp);  
		return true;
	}
	void save(char stlname[]){
		FILE *fp=fopen(stlname,"wb");
		char bw=0;
		
		fwrite(text,80,1,fp);
		fwrite(&number,sizeof(number),1,fp);
		for(int i=0;i<number;i++)
		{
			fwrite(&f[i].normal.x,sizeof(float),1,fp);
			fwrite(&f[i].normal.y,sizeof(float),1,fp);
			fwrite(&f[i].normal.z,sizeof(float),1,fp);
			
			fwrite(&f[i].vertex[0].x,sizeof(float),1,fp);
			fwrite(&f[i].vertex[0].y,sizeof(float),1,fp);
			fwrite(&f[i].vertex[0].z,sizeof(float),1,fp);
			
			fwrite(&f[i].vertex[1].x,sizeof(float),1,fp);
			fwrite(&f[i].vertex[1].y,sizeof(float),1,fp);
			fwrite(&f[i].vertex[1].z,sizeof(float),1,fp);
			
			fwrite(&f[i].vertex[2].x,sizeof(float),1,fp);
			fwrite(&f[i].vertex[2].y,sizeof(float),1,fp);
			fwrite(&f[i].vertex[2].z,sizeof(float),1,fp);
			
			fwrite(&bw,1,2,fp);  
		}
		fclose(fp);
	}
	void write_text(char note[]){
		for(int i=0;note[i]!='\0'&&i<80;i++)text[i]=note[i];
	}
	void remove_facet(int num){
		if(num>=number)return;
		f[num]=f[number-1];
		number--;
	}
	void space_apply(int i){
		number=i;
		f=new facet[number];
	}
	void standardize(){//move to x>=0 y>=0 z>=0
		float xmin=f[0].vertex[0].x,ymin=f[0].vertex[0].y,zmin=f[0].vertex[0].z;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				if(f[i].vertex[j].x<xmin)xmin=f[i].vertex[j].x;
				if(f[i].vertex[j].y<ymin)ymin=f[i].vertex[j].y;
				if(f[i].vertex[j].z<zmin)zmin=f[i].vertex[j].z;
			}
		}
		if(xmin>=0&&ymin>=0&&zmin>=0)return;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				f[i].vertex[j].x-=xmin;
				f[i].vertex[j].y-=ymin;
				f[i].vertex[j].z-=zmin;
			}
		}
	}
	void calc(){
		for(int i=0;i<number;i++)
		{
			f[i].calc();
		}
	}
	void space_apply(){
		f=new facet[number];
	}
	void resize(float bs){
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				f[i].vertex[j].x*=bs;
				f[i].vertex[j].y*=bs;
				f[i].vertex[j].z*=bs;
			}
		}
	}
	void mirror(){
		vec3d ls; 
		for(int i=0;i<number;i++)
		{
			f[i].normal.x=-f[i].normal.x;
			f[i].vertex[0].x=-f[i].vertex[0].x;
			f[i].vertex[1].x=-f[i].vertex[1].x;
			f[i].vertex[2].x=-f[i].vertex[2].x;
			
			ls=f[i].vertex[0];
			f[i].vertex[0]=f[i].vertex[1];
			f[i].vertex[1]=ls;
		}
		//standardize();
	}
	void fix(){
		double v=0;
		bool *fixed;
		fixed=new bool[number];
		memset(fixed,false,number);
		queue<int>waiting;
		queue<int>done;
		
		int cp[3][3]={3,2,1,2,3,0,1,0,3};
		
		fixed[0]=true;
		waiting.push(0);
		f[0].calc();
		while(!waiting.empty())
		{
			v=0;
			while(!waiting.empty())
			{
				int num=waiting.front(),sid,sides;
				waiting.pop();
				done.push(num);
				
				int loc=0,co[2];
				for(int j=0;j<number;j++)
				{
					if(j==num)continue;
					loc=0;
					co[1]=3;
					co[2]=3;
					for(int k=0;k<3;k++)
					{
						if(f[j].vertex[k]==f[num].vertex[0]){co[loc]=0;loc++;}
						if(f[j].vertex[k]==f[num].vertex[1]){co[loc]=1;loc++;}
						if(f[j].vertex[k]==f[num].vertex[2]){co[loc]=2;loc++;}
					}
					if(loc==2)
					{
						sid=j;
						if(fixed[sid])continue;
						sides=cp[co[0]][co[1]];
						if(f[sid].direction(f[num].vertex[(sides+1)%3],f[num].vertex[(sides+2)%3])==1)
						{
							f[sid].reverse();
						}
						waiting.push(sid);
						fixed[sid]=true;
					}
				}
				f[num].calc();
				v=v+f[num].normal.z*(f[num].vertex[0].z+f[num].vertex[1].z+f[num].vertex[1].z);//3x
			}
			if(v<0)
			{
				while(!done.empty())
				{
					f[done.front()].reverse();
					done.pop();
				}
			}
			while(!done.empty())done.pop();
			v=0;
			for(int i=0;i<number;i++)
			{
				if(!fixed[i])
				{
					waiting.push(i);
					fixed[i]=true;
					break;
				}
			}
		}
		delete[] fixed;
	}
	double volume(){
		double v=0;
		for(int i=0;i<number;i++)
		{
			f[i].calc();
			v=v+f[i].normal.z*(f[i].vertex[0].z+f[i].vertex[1].z+f[i].vertex[1].z)/6.0;
		}
		return v;
	}
	double surface(){
		double ans=0;
		for(int i=0;i<number;i++)
		{
			f[i].calc();
			ans=ans+sqrt(f[i].normal/f[i].normal)/2.0;
		}
		return ans;
	}
	friend stlfile operator+(const stlfile&qs,const stlfile&hs){
		stlfile ans;
		ans.number=qs.number+hs.number;
		ans.space_apply();
		for(int i=0;i<qs.number;i++)
		{
			ans.f[i]=qs.f[i];
		}
		for(int i=0;i<hs.number;i++)
		{
			ans.f[i+qs.number]=hs.f[i];
		}
		return ans;
	}
	void copyfrom(stlfile a){
		number=a.number;
		space_apply();
		for(int i=0;i<80;i++)text[i]=a.text[i];
		/*
		for(int i=0;i<number;i++)
		{
			f[i]=a.f[i];
		}
		*/
		memcpy(f,a.f,number*sizeof(facet));
	}
	void move(float x,float y,float z){
		vec3d mp=vec3d(x,y,z);
		for(int i=0;i<number;i++)
		{
			f[i].vertex[0]=f[i].vertex[0]+mp;
			f[i].vertex[1]=f[i].vertex[1]+mp;
			f[i].vertex[2]=f[i].vertex[2]+mp;
		}
	}
	queue<stlfile> split(){
		queue<stlfile> ans;
		stlfile lls;
		bool *fixed;
		int numb;
		fixed=new bool[number];
		memset(fixed,false,number);
		queue<int>waiting;
		queue<int>done;
		
		int cp[3][3]={3,2,1,2,3,0,1,0,3};
		
		fixed[0]=true;
		waiting.push(0);
		f[0].calc();
		while(!waiting.empty())
		{
			numb=0;
			while(!waiting.empty())
			{
				int num=waiting.front(),sid,sides;
				waiting.pop();
				numb++;
				done.push(num);
				
				int loc=0,co[2];
				for(int j=0;j<number;j++)
				{
					if(j==num)continue;
					loc=0;
					co[1]=3;
					co[2]=3;
					for(int k=0;k<3;k++)
					{
						if(f[j].vertex[k]==f[num].vertex[0]){co[loc]=0;loc++;}
						if(f[j].vertex[k]==f[num].vertex[1]){co[loc]=1;loc++;}
						if(f[j].vertex[k]==f[num].vertex[2]){co[loc]=2;loc++;}
					}
					if(loc==2)
					{
						sid=j;
						if(fixed[sid])continue;
						waiting.push(sid);
						fixed[sid]=true;
					}
				}
				f[num].calc();
			}
			lls.space_apply(numb);
			for(int i=0;!done.empty();i++)
			{
				lls.f[i]=f[done.front()];
				done.pop();
			}
			ans.push(lls);
			for(int i=0;i<number;i++)
			{
				if(!fixed[i])
				{
					waiting.push(i);
					fixed[i]=true;
					break;
				}
			}
		}
		delete[] fixed;
		return ans;
	}
	void rotate(vec3d zh,double the){
		if(zh.length()<1e-6)return;
		vec3d Rx,Ry,Rz,tmp;
		double si=sin(the),co=cos(the);
		zh.guiyihua();
		Rx.x=co+(1-co)*zh.x*zh.x;
		Rx.y=(1-co)*zh.x*zh.y-si*zh.z;
		Rx.z=(1-co)*zh.x*zh.z+si*zh.y;
		Ry.x=(1-co)*zh.x*zh.y+si*zh.z;
		Ry.y=co+(1-co)*zh.y*zh.y;
		Ry.z=(1-co)*zh.z*zh.y-si*zh.x;
		Rz.x=(1-co)*zh.x*zh.z-si*zh.y;
		Rz.y=(1-co)*zh.z*zh.y+si*zh.x;
		Rz.z=co+(1-co)*zh.z*zh.z;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				tmp.x=f[i].vertex[j]/Rx;
				tmp.y=f[i].vertex[j]/Ry;
				tmp.z=f[i].vertex[j]/Rz;
				f[i].vertex[j]=tmp;
			}
		}
		calc();
		//standardize();
	}
	void transform(vec3d oldx, vec3d oldy, vec3d oldz, vec3d newx, vec3d newy, vec3d newz){
		float det;
		vec3d x,y,z,rex,rey,rez,ntx,nty,ntz,tmp;
		if((oldz/oldz)<1e-6)
		{
			oldx.guiyihua();
			oldy.guiyihua();
			oldz=oldx*oldy;
			newx.guiyihua();
			newy.guiyihua();
			newz=newx*newy;
		}
		det = oldx.x*(oldy.y*oldz.z-oldz.y*oldy.z) - oldy.x*(oldx.y*oldz.z-oldz.y*oldx.z) + oldz.x*(oldx.y*oldy.z-oldy.y*oldx.z);
		if(det<1e-6)return;
		ntx.x=newx.x;
		ntx.y=newy.x;
		ntx.z=newz.x;
		nty.x=newx.y;
		nty.y=newy.y;
		nty.z=newz.y;
		ntz.x=newx.z;
		ntz.y=newy.z;
		ntz.z=newz.z;
		rex.x=(oldy.y*oldz.z-oldz.y*oldy.z)/det;
		rex.y=(oldz.y*oldx.z-oldx.y*oldz.z)/det;
		rex.z=(oldx.y*oldy.z-oldy.y*oldx.z)/det;
		rey.x=(oldy.z*oldz.x-oldy.x*oldz.z)/det;
		rey.y=(oldx.x*oldz.z-oldz.x*oldx.z)/det;
		rey.z=(oldy.x*oldx.z-oldx.x*oldy.z)/det;
		rez.x=(oldy.x*oldz.y-oldz.x*oldy.y)/det;
		rez.y=(oldx.y*oldz.x-oldx.x*oldz.y)/det;
		rez.z=(oldx.x*oldy.y-oldy.x*oldx.y)/det;
		x.x=ntx/rex;
		x.y=ntx/rey;
		x.z=ntx/rez;
		y.x=nty/rex;
		y.y=nty/rey;
		y.z=nty/rez;
		z.x=ntz/rex;
		z.y=ntz/rey;
		z.z=ntz/rez;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				tmp.x=f[i].vertex[j]/x;
				tmp.y=f[i].vertex[j]/y;
				tmp.z=f[i].vertex[j]/z;
				f[i].vertex[j]=tmp;
			}
			f[i].calc();
		}
	}
	void face_to_ground(int num){
		f[num].calc();
		vec3d Rx,Ry,Rz,tmp,zh;
		double si,co;
		tmp=f[num].normal;
		tmp.guiyihua();
		co=0.0-tmp.z;
		si=sqrt(1-co*co);
		zh=f[num].normal*vec3d(0,0,-1);
		if(si==0)zh=vec3d(1,0,0);
		zh.guiyihua();
		//cout<<"sin="<<si<<"\ncos="<<co<<"\nzh="<<zh<<endl;
		Rx.x=co+(1-co)*zh.x*zh.x;
		Rx.y=(1-co)*zh.x*zh.y-si*zh.z;
		Rx.z=(1-co)*zh.x*zh.z+si*zh.y;
		Ry.x=(1-co)*zh.x*zh.y+si*zh.z;
		Ry.y=co+(1-co)*zh.y*zh.y;
		Ry.z=(1-co)*zh.z*zh.y-si*zh.x;
		Rz.x=(1-co)*zh.x*zh.z-si*zh.y;
		Rz.y=(1-co)*zh.z*zh.y+si*zh.x;
		Rz.z=co+(1-co)*zh.z*zh.z;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				tmp.x=f[i].vertex[j]/Rx;
				tmp.y=f[i].vertex[j]/Ry;
				tmp.z=f[i].vertex[j]/Rz;
				f[i].vertex[j]=tmp;
			}
		}
		calc();
		//standardize();
	}
	void create_ball(float r,int xf){//半径 细分
		if(r<0)r=-r;
		if(xf<2)xf=2;
		int loc=0;
		number=xf*2*(xf*2-2);
		space_apply();
		float *zcos;
		float *zsin;
		vec3d *points;
		points=new vec3d[xf*2*(xf+1)];
		zcos=new float[xf+1];
		zsin=new float[xf+1];
		for(int i=xf;i>=0;i--)
		{
			zcos[i]=cos(3.1415926535897932384626*i/xf);
			zsin[i]=sin(3.1415926535897932384626*i/xf);
		}
		for(int j=xf;j>=0;j--)
		{
			for(int i=xf*2-1;i>=0;i--)
			{
				points[xf*2*j+i].x=zsin[j]*cos(3.1415926535897932384626*(i+j*0.5)/xf);
				points[xf*2*j+i].y=zsin[j]*sin(3.1415926535897932384626*(i+j*0.5)/xf);
				points[xf*2*j+i].z=zcos[j];
			}
		}
		for(int i=xf*2-1;i>=0;i--)
		{
			f[loc].vertex[0]=vec3d(0,0,1);
			f[loc].vertex[1]=points[xf*2+(i%(xf*2))];
			f[loc].vertex[2]=points[xf*2+((i+1)%(xf*2))];
			loc++;
			f[loc].vertex[0]=vec3d(0,0,-1);
			f[loc].vertex[1]=points[xf*2*(xf-1)+(i%(xf*2))];
			f[loc].vertex[2]=points[xf*2*(xf-1)+((i+1)%(xf*2))];
			loc++;
		}
		for(int j=xf-2;j>0;j--)
		{
			for(int i=xf*2-1;i>=0;i--)
			{
				f[loc].vertex[0]=points[j*xf*2+(i%(xf*2))];
				f[loc].vertex[1]=points[j*xf*2+((i+1)%(xf*2))];
				f[loc].vertex[2]=points[(j+1)*xf*2+(i%(xf*2))];
				loc++;
				f[loc].vertex[0]=points[(j+1)*xf*2+((i+1)%(xf*2))];
				f[loc].vertex[1]=f[loc-1].vertex[1];
				f[loc].vertex[2]=f[loc-1].vertex[2];
				loc++;
			}
		}
		calc();
		fix();
		//standardize();
		resize(r);
		delete[] zcos;
		delete[] zsin;
		delete[] points;
	}
	void create_cube(float r){//side_length
		number=12;
		r/=2;
		space_apply();
		f[0].vertex[0]=vec3d(-r,-r,-r);
		f[0].vertex[1]=vec3d(r,-r,-r);
		f[0].vertex[2]=vec3d(r,r,-r);
		f[1].vertex[0]=vec3d(r,r,-r);
		f[1].vertex[1]=vec3d(-r,-r,-r);
		f[1].vertex[2]=vec3d(-r,r,-r);
		
		f[2].vertex[0]=vec3d(-r,-r,r);
		f[2].vertex[1]=vec3d(r,-r,r);
		f[2].vertex[2]=vec3d(r,r,r);
		f[3].vertex[0]=vec3d(r,r,r);
		f[3].vertex[1]=vec3d(-r,-r,r);
		f[3].vertex[2]=vec3d(-r,r,r);
		
		f[4].vertex[0]=vec3d(-r,-r,-r);
		f[4].vertex[1]=vec3d(-r,r,-r);
		f[4].vertex[2]=vec3d(-r,-r,r);
		f[5].vertex[0]=vec3d(-r,r,-r);
		f[5].vertex[1]=vec3d(-r,-r,r);
		f[5].vertex[2]=vec3d(-r,r,r);
		
		f[6].vertex[0]=vec3d(r,-r,-r);
		f[6].vertex[1]=vec3d(r,r,-r);
		f[6].vertex[2]=vec3d(r,-r,r);
		f[7].vertex[0]=vec3d(r,r,-r);
		f[7].vertex[1]=vec3d(r,-r,r);
		f[7].vertex[2]=vec3d(r,r,r);
		
		f[8].vertex[0]=vec3d(-r,-r,-r);
		f[8].vertex[1]=vec3d(r,-r,-r);
		f[8].vertex[2]=vec3d(-r,-r,r);
		f[9].vertex[0]=vec3d(-r,-r,r);
		f[9].vertex[1]=vec3d(r,-r,r);
		f[9].vertex[2]=vec3d(r,-r,-r);
		
		f[10].vertex[0]=vec3d(-r,r,-r);
		f[10].vertex[1]=vec3d(r,r,-r);
		f[10].vertex[2]=vec3d(-r,r,r);
		f[11].vertex[0]=vec3d(-r,r,r);
		f[11].vertex[1]=vec3d(r,r,r);
		f[11].vertex[2]=vec3d(r,r,-r);
		
		calc();
		fix();
		//standardize();
	}
	void create_tetrahedron(double le){//4
		le=le/sqrt(8);
		space_apply(4);
		vec3d points[4];
		points[0]=vec3d(le,le,le);
		points[1]=vec3d(-le,-le,le);
		points[2]=vec3d(-le,le,-le);
		points[3]=vec3d(le,-le,-le);
		int faces[4][3]={
			1,2,3,
			0,2,3,
			0,1,3,
			0,1,2
		};
		for(int i=0;i<number;i++)
		{
			f[i].vertex[0]=points[faces[i][0]];
			f[i].vertex[1]=points[faces[i][1]];
			f[i].vertex[2]=points[faces[i][2]];
			f[i].calc();
		}
		fix();
		calc();
	}
	void create_hexahedron(double le){//6
		create_cube((float)le);
	}
	void create_octahedron(double le){//8
		le=le/sqrt(2);
		space_apply(8);
		vec3d points[6];
		points[0]=vec3d(le,0,0);
		points[1]=vec3d(-le,0,0);
		points[2]=vec3d(0,le,0);
		points[3]=vec3d(0,-le,0);
		points[4]=vec3d(0,0,le);
		points[5]=vec3d(0,0,-le);
		int faces[8][3]={
			0,2,4,
			0,2,5,
			0,3,4,
			0,3,5,
			1,2,4,
			1,2,5,
			1,3,4,
			1,3,5
		};
		for(int i=0;i<number;i++)
		{
			f[i].vertex[0]=points[faces[i][0]];
			f[i].vertex[1]=points[faces[i][1]];
			f[i].vertex[2]=points[faces[i][2]];
			f[i].calc();
		}
		fix();
		calc();
		//face_to_ground(0);
	}
	void create_dodecahedron(double le){//12
		space_apply(36);
		le=le/2;
		double dd=(sqrt(9+4*sqrt(5))+1)/2*le,ll=sqrt((7+2*sqrt(5)+sqrt(9+4*sqrt(5)))/6)*le;
		vec3d points[20];
		points[0]=vec3d(dd,0,le);
		points[1]=vec3d(dd,0,-le);
		points[2]=vec3d(-dd,0,le);
		points[3]=vec3d(-dd,0,-le);
		points[4]=vec3d(le,dd,0);
		points[5]=vec3d(-le,dd,0);
		points[6]=vec3d(le,-dd,0);
		points[7]=vec3d(-le,-dd,0);
		points[8]=vec3d(0,-le,dd);
		points[9]=vec3d(0,le,dd);
		points[10]=vec3d(0,-le,-dd);
		points[11]=vec3d(0,le,-dd);
		points[12]=vec3d(ll,ll,ll);
		points[13]=vec3d(-ll,ll,ll);
		points[14]=vec3d(-ll,-ll,ll);
		points[15]=vec3d(ll,-ll,ll);
		points[16]=vec3d(ll,ll,-ll);
		points[17]=vec3d(-ll,ll,-ll);
		points[18]=vec3d(-ll,-ll,-ll);
		points[19]=vec3d(ll,-ll,-ll);
		int faces[12][5]={
			0,12,9,8,15,
			2,14,8,9,13,
			1,19,10,11,16,
			3,17,11,10,18,
			6,19,1,0,15,
			4,12,0,1,16,
			7,18,3,2,14,
			5,17,3,2,13,
			8,14,7,6,15,
			10,18,7,6,19,
			11,17,5,4,16,
			9,12,4,5,13
		};
		for(int i=0;i<12;i++)
		{
			f[i*3].vertex[0]=points[faces[i][0]];
			f[i*3].vertex[1]=points[faces[i][1]];
			f[i*3].vertex[2]=points[faces[i][2]];
			f[i*3+1].vertex[0]=points[faces[i][0]];
			f[i*3+1].vertex[1]=points[faces[i][2]];
			f[i*3+1].vertex[2]=points[faces[i][3]];
			f[i*3+2].vertex[0]=points[faces[i][0]];
			f[i*3+2].vertex[1]=points[faces[i][3]];
			f[i*3+2].vertex[2]=points[faces[i][4]];
		}
		calc();
		fix();
	}
	void create_icosahedron(double le){//20mt
		number=20;
		le=le/2;
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
		int faces[20][3]={
			0,1,4,
			0,1,6,
			2,3,7,
			2,3,5,
			4,5,8,
			4,5,10,
			6,7,9,
			6,7,11,
			8,9,2,
			8,9,0,
			10,11,1,
			10,11,3,
			4,0,8,
			6,0,9,
			2,7,9,
			2,5,8,
			1,4,10,
			1,6,11,
			3,7,11,
			3,5,10
		};
		space_apply();
		for(int i=0;i<number;i++)
		{
			f[i].vertex[0]=points[faces[i][0]];
			f[i].vertex[1]=points[faces[i][1]];
			f[i].vertex[2]=points[faces[i][2]];
			f[i].calc();
		}
		fix();
		calc();
		//face_to_ground(0);
		//rotate(vec3d(0,1,0),Pi*31.71748/180);//点朝上 
		//rotate(vec3d(0,1,0),Pi*69.09484/180);//面朝上 
	}
	void create_regular_polyhedron(int faces_number,double length){
		switch(faces_number)
		{
			case 4:create_tetrahedron(length);break;
			case 6:create_hexahedron(length);break;
			case 8:create_octahedron(length);break;
			case 12:create_dodecahedron(length);break;
			case 20:create_icosahedron(length);break;
			default:break;
		}
	}
	void squeeze(vec3d newx, vec3d newy, vec3d newz)
	{
		vec3d ans;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				ans.x=f[i].vertex[j].x*newx.x+f[i].vertex[j].y*newy.x+f[i].vertex[j].z*newz.x;
				ans.y=f[i].vertex[j].x*newx.y+f[i].vertex[j].y*newy.y+f[i].vertex[j].z*newz.y;
				ans.z=f[i].vertex[j].x*newx.z+f[i].vertex[j].y*newy.z+f[i].vertex[j].z*newz.z;
				f[i].vertex[j]=ans;
			}
			f[i].calc();
		}
	}
	void foces_extend(float dis)
	{
		vec3d ans;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				ans.z=f[i].vertex[j].z;
				ans.x=f[i].vertex[j].x*dis/(dis-ans.z);
				ans.y=f[i].vertex[j].y*dis/(dis-ans.z);
				f[i].vertex[j]=ans;
			}
			f[i].calc();
		}
	}
	void set_to_c1(float biasx, float dis, float bs)
	{
		vec3d ans;
		for(int i=0;i<number;i++)
		{
			for(int j=0;j<3;j++)
			{
				ans.x=(f[i].vertex[j].x+f[i].vertex[j].z*biasx)*dis/(dis-f[i].vertex[j].z)*bs;
				ans.y=f[i].vertex[j].y*dis/(dis-f[i].vertex[j].z)*bs;
				ans.z=f[i].vertex[j].z*bs;
				f[i].vertex[j]=ans;
			}
			f[i].calc();
		}
	}
};
