void charplus(char a[],char b[])
{
	int len=-1,bl=-1;
	while(a[++len]);
	while(b[++bl]);
	for(int i=0;i<=bl;i++)a[i+len]=b[i];
}
int charfind(char a[],char b[])
{
	if(strlen(b)==0)return -1;
	bool finded;
	for(int i=0;a[i]!='\0';i++)
	{
		finded=true;
		for(int j=0;b[j]!='\0'&&a[i+j]!='\0';j++)
		{
			if(a[i+j]!=b[j])
			{
				finded=false;
				break;
			}
		}
		if(finded)return i;
	}
	return -1;
}
int charreplace(char a[],char o[],char p[])
{
	int times=0,st=0,loc=0,alen=strlen(a),olen=strlen(o),plen=strlen(p);
	char ls[1000]="\0";
	while(1)
	{
		st=charfind(&a[loc],o);
		if(st==-1)break;
		st=st+loc;
		ls[0]='\0';
		charplus(ls,&a[st+olen]);
		a[st]='\0';
		charplus(a,p);
		charplus(a,ls);
		loc=st+plen;
		times++;
	}
	return times;
}
