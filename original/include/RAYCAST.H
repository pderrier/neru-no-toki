#include "include\interpol.h"

void RAYCAST_Close(void)
{
	free(ray_fond);
	free(textureraycast);
	free(map1);
}

void Rotate(structray* ray,float angx,float angy,float angz)
{
	float mx,my,mz;

	my=+ray->y*cos(angx)-ray->z*sin(angx);
	mz=+ray->y*sin(angx)+ray->z*cos(angx);
	ray->y=my;
	ray->z=mz;
	mx=+ray->x*cos(angy)+ray->z*sin(angy);
	mz=-ray->x*sin(angy)+ray->z*cos(angy);
	ray->x=mx;
	ray->z=mz;
	mx=ray->x*cos(angz)-ray->y*sin(angz);
	my=ray->x*sin(angz)+ray->y*cos(angz);
	ray->x=mx;
	ray->y=my;
}


int RAYCAST_Init(void)
{
	tmpraycast=(structRGB*)malloc(sizeof(structRGB)*512*512);
	if(tmpraycast==NULL) return 0;
	map1=(char *)malloc(512*512); //map de Z
	if(map1==NULL) return 0;
	if(loadpcx("data\\ray_map1.pcx",tmpraycast,512,512)!=0) return 0;
	for(x=0;x<512*512;x++)
		map1[x]=tmpraycast[x].R;
	free(tmpraycast);
	textureraycast=(unsigned char*)malloc(256*256*4);
	if(textureraycast==NULL) return 0;
	if(loadpcxUCHAR("data\\ray_text.pcx",textureraycast,256,256)!=0) return 0;

	ray_fond=(structRGB*)malloc(sizeof(structRGB)*320*200);
	if(ray_fond==NULL) return 0;
	if(loadpcx("data\\ray_fond.pcx",ray_fond,320,200)!=0) return 0;
//        for(x=0;x<320*200;x++)
//		ray_fond[x].RGB=ray_fond[x].B+(ray_fond[x].G<<8)+(ray_fond[x].R<<16);
	for(y=0;y<25;y++)
	for(x=0;x<40;x++)
	{
	    //vecteur direction
	    dlong.x=x*8-160;
	    dlong.y=y*8-100;
	    dlong.z=128;
	    //on normalise le vecteur
	    float lengthd=sqrt(dlong.x*dlong.x+dlong.y*dlong.y+dlong.z*dlong.z);
	    d.x=dlong.x/lengthd;
	    d.y=dlong.y/lengthd;
	    d.z=dlong.z/lengthd;
	    precaldir[x+y*40]=d;
	}

	o.z=-128;
	o.x=o.y=0;
	Effect1=2;
	Effect2=2;
	EffectTransition=0.0;
return 1;
}


structintray RayCastSpheres(int x,int y)
{
        structintray fin;
        float length,tmplength;
        float t;

        length=0;

        d=precaldir[x+y*40];
        Rotate(&d,plangx/2,plangy/2,plangz/2);

        t=(700+pldecaz/2)/(d.z); //calcule l'intersection avec le plan
        in.x=o.x+t*d.x;
        in.y=o.y+t*d.y;

        for(x=0;x<4;x++)
        {
	    tmplength=sqrt((in.x-pzo_sphere[x].x)*(in.x-pzo_sphere[x].x)+(in.y-pzo_sphere[x].y)*(in.y-pzo_sphere[x].y));
            tmplength=450-tmplength;
            if(tmplength<0)tmplength=0;
            length+=tmplength;
        }

        in.z=-cos(length*pi/(450*3))*350+350;

        fin.x=(int)((in.x)*256/(in.z+512)+128)*65536+tudecaz*65536;
        fin.y=(int)((in.y)*256/(in.z+512)+128)*65536;
        fin.l=(int)((-in.z/2+100)*65536);

        return fin;
}

structintray RayCastPlanMap(int x,int y)
{
       structintray fin;

       d=precaldir[x+y*40];
       Rotate(&d,plangx,plangy,plangz);

       t=(400+pldecaz)/(d.z); //calcule l'intersection avec le plan
       in.x=o.x+t*d.x;
       in.y=o.y+t*d.y;

       in.z=sin(map1[(((int)(in.x/6)+decamapx)&511)+(((int)(in.y/6)+decamapy)&511)*512]*pi/128+decasin)*30;

       float tmpz;
       float doi=sqrt((in.x-o.x)*(in.x-o.x)+(in.y-o.y)*(in.y-o.y)); //pas de sqrt, c juste pour avoir une approche de la longueur
       if(doi<(dist1))tmpz=0;else
       if(doi>(dist2))tmpz=356;else
       tmpz=(doi-dist1)*356/(dist2-dist1);

       if(t<0)in.z=-1000;
       fin.x=(int)((in.x)*256/(in.z+312)+128)*65536+tudecaz*65536;
       fin.y=(int)((in.y)*256/(in.z+312)+128)*65536;
       fin.l=(int)((tmpz-in.z)*65536);

       return fin;
}


structintray RayCastTunnel(int x,int y)
{
        structintray fin;

        d=precaldir[x+y*40];
        Rotate(&d,tuangx,tuangy,tuangz*2);

        float a=d.x*d.x+d.y*d.y;
        float b=2*(o.x*d.x+o.y*d.y);
        float rayon=(sin(5*atan2(d.x,d.y))*tuqtenoise)+256;
        float c=o.x*o.x+o.y*o.y-rayon*rayon;
        float delta=b*b-4*a*c;
        t=(-b+sqrt(delta))/(2*a);
        in.x=o.x+t*d.x;
        in.y=o.y+t*d.y;
        in.z=o.z+t*d.z;
        fin.x=(int)((in.z)*65536/3)+tudecaz*65536;
        fin.y=(int)abs(atan2(in.y,in.x)*256*65536/pi);
        if(in.z>1024)
           in.z=1024;
        fin.l=(int)((in.z*65536))/10;

        return fin;
}


void RAYCAST_Go(int synch)
{

		decasin=frames*0.2;
		decamapx=frames*3;
		decamapy=frames;
		plangx=sin(frames*pi/110)/2.5;
		plangy=cos(frames*pi/70)/3;
		plangz=(float)(frames)/90;
		pldecaz=sin(frames*pi/100)*200;
		tuangx=plangx;
		tuangy=float(frames)*incre/40;
		if(synch>=18) incre+=(float(frames-900)/100000.0);
		tuangz=plangz;
		tudecaz=(frames*3);
		tuqtenoise=sin(frames*pi/50)*130;
		tuqtenoise2=sin(frames*pi/20)*130;
		pzo_sphere[0].x=cos(frames*pi/70)*(150+200);
		pzo_sphere[0].y=sin(frames*pi/50)*(100+200);
		pzo_sphere[1].x=cos(frames*pi/110)*(250+200);
		pzo_sphere[1].y=sin(frames*pi/60)*(76+200);
		pzo_sphere[2].x=cos(frames*pi/100)*(91+200);
		pzo_sphere[2].y=sin(frames*pi/150)*(199+200);
		pzo_sphere[3].x=sin(frames*pi/100+10)*(61+200);
		pzo_sphere[3].y=cos(frames*pi/60+50)*(109+200);
		if(synch==14){Effect2=3;EffectTransition=0.0;}
		if(synch==16){Effect2=1;EffectTransition=0.0;}

		for(y=0;y<25;y++)
		for(x=0;x<40;x++)
		{
		   if(Effect1==Effect2)
		   {
			switch(Effect1)
			{
				case 1:fin=RayCastTunnel(x,y);break;
				case 2:fin=RayCastPlanMap(x,y);break;
				case 3:fin=RayCastSpheres(x,y);break;
			}
		   }
		   else
		   {
			switch(Effect1)
			{
				case 1:fin1=RayCastTunnel(x,y);break;
				case 2:fin1=RayCastPlanMap(x,y);break;
				case 3:fin1=RayCastSpheres(x,y);break;
			}
			switch(Effect2)
			{
				case 1:fin2=RayCastTunnel(x,y);break;
				case 2:fin2=RayCastPlanMap(x,y);break;
				case 3:fin2=RayCastSpheres(x,y);break;
			}
			fin.x=(int)((fin2.x-fin1.x)*EffectTransition+fin1.x);
			fin.y=(int)((fin2.y-fin1.y)*EffectTransition+fin1.y);
			fin.l=(int)((fin2.l-fin1.l)*EffectTransition+fin1.l);
		   }

		   tmpbufinterpol[x+y*40]=fin;
		}

		for(y=0;y<24;y++)
		    for(x=0;x<39;x++)
		    {
			Interpole(x*8+2,y*8+2,tmpbufinterpol[x+y*40].x,(int)tmpbufinterpol[x+y*40].y,(int)tmpbufinterpol[x+y*40].l,
						  tmpbufinterpol[x+y*40+1].x,(int)tmpbufinterpol[x+y*40+1].y,(int)tmpbufinterpol[x+y*40+1].l,
						  tmpbufinterpol[x+y*40+40].x,(int)tmpbufinterpol[x+y*40+40].y,(int)tmpbufinterpol[x+y*40+40].l,
						  tmpbufinterpol[x+y*40+41].x,(int)tmpbufinterpol[x+y*40+41].y,(int)tmpbufinterpol[x+y*40+41].l);
		    }

		EffectTransition+=0.02;
		if(EffectTransition>1.0)
		{
		       EffectTransition=0.0;
		       Effect1=Effect2;
		}

}

