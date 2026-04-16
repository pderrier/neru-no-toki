//NERU NO TOKI (c) ZeN 1999

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#include "midas\midas.h"
#include "ptc\ptc.h"

PTC ptc(320,240,0,0);
#include "include\nnt.h"

int main(int argc, char *argv[])
{
argc=argc;
argv=argv;

//***************************************************************************
//MIDAS STARTUP
//***************************************************************************
  MIDASstartup();
  if(!MIDAS_Init()) return 0;

//***************************************************************************
// ZEN START SYSTEM
//***************************************************************************

//ALLOCATION
//==========
printf("\nþ Allocation\n");
//CHAOS
    fond =  (structRGB *)malloc(sizeof(structRGB)*3064*200);
    if(fond==NULL) {TERMINE}
     printf("  - CHAOS : fond (structRGB 3064*200)\n");
    bump = (structRGB *)malloc(sizeof(structRGB)*960*200);
    if(bump==NULL) {TERMINE}
     printf("  - CHAOS : bump (structRGB 960*200)\n");
    img2=(structRGB*)malloc(sizeof(structRGB)*320*705);
    if(img2==NULL) {TERMINE}
     printf("  - CHAOS : img2 (structRGB 320*705)\n");
    spritessalete=(unsigned char*)malloc(128*16*4);
    if(spritessalete==NULL) {TERMINE}
     printf("  - CHAOS : spritessalete (UCHAR 4*128*16)\n");
//NNTFX1
    char *nomfich[]={"data\\nnt_brui.pcx","data\\nnt_poll.pcx","data\\nnt_hain.pcx","data\\nnt_stre.pcx","data\\nnt_conn.pcx"};

    img=(structRGB*)malloc(sizeof(structRGB)*320*384);
    if(img==NULL) {TERMINE}
     printf("  - NNTFX1 : img (structRGB 320*384)\n");
    for(int x=0;x<5;x++) {photo[x]=(unsigned char*)malloc(4*320*240);if(photo[x]==NULL) {TERMINE}}
     printf("  - NNTFX1 : photos (4*structRGB 320*240)\n");
    tmpbuf=(structRGB*)malloc(sizeof(structRGB)*320*240);
    if(tmpbuf==NULL) {TERMINE}
     printf("  - NNTFX1 : tmpbuf (structRGB 320*240)\n");
//RADIAL
    radialscr=(unsigned char*) malloc(320*240*4);
    if(radialscr==NULL) {TERMINE}
     printf("  - RADIAL : radialscr (UCHAR 4*320*240)\n");
//FX2D
    tex = (structRGB *)malloc(256*256*sizeof(structRGB));
    if(tex==NULL) {TERMINE}
     printf("  - FX2D : tex (structRGB 256*256)\n");
    for(i=0;i<3;i++)
     { texte[i]=(structRGB *)malloc(320*240*sizeof(structRGB));
       if(texte[i]==NULL) {TERMINE}
     }
     printf("  - FX2D : PoŠmes (3*structRGB 320*240)\n");
//KATA
    s320240 = (structRGB *)malloc(320*240*sizeof(structRGB));
    if(s320240==NULL) {TERMINE}
     printf("  - KATA : s320240s (structRGB 320*240)\n");
    anim=(structRGB *)malloc(224*352*sizeof(structRGB));
    if(anim==NULL) {TERMINE}
     printf("  - KATA : anim (structRGB 224*352)\n");

// LOADING
//=========
printf("\nþ Loading\n");
//CHAOS
    if(loadpcx("data\\ch_paysa.pcx",fond,3064,200)!=0) {TERMINE}
     printf("  - CHAOS : Paysage Chaos\n");
    if(loadpcx("data\\ch_decal.pcx",bump,960,200)!=0) {TERMINE}
     printf("  - CHAOS :  Brume\n");
    if(loadpcx("data\\ch_texte.pcx",img2,320,705)!=0) {TERMINE}
     printf("  - CHAOS :  Cr‚dits\n");
    if(loadpcxUCHAR("data\\ch_sale.pcx",spritessalete,128,16)!=0) {TERMINE}
     printf("  - CHAOS :  Salet‚s\n");
//NNTFX1
    if(loadpcx("data\\nnt_text.pcx",img,320,384)!=0) {TERMINE}
     printf("  - NNTFX1 : Textes\n");
    for(x=0;x<5;x++) loadpcxUCHAR(nomfich[x],photo[x],320,240);
     printf("  - NNTFX1 : Photos\n");
//FX2D
    if(loadpcx("data\\fx2d.pcx",tex,256,256)!=0) {TERMINE}
     printf("  - FX2D : Texture\n");
    if(loadpcx("data\\poeme.pcx",texte[0],320,240)!=0) {TERMINE}
     printf("  - FX2D : PoŠme 1\n");
    if(loadpcx("data\\poeme2.pcx",texte[1],320,240)!=0) {TERMINE}
     printf("  - FX2D : PoŠme 2\n");
    if(loadpcx("data\\poeme3.pcx",texte[2],320,240)!=0) {TERMINE}
     printf("  - FX2D : PoŠme 3\n");
//KATA
    if(loadpcx("data\\kat_fond.pcx",s320240,320,240)!=0) {TERMINE}
     printf("  - KATA : Fond\n");
    if(loadpcx("data\\kat_anim.pcx",anim,224,352)!=0) {TERMINE}
     printf("  - KATA : Anim\n");
//MUSIQUE
    Load_Music("data\\exel.xm");
     printf("  - Musique EXEL\n");

// INITIALISATIONS / PRECALCULS
//==============================
printf("\nþ Initialising\n");
//CHAOS
    for(x=0;x<320*705;x++)
    {
     if(img2[x].R<img2[x].G)img2[x].R=img2[x].G;
     if(img2[x].R<img2[x].B)img2[x].R=img2[x].B;
    }
    for(x=0;x<256;x++) _sin[x]=(int)(sin(x*PI/4)*3.9)+4;
     printf("  - CHAOS Init (img2)\n");
//NNTFX1
    for(x=0;x<320*384;x++)
    {
     if(img[x].R<img[x].G)img[x].R=img[x].G;
     if(img[x].R<img[x].B)img[x].R=img[x].B;
    }
     printf("  - NNTFX1 Init (img)\n");
    memset(tmpbuf,0,sizeof(structRGB)*320*240);
     printf("  - NNTFX1 Init (tmpbuf)\n");
//RADIAL
    map=(structRGB *)malloc(320*240*sizeof(structRGB));
    if(map==NULL) {TERMINE}
    if(loadpcx("data\\transit1.pcx",map,320,240)!=0) {TERMINE}
    for(i=0;i<320*240;i++)
    {
     radialscr[i*4]=map[i].R;
     radialscr[i*4+1]=map[i].G;
     radialscr[i*4+2]=map[i].B;
    }
    free(map);

    int xo,yo;
    int offset,dy,dx,angle;
    offset=0;
    for ( angle=0; angle<512;angle++ )
	for ( dx=-320; dx<320; dx++ )
	{
	dxcosine[offset]=(int) ((dx-0.01)*cos(angle*2*pi/nbrangletab));
	dxsine[offset]=(int) ((dx-0.01)*sin(angle*2*pi/nbrangletab));
	offset++;
	}
    offset=0;
    for ( angle=0; angle<512;angle++ )
	for ( dy=-240; dy<240; dy++ )
	{
	dycosine[offset]=(int) ((dy-0.01)*cos(angle*2*pi/nbrangletab));
	dysine[offset]=(int) ((dy-0.01)*sin(angle*2*pi/nbrangletab));
	offset++;
	}
    printf("  - RADIAL Init (radialscr)\n");
//FX2D
    for(y=-240;y<240;y++)
    {
     for(x=-320;x<320;x++)
     {
      a=atan2(y,x);
      a2=a*256/PI;
      tatan2[(y+240)*640+x+320]=a2;
      z=sqrt((double)x*(double)x+(double)y*(double)y);
      prof[(y+240)*640+x+320]=z;
     }
    }
    for(x=0;x<256;x++)
    {
     _cos1[x]=10*cos((x<<1)*PI/128)+80;
     _sin1[x]=50*sin((x<<1)*PI/128)+75;
     _cos2[x]=70*cos((x)*PI/128)+80;
     _sin2[x]=30*sin((x+10)*PI/128)+175;
     _cos3[x]=70*cos((x+220)*PI/128)+140;
     _sin3[x]=2*sin((x+50)*PI/128)+190;
     _sin4[x]=sin(x*PI/128)+17.0;
    }
     printf("  - FX2D Init (tables)\n");
//3D
    Init_3d_Part1();
      printf("  - 3D PART 1 Init\n");
//RAY
    if(!RAYCAST_Init()) {TERMINE}
      printf("  - Raycast Init\n");
//PTC
   if (!ptc.ok()) if (!ptc.Init(320,240)) {TERMINE}
   Surface surface(ptc,320,240,ARGB8888);
   vbuffer=(unsigned int *)surface.Lock();
   if(vbuffer==NULL) {TERMINE}

//KATA
   for(i=0;i<360;i++) _sinkata[i]=(sin(double(i)*PI/180)*2.0);
   for(i=0;i<64;i++) _sinkata2[i]=(sin(double(i)*20/PI))*1.8;
   for(i=0;i<241;i++) t320[i]=i*320;
   InitMeta(nbmeta,surface);

__asm {
	mov	ax,0c00h
	int 21h
}
//***************************************************************************
//DEBUT DE LA DEMO
//***************************************************************************

memset(vbuffer,0,320*240*4);
Start_Music();
///*
//NNTFX1
//======

	  while((syncc=Synchro())<5 &&  inp(0x60)!=1)
	   {
	    iDecalageTexte=320*32+(320*64)*syncc;
		///copie du fond decale dans tmpbuf
		randfondX=rand()%11;
		randfondY=rand()%11;

		//Cr‚e texte avec flou vertical random
		int col;
		iNbPixFlou=(rand()%10); //entre 0 et 9 pixel de flou
		iDiviseur=iNbPixFlou*2+1;

		//Precalcul des divisions
		for(tmp=0;tmp<256;tmp++)
		{
		 for(i=0;i<iDiviseur;i++)
		  iDiv[tmp*iDiviseur+i]=tmp;
		}
		//Calcul du flou directionnel
		for(x=0;x<320;x++)
		{
		 ofs=x;
		 col=0;
		 for(tmp=-iNbPixFlou;tmp<=iNbPixFlou;tmp++)
		  col+=(int)img[ofs+tmp*320+iDecalageTexte].R;
		 for(y=0;y<64;y++)
		 {
		  tmpbuf[ofs+(120-32)*320].R=iDiv[col];
		  col+=(int)img[ofs+iNbPixFlou*320+iDecalageTexte].R-(int)img[ofs-iNbPixFlou*320+iDecalageTexte].R;
		  ofs+=320;
		 }
		}

		for(y=0;y<240-10;y++)
		 for(x=0;x<320-10;x++)
		  vbuffer[x+y*320+randfondX+randfondY*320]=photo[syncc][(x+y*320)*4];

		for(y=0;y<240-10;y++)
		 for(x=0;x<320-10;x++)
		 {
		  ofs=x+y*320;
		  if(tmpbuf[ofs].R==0)
		   vbuffer[ofs]=RGB32(photo[syncc][(ofs+randfondX+randfondY*320)*4],photo[syncc][(ofs+randfondX+randfondY*320)*4+1],photo[syncc][(ofs+randfondX+randfondY*320)*4+2]);
		  else
		  {
		   R=photo[syncc][(ofs+randfondX+randfondY*320)*4];
		   G=photo[syncc][(ofs+randfondX+randfondY*320)*4+1];
		   B=photo[syncc][(ofs+randfondX+randfondY*320)*4+2];
		   R+=tmpbuf[ofs].R;
		   G+=tmpbuf[ofs].R;
		   B+=tmpbuf[ofs].R;
		   if(R>255)R=255;if(G>255)G=255;if(B>255)B=255;
		   vbuffer[ofs]=RGB32(R,G,B);
		  }
		}
		surface.Update();
	    }
//*/
  free(img);
  for(x=0;x<5;x++) free(photo[x]);

__asm {
	mov	ax,0c00h
	int 21h
}



//RADIAL
//=======
memset(vbuffer,0,320*240*4);
///*
	   frames=0;
	   v=128;
	   while((v>1)&&inp(0x60)!=1)
	   {
	    v--;
	    Set_Volume(int(v/2));

	    xo=(int)(160+sin(frames*pi/50)*20+cos(frames*pi/40)*40);
	    yo=(int)(130+sin(frames*pi/60)*15+cos(frames*pi/70)*15);
	    for(i=0;i<320*240;i++)
	     vbuffer[i]=RGB32(radialscr[i*4],radialscr[i*4+1],radialscr[i*4+2]);
	    RADIAL_Go (50,82,220,64,40,80,xo,yo);
	    surface.Update();
	    frames++;
	   }

   free(radialscr);
//*/

Stop_Music();
Free_Music();
Load_Music("data\\alma_del.xm");
Start_Music();
Set_Volume(64);
__asm {
	mov	ax,0c00h
	int 21h
}
memset(vbuffer,0,320*240*4);

//FX2D
//====
///*
	  i=0;compt=0;

	    NUMPOEME=0;
	    while(inp(0x60)!=1 && ((syncc=Synchro())<3))
	    {
	     NUMPOEME=syncc;
	     if(NUMPOEME<0) NUMPOEME=0;
	     if(NUMPOEME>2) NUMPOEME=2;
	     Do_FX2D();
	     surface.Update();

	    }
//*/
	free(tex);
	for(i=0;i<3;i++) free(texte[i]);


//KATA
//=====
///*
		addXMETA=-10;
		addYMETA=40;
		vaguec=0;
		ofsanim=0;
		frame=-200;

		while((Synchro()<5) && inp(0x60)!=1)
		{
		 if(Synchro()<4) Do_Kata(surface,0);
		 else
		  Do_Kata(surface,1);
		 frame++;
		 surface.Update();
		}

//*/
	free(s320240);
	free(anim);

__asm {
	mov	ax,0c00h
	int 21h
}
memset(vbuffer,0,320*240*4);

//CHAOS
//=====
///*
 memset(tmpbuf,0,320*240*4);
 compteur=0;
 Tpos=20*320;
 v=0;
 posx=posx2=posx3=0;


	while(((syncc=Synchro())<13) && inp(0x60)!=1)
	  {
	   Do_Chaos();
	   SALETE_Go();
	   surface.Update();
	   iDecalageTexte=320*64*(syncc-4);
	  }
//*/
  free(fond);
  free(spritessalete);
  free(bump);
  free(img2);
  free(tmpbuf);
__asm {
	mov	ax,0c00h
	int 21h
}
memset(vbuffer,0,320*240*4);
//RAYCAST
//=======
///*
  frames=0;
  int a;
  while(((syncc=Synchro())<0x20) && inp(0x60)!=1)
   {
    RAYCAST_Go(syncc);
    frames++;
    surface.Update();
   }

  RAYCAST_Close();


//PARTIE 3D
//=========
frame=Make_3d_Part1(surface);

//***************************************************************************
// FIN DE LA DEMO
//***************************************************************************
  ptc.Close();
  MIDAS_Close();

  printf("\t                     NERU NO TOKI\n");
  printf("\t            \"Le moment o— l'on se repose\"\n");
  printf("\t                        ZeN 1999\n");
  printf("\n\nPre-release version, pas fini enkore... :)\n");

return 0;
}

