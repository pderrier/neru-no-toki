#include "include\common.h"
#include "include\nntvar.h"

//***************************************************************************
//RAYCASTING
//***************************************************************************
//Code : Prozero
//***************************************************************************
#include "include\raycast.h"

//***************************************************************************
//RADIAL BLUR
//***************************************************************************
// Code : Decibel
//***************************************************************************
void RADIAL_Go (int xspr,int yspr,int taillex,int tailley,int xaff,int yaff,int xo,int yo)
{
int xr,yr;
unsigned int Rr;
char compteur;
int xsprc,ysprc; //x et y courrants du sprite
int offsetprecalcx,offsetprecalcy;
int y320;
int calcoffdeb; //pour l'optim...

// ATTENTION, xr et yr non plus rien … voir avec les x et y...!!!!!
	calcoffdeb=xo*4+yo*1280;
	xo-=320;
	yo-=240;
	y320=320*yaff;
	for ( y=yaff,ysprc=yspr;y<(yaff+tailley);y++,ysprc++ )
	{

		for ( x=xaff,xsprc=xspr;x<(xaff+taillex);x++,xsprc++ )
		{
			Rr=0;
			offsetprecalcx=(xsprc-xo);
			offsetprecalcy=(ysprc-yo);

			for ( compteur=0; compteur<8; compteur++ )
			{
				xr=calcoffdeb+(4*dxcosine[offsetprecalcx]-4*dysine[offsetprecalcy]);


				yr=(1280*dxsine[offsetprecalcx]+1280*dycosine[offsetprecalcy]);
				Rr+=radialscr[xr+yr];
				offsetprecalcx+=640;
				offsetprecalcy+=480;
			}
			Rr>>=3;
			vbuffer[x+y320]=RGB32(Rr, Rr, Rr);
		}
		y320+=320;
	}
}

//***************************************************************************
//ASPIRO
//***************************************************************************
//Code : Wondy
//***************************************************************************
void Do_FX2D()
{
  ccy+=3;
  ccx+=2;
  dyang1=decaly;
  dxang1=decalx;
  dyang2=_cos1[ccy];
  dxang2=_sin1[ccx];
  dyang3=_cos2[ccy];
  dxang3=_sin2[ccy];
  decaly=_cos3[ccy];
  decalx=_sin3[ccx];

  __asm {
   mov eax,decaly
   mov ebx,eax
   shl eax,7
   shl ebx,9
   add eax,ebx
   add eax,decalx
   mov totdeca,eax
   };
  ofs=0;
  unsigned o_ffset=dyang1*640+dxang1;
  unsigned o_ffset2=dyang2*640+dxang2;
  unsigned o_ffset3=dyang3*640+dxang3;

  for(y=0;y<240;y++)
  {
   for(x=0;x<320;x++)
    {
    int zob=ofs+((decaly-140)>>4);
    unsigned char ty=prof[(y<<7)+(y<<9)+x+totdeca];
    txtofs=(ty<<8)+tatan2[o_ffset]-tatan2[o_ffset2]+tatan2[o_ffset3]+xaddi;
      //Affichage du point
     int pouet=vbuffer[ofs];
     R=(tex[txtofs].R+((pouet>>16)&255))>>1;
     G=(tex[txtofs].G+((pouet>>8)&255))>>1;
     B=(tex[txtofs].B+(pouet&255))>>1;
    if(texte[NUMPOEME][zob].R) {R-=texte[NUMPOEME][zob].R;G-=texte[NUMPOEME][zob].G;B-=texte[NUMPOEME][zob].B;
    if(R<0) R=0;    if(G<0) G=0;    if(B<0) B=0;}

    vbuffer[ofs]=RGB32(R,G,B);
    ofs++;
    o_ffset2++;
    o_ffset3++;
    o_ffset++;
    }
    o_ffset+=320;
    o_ffset2+=320;
    o_ffset3+=320;
   }

   yaddi+=_sin4[ccx];
   xaddi+=3;
}

 unsigned long ploup;
//***************************************************************************
//KATA
//***************************************************************************
//Code : Wondy
//***************************************************************************
void Do_Kata(Surface & surface,int dometa)
{
 for(i=0;i<320*120;i++) vbuffer[i]=RGB32(s320240[i].R,s320240[i].G,s320240[i].B);

//Affichage du bonzome en (156,41)
 int offset=0;
 for(y=13440;y<23680;y+=320)
 { // =42*320 =(42+32)*320
  for(x=156;x<188;x++)
   {       //=156+32
   vbuffer[x+y]=RGB32(anim[ofsanim+offset].R,anim[ofsanim+offset].G,anim[ofsanim+offset].B);
   offset++;
   }
  offset+=192;
 }      //=224-32


if (dometa)
{
 MakeMeta(surface);
 addXMETA+=pas;
 if(addXMETA==400) pas=-2;
 if(addXMETA==0) pas=2;
}


//Effet vague sur l'eau
//donn‚es : limite de la mer sur l'image = 120

 int offset2=120*320;
 for(y=120;y<240;y++)
 {
  int val=_sinkata2[(y+vaguec)&63];
  val=(239-y+val)*320;
  for(x=0;x<320;x++)
   {
   //reflet : vbuffer[mirroir_y+d‚calage mer]
   ploup=vbuffer[val+x+_sinkata2[(x+vaguec)&63]];
   vbuffer[offset2]=RGB32(((ploup>>16)&255)>>3,((ploup>>8)&255)>>2,ploup&255);
   offset2++;
   }

 }

 //Animation du bonzome
  if(!dometa)
   {
   if(++meuh==5)
    {
    meuh=0;
    if(c>=6) {c=0;ofsanim+=31*224+32;}
    else {ofsanim+=32;c++;}
    if(ofsanim>=224*352) {ofsanim=0;c=0;}
    if(sinc<358) sinc+=2; else sinc=0;
   }
  }
 else
  {
   if(c>=6) {c=0;ofsanim+=31*224+32;}
    else {ofsanim+=32;c++;}
    if(ofsanim>=224*352) {ofsanim=0;c=0;}
    if(sinc<358) sinc+=2; else sinc=0;
  }
 vaguec=(vaguec+3)&63; //mer*/
}

//***************************************************************************
//CHAOS
//***************************************************************************
//Code : Wondy / Prozero
//***************************************************************************
void Do_Chaos()
{

iNbPixFlou=_sin[compteur];//(rand()%10); //entre 0 et 9 pixel de flou
 //Cr‚e texte avec flou vertical random
 compteur++;
 if(compteur>255) compteur=0;
 int col;
 iDiviseur=iNbPixFlou*2+1;

 //Precalcul des divisions
	for(j=0;j<256;j++)
	  {
	   for(i=0;i<iDiviseur;i++)
		iDiv[j*iDiviseur+i]=j;
	  }
  //Calcul du flou directionnel
	for(int x=0;x<320;x++)
	{
	   int ofs=x;
	   col=0;
	   for(j=-iNbPixFlou;j<=iNbPixFlou;j++)
	   {
		 col+=(int)img2[ofs+j*320+iDecalageTexte].R;
	   }
	   for(int y=0;y<64;y++)
	   {
	     tmpbuf[ofs+Tpos].R=iDiv[col];
	     col+=(int)img2[ofs+iNbPixFlou*320+iDecalageTexte].R-(int)img2[ofs-iNbPixFlou*320+iDecalageTexte].R;
	     ofs+=320;
	   }
	}

  offset=20*320;
  offset2=posx;
  offset3=posx2;
  for(i=20;i<220;i++)
   {
   for(j=0;j<320;j++)
    {
    C=(bump[offset3].R+bump[960*200-offset3-posx3+posx2].R)>>1;
    C2=offset2+(C>>4)-8;
    R=tmpbuf[offset].R+((C+fond[C2].R)>>1);
    G=tmpbuf[offset].R+((C+fond[C2].G)>>1);
    B=tmpbuf[offset].R+((C+fond[C2].B)>>1);
    if(R>255) R=255;
    if(G>255) G=255;
    if(B>255) B=255;
    vbuffer[offset]=RGB32(R,G,B);

    offset++; //‚cran
    offset2++; //paysage
    offset3++; //brume
    }
   offset2+=2744;
   offset3+=640;
   }
  posx+=4;if(posx>2744) posx=0;
  posx2+=6;if(posx2>640) posx2=0;
  posx3+=8;if(posx3>640) posx3=0;
}

//***************************************************************************
//SALETE
//***************************************************************************
//Code : Prozero
//***************************************************************************
void SALETE_Go (void)
{
#define _TOSUB 25
#define _TOSUB2 60
	int x;
	int ofssprite;
	int ofs;

	for(i=0;i<5;i++)
	{
		x=rand()%320;								//Donne la colonne à modifier (random)
		__asm										//Affichage d'une colonne
		{
			mov edi,x
			shl edi,2
			add edi,vbuffer
			mov ecx,240

		LoopYSalete:
			xor eax,eax
			mov ax,[edi]						//Contient G(ah) et B(al)
			mov bx,[edi+2]						//Contient A(bh) et R(bl)
			sub bl,_TOSUB
			jnc okR
			xor bl,bl
		okR:
			sub ah,_TOSUB
			jnc okG
			xor ah,ah
		okG:
			sub al,_TOSUB
			jnc okB
			xor al,al
		okB:
			shl ebx,16
			mov bx,ax
			mov [edi],ebx
			add edi,1280 //320*4

			dec ecx
			jne LoopYSalete
		}
	}
	for(i=0;i<8;i++)
	{
		ofssprite=(rand()%8)*16;					//Contient le decalage pour prendre un sprite au pif
		ofs=(rand()%(320-16)) + (rand()%(240-16))*320;//offset du coin haut gauche du sprite qu'on va coller

		__asm
		{
			mov esi,ofssprite
			shl esi,2
			add esi,spritessalete
			mov edi,ofs
			shl edi,2
			add edi,vbuffer
			mov ch,16
		LoopYSalete2:

			mov cl,16
		LoopXSalete2:
			mov al,[esi]
			or al,al
			je NoModif

			xor eax,eax
			mov ax,[edi]						//Contient G(ah) et B(al)
			mov bx,[edi+2]						//Contient A(bh) et R(bl)
			sub bl,_TOSUB2
			jnc okR2
			xor bl,bl
		okR2:
			sub ah,_TOSUB2
		jnc okG2
			xor ah,ah
		okG2:
			sub al,_TOSUB2
			jnc okB2
			xor al,al
		okB2:
			shl ebx,16
			mov bx,ax
			mov [edi],ebx

		NoModif:

			add edi,4
			add esi,4

			dec cl
			jne LoopXSalete2

			add edi,1216
			add esi,448
			dec ch
			jne LoopYSalete2
		};
	}
}
