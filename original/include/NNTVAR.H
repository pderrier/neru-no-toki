//VARIABLES POUR LES EFFETS DE NERU NO TOKI

int v,syncc;
typedef unsigned char UBYTE;
clock_t Tdebut,Tcour,Tend;
int frames;
structRGB *map;
unsigned long lFrames;//,lTimeFin,lTimeDebut;			//Pour calcul de nombre de Frames/sec de fin
int x,y,i,j,ofs;
int R,G,B;
unsigned long Images=0;


//NNTFX
	#define _CENTRE 100*320
	structRGB *img,*tmpbuf;
	unsigned char *photo[6];
	int tmp;
	unsigned int randfondX,randfondY,randflou;
	int iNbPixFlou,iDiviseur;
	unsigned char iDiv[20*256*2+1]; //mettre en memset

//RADIAL
	#define nbrangletab 256
	unsigned char *radialscr;
	int dxcosine[640*512];
	int dxsine[640*512];
	int dycosine[480*512];
	int dysine[480*512];


//FX2D
	structRGB *tex; //texture
	structRGB *cadre;
	structRGB *texte[3];
	int s=0;
	int NUMPOEME=0;
	//tables de pr‚ca
	unsigned char prof[640*480]; //racines carr‚es
	unsigned tatan2[480*640]; //angle
	int _cos1[256]; //tables pour les mouvements
	int _sin1[256];
	int _cos2[256];
	int _sin2[256];
	int _cos3[256];
	int _sin3[256];
	int _sin4[256];
	int pouet;
	unsigned char pal8[768];
	unsigned char *map8;
	UBYTE *ptr;
	//int x,y,r,g,b,i;
	int dyang1,dxang1;
	int dyang2,dxang2;
	int dyang3,dxang3;
	unsigned a2;
	double a,z;
	unsigned xaddi=0,yaddi=0;
	unsigned char ccx=0,ccy=0;
	int decaly=0,decalx=0;
	int totdeca,txtofs,nb=1,compt=0;


//CHAOS
	structRGB *fond;
	structRGB *bump;
	structRGB *img2;
	unsigned posx,posx2,posx3;
	unsigned offset,offset2,offset3;
	int C,C2;
	int _sin[256];
	unsigned compteur;
	int iDecalageTexte;
	unsigned Tpos;
	int nit;


//SALETE
	int NoirEtBlanc;
	unsigned char *spritessalete;


//KATA (+METABALLS)
	long nbmeta=4;
	unsigned t320[241];
	#define NB_IMAGES_ANIM 77
	#include "m3d\bmp.h"
	#include "m3d\variable.h"
	#include "m3d\variabl.h"
	#include "m3d\meta10.h"
	#include "m3d\3ds.h"
	#include "m3d\new12.h"
	structRGB *s320240;
	structRGB *anim;
	int _sinkata[360];
	int _sinkata2[64];
	int vaguec;
	unsigned c=0;
	unsigned sinc=0;
	unsigned meuh=0;
	unsigned ofsanim=0;
	int pas=2;

//RAY
	typedef struct
	{
		float x,y;
	}structsphere;

	typedef struct
	{
	   float x,y,z;
	}structray;

	typedef struct
	{
	   int x,y,l;
	}structintray;
	float incre=1.0;
	structsphere pzo_sphere[4];  //4 spheres
	structray precaldir[40*25];
	structray o,d,in,dlong;  //origin, direction(normalis‚), intersection, direction (PAS normalis‚)
	structintray fin,fin1,fin2;  // fin(coordmapping)
	structintray tmpbufinterpol[40*25]; //buffer avvant interpolation;
	float decasin;
	float plangx,plangy,plangz;
	float tuangx,tuangy,tuangz;
	char* map1; //512*512
	unsigned char* textureraycast; //256*256
	structRGB* ray_fond;    //320*200
	structRGB* tmpraycast;
	float t;
	int decamapx,decamapy;
	#define dist1 1555
	#define dist2 3580
	int Effect1,Effect2;
	float EffectTransition;
	float pldecaz; //pour zoom
	int tudecaz; //mouvements d‚pendants de l'effet
	float tuqtenoise,tuqtenoise2; //noise pour rayon du tunnel
	float spdecax,spdecay; //mouvement de la sphere
