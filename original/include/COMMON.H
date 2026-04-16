#define TERMINE {ptc.Close();MIDAS_Close();printf("Abnormal Termination, mon frŠre...\n");exit(1);}
#define PI (float)3.1415926535897932384626433832795
#define pi PI

typedef struct
 {
 unsigned char R,G,B;
 }structRGB;

unsigned int * vbuffer;

//***************************************************************************
//ATTENTE VBL
//***************************************************************************
void VBL(void)
{
__asm {
	mov dx,0x3da
      wait1:
	in al,dx
	test al,0x08
	jnz wait1
      wait2:
	in al,dx
	test al,0x08
	jz wait2
};
}


//***************************************************************************
//CHARGEMENT PCX 24b -> UCHAR
//***************************************************************************
//  Code : Wondy
//  renvoie 0 si OK
//***************************************************************************
int loadpcxUCHAR(char *nom,unsigned char *scr,int tax,int tay)
{
FILE *fi; //pointeur sur fichier
char octet;
int tempr=0,tempv=0,tempb=0;
int dr,dv,db;
int i,j;

if((fi=fopen(nom,"rb"))==NULL)//on ouvre le fichier en mode binaire
 return -1; //si echec on se casse... :)

 for(i=0;i<128;i++)
  octet=fgetc(fi);     //on charge les 128 octets du header

  dr=0;
  db=0;
  dv=0;

BOUCLE_CHARGE:

while(dr<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[(tempr+i)*4+0]=octet;
 tempr+=j;dr+=j;
 }
if(dr>=tax) dr-=tax;


while(dv<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[(tempv+i)*4+1]=octet;
 tempv+=j;dv+=j;
 }
if(dv>=tax) dv-=tax;


while(db<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[(tempb+i)*4+2]=octet;
 tempb+=j;db+=j;
 }
if(db>=tax) db-=tax;

if(tempb+1<tax*tay) goto BOUCLE_CHARGE;

   fclose(fi);
   return 0;
}

//***************************************************************************
//CHARGEMENT PCX 24b -> structRGB
//***************************************************************************
//  Code : Wondy
//  renvoie 0 si OK
//***************************************************************************
int loadpcx(char *nom,structRGB *scr,int tax,int tay)
{
FILE *fi; //pointeur sur fichier
char octet;
int tempr=0,tempv=0,tempb=0;
int dr,dv,db;
int i,j;

if((fi=fopen(nom,"rb"))==NULL)//on ouvre le fichier en mode binaire
 return -1; //si echec on se casse... :)

 for(i=0;i<128;i++)
  octet=fgetc(fi);     //on charge les 128 octets du header

  dr=0;
  db=0;
  dv=0;

BOUCLE_CHARGE:

while(dr<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[tempr+i].R=octet;
 tempr+=j;dr+=j;
 }
if(dr>=tax) dr-=tax;


while(dv<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[tempv+i].G=octet;
 tempv+=j;dv+=j;
 }
if(dv>=tax) dv-=tax;


while(db<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[tempb+i].B=octet;
 tempb+=j;db+=j;
 }
if(db>=tax) db-=tax;

if(tempb+1<tax*tay) goto BOUCLE_CHARGE;

   fclose(fi);
   return 0;
}


//***************************************************************************
//CHARGEMENT PCX 24b -> UCHAR inverse R/B
//***************************************************************************
//  Code : Wondy
//  renvoie 0 si OK
//***************************************************************************
int loadpcxUCHAR2(char *nom,unsigned char *scr,int tax,int tay)
{
FILE *fi; //pointeur sur fichier
char octet;
int tempr=0,tempv=0,tempb=0;
int dr,dv,db;
int i,j;

if((fi=fopen(nom,"rb"))==NULL)//on ouvre le fichier en mode binaire
 return -1; //si echec on se casse... :)

 for(i=0;i<128;i++)
  octet=fgetc(fi);     //on charge les 128 octets du header

  dr=0;
  db=0;
  dv=0;

BOUCLE_CHARGE:

while(dr<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[(tempr+i)*4+2]=octet;
 tempr+=j;dr+=j;
 }
if(dr>=tax) dr-=tax;


while(dv<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[(tempv+i)*4+1]=octet;
 tempv+=j;dv+=j;
 }
if(dv>=tax) dv-=tax;


while(db<tax)
 {
 octet=fgetc(fi);
 j=1;
 if(octet>192)
  {
  j=octet-192;
  octet=fgetc(fi);
  }
 for(i=0;i<j;i++) scr[(tempb+i)*4]=octet;
 tempb+=j;db+=j;
 }
if(db>=tax) db-=tax;

if(tempb+1<tax*tay) goto BOUCLE_CHARGE;

   fclose(fi);
   return 0;
}
