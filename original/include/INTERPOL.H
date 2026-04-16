//LA VERSION ASM EST DESSOUS
void Interpole(unsigned int destx,unsigned int desty,int xg,int yg,int lg,int xd,int yd,int ld,int x3,int y3,int l3,int x4,int y4,int l4)
{
//  1ÚÄ¿2
//  3ÀÄÙ4

        int ix,iy;//interpolant x et y
        int xfin,yfin,lfin,xfinpente,yfinpente,lfinpente;
        int ygpente,ydpente,xgpente,xdpente,lgpente,ldpente;
        int ofsdest;
		int decafond;
		decafond=(int)(sin(frames*pi/20)*15);
		decafond=0;

        if( (lg>=256*65536) && (ld>=256*65536) && (l3>=256*65536) && (l4>=256*65536) )//si lumière toute basse
        {
        ofsdest=destx+desty*320;
        for(iy=0;iy<8;iy++)
        {
            for(ix=0;ix<8;ix++)
            {
				if((ofsdest+decafond>=0)&&(ofsdest+decafond<64000))
					vbuffer[ofsdest+320*20]=RGB32(ray_fond[ofsdest+decafond].R,ray_fond[ofsdest+decafond].G,ray_fond[ofsdest+decafond].B);
                ofsdest++;
            }
            ofsdest+=312;
        }
         goto endinterpole;
        }        

        ygpente=(y3-yg)>>3;ydpente=(y4-yd)>>3;
        xgpente=(x3-xg)>>3;xdpente=(x4-xd)>>3;
        lgpente=(l3-lg)>>3;ldpente=(l4-ld)>>3;

        ofsdest=destx+desty*320;
      //Interpole les quatre parametres (=coord text)
        for(iy=0;iy<8;iy++)
        {
            yg+=ygpente;yd+=ydpente;
            xg+=xgpente;xd+=xdpente;
            lg+=lgpente;ld+=ldpente;

            xfin=xg;
            xfinpente=(xd-xg)>>3;
            yfin=yg;
            yfinpente=(yd-yg)>>3;
	    lfin=lg;
	    lfinpente=(ld-lg)>>3;
	    for(ix=0;ix<8;ix++)
	    {
	     if((ofsdest+decafond>=0)&&(ofsdest+decafond<64000))
	     if(ray_fond[ofsdest+decafond].R==0&&ray_fond[ofsdest+decafond].G==0&&ray_fond[ofsdest+decafond].B==0)
	      {
		int ofstex=((xfin>>16)&255)+((yfin>>8)&(65280));
		int R=textureraycast[ofstex*4+0];
		int G=textureraycast[ofstex*4+1];
		int B=textureraycast[ofstex*4+2];
		R-=(lfin>>15);G-=(lfin>>15);B-=(lfin>>15);
		if(lfin>0){if(R<0)R=0;if(G<0)G=0;if(B<0)B=0;}		//Teste que celui qui est probable entre >0 et <0
		else {if(R>255)R=255;if(G>255)G=255;if(B>255)B=255;}
		vbuffer[ofsdest+320*20]=RGB32(R,G,B);
	      }
	      else
	      {
				  if((ofsdest+decafond>=0)&&(ofsdest+decafond<64000))
					vbuffer[ofsdest+320*20]=RGB32(ray_fond[ofsdest+decafond].R,ray_fond[ofsdest+decafond].G,ray_fond[ofsdest+decafond].B);
              }
                xfin+=xfinpente;
                yfin+=yfinpente;
                lfin+=lfinpente;
                ofsdest++;
            }
            ofsdest+=312;
        }
        endinterpole:;
}

