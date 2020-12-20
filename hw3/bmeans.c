/**********************************************************************/
/*                                                                    */
/*             File bmeans.c:  Batch Means Analysis Module            */
/*                                                                    */
/*  This is the batch means analysis module of Figure 4.7, extended   */
/*  to produce a trace message at the end of each batch.  This mod-   */
/*  ule is intended to be separately compiled and linked with the ob- */
/*  ject files of the "smpl" and "rand" modules and the object file   */
/*  of the simulation program.  Alternatively, the "smpl", "rand",    */
/*  and "bmeans" object files can be used to construct a "smpl" lib-  */
/*  rary file, and the simulation program object file then linked     */
/*  with that library module.  Note that this file "#include"s the    */
/*  "stat.c" file.                                                    */
/*                                                                    */
/**********************************************************************/
/*  KJC (10/10/95) - Changed <> to "" on include statements           */
/*  KJC (11/06/95) - Added include for math.h                         */
/*  KJC (11/06/95) - Inserted stat.c inline following variable defs.  */
/*                   This done to make UNIX compilers happy.          */
/**********************************************************************/
#include "smpl.h"

static int d,k,m,n;
static real smy,smY,smY2,Y,h;

/*----- stat.c starts here... ----------------------------------------*/

/*--------  COMPUTE pth QUANTILE OF THE NORMAL DISTRIBUTION  ---------*/
real Z(real p)
    { /* This function computes the pth upper quantile of the stand-  */
      /* ard normal distribution (i.e., the value of z for which the  */
      /* are under the curve from z to +infinity is equal to p).  'Z' */
      /* is a transliteration of the 'STDZ' function in Appendix C of */
      /* "Principles of Discrete Event Simulation", G. S. Fishman,    */
      /* Wiley, 1978.   The  approximation used initially appeared in */
      /* in  "Approximations for Digital Computers", C. Hastings, Jr.,*/
      /* Princeton U. Press, 1955. */
      real q,z1,n,d; double sqrt(),log();
      q=(p>0.5)? (1-p):p; z1=sqrt(-2.0*log(q));
      n=(0.010328*z1+0.802853)*z1+2.515517;
      d=((0.001308*z1+0.189269)*z1+1.43278)*z1+1;
      z1-=n/d; if (p>0.5) then z1=-z1;
      return(z1);
    }

/*-----------  COMPUTE pth QUANTILE OF THE t DISTRIBUTION  -----------*/
real T(real p,int ndf)
    { /* This function computes the upper pth quantile of the t dis-  */
      /* tribution (the value of t for which the area under the curve */
      /* from t to +infinity is equal to p).  It is a transliteration */
      /* of the 'STUDTP' function given in Appendix C of  "Principles */
      /* of Discrete Event Simulation", G. S. Fishman, Wiley, 1978.   */
      int i; real z1,z2,h[4],x=0.0; double fabs();
      z1=fabs(Z(p)); z2=z1*z1;
      h[0]=0.25*z1*(z2+1.0); h[1]=0.010416667*z1*((5.0*z2+16.0)*z2+3.0);
      h[2]=0.002604167*z1*(((3.0*z2+19.0)*z2+17.0)*z2-15.0);
      h[3]=0.000010851*z1*((((79.0*z2+776.0)*z2+1482.0)*z2-1920.0)*z2-945.0);
      for (i=3; i>=0; i--) x=(x+h[i])/(real)ndf;
      z1+=x; if (p>0.5) then z1=-z1;
      return(z1);
    }

/*----- stat.c ends here... ------------------------------------------*/

void init_bm(int m0,int mb)
    { /* set deletion amount & batch size */
      d=m0; m=mb; smy=smY=smY2=0.0; k=n=0;
    }

int obs(real y)
    {
      int r=0; real var;
      if (d) then {d--; return(r);}
      smy+=y; n++;
      if (n==m) then
        { /* batch complete:  update sums & counts */
          smy/=n; smY+=smy; smY2+=smy*smy; k++;
          printf("batch %2d mean = %.3f",k,smy);
          smy=0.0; n=0;
          if (k>=10) then
            { /* compute grand mean & half width */
              Y=smY/k; var=(smY2-k*Y*Y)/(k-1);
              h=T(0.025,k-1)*sqrt(var/k);
              printf(", rel. HW = %.3f",h/Y);
              if (h/Y<=0.1) then r=1;
            }
          printf("\n");
        }
      return(r);
    }

void civals(real *mean,real *hw,int *nb)
    { /* return batch means analysis results */
      *mean=Y; *hw=h; *nb=k;
    }
