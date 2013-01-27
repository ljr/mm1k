/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

capiran.cpp
-------------------------------------------------------------------------------
simpack c-language application progrmamming interface
Part 3: pseudo-random-number-related.
-------------------------------------------------------------------------------
These random number generator routines were written by M.H.  MacDougall,
"Simulating Computer Systems", MIT Press, 1987.

Code was extracted 950412 from queuing.c in "old simpack" into this file.  i
changed the function parameter and return declarations.  other than that i made
almost NO changes, for fear of altering the way the calculations work.

/// There are numerous error-checks in this code which have been commented out.
/// Do we want to re-enable them?  i'd guess yes.
-----------------------------------------------------------------------------*/

#include "queuing.h"
#include <math.h> // log(), sqrt()

#define A 16807L           /* multiplier (7**5) for 'ranf' */
#define M 2147483647L      /* modulus (2**31-1) for 'ranf' */
#define then

typedef double real;


long In0 [] =
  {
              0L,   1973272912L,  747177549L,   20464843L,
      640830765L,   1098742207L,   78126602L,   84743774L,
      831312807L,    124667236L, 1172177002L, 1124933064L,
     1223960546L,   1878892440L, 1449793615L,  553303732L
  };

long In [] =
  {
              0L,   1973272912L,  747177549L,   20464843L,
      640830765L,   1098742207L,   78126602L,   84743774L,
      831312807L,    124667236L, 1172177002L, 1124933064L,
     1223960546L,   1878892440L, 1449793615L,  553303732L
  };

int strm = 1;                /* index of current stream */



/*--------------------  SELECT GENERATOR STREAM  ---------------------*/
int stream (int n)
    { /* set stream for 1<=n<=15, return stream for n=0 */
      if ( (n<0) || (n>15) ) ErrXit (9032, "bad n");
      if (n) strm=n;
      return(strm);
    }

void ResetRNstream () // internal use, NOT in API
  {
  int ii;
  for (ii = 0; ii < 16; ii++)
    In [ii] = In0 [ii];
  }//end fn


#if TURBOC
/*-------------  UNIFORM [0, 1] RANDOM NUMBER GENERATOR  -------------*/
/*                                                                    */
/* This implementation is for Intel 8086/8 and 80286/386 CPUs using   */
/* C compilers with 16-bit short integers and 32-bit long integers.   */
/*                                                                    */
/*--------------------------------------------------------------------*/
real ranf()
  {
    short *p,*q,k; long Hi,Lo;
    /* generate product using double precision simulation  (comments  */
    /* refer to In's lower 16 bits as "L", its upper 16 bits as "H")  */
    p=(short *)&In[strm]; Hi= *(p+1)*A;                 /* 16807*H->Hi */
    *(p+1)=0; Lo=In[strm]*A;                           /* 16807*L->Lo */
    p=(short *)&Lo; Hi+= *(p+1);    /* add high-order bits of Lo to Hi */
    q=(short *)&Hi;                       /* low-order bits of Hi->LO */
    *(p+1)= *q&0X7FFF;                               /* clear sign bit */
    k= *(q+1)<<1; if (*q&0X8000) then k++;         /* Hi bits 31-45->K */
    /* form Z + K [- M] (where Z=Lo): presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) then Lo+=M;
    In[strm]=Lo;
    return((real)Lo*4.656612875E-10);             /* Lo x 1/(2**31-1) */
  }

///#endif
#else
///#if UNIX || UNIXX

/*-------------  UNIFORM [0, 1] RANDOM NUMBER GENERATOR  -------------*/
/*                                                                    */
/* This implementation is for Motorola 680x0 CPUs using C compilers   */
/* with 16-bit short integers and 32-bit long integers.               */
/*                                                                    */
/*--------------------------------------------------------------------*/
real ranf_old()
  {
    short *p,*q,k; long Hi,Lo;
    /* generate product using double precision simulation  (comments  */
    /* refer to In's lower 16 bits as "L", its upper 16 bits as "H")  */
    p=(short *)&In[strm]; Hi= *(p)*A;                   /* 16807*H->Hi */
    *(p)=0; Lo=In[strm]*A;                             /* 16807*L->Lo */
    p=(short *)&Lo; Hi+= *(p);      /* add high-order bits of Lo to Hi */
    q=(short *)&Hi;                       /* low-order bits of Hi->LO */
    *(p)= *(q+1)&0X7FFF;                             /* clear sign bit */
    k= *(q)<<1; if (*(q+1)&0X8000) then k++;       /* Hi bits 31-45->K */
    /* form Z + K [- M] (where Z=Lo): presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) then Lo+=M;
    In[strm]=Lo;
    return((real)Lo*4.656612875E-10);             /* Lo x 1/(2**31-1) */
  }

/*
 * New version of the RNG from simpack-2.22, more portable
 * than the original.
 *
 * hrp@cray.com, 94/05/26
 */

/*-------------  UNIFORM [0, 1] RANDOM NUMBER GENERATOR  -------------*/
/*                                                                    */
/* This implementation should be portable to machines with            */
/* sizeof(int) >=4.                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
real ranf()
  {
    unsigned int k; long Hi,Lo;
    /* generate product using double precision simulation  (comments  */
    /* refer to In's lower 16 bits as "L", its upper 16 bits as "H")  */
    Hi = ((In[strm] >> 16) & 0xffff) * A; /* 16807*H->Hi */
    Lo = (In[strm] & 0xffff) * A;       /* 16807*L->Lo */
    Hi += Lo >> 16;             /* add high-order bits of Lo to Hi */
    Lo &= 0xffff; Lo += (Hi & 0x7fff) << 16; /* low-order bits of Hi->Lo */
    k = Hi >> 15;
    /* form Z + K [- M] (where Z=Lo): presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) then Lo+=M;
    In[strm]=Lo;
    return((real)Lo*4.656612875E-10); /* Lo / (2**31-1) */
  }
#endif

/*--------------------------  SET/GET SEED  --------------------------*/
long seed (long Ik, int n)
    { /* set seed of stream n for Ik>0, return current seed for Ik=0  */
      if ( (n<1) || (n>15) ) ErrXit (9033, "Bad n");
      if (Ik>0L) In[n]=Ik;
      return(In[n]);
    }

/*------------  UNIFORM [a, b] RANDOM VARIATE GENERATOR  -------------*/
real uniform (real a, real b)
    { /* 'uniform' returns a psuedo-random variate from a uniform     */
      /* distribution with lower bound a and upper bound b.           */
      if (a>b) ErrXit (9034, "a > b");
      return(a+(b-a)*ranf());
    }

/*--------------------  RANDOM INTEGER GENERATOR  --------------------*/
int random (int i, int n)
    { /* 'random' returns an integer equiprobably selected from the   */
      /* set of integers i, i+1, i+2, . . , n.                        */
      if (i>n) ErrXit (9035, "i > n");
      n-=i;
      n = int ( (n+1.0)*ranf() );
      return(i+n);
    }

/*--------------  EXPONENTIAL RANDOM VARIATE GENERATOR  --------------*/
real expntl (real x)
    { /* 'expntl' returns a psuedo-random variate from a negative     */
      /* exponential distribution with mean x.                        */
      return(-x*log(ranf()));
    }

/*----------------  ERLANG RANDOM VARIATE GENERATOR  -----------------*/
real erlang (real x, real s)
    { /* 'erlang' returns a psuedo-random variate from an erlang      */
      /* distribution with mean x and standard deviation s.           */
      int i,k; real z;
      /* if (s>x) then error(0,"erlang Argument Error: s > x"); */
      z=x/s;
      k = int (z*z);
      z=1.0; for (i=0; i<k; i++) z *= ranf();
      return(-(x/k)*log(z));
    }

/*-----------  HYPEREXPONENTIAL RANDOM VARIATE GENERATION  -----------*/
real hyperx (real x, real s)
    { /* 'hyperx' returns a psuedo-random variate from Morse's two-   */
      /* stage hyperexponential distribution with mean x and standard */
      /* deviation s, s>x.  */
      real cv,z,p;
      if (s<=x) ErrXit (9036, "s is less than or equal to x");
      cv=s/x; z=cv*cv; p=0.5*(1.0-sqrt((z-1.0)/(z+1.0)));
      z=(ranf()>p)? (x/(1.0-p)):(x/p);
      return(-0.5*z*log(ranf()));
    }

/*-----------  TRIANGULAR RANDOM VARIATE GENERATION  -----------*/
real triang (real a, real c, real b)
{
 /* triangular distribution with left and right being [a,b] and the
    mode being at point c */
  real sample,point;
  point = (c-a)/(b-a);
  sample = uniform(0.0,1.0);
  if (sample <= point)
    return(sqrt(sample*(b-a)*(c-a)) + a);
  else
    return(b - sqrt((1.0-sample)*(b-a)*(b-c)));
}

/*-----------------  NORMAL RANDOM VARIATE GENERATOR  ----------------*/
real normal (real x, real s)
    { /* 'normal' returns a psuedo-random variate from a normal dis-  */
      /* tribution with mean x and standard deviation s.              */
      real v1,v2,w,z1; static real z2=0.0;
      if (z2!=0.0)
        then {z1=z2; z2=0.0;}  /* use value from previous call */
        else
          {
            do
              {v1=2.0*ranf()-1.0; v2=2.0*ranf()-1.0; w=v1*v1+v2*v2;}
            while (w>=1.0);
            w=sqrt((-2.0*log(w))/w); z1=v1*w; z2=v2*w;
          }
      return(x+z1*s);
  }

// end capiran.cpp

