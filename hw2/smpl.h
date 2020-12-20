/**********************************************************************/
/*                                                                    */
/*                           File "smpl.h"                            */
/*  Includes, Defines, & Extern Declarations for Simulation Programs  */
/*                                                                    */
/*                                         (c) 1987  M. H. MacDougall */
/*                                                                    */
/* Note: (1)   The function prototypes were added by Jason Fricot     */
/*             on 3/26/98 to comply with the newer C compilers.       */
/*                                                                    */
/*       (2)   On 32 bit processors int is stored as long int.        */
/*             On 16 bit machines overflow will occur with numbers    */
/*             greater than 32767.                                    */
/*                                                                    */
/**********************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef double real;
#define then

/*  ----------  PROTOTYPES  ----------------------------------------  */
/*  ----------  for functions in smpl.c  ---------------------------  */

static int save_name(char *s, int m);
static int get_blk(int n);
static int get_elm(void);
static void put_elm(int i);
static int suspend(int tkn);
static void enlist(int *head, int elm);
static void resetf(void);
static void enqueue(int f, int j, int pri, int ev, real te);
static void msg(int n, int i, char *s, int q1, int q2);
static void end_line(void);
static int rept_page(int fnxt);
void smpl(int m, char *s);
void reset(void);
void schedule( int ev, real te, int tkn);
void cause(int *ev, int *tkn);
void release(int f, int tkn);
void trace(int n);
void pause(void);
void error(int n, char *s);
void report(void);
void reportf(void);
void endpage(void);
void newpage(void);
real time(void);
real U(int f);
real B(int f);
real Lq(int f);
char *fname(int f);
char *mname(void);
int facility(char *s, int n);
int request(int f, int tkn, int pri);
int cancel (int ev);
int preempt(int f, int tkn, int pri);
int status(int f);
int inq(int f);
int lns(int i);
FILE * sendto(FILE *dest);

/*  ----------  PROTOTYPES  ----------------------------------------  */
/*  ----------  for functions in bmeans.c  -------------------------  */

int obs(real y);
real Z(real p);
real T(real p,int ndf);
void civals(real *mean,real *hw, int *nb);
void init_bm(int m0,int mb);


/*  ----------  PROTOTYPES  ----------------------------------------  */
/*  ----------  for functions in rand.c  ---------------------------  */

int stream(int n);
int seed(int Ik, int n);
#ifdef ORG
int random(int i, int n);
#endif
real ranf(void);
real uniform(real a, real b);
real expntl(real x);
real erlang(real x, real s);
real hyperx(real x, real s);
real normal(real x, real s);
real triangular(real a, real b,real c);

