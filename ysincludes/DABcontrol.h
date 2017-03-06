/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "math.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "ysPWM.h"

/******************************************************************************
| defines
|----------------------------------------------------------------------------*/
/* Motor parameters */
#define Rs 4.0
#define Ls 0.289368
#define Rr 3.3278
#define Lr 0.289368
#define Lm 0.27325
#define Tr 0.0869557
#define np 2

/* control period */
#define DAB_Ts 4.7619e-5  // 21kHz
#define Fleg_Ts 14.2857e-5  // 7kHz
//#define DAB_Ts 0.3333e-4  // 21kHz
//#define Fleg_Ts 1e-4  // 10kHz

/* PI parameters */
  // Ulcmd
//#define Kp_Ulcmd 0.5
//#define Ki_Ulcmd 0
#define Uplim_Ulcmd 1
#define Downlim_Ulcmd -1

/* auxiliary */
#define pi 3.1415926
//#define IgRefRatio 0.2

/******************************************************************************
| types
|----------------------------------------------------------------------------*/
typedef struct
{
  double a, b, c;
} PHASE_ABC;

typedef struct
{
  double al, be;
} PHASE_ALBE;

typedef struct
{
  double d,q;
} PHASE_DQ;

/******************************************************************************
| global variables
|----------------------------------------------------------------------------*/
/* 观测值 */
  // 电压
extern double Ui;
extern double Ug;
  // 电流
extern double Ii;
extern double Ig;

/* 给定值 */
  // 电压
extern double Ulcmd;
  // 电流
extern double IgRefRatio;
extern double Igcmd;

/* PI 变量 */
extern double Kp_Ulcmd;
extern double Ki_Ulcmd;
extern double Ig_intgrt;

/******************************************************************************
| local functions prototypes
|----------------------------------------------------------------------------*/

/******************************************************************************
| exported functions
|----------------------------------------------------------------------------*/
/* PI module */  
extern double PImodule(double Kp, double Ki, double err, double *intgrt, double Uplim, double Downlim, double Ts);
extern double Integrator(double paramin, double sum, double ts);
extern double LPfilter(double x, double lasty, double wc, double ts);
