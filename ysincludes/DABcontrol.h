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
#define Ts 4.7619e-5  // 21kHz

/* PI parameters */
  // Ulcmd
#define Kp_Ulcmd 10
#define Ki_Ulcmd 500
#define Uplim_Ulcmd
#define Downlim_Ulcmd

/* auxiliary */
#define pi 3.1415926

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
extern double Igcmd;

/* PI 变量 */
extern double Iglasterr;

/******************************************************************************
| local functions prototypes
|----------------------------------------------------------------------------*/

/******************************************************************************
| exported functions
|----------------------------------------------------------------------------*/
/* PI module */  
extern double PImodule(double Kp, double Ki, double output, double err, double *lasterr, double Uplim, double Downlim);
extern double Integrator(double paramin, double sum, double ts);
extern double LPfilter(double paramin, double lasty, double wc, double ts);
