/******************************************************************************
| includes                          
|----------------------------------------------------------------------------*/
#include "DABcontrol.h"

/******************************************************************************
| local variable definitions                          
|----------------------------------------------------------------------------*/

/******************************************************************************
| global variable definitions                          
|----------------------------------------------------------------------------*/
/* 观测值 */
  // 电压
extern double Ui = 0;
extern double Ug = 0;
  // 电流
extern double Ii = 0;
extern double Ig = 0;

/* 给定值 */
  // 电压
extern double Ulcmd = 0;
  // 电流
extern double Igcmd = 0;

/* PI 变量 */
extern double Iglasterr = 0;

/******************************************************************************
@brief   PI Module 
******************************************************************************/
double PImodule(double Kp, double Ki, double output, double err, double *lasterr, double Uplim, double Downlim)
{
	output += Kp * (err - *lasterr) + Ki * Ts * err;
	*lasterr = err;

	if (output >= Downlim && output <= Uplim)
		return output;
	else if (output > Uplim)
		return Uplim;
	else
		return Downlim;
}

double Integrator(double paramin, double sum, double ts)
{
  return paramin * ts + sum;
}

double LPfilter(double paramin, double lasty, double wc, double ts)
{
  return (lasty + ts * wc * paramin) / (1 + ts * wc);
}
