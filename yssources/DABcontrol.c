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
extern double IgRefRatio = 0.1;
extern double Igcmd = 0;

/* PI 变量 */
extern double Kp_Ulcmd = 0.4;
extern double Ki_Ulcmd = 20;
extern double Ig_intgrt = 0;

/******************************************************************************
@brief   PI Module 
******************************************************************************/
double PImodule(double Kp, double Ki, double err, double *intgrt, double Uplim, double Downlim, double Ts)
{
	double output = 0;

	*intgrt += Ki * Ts * err;
	if (*intgrt >= Uplim)
	{
		*intgrt = Uplim;
		return Uplim;
	}
	else if (*intgrt <= Downlim)
	{
		*intgrt = Downlim;
		return Downlim;
	}
	else
	{
		output = Kp * err + *intgrt;

		if (output >= Downlim && output <= Uplim)
			return output;
		else if (output > Uplim)
			return Uplim;
		else
			return Downlim;
	}
}

double Integrator(double paramin, double sum, double ts)
{
  return paramin * ts + sum;
}

double LPfilter(double x, double lasty, double wc, double ts)
{
  return (lasty + ts * wc * x) / (1 + ts * wc);
}
