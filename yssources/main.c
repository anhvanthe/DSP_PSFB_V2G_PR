/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "main.h"

/******************************************************************************
| local variable definitions
|----------------------------------------------------------------------------*/
double mSample[4];
double Dutycycle = 0;
Uint16 D_Leg1 = 0;  // 整流占空比（时钟数）
Uint16 D_Leg2 = 0;  // 整流占空比（时钟数）
Uint16 D_Leg3 = 0;  // 整流占空比（时钟数）
Uint16 D_Leg4 = 0;  // 整流占空比（时钟数）

Uint16 LegCount = 0;  // 判断中断时控制的输出桥臂
int indexDA = 0;
double Igerr = 0;

/******************************************************************************
@brief  Main
******************************************************************************/
void main()
{
   InitSysCtrl();

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;
   PieVectTable.EPWM1_INT = &epwm1_timer_isr;  // ePWM1中断入口
   EDIS;

   InitPORT();
   InitPWM();
   InitADC();
	
   IER |= M_INT3;  // enable ePWM CPU_interrupt
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  // enable ePWM1 pie_interrupt

   EINT;   // 总中断 INTM 使能
   ERTM;   // Enable Global realtime interrupt DBGM


   int i;
   for(; ;)
   {
	   asm("          NOP");
	   for(i=1;i<=10;i++)
	   {}
   }

}

interrupt void epwm1_timer_isr(void)
{
	// Clear INT flag for this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

	/* ======== 电压电流采样 ======== */
	ParallelRD(mSample, 4);
	Ui = mSample[0] * HallRatioVi;
	Ug = -mSample[1] * HallRatioVg;  // 采样方向和正方向相反
	Ii = mSample[2] * HallRatioIi;
	Ig = LPfilter(mSample[3] * HallRatioIg, Ig, 12000, DAB_Ts);

    switch(LegCount)
    {
		case 0:
		{
			EPwm3Regs.TBPHS.half.TBPHS = DABperiod;  // 时基周期寄存器
			EPwm4Regs.TBPHS.half.TBPHS = DABperiod;  // 时基周期寄存器
			EPwm5Regs.TBPHS.half.TBPHS = DABperiod;  // 时基周期寄存器
			EPwm6Regs.TBPHS.half.TBPHS = DABperiod;  // 时基周期寄存器
			LegCount++;
			break;
		}
		case 1:
		{
			EPwm3Regs.TBPHS.half.TBPHS = DABperiod * 2;  // 时基周期寄存器
			EPwm4Regs.TBPHS.half.TBPHS = DABperiod * 2;  // 时基周期寄存器
			EPwm5Regs.TBPHS.half.TBPHS = DABperiod * 2;  // 时基周期寄存器
			EPwm6Regs.TBPHS.half.TBPHS = DABperiod * 2;  // 时基周期寄存器
			LegCount++;
			break;
		}
		case 2:
		{
			EPwm3Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
			EPwm4Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
			EPwm5Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
			EPwm6Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
			LegCount = 0;

			/* ======== 输出桥臂设置 ======== */
			Igcmd = Ug * IgRefRatio;  // 电流给定
			Igerr = Igcmd - Ig;  // 电流误差

			// 输出电感电压给定
			Ulcmd = PImodule(Kp_Ulcmd, Ki_Ulcmd, Igerr, &Ig_intgrt, Uplim_Ulcmd, Downlim_Ulcmd, Fleg_Ts);
			Dutycycle = 0.5 * Ulcmd + 0.5;
		    D_Leg1 = Flegperiod * Dutycycle;  // A相占空比
		    D_Leg2 = Flegperiod * Dutycycle;  // B相占空比
		    D_Leg3 = Flegperiod * Dutycycle;  // C相占空比
		    D_Leg4 = Flegperiod * Dutycycle;  // 第四桥臂占空比
		    // 寄存器设置
		    EPwm3Regs.CMPA.half.CMPA = D_Leg4;
		    EPwm4Regs.CMPA.half.CMPA = D_Leg1;
		    EPwm5Regs.CMPA.half.CMPA = D_Leg2;
		    EPwm6Regs.CMPA.half.CMPA = D_Leg3;
			break;
		}
		default:
		{
			EPwm1Regs.TBCTL.bit.CTRMODE = TB_FREEZE;  // 向上计数
			EPwm2Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
			EPwm3Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
			EPwm4Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
			EPwm5Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
			EPwm6Regs.TBCTL.bit.CTRMODE = TB_FREEZE;
		}
    }

	switch(indexDA)
	{
		case 0:
		{
			DACout(0, Ig);
			DACout(1, Igcmd);
			break;
		}
		default:
		{
			DACout(0, Igerr);
			DACout(1, Ulcmd);
		}
	}

    // Clear INT flag for this timer
   	while(EPwm1Regs.ETFLG.bit.INT == 1)
   		EPwm1Regs.ETCLR.bit.INT = 1;

   // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
