/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "main.h"

/******************************************************************************
| local variable definitions
|----------------------------------------------------------------------------*/
double mSample[4];
double Uab, Ubc, Uca;
Uint16 dutycycle;  // ����ռ�ձȣ�ʱ������

int indexDA = 7;
double ualsum = 0;
double ubesum = 0;
double ialsum = 0;
double ibesum = 0;

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
   PieVectTable.EPWM1_INT = &epwm1_timer_isr;  // ePWM1�ж����
   //PieVectTable.TINT0 = &ISRTimer0;
   EDIS;

   InitPORT();
   InitPWM();
   InitADC();
   //InitSCI();
   //InitSPI();
   //InitCpuTimers();  // ����ת�ٺ�ת�ٸ���ֵ

   //ConfigCpuTimer(&CpuTimer0, 150, 100000);  // 100ms
   //CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0
	
   IER |= M_INT3;  // enable ePWM CPU_interrupt
   //IER |= M_INT1;  // CpuTimer
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  // enable ePWM1 pie_interrupt
   //PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

   EINT;   // ���ж� INTM ʹ��
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

	/* ======== ��ѹ�������� ======== */
	ParallelRD(mSample, 4);
	Ui = mSample[0] * HallRatioVi;
	Ug = -mSample[1] * HallRatioVg;  // ����������������෴
	Ii = mSample[2] * HallRatioIi;
	Ig = mSample[3] * HallRatioIg;

	/* ======== ����ű����� ======== */
	Igcmd = Ug * IgRefRatio;  // ��������
	Igerr = Igcmd - Ig;  // �������
	// �����е�ѹ����
	Ulcmd = PImodule(Kp_Ulcmd, Ki_Ulcmd, Ulcmd, Igerr, *Iglasterr, Uplim_Ulcmd, Downlim_Ulcmd) + Ug;
    D_load1 = Ulcmd * reciprocal_Udc;  // A��ռ�ձ�
    D_load2 = Ulcmd * reciprocal_Udc;  // B��ռ�ձ�
    D_load3 = Ulcmd * reciprocal_Udc;  // C��ռ�ձ�
    // �Ĵ�������
    EPwm4Regs.CMPA.half.CMPA = period * D_load1;
    EPwm5Regs.CMPA.half.CMPA = period * D_load2;
    EPwm6Regs.CMPA.half.CMPA = period * D_load3;

	switch(indexDA)
	{
	}

    // Clear INT flag for this timer
   	while(EPwm1Regs.ETFLG.bit.INT == 1)
   		EPwm1Regs.ETCLR.bit.INT = 1;

   // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
