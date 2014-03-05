/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    BLinky.c
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "STM32F10x.h"    // Device header
#include "LED.h"
#include "GLCD.h"

#define __FI    1                       /* Font index 16x24                  */

osThreadId tid_phaseA;                  /* Thread id of thread: phase_a      */
osThreadId tid_phaseB;                  /* Thread id of thread: phase_b      */
osThreadId tid_phaseC;                  /* Thread id of thread: phase_c      */
osThreadId tid_phaseD;                  /* Thread id of thread: phase_d      */
osThreadId tid_clock;                   /* Thread id of thread: clock        */
osThreadId tid_lcd;                     /* Thread id of thread: lcd          */

osMutexId mut_GLCD;                     /* Mutex to controll GLCD access     */

#define LED_A   0
#define LED_B   1
#define LED_C   2
#define LED_D   3
#define LED_CLK 7


/*----------------------------------------------------------------------------
 *      Switch LED on
 *---------------------------------------------------------------------------*/
void Switch_On (unsigned char led) {

  LED_On(led);
  osMutexWait(mut_GLCD, osWaitForever);
  GLCD_SetBackColor(White);                     /* Set the Back Color        */
  GLCD_SetTextColor(Green);                     /* Set the Text Color        */
  GLCD_DisplayChar(4, 5+led, __FI, 0x80+1);     /* Circle Full               */
  osMutexRelease(mut_GLCD);
}

/*----------------------------------------------------------------------------
 *      Switch LED off
 *---------------------------------------------------------------------------*/
void Switch_Off (unsigned char led) {

  LED_Off(led);
  osMutexWait(mut_GLCD, osWaitForever);
  GLCD_SetBackColor(White);                     /* Set the Back Color        */
  GLCD_SetTextColor(Green);                     /* Set the Text Color        */
  GLCD_DisplayChar(4, 5+led, __FI, 0x80+0);     /* Circle Empty              */
  osMutexRelease(mut_GLCD);
}


/*----------------------------------------------------------------------------
 *      Function 'signal_func' called from multiple threads
 *---------------------------------------------------------------------------*/
void signal_func (osThreadId tid)  {
  osSignalSet(tid_clock, 0x0100);           /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osSignalSet(tid_clock, 0x0100);           /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osSignalSet(tid, 0x0001);                 /* set signal to thread 'thread' */
  osDelay(500);                             /* delay 500ms                   */
}

/*----------------------------------------------------------------------------
 *      Thread 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
void phaseA (void const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    Switch_On (LED_A);
    signal_func(tid_phaseB);                /* call common signal function   */
    Switch_Off(LED_A);
  }
}

/*----------------------------------------------------------------------------
 *      Thread 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
void phaseB (void const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    Switch_On (LED_B);
    signal_func(tid_phaseC);                /* call common signal function   */
    Switch_Off(LED_B);
  }
}

/*----------------------------------------------------------------------------
 *      Thread 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
void phaseC (void const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    Switch_On (LED_C);
    signal_func(tid_phaseD);                /* call common signal function   */
    Switch_Off(LED_C);
  }
}

/*----------------------------------------------------------------------------
 *      Thread 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
void phaseD (void  const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    Switch_On (LED_D);
    signal_func(tid_phaseA);                /* call common signal function   */
    Switch_Off(LED_D);
  }
}

/*----------------------------------------------------------------------------
 *      Thread 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
void clock (void  const *argument) {
  for (;;) {
    osSignalWait(0x0100, osWaitForever);    /* wait for an event flag 0x0100 */
    Switch_On (LED_CLK);
    osDelay(80);                            /* delay 80ms                    */
    Switch_Off(LED_CLK);
  }
}

/*----------------------------------------------------------------------------
 *      Thread 6 'lcd': LCD Control thread
 *---------------------------------------------------------------------------*/
void lcd (void const *argument) {

  for (;;) {
    osMutexWait(mut_GLCD, osWaitForever);
    GLCD_SetBackColor(Blue);
    GLCD_SetTextColor(White);
    GLCD_DisplayString(0, 0, __FI, "  MCBSTM32E Demo ");
    GLCD_DisplayString(1, 0, __FI, "    RTX Blinky      ");
    GLCD_DisplayString(2, 0, __FI, "   www.keil.com     ");
    osMutexRelease(mut_GLCD);
    osDelay(4000);

    osMutexWait(mut_GLCD, osWaitForever);
    GLCD_SetBackColor(Blue);
    GLCD_SetTextColor(Red);
    GLCD_DisplayString(0, 0, __FI, "  MCBSTM32E Demo ");
    GLCD_DisplayString(1, 0, __FI, "    RTX Blinky      ");
    GLCD_DisplayString(2, 0, __FI, "   www.keil.com     ");
    osMutexRelease(mut_GLCD);
    osDelay(4000);
  }
}


osMutexDef(mut_GLCD);

osThreadDef(phaseA, osPriorityNormal, 1, 0);
osThreadDef(phaseB, osPriorityNormal, 1, 0);
osThreadDef(phaseC, osPriorityNormal, 1, 0);
osThreadDef(phaseD, osPriorityNormal, 1, 0);
osThreadDef(clock,  osPriorityNormal, 1, 0);
osThreadDef(lcd,    osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {

  SystemCoreClockUpdate();
  LED_Initialize();                         /* Initialize the LEDs           */
  GLCD_Initialize();                        /* Initialize the GLCD           */
 
  GLCD_Clear(White);                        /* Clear the GLCD                */

  mut_GLCD = osMutexCreate(osMutex(mut_GLCD));

  tid_phaseA = osThreadCreate(osThread(phaseA), NULL);
  tid_phaseB = osThreadCreate(osThread(phaseB), NULL);
  tid_phaseC = osThreadCreate(osThread(phaseC), NULL);
  tid_phaseD = osThreadCreate(osThread(phaseD), NULL);
  tid_clock  = osThreadCreate(osThread(clock),  NULL);
  tid_lcd    = osThreadCreate(osThread(lcd),    NULL);

  osSignalSet(tid_phaseA, 0x0001);          /* set signal to phaseA thread   */

  osDelay(osWaitForever);
  while(1);
}
