#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h" // CMSIS-Core
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"
#include "tc_driver.h"

extern void tcInit(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
  
  GPIOPinConfigure(GPIO_PD2_T1CCP0);
  GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_2);
  

  TIMER1_CTL_R = 0x0;
  
  TIMER1_CFG_R = 0x4;
  
  TIMER1_TAMR_R = 0x13;
  
  TIMER1_TAMATCHR_R = 0xFFFE;
  TIMER1_TAILR_R = 0xFFFF;
  
  TIMER1_TAPMR_R = 0x00;
  TIMER1_TAPR_R = 0xFF;
  
  TIMER1_TAV_R = 0x00;
  
  TIMER1_CTL_R  = 0x1;
}

extern void PWMInit(){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER2));
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  
  GPIOPinConfigure(GPIO_PA4_T2CCP0);
  GPIOPinTypeTimer(GPIO_PORTA_BASE, GPIO_PIN_4);
  
  TIMER2_CTL_R = 0x0;

  TIMER2_CFG_R = 0x4;

  TIMER2_TAMR_R = 0xA;

  TIMER2_TAILR_R = 12000;
  TIMER2_TAMATCHR_R = 6000;

  TIMER2_CTL_R = 0x1;
}

extern void setCycle(float cycle){
  TIMER2_TAMATCHR_R = TIMER2_TAILR_R - 120*cycle;
}

extern uint32_t tcRead(){
  return TIMER1_TAV_R;
}

extern void tcReset(){
  TIMER1_TAV_R = 0;
}

