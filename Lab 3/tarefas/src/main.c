#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS
#include "driverlib/joy.h"
#include "grlib/grlib.h"
#include "driverlib/gpio.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "cfaf128x128x16.h"
#include "driverlib/sysctl.h"
#include "tc_driver.h"
#include <stdio.h>


osThreadId_t t_SetPoint_id, t_ContagemPulsos_id, t_ControleVelocidade_id;
float setPoint = 60;
int velo_setPoint = 2500;
int velocidade;
int nRanhuras = 4;
tContext sContext;

osMutexId_t m_velo_id;

const osMutexAttr_t m_velo_attr = {
  "Velo",                            // human readable mutex name
  osMutexRecursive | osMutexPrioInherit,    // attr_bits
  NULL,                                     // memory for control block   
  0U                                        // size for control block
  };

void atualizaDisplay(){
    char str[23];
    sprintf(str,"Velocidade %d          ",velocidade); 
    GrStringDraw(&sContext, str, -1, 0, 60, true);
    char str2[23];
    sprintf(str2,"Set-point %.1f          ",setPoint); 
    GrStringDraw(&sContext, str2, -1, 0, 100, true);
    char str3[23];
    sprintf(str3,"velo_Set-point %d     ",velo_setPoint); 
    GrStringDraw(&sContext, str3, -1, 0, 80, true);
}

void initOLED (){
  cfaf128x128x16Init();
  cfaf128x128x16Clear();
  GrContextInit(&sContext, &g_sCfaf128x128x16);
  GrFlush(&sContext);
  GrContextFontSet(&sContext, g_psFontFixed6x8);
  GrContextForegroundSet(&sContext, ClrWhite);
  GrContextBackgroundSet(&sContext, ClrBlack);  
  GrStringDraw(&sContext, "Velocidade            ", -1, 0, 60, true);
}

void t_SetPoint(void *arg){
  while(1){
    osMutexAcquire(m_velo_id, osWaitForever); // try to acquire mutex
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) != GPIO_PIN_0){// Testa estado do push-button SW1
      velo_setPoint = velo_setPoint - 250;
    }
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) != GPIO_PIN_1){ // Testa estado do push-button SW2
      velo_setPoint = velo_setPoint + 250;
    }
    osMutexRelease(m_velo_id);
    if(joy_read_x() > 3000)
      GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5,  GPIO_PIN_5);
    else if( joy_read_x() < 1000)
      GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0); 
    
    atualizaDisplay();
    osDelayUntil(500);
  }
} 

void t_ContagemPulsos(void *arg){
  uint32_t tick;
  uint32_t time = 100;
  uint32_t aux = 1000/time;
  int val = 60*aux/nRanhuras;
  while(1){ //0,1s
    //osMutexAcquire(m_velo_id, osWaitForever); // try to acquire mutex
    tick = osKernelGetTickCount();
    uint32_t tc = tcRead();
    velocidade = tc*val; //vel em rpm 
    tcReset();
    osDelayUntil(tick + time);
    //osMutexRelease(m_velo_id);
  }
}

void t_ControleVelocidade(void *arg){
  uint32_t tick;
  while(1){
    tick = osKernelGetTickCount();
    osMutexAcquire(m_velo_id, osWaitForever); // try to acquire mutex
    if(velocidade < velo_setPoint && setPoint < 100)
      setPoint = setPoint + 0.5;
    else if(velocidade > velo_setPoint && setPoint > 40)
      setPoint = setPoint - 0.5;
    osMutexRelease(m_velo_id);
    setCycle(setPoint);
    osDelayUntil(tick + 100);
    
  }
} 

void main(void){
  SystemInit();
  initOLED();
  tcInit();
  PWMInit();
  joy_init();
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); // Habilita GPIO J (push-button SW1 = PJ0, push-button SW2 = PJ1)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)); // Aguarda final da habilita��o
  
  GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1); // push-buttons SW1 e SW2 como entrada
  GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Habilita GPIO A
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)); // Aguarda final da habilitação
  
  GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_5);
  GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5, 0); 
  GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_5, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0); 

  osKernelInitialize();

  t_SetPoint_id = osThreadNew(t_SetPoint, NULL, NULL);
  t_ContagemPulsos_id = osThreadNew(t_ContagemPulsos, NULL, NULL);
  t_ControleVelocidade_id = osThreadNew(t_ControleVelocidade, NULL, NULL);

  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
}

