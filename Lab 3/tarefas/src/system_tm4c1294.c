#include <stdbool.h>
#include <stdint.h>
#include "driverlib/sysctl.h"

uint32_t SystemCoreClock;

void SystemInit(void){
  #if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t) &__Vectors;
#endif

#if defined (__FPU_USED) && (__FPU_USED == 1U)
  SCB->CPACR |= ((3U << 10U*2U)|  // set CP10 Full Access
                 (3U << 11U*2U)); // set CP11 Full Access
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
  SystemCoreClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                              SYSCTL_OSC_MAIN |
                                              SYSCTL_USE_PLL |
                                              SYSCTL_CFG_VCO_480),
                                              120000000); // 25MHz
} // SystemInit

void SystemCoreClockUpdate(void){
  // Not implemented
} // SystemCoreClockUpdate
