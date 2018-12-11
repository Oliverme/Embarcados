#ifndef __TC_DRIVER_H__
#define __TC_DRIVER_H__
extern void tcInit();
extern void PWMInit();
extern void setCycle(float cycle);
extern uint32_t tcRead();
extern void tcReset();
#endif