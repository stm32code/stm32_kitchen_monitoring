#ifndef __RELAY_H
#define __RELAY_H

#include "stm32f10x.h"

#define BEEP_GPIO_PORT GPIOB
#define BEEP_GPIO_CLK RCC_APB2Periph_GPIOB
#define BEEP_GPIO_PIN GPIO_Pin_8

#define RELAY1_GPIO_PORT GPIOB
#define RELAY1_GPIO_CLK RCC_APB2Periph_GPIOB
#define RELAY1_GPIO_PIN GPIO_Pin_9

#define RELAY2_GPIO_PORT GPIOB
#define RELAY2_GPIO_CLK RCC_APB2Periph_GPIOB
#define RELAY2_GPIO_PIN GPIO_Pin_7

#define RELAY3_GPIO_PORT GPIOB
#define RELAY3_GPIO_CLK RCC_APB2Periph_GPIOB
#define RELAY3_GPIO_PIN GPIO_Pin_6

#define Beepout PBout(8)   // BEEP接口
#define Beepin PBin(8)   // BEEP接口

#define relay1in PBin(9)
#define relay2in PBin(7)
#define relay3in PBin(6)

#define relay1out PBout(9) 
#define relay2out PBout(7) 
#define relay3out PBout(6) 

void BeepNo(u16 tiem);
void RELAY_GPIO_Config(void);

#endif /* __RELAY_H */
