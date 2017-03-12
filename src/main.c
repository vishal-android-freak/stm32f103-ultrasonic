#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f10x_gpio.h"
#include "delay.h"

#define TRIGPIN GPIO_Pin_6
#define ECHOPIN GPIO_Pin_8

unsigned long pulseIn(uint16_t gpioPin, GPIO_TypeDef *gpiox, uint8_t state, unsigned long timeout);

int main(void) {

	//DWT_Init();

	//Initialize Port E and enable the port
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	//
	GPIO_InitTypeDef gpioInitTrig;

	gpioInitTrig.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitTrig.GPIO_Pin = TRIGPIN;
	gpioInitTrig.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_Init(GPIOE, &gpioInitTrig);

	gpioInitTrig.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitTrig.GPIO_Pin = ECHOPIN;
	gpioInitTrig.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_Init(GPIOE, &gpioInitTrig);

	while(1) {
		GPIO_SetBits(GPIOE, TRIGPIN);
		DWT_Delay(10);
		GPIO_ResetBits(GPIOE, TRIGPIN);

		unsigned long response = pulseIn(ECHOPIN, GPIOE, 1, 1000);

		trace_printf("%d\n", (response*170)/10000);
	}
}

unsigned long pulseIn(uint16_t gpioPin, GPIO_TypeDef *gpiox, uint8_t state, unsigned long timeout) {

	uint16_t stateMask = (state ? gpioPin : 0);
	unsigned long width = 0; // keep initialization out of time critical area

	unsigned long numloops = 0;
	unsigned maxloops = (72 * timeout)/16;

	while((gpiox->IDR & gpioPin) == stateMask) {
		if (numloops++ == maxloops) {
					return 0;
		}
	}

	while((gpiox->IDR & gpioPin) != stateMask) {
			if (numloops++ == maxloops) {
						return 0;
			}
		}

	while((gpiox->IDR & gpioPin) == stateMask) {
		width++;
	}

	return ((width * 10 + 16)/72);
}
