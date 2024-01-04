#include "main_base.h"

int x = 120, y = 160, dx = 1, dy = 1;
int timer=0;
char light=1;

int main(void) {

	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOG; //Odblokowanie zegara portu G
	GPIOG->MODER |= GPIO_Mode_OUT << (13*2);// 1 - output
	RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA; //Odblokowanie zegara portu A

	SysTick->CTRL=5;
	SysTick->LOAD=8000000-1; //zegar 16megaHerców
	while (1)
	{
		if(SysTick->CTRL&0x00010000)
		{
			if(light==1)
			{
				GPIOG->BSRR = GPIO_Pin_13; //0x4000 //bitset
				light=0;
			}
			else
			{
				GPIOG->BSRR = GPIO_Pin_13<<16; //0x40000000 //bitreset
				light=1;
			}
		}
		//if((GPIOA->IDR&1)) GPIOG->BSRR = GPIO_Pin_13<<16;
	}
}

