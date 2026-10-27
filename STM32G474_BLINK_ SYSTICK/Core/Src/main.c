#include "stm32g474xx.h"
volatile uint32_t msTicks = 0; // Counter for milliseconds
volatile uint32_t t_anterior = 0; // Variable to store the previous tick count


int main(void) {
  SystemCoreClockUpdate(); // Update the SystemCoreClock variable
  SysTick_Config(SystemCoreClock / 1000); // Configure SysTick for 1ms interrupts

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable GPIOA clock
  
  GPIOA->MODER &= ~GPIO_MODER_MODE5; // Clear mode bits for PA5 
  GPIOA->MODER |= GPIO_MODER_MODE5_0; // Set PA5 as output mode


  GPIOA->OTYPER &= ~GPIO_OTYPER_OT5; // Set PA5 as push-pull
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; // No pull-up, pull-down for PA5
 

  while (1) {

   if (msTicks - t_anterior >= 3000 ) { // Check if 3 seconds have passed
      t_anterior = msTicks; // Update the previous tick count
      GPIOA->ODR ^= GPIO_ODR_OD5; // Toggle PA5
    }

    }

    return 0;
}