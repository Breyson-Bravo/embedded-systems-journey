#include "stm32g474xx.h"

void delay(volatile uint32_t count) {
  while (count--) {
    __NOP(); // No operation, just waste time
  }
}

int main(void) {
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable GPIOA clock
  
  GPIOA->MODER &= ~GPIO_MODER_MODE5; // Clear mode bits for PA5 
  GPIOA->MODER |= GPIO_MODER_MODE5_0; // Set PA5 as output mode


  GPIOA->OTYPER &= ~GPIO_OTYPER_OT5; // Set PA5 as push-pull
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; // No pull-up, pull-down for PA5
 

  while (1) {
  GPIOA->ODR ^= GPIO_ODR_OD5; // Toggle PA5
  delay(1000000); // Delay for a while
    }

    return 0;
}