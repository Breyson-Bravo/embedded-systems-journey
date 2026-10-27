# Resumen de Aprendizaje: Timers y Retardos en Bare Metal (STM32G474RE)

A continuación se resume la transición realizada desde el control básico de un LED mediante ciclos de desperdicio de CPU hasta la implementación de temporización precisa y no bloqueante mediante la interrupción por hardware `SysTick`.

---

## 1. Manipulación Directa de Registros (GPIO)

Para manipular el LED de la tarjeta (**PA5**) a nivel de registros sin usar la librería HAL, se utilizaron las siguientes operaciones bit a bit (*bitwise*):

* **Poner a 1 (Set):** Se utiliza el operador `|=` o el registro `BSRR`.
  ```c
  GPIOA->ODR |= GPIO_ODR_OD5;       // Pone PA5 a nivel alto (1)
  GPIOA->BSRR = GPIO_BSRR_BS5;      // Forma atómica de poner PA5 a 1
  ```

* **Poner a 0 (Reset):** Se combina el operador `&=` con la negación `~`, o el registro `BSRR`.
  ```c
  GPIOA->ODR &= ~GPIO_ODR_OD5;      // Pone PA5 a nivel bajo (0)
  GPIOA->BSRR = GPIO_BSRR_BR5;      // Forma atómica de poner PA5 a 0
  ```

* **Conmutar / Invertir (Toggle):** Se utiliza el operador XOR (`^=`).
  ```c
  GPIOA->ODR ^= GPIO_ODR_OD5;       // Invierte el estado actual de PA5
  ```

---

## 2. Blink con Retardo Bloqueante (`__NOP()`)

Inicialmente se usó una función basada en un bucle `while` que ejecuta la instrucción en ensamblador `__NOP()` (*No Operation*) para desperdiciar ciclos de reloj.

```c
void delay(volatile uint32_t count) {
    while (count--) {
        __NOP(); // Desperdicia ciclos de reloj de la CPU
    }
}
```

### Limitaciones
* **Bloqueante:** Mantiene a la CPU ocupada en un bucle cerrado, impidiendo la ejecución de otras tareas.
* **Impreciso:** El tiempo transcurrido depende de la frecuencia del reloj, las optimizaciones del compilador y el pipeline del procesador.

---

## 3. Blink con Interrupción Hardware `SysTick`

Para lograr una temporización exacta y no bloqueante, se configuró el temporizador `SysTick` integrado en el núcleo **ARM Cortex-M4**.

### Configuración e Interrupción
Se configuró el `SysTick` para generar una interrupción cada 1 ms aprovechando el estándar **CMSIS**:

```c
// Se actualiza el reloj del sistema y se asigna el valor de recarga
SystemCoreClockUpdate();
SysTick_Config(SystemCoreClock / 1000); // Interrupción cada 1 ms
```

La interrupción `SysTick_Handler` incrementa un contador global `msTicks`:

```c
extern volatile uint32_t msTicks;

void SysTick_Handler(void) {
    msTicks++; // Se ejecuta automáticamente cada 1 ms
}
```

### Lógica No Bloqueante
En el bucle principal (`main`), se sustituyó la función bloqueante por una comparación de marcas de tiempo similar a `millis()`:

```c
uint32_t t_anterior = 0;

while (1) {
    // Comprueba si han transcurrido los milisegundos deseados
    if (msTicks - t_anterior >= 1000) {
        t_anterior = msTicks;
        GPIOA->ODR ^= GPIO_ODR_OD5; // Toggle del LED
    }
    
    // La CPU queda libre para ejecutar otros procesos aquí
}
```

### Manejo de Desbordes (*Overflow*)
Dado que `msTicks` es de tipo `uint32_t`, se desborda aproximadamente cada 49.7 días. Gracias a la aritmética de enteros sin signo y la representación en complemento a dos, la resta `msTicks - t_anterior` mantiene la precisión exacta incluso durante y después del desborde.

---

## 4. Mejoras Futuras

* **Implementar un Timer Propio como Tarea:** Crear un temporizador basado en un timer periférico (por ejemplo, `TIM2` o `TIM3`) configurado mediante registros (`PSC`, `ARR`, `DIER`) para manejar tareas periódicas o timeouts en segundo plano independientemente del `SysTick`.
* **Programación orientada a eventos / Máquina de estados:** Diseñar un planificador de tareas (*scheduler*) ligero o una arquitectura no bloqueante para gestionar múltiples periféricos (lectura de botones, comunicación UART) de forma concurrente.