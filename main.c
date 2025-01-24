#include <inttypes.h>
#include <stdbool.h>

#define BIT(x) (1UL << (x))
#define PIN(bank, num) ((((bank)-'A')<<8) | (num))
#define PINNO(pin) (pin & 255)
#define PINBANK(pin) (pin >> 8)

struct gpio {
 volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL, AFRH, BRR; 
};
#define GPIO(bank) ((struct gpio *) (0x48000000 + 0x400 * (bank)))
//Enum vlaues are per datasheet: 0,1,2,3...for specifc function
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };


static inline void gpio_set_mode(uint16_t pin, uint8_t mode) {
  // Get the GPIO bank (port) for the specified pin (A, B, C, etc.)
  struct gpio *gpio = GPIO(PINBANK(pin));
  // Extract the pin number (0-15) from the pin identifier (e.g., 'A5' -> 5)
  int n = PINNO(pin);
  // Clear the existing 2-bit mode configuration for the specific pin
  // The 2 bits for each pin are at positions 2*n and 2*n+1 in MODER.
  gpio->MODER &= ~(3U << (n * 2));
  // Set the new mode by OR'ing the correct mode value at the proper position
  // (mode & 3) ensures we only take the lower 2 bits of the mode
  // and then shift them to the correct position for the pin.
  gpio->MODER |= (mode & 3U) << (n * 2);
}


//In order to enable a GPIO peripheral, it should be enabled (clocked) via the RCC (Reset and Clock Control) unit.

struct rcc {
 volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR, BDCR, CSR, AHBRSTR, CFGR2, CFGR3, CR2;
};
#define RCC ((struct rcc *) 0x40021000)
#define RCC_GPIO_CLK_ENABLE(bank)  (RCC->AHBENR |= BIT(17 + (bank)))
#define RCC_GPIO_CLK_DISABLE(bank) (RCC->AHBENR &= ~BIT(17 + (bank)))
// Define a function to write a value (high/low) to a specific GPIO pin
static inline void gpio_write(uint16_t pin, bool val) {
  // Access the GPIO structure corresponding to the pin's bank (group of pins)
  struct gpio *gpio = GPIO(PINBANK(pin));
  // Write the value to the pin's bit set/reset register (BSRR).
  // The value is shifted left based on whether 'val' is true or false.
  // If 'val' is true (high), it sets the corresponding pin high.
  // If 'val' is false (low), it resets the pin (sets it low).
  gpio->BSRR = (1U << PINNO(pin)) << (val ? 0 : 16);
}

static inline void spin(volatile uint32_t count) {
  while (count--) (void) 0;
}
    uint16_t led=PIN('C',6);

struct systick {
 volatile uint32_t CSR, RVR, CVR, CALIB;
};
#define SYSTICK ((struct systick *) 0xE000E010)
static inline void systick_init(uint32_t ticks) {
 RCC->APB2ENR |= (1<<0);
 if((ticks-1) > 0xffffff) return;

 SYSTICK->RVR = ticks - 1;
 SYSTICK->CVR = 0;
 SYSTICK->CSR = 0x00000007;

}
//those values in memory that get updated by interrupt handlers, or by the hardware, declare as volatile
//static bool onn=true;

static volatile uint32_t s_ticks;
void SysTick_Handler(void) {
   s_ticks++;
}

// Check if the timer has expired
// Parameters:
//  expirationTime: pointer to the expiration timestamp
//  interval: timer interval in milliseconds
//  currentTime: current system time in milliseconds
// Returns true if the timer has expired, false otherwise
/*
We use the timerExpired function to check if a user-defined timer has elapsed based on a 1ms system tick, avoiding the need to manually calculate expiration logic every time we use the timer.
*/
bool timerExpired(uint32_t *expirationTime, uint32_t interval, uint32_t currentTime) {
    // Handle timer wrap-around by resetting the expiration time
    if (currentTime + interval < *expirationTime) {
        *expirationTime = 0;  
    }
    // Initialize expiration time if it hasn't been set
    if (*expirationTime == 0) {
        *expirationTime = currentTime + interval;  
    }
    // If the expiration time is still in the future, the timer hasn't expired
    if (*expirationTime > currentTime) {
        return false;  
    }
    // Update expiration time for the next interval, considering potential wrap-around
    if ((currentTime - *expirationTime) > interval) {
        *expirationTime = currentTime + interval;  // Set a new future expiration time
    } else {
        *expirationTime += interval;  // Schedule the next expiration event
    }
    return true;  // Timer has expired
}

int main(void){
    
    //uint16_t led=PIN('C',6);
    RCC_GPIO_CLK_ENABLE(PINBANK(led));
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
   /* for (;;) {

     gpio_write(led, 1);
      spin(999999);
     gpio_write(led, 0);
      spin(999999);
   
    }*/


   systick_init(8000000/1000);//tick every 1 ms

   uint32_t timer=0, period=500;
    for(;;){
     if(timerExpired(&timer, period, s_ticks)){
         static bool on;
         gpio_write(led,on);//every 'period' ms 
         on=!on;//toggle led state
        }

     //Here we can perform other activities
    }
    return 0;
}


//Startup code
__attribute__((naked, noreturn)) 
void _reset(void){
//long is quivalent to uint32_t
extern long _sbss, _ebss, _sdata, _edata, _srcdata;

for(long *dst = &_sbss; dst < &_ebss;dst++) *dst=0;
for(long *dst= &_sdata, *src= &_srcdata;dst< &_edata;) *dst++=*src++;

main();
for(;;) (void) 0; //infinite loop incase main() returns

}

extern void _estack(void); //defined in linker script

//7 standard and 32 STM32-specific handlers
__attribute__((section(".vectors")))
//An array of 38  constant function pointers,
// where each function returns void and takes no arguments.
 void (*const vector_table[7+32])(void)={
 _estack,
 _reset,
 0,
 0,
 0,
 0,
 SysTick_Handler

}; 


