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
/*
 * Millisecond delay function.
 *   volatile keyword is used so that compiler does not optimize it away
 * Polling method (If interrupt is not enabled)
 */
void delay_ms(volatile uint32_t s)
{
    for(s; s>0; s--){
        while(!(SYSTICK->CSR & (1 << 16))); // Wait until COUNTFLAG is 1
    }
}

// SysTick Interrupt Handler
void SysTick_Handler(void) {
    static bool led_state = true;
    gpio_write(led, led_state);  // Toggle LED state
    led_state = !led_state;      // Flip the state for next interrupt
}


void NMI_Handler(void) {
    while (1); // Infinite loop for debugging
}

void HardFault_Handler(void) {
    while (1); // Infinite loop for debugging
}

void SVC_Handler(void) {
    while (1); // Infinite loop for debugging
}

void PendSV_Handler(void) {
    while (1); // Infinite loop for debugging
}

/*
 * Initialize SysTick Timer
 *
 * Since it is set up to run at 1Mhz, an s value of
 * 1Khz needed to make 1 millisecond timer
 */
void init_systick(uint32_t s, uint32_t en)
{
    // Main clock source is running with HSI by default which is at 8 Mhz.
    // SysTick clock source can be set with CTRL register (Bit 2) (DDI0419E_armv6m_arm.pdf)
    // 0: AHB/8 -> (1 MHz)
    // 1: Processor clock (AHB) -> (8 MHz)
    SYSTICK->CSR |= 0x00000; // Currently set to run at 1 Mhz
    // Enable callback
    SYSTICK->CSR |= (en << 1);
    // Load the reload value
    SYSTICK->RVR = s;
    // Set the current value to 0
    SYSTICK->CVR = 0;
    // Enable SysTick
    SYSTICK->CSR |= (1 << 0);
}

int main(void){

    //uint16_t led=PIN('C',6); //used as a global var
    RCC_GPIO_CLK_ENABLE(PINBANK(led));
    gpio_set_mode(led, GPIO_MODE_OUTPUT);
    /* for (;;) {
       //without systick blink led code
       gpio_write(led, 1);
       spin(999999);
       gpio_write(led, 0);
       spin(999999);

       }*/
    //init_systick(1000, 0);//polling mode
    init_systick(1000000, 1);//interrupt  mode 1 sec
#if 0 //systick polling mode code
    for(;;){
        delay_ms(1000);  // 1 second
        gpio_write(led,on);//every 'period' ms 
        on=!on;//toggle led state

        //Here we can perform other activities
    }
#endif

    while(1){
            // Here you can do other tasks if needed, or just wait for SysTick interrupts
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
//Read arm cortex m0 user guide: DUI0497A_cortex_m0_r0p0_generic_ug.pdf
__attribute__((section(".vectors")))
void (*const vector_table[7+32])(void) = {
    _estack,
    _reset,
    NMI_Handler,         // NMI 
    HardFault_Handler,   // HardFault 
    0,                   // Reserved (MPU Fault - not available in Cortex-M0)
    0,                   // Reserved (Bus Fault - not available in Cortex-M0)
    0,                   // Reserved (Usage Fault - not available in Cortex-M0)
    0, 0, 0, 0,          // Reserved
    SVC_Handler,         // SVCall 
    0,                   // Reserved
    0,                   // Reserved
    PendSV_Handler,      // PendSV 
    SysTick_Handler      // SysTick 
};


