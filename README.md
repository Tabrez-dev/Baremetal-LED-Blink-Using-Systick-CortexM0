# Baremetal LED Blink Using SysTick (Cortex-M0)

## Project Overview
This repository demonstrates a bare-metal implementation of LED blinking on the STM32F072RB-DISCO board using the SysTick timer of the Cortex-M0. It is a minimalistic program designed to showcase GPIO control, SysTick timer initialization, and non-blocking multitasking techniques.

### Inspiration
This project is inspired by the [bare-metal-programming-guide](https://github.com/cpq/bare-metal-programming-guide?tab=readme-ov-file). I am deeply grateful to the author of that repository for their clear and insightful explanation of embedded programming concepts.

---

## Technical Details

### GPIO Control
The GPIO (General Purpose Input/Output) peripheral is configured using a `struct` that maps to the hardware registers. Pin control is achieved using:

- **Mode Register (MODER):** Configures the mode of a pin (Input, Output, Alternate Function, or Analog).
- **Bit Set/Reset Register (BSRR):** Used for atomic pin state changes (set/reset).

### SysTick Timer
The SysTick timer is initialized to generate periodic interrupts at a 1ms interval. The key registers involved are:

- **CSR (Control and Status Register):** Configures the timer and enables its interrupt.
- **RVR (Reload Value Register):** Sets the countdown value for the timer.
- **CVR (Current Value Register):** Tracks the current countdown value.

### Timer Expired Function
The `timerExpired` function abstracts periodic task scheduling by checking if a timer has elapsed. It handles wrap-around scenarios and allows for precise non-blocking delays.

#### Function Logic
1. Checks if the expiration time has been initialized or wrapped around.
2. Compares the expiration time with the current system time (`s_ticks`).
3. If expired, updates the expiration time for the next interval.

This approach minimizes CPU usage and ensures accurate timing without blocking execution.

---

## Code
The full implementation of the project can be found in the [main.c](main.c) file. Key functionalities include:

1. GPIO pin initialization for LED control.
2. SysTick timer initialization.
3. Non-blocking LED blinking using the `timerExpired` function.
4. Minimal startup code for Cortex-M0, including vector table setup.

---

## Getting Started
### Prerequisites
- STM32F072RB-DISCO board
- ARM GCC Toolchain
- OpenOCD for flashing

### Build and Flash Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Baremetal-LED-Blink-Using-Systick-CortexM0.git
   cd Baremetal-LED-Blink-Using-Systick-CortexM0
   ```
2. Build the project using the provided Makefile:
   ```bash
   make
   ```
3. Flash the `.elf` file to the board using OpenOCD:
   ```bash
   openocd -f stm32f0discovery.cfg -c "program boot_up.elf verify reset exit"
   ```

---

## Disclaimer
This code has been written to the best of my understanding of the Cortex-M0 Technical Reference Manual (TRM) and the STM32F072RB-DISCO user manual. However, the SysTick timer caused a hard fault for reasons I couldn't debug. Despite this, the project was an excellent learning experience. If you identify the issue or have improvements, feel free to submit a pull request!

---

