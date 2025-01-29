# STM32-Systick-Device-Driver-Cortex-M0

This repository provides a bare-metal **SysTick** driver for the STM32F072RBT6 Cortex-M0 microcontroller, enabling the configuration of SysTick in both **polling** and **interrupt** modes. It also includes human-readable functions for GPIO initialization and a Makefile that supports flashing via **J-Link** and **ST-Flash** (ST-Link v2). 

**Inspiration**

This project is inspired by the [bare-metal-programming-guide](https://github.com/cpq/bare-metal-programming-guide). I am deeply grateful to the author of that repository for their clear and insightful explanation of embedded programming concepts.

## Features:
- **SysTick Configuration**: 
  - Configure SysTick in **polling** mode for basic delay functionality.
  - Configure SysTick in **interrupt** mode for more efficient, interrupt-driven timing.

- **GPIO Initialization**: 
  - Provides human-readable functions to configure GPIO pins for input/output modes.

- **Makefile**:
  - Includes targets for flashing the firmware using **J-Link** and **ST-Flash (ST-Link v2)**.
  - **J-Link** is used for debugging via Segger Ozone.
  - **ST-Link** is another way of flashing firmware.

## Development Journey:
- **Segger Ozone Debugging**: 
  - During the development of this project, I learned to use **Segger Ozone** for debugging, which helped me analyze the behavior of the code and troubleshoot issues efficiently.
  
- **ST-Link & J-Link**: 
  - I gained hands-on experience with **ST-Link** and **J-Link** tools for flashing and debugging STM32 devices. This repository includes a Makefile to easily flash the device with either tool.

- **System-On-Chip (SoC) User Guide**:
  - I learned the importance of referring to the **SoC-specific user guides** to understand the functionalities of various hardware features, such as the **SysTick timer**, which is covered in the **Cortex-M0 User Guide** for STM32F0 series.

## Getting Started:

1. **Clone the Repository**:
   ```
   git clone https://github.com/Tabrez-dev/STM32-Systick-Device-Driver-Cortex-M0.git
   ```

2. **Build the Project**:
   Use the provided **Makefile** to build the project:
   ```
   make build
   ```

3. **Flash the Firmware**:
   Use **J-Link** or **ST-Link** to flash the firmware. The Makefile provides targets to flash the device with either tool:
   
   - Flash with **J-Link**:
     ```
     make flash-jlink
     ```
   - Flash with **ST-Link**:
     ```
     make flash-stlink
     ```

5. **Debugging**:
   - For debugging, use **Segger Ozone** with **J-Link** to step through the code and analyze register/memory states.
   - Alternatively, use **ST-Link** for simple debugging using tools like OpenOCD.

Using ST-Link Reflash Utility and Segger Ozone for Debugging

Download [Ozone](https://www.segger.com/products/development-tools/ozone-j-link-debugger/) from the Segger website. Before we can use it with our discovery board, we need to convert the **ST-LINK firmware** on the onboard debugger to **J-Link firmware** that Ozone understands. Follow the instructions on the Segger site [here](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/) to update your **ST-Link** to **J-Link** firmware.

After the J-Link firmware is installed on the ST-Link, you can use Segger Ozone for advanced debugging of your STM32 microcontroller project.

![image](https://github.com/user-attachments/assets/a458cbcc-68ad-4df5-b715-e30ca1bf9022)

Do Not Forget To Add .svd file in **Peripherals** to use Ozone properly for this board. Use appropriate .svd file for your board. 
