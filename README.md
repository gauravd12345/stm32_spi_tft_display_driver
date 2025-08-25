# STM32F4xx Display Driver for LCD TFT Display
This repository contains the implementation for an STM32NucleoF446RE display driver for an ILI9488 Display Module. See the following sections for more information.

## Display Information
<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/1c5272f4-9754-4432-b124-d50ed42d629e" />

Image source: https://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520

## Using the driver 

To use the driver, simply include the ```ili9488.h``` and ```ili9488.c``` files in your project directory. Ensure that you have a proper linker script and startup file containing the interrupt vector table. There are already ```startup.c``` and ```link.ld``` files included in this repository so you can just use them directly. 

To initialize the display, you will need to call ```gpio_init()```,  ```spi_config()``` , and ```ili9488_init()``` in your main function and that's it! You can now begin to use the display. Shown below is an example that draws a rectangle to the screen:

```
int main(void){
    gpio_init();
    spi_config();
    ili9488_init();

    fill_screen(BLACK);
    draw_rectangle(110, 90, 100, 300, GREEN); // 200 * 300 pixels rectangle
    while(1){
         
    }
}
``` 



## Understanding the ILI9488 Display 
Before writing any code, it is extremely important to read through the datasheet to understand how this display works. A link to the datasheet is included in the Resources section and should you encounter any difficulties, referring to the datasheet will be a huge help.

### Sending data over 4-line SPI

The ILI9488 controller uses the SPI data communication protocol to communicate between the MCU and display module. It accepts multiple configurations such as 3-line SPI, 4-line SPI, 3-line parallel SPI, etc but for the display used in this repository, we will be using 4-line SPI as it is already configured that way(refer to the display information). 

The general layout of SPI is like so:
1. SCLK -> Clock signal
2. MOSI -> Master Out Slave In
3. MISO -> Master In Slave Out
4. CS -> Chip Select

How it works is that the MCU will send out a synchronized clock signal on SCLK and then pull CS low to select the device you wish to communicate with. Then, data is sent over MOSI from the MCU to the device.
To better understand, here is an oscilloscope output I generated when developing this driver that shows SCLK and MOSI:

<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/56dd4d00-f29f-451f-858a-e470a237e0e3" />

As you can see, the periodic, yellow signal is SCLK and the blue signal is MOSI.  Since we don't need to read anything from the display, we don't need to use the MISO line. Instead, the 4th wire we will use will be the DC/RS pin. 

### Power On/Off Sequence

The power on/off sequence can be tricky to implement because it can be difficult to debug. However, once again referring to the datasheet will help. 
If we refer to the Power On/Off Sequence in the datasheet, we can see that there are 3 main functions we need to implement: hardware reset, software reset, and sleep out. Software reset and sleep out are simply command functions and can be written to the display by sending 8 bits over MOSI. Hardware reset simply refers to toggling the RESET line using our GPIO connection. If we implement these functions we can get the following:
```
void tft_hw_reset(void){
    GPIO('B')->ODR &= ~(1U << PB5); // RESET line LOW
    delay_us(5000);
    GPIO('B')->ODR |= (1U << PB5); // RESET line HIGH, reseting the display
    delay_us(5000);
    GPIO('A')->ODR &= ~(1U << PA10); // RESET line LOW

}

void ili9488_init(void){
    tft_hw_reset();
    spi_tft_write_command(SOFTWARE_RESET); delay_us(5000);
    spi_tft_write_command(SLEEP_OUT); delay_us(120000); 
}    
```

Then, we need to specify the RGBformat and the memory access control format. For this particular display, use RGB66(6 bits per value). Both the RGBformat and memory access control can be configured done by writing a command. Lastly, once all of these have been written, we can turn the display ON using a similar command. The completed power on/off sequence is shown below:

```
void ili9488_init(void){
    tft_hw_reset();
    spi_tft_write_command(SOFTWARE_RESET); delay_us(5000);
    spi_tft_write_command(SLEEP_OUT); delay_us(120000);

    spi_tft_write_command(MEMORY_ACCESS_CONTROL); spi_tft_write_data(0x08); // BGR bit
    
    spi_tft_write_command(INTERFACE_PIXEL_FORMAT); spi_tft_write_data(0x66); // RGB666 format
    
    spi_tft_write_command(DISPLAY_ON); delay_us(20000);
    
}
```
Note the delay functions used; the power on/off sequence is <b>timing-sensitive</b> so you have to be careful when you write code.

## Resources
<ul>
  <li>https://www.hpinfotech.ro/ILI9488.pdf</li>
  <li>https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf</li>
  <li>https://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520</li>
</ul>
