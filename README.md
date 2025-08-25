# STM32F4xx Display Driver for LCD TFT Display
This repository contains the implementation for an STM32NucleoF446RE display driver for the ILI9488 Display Module. See the following sections for more information.

## Display Information
<img width="500" height="500" alt="image" src="https://github.com/user-attachments/assets/1c5272f4-9754-4432-b124-d50ed42d629e" />

Image source: https://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520

## Using the ILI9488 Display 
Before writing any code, it is extremely important to read through the datasheet to understand how this display works. A link to the datasheet is included in the Resources section and should you encounter any difficulties, referring to the datasheet will be a huge help.

### Power On/Off Sequence

The power on/off sequence can be tricky to implement because it can be difficult to debug. However, once again referring to the datasheet will help. 
If we refer to the Power On/Off Sequence in the datasheet, we can see that there are 3 main functions we need to implement: hardware reset, software reset, and sleep out. Software reset and sleep out are simply command functions and can be written to the display with an 8-bit write over MOSI. Hardware reset simply refers to toggling the RESET line using our GPIO connection. If we implement these functions we get the following:
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

Then, we need to specify the RGBformat and the memory access control format. Both of these can be done by writing a command. Lastly, once all of these have been configured, we can turn the display ON using another command. The completed power on/off sequence is shown below:

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

## Resources
<ul>
  <li>https://github.com/cpq/bare-metal-programming-guide</li>
  <li>https://www.hpinfotech.ro/ILI9488.pdf</li>
  <li>https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf</li>
  <li>https://www.lcdwiki.com/3.5inch_SPI_Module_ILI9488_SKU:MSP3520</li>
</ul>
