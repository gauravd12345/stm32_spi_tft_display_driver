#include <stdio.h>
#include "hal.h"
#include "ili9488.h"
#include "math.h"


void gpio_init(void){
    // Enabling the SPI1, GPIOB, and GPIOA clock 
    RCC->AHB1ENR |= (1 << 0);
    RCC->AHB1ENR |= (1 << 1);
    RCC->APB2ENR |= (1 << 12);

    // Clearing and setting the GPIOA_MODER register to AF mode
    GPIO('A')->MODER &= ~(0b11U << (PA8 * 2));
    GPIO('B')->MODER &= ~(0b11U << (PB5 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA10 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA3 * 2));

    GPIO('A')->MODER &= ~(0b11U << (PA5 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA6 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA7 * 2));

    GPIO('A')->MODER |= (GPIO_MODE_OUTPUT << (PA8 * 2));
    GPIO('B')->MODER |= (GPIO_MODE_OUTPUT << (PB5 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_OUTPUT << (PA10 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_OUTPUT << (PA3 * 2));

    GPIO('A')->MODER |= (GPIO_MODE_AF << (PA5 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_AF << (PA6 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_AF << (PA7 * 2));

    // Clearing and setting GPIOA_AFLR register to AF5(SPI1 function)
    GPIO('A')->AFR[0] &= ~(0b1111U << (PA5 * 4));
    GPIO('A')->AFR[0] &= ~(0b1111U << (PA6 * 4));
    GPIO('A')->AFR[0] &= ~(0b1111U << (PA7 * 4));

    GPIO('A')->AFR[0] |= (0x5U << (PA5 * 4));
    GPIO('A')->AFR[0] |= (0x5U << (PA6 * 4));
    GPIO('A')->AFR[0] |= (0x5U << (PA7 * 4));
    
}   

void spi_config(void){ 
    SPI->SPI_CR1 &= ~(1U << 6); // Setting SPE bit to 0, disabling SPI communcation

    SPI->SPI_CR1 &= ~(0b111U << 3);   
    SPI->SPI_CR1 |= (0b011U << 3); // Setting baud rate control to f_pclk / 4

    SPI->SPI_CR1 &= ~(1U << 1); // CPOL bit
    SPI->SPI_CR1 &= ~(1U << 0); // CHPA bit
    
    SPI->SPI_CR1 &= ~(1U << 15); // BIDIMODE bit: 1-line bidirectional data mode selected
    SPI->SPI_CR1 &= ~(1U << 14); // BIDIOE bit: Output enabled (transmit-only mode)

    SPI->SPI_CR1 |= (1 << 9); // SSM = 1 (Software slave management)
    SPI->SPI_CR1 |= (1 << 8); // SSI = 1 (Set NSS high)

    SPI->SPI_CR1 &= ~(1U << 7); // LSBFIRST bit: MSB transmitted first
    SPI->SPI_CR1 |= (1 << 2); // MSTR bit: Master Configuration
    SPI->SPI_CR1 &= ~(1U << 11); // DFF bit: 8-bit data frame format is selected for transmission/reception
    
    SPI->SPI_CR1 |= (1 << 6); // Setting SPE bit to 1, enabling SPI communcation

}

void delay_us(uint32_t delay){
    for (volatile uint32_t i = 0; i< delay * 9U; ++i) {
         __asm volatile("nop"); 
    }
    
}

void spi_busy(void){
    while( (SPI->SPI_SR & (1U << 7))){} // wait for BSY buffer to be empty
    
}

void spi_tft_write_command(uint8_t buf){
    GPIO('A')->ODR &= ~(1U << PA8); // Pulling DC line LOW
    //GPIO('A')->ODR &= ~(1U << PA10); // Pulling CS line LOW, selecting the peripheral

    while(!(SPI->SPI_SR & (1U << 1))){} 
    SPI->SPI_DR = buf;
    spi_busy();

    //GPIO('A')->ODR |= (1U << PA8);; // Pulling DC line HIGH
    //GPIO('A')->ODR |= (1U << PA10); // Pulling CS line HIGH, setting peripheral in idle state

}


void spi_tft_write_data(uint8_t buf){
    //GPIO('A')->ODR &= ~(1U << PA10); // Pulling CS line LOW, selecting the peripheral
    GPIO('A')->ODR |= (1U << PA8); // Pulling DC line HIGH

    while(!(SPI->SPI_SR & (1U << 1))){} 
    SPI->SPI_DR = buf;
    spi_busy();
    //GPIO('A')->ODR |= (1U << PA10); // Pulling CS line HIGH, setting peripheral in idle state

}

void tft_hw_reset(void){
    GPIO('B')->ODR &= ~(1U << PB5); // RESET line LOW
    delay_us(5000);
    GPIO('B')->ODR |= (1U << PB5); // RESET line HIGH, reseting the display
    delay_us(5000);

    GPIO('A')->ODR &= ~(1U << PA10); // Pulling CS line low
    GPIO('A')->ODR &= ~(1U << PA3); // Pulling TFT_CS line low

}

void ili9488_init(void){
    tft_hw_reset();
    spi_tft_write_command(SOFTWARE_RESET); delay_us(5000);
    spi_tft_write_command(SLEEP_OUT); delay_us(120000);

    spi_tft_write_command(MEMORY_ACCESS_CONTROL); spi_tft_write_data(0x08);
    
    spi_tft_write_command(INTERFACE_PIXEL_FORMAT); spi_tft_write_data(0x66);
    
    spi_tft_write_command(DISPLAY_ON); delay_us(20000);
    
}   

void set_window_frame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
    spi_tft_write_command(COLUMN_ADDRESS_SET);

    spi_tft_write_data((uint8_t) (x1 >> 8));
    spi_tft_write_data((uint8_t) (x1 & 0xFFU));
    spi_tft_write_data((uint8_t) (x2 >> 8));
    spi_tft_write_data((uint8_t) (x2 & 0xFFU));

    spi_tft_write_command(PAGE_ADDRESS_SET);

    spi_tft_write_data((uint8_t) (y1 >> 8));
    spi_tft_write_data((uint8_t) (y1 & 0xFFU));
    spi_tft_write_data((uint8_t) (y2 >> 8));
    spi_tft_write_data((uint8_t) (y2 & 0xFFU));

}


void fill_screen(rgb color){
    spi_tft_write_command(MEMORY_WRITE);
    // Display_area is 320 * 480 pixels
    for(uint16_t i = 0; i < 320; i++){
        for(uint16_t j = 0; j < 480; j++){
            spi_tft_write_data(color.r); // r
            spi_tft_write_data(color.g); // g
            spi_tft_write_data(color.b); // b
        }
        
    }
   
}


void draw_square(uint16_t x, uint16_t y, uint16_t size, rgb color){
    set_window_frame(x, y, x + size, y + size);
    spi_tft_write_command(MEMORY_WRITE);
    for(uint16_t i = 0; i < size; i++){
        for(uint16_t j = 0; j < size; j++){
            spi_tft_write_data(color.r); // r
            spi_tft_write_data(color.g); // g
            spi_tft_write_data(color.b); // b
            
        }
        
    }
    
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t length, uint16_t width, rgb color){
    set_window_frame(x, y, x + length, y + width);
    spi_tft_write_command(MEMORY_WRITE);
    for(uint16_t i = 0; i < (length); i++){
        for(uint16_t j = 0; j < (width); j++){
            spi_tft_write_data(color.r); // r
            spi_tft_write_data(color.g); // g
            spi_tft_write_data(color.b); // b
            
        }
        
    }
    
}

void draw_circle(uint16_t x, uint16_t y, uint16_t radius, rgb color) {
    int32_t r2 = (int32_t)radius * (int32_t)radius;

    for (uint16_t i = 0; i < (uint16_t)(radius * 2U); i++) {
        for (uint16_t j = 0; j < (uint16_t)(radius * 2U); j++) {
            int32_t dx = (int32_t)j - (int32_t)radius; // relative to center
            int32_t dy = (int32_t)i - (int32_t)radius;
            if ((dx*dx + dy*dy) <= r2) {
                uint16_t x_pos = (uint16_t)((int32_t)x + dx);
                uint16_t y_pos = (uint16_t)((int32_t)y + dy);

                set_window_frame(x_pos, y_pos, x_pos, y_pos);
                spi_tft_write_command(MEMORY_WRITE);
                spi_tft_write_data(color.r);
                spi_tft_write_data(color.g);
                spi_tft_write_data(color.b);
            }
        }
    }
}


int main(void){
    gpio_init();
    spi_config();
    ili9488_init();

    fill_screen(BLACK);
    draw_rectangle(110, 90, 100, 300, GREEN); // 200 * 300 pixels rectangle
    draw_circle(110, 90, 40, RED); 
    while(1){
         
    }
}