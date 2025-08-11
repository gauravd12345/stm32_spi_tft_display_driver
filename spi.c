#include <stdio.h>
#include "hal.h"

#define GPIO(port) ((struct gpio *) 0x40020000 + (0x04 * (port - 'A')))
#define RCC ((struct rcc *) 0x40023800)
#define SPI ((struct spi *) 0x40013000)

#define PA5 5 // SPI1_SCK
#define PA6 6 // SPI1_MISO
#define PA7 7 // SPI1_MOSI

#define PA10 10 // Pin D2 on MCU -> CS line
#define PB5 5 // Pin D4 on MCU -> RESET line
#define PA8 8 // Pin D7 on MCU -> DC/RS line

void gpio_init(void){
    // Enabling the SPI1, GPIOB, and GPIOA clock 
    RCC->AHB1ENR |= (1 << 0);
    RCC->AHB1ENR |= (1 << 1);
    RCC->APB2ENR |= (1 << 12);

    // Clearing and setting the GPIOA_MODER register to AF mode
    GPIO('A')->MODER &= ~(0b11U << (PA8 * 2));
    GPIO('B')->MODER &= ~(0b11U << (PB5 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA10 * 2));

    GPIO('A')->MODER &= ~(0b11U << (PA5 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA6 * 2));
    GPIO('A')->MODER &= ~(0b11U << (PA7 * 2));

    GPIO('A')->MODER |= (GPIO_MODE_OUTPUT << (PA8 * 2));
    GPIO('B')->MODER |= (GPIO_MODE_OUTPUT << (PB5 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_OUTPUT << (PA10 * 2));

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


void spi_display_write(uint8_t buf){
    while(!(SPI->SPI_SR & (1U << 1))){ // wait for transmit buffer to be empty

    }

    SPI->SPI_DR = buf;

}

uint32_t spi_read(void){
    while(!(SPI->SPI_SR & (0U << 0))){ // wait for transmit buffer to be empty

    }
    return SPI->SPI_DR;
}


int main(void){
    gpio_init();
    spi_config();
    while(1){
        spi_display_write(0x1U);
        
    }

}