#include <stdio.h>
#include "hal.h"


#define GPIO(port) ((struct gpio *) 0x40020000 + (0x400 * (port - 'A')))
#define RCC ((struct rcc *) 0x40023800)
#define SPI ((struct spi *) 0x40013000)

#define PA4 4 // CS
#define PA5 5 // SPI1_SCK
#define PA6 6 // SPI1_MISO
#define PA7 7 // SPI1_MOSI

void gpio_init(void){
    // Enabling the GPIOA and SPI1 clock 
    RCC->AHB1ENR |= (1 << 0);
    RCC->APB2ENR |= (1 << 12);

    // Clearing and setting the GPIOA_MODER register to AF mode
    GPIO('A')->MODER &= ~(0b11 << (PA4 * 2));
    GPIO('A')->MODER &= ~(0b11 << (PA5 * 2));
    GPIO('A')->MODER &= ~(0b11 << (PA6 * 2));
    GPIO('A')->MODER &= ~(0b11 << (PA7 * 2));

    GPIO('A')->MODER |= (GPIO_MODE_OUTPUT << (PA4 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_AF << (PA5 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_AF << (PA6 * 2));
    GPIO('A')->MODER |= (GPIO_MODE_AF << (PA7 * 2));

    // Clearing and setting GPIOA_AFLR register to AF4(SPI1 function)
    GPIO('A')->AFR[0] &= ~(0b1111 << (PA5 * 4));
    GPIO('A')->AFR[0] &= ~(0b1111 << (PA6 * 4));
    GPIO('A')->AFR[0] &= ~(0b1111 << (PA7 * 4));

    GPIO('A')->AFR[0] |= (0b0101 << (PA5 * 4));
    GPIO('A')->AFR[0] |= (0b0101 << (PA6 * 4));
    GPIO('A')->AFR[0] |= (0b0101 << (PA7 * 4));

}

void spi_config(void){
    GPIO('A')->ODR |= (1 << PA4); // Pulling CS line HIGH, setting peripheral in idle state

    SPI->SPI_CR1 |= (0b001 << 3); // Setting baud rate control  to f_pclk / 4

    SPI->SPI_CR1 |= (0 << 0); // CHPA bit
    SPI->SPI_CR1 |= (0 << 0); // CPOL bit

    SPI->SPI_CR1 |= (0 << 15); // BIDIMODE bit: 1-line bidirectional data mode selected
    SPI->SPI_CR1 |= (0 << 14); // BIDIOE bit: Output enabled (transmit-only mode)

    SPI->SPI_CR1 |= (0 << 7); // LSBFIRST bit: MSB transmitted first
    SPI->SPI_CR1 |= (1 << 2); // MSTR bit: Master Configuration
    SPI->SPI_CR1 |= (0 << 11); // DFF bit: 8-bit data frame format is selected for transmission/reception

    SPI->SPI_CR1 |= (1 << 6); // Setting SPE bit to 1, enabling SPI communcation
}


/*
Initializing the display using the SPI protocol:
    MCU has to enable the display module by toggling its CS(Chip Select)line low. This way, if other peripherals are connected, 
    to the SPI bus, the display can still be uniquely identified and enabled. 
*/
void spi_start_communication(void){
    GPIO('A')->ODR &= ~(1 << PA4); // Pulling CS line LOW, selecting the peripheral

}
