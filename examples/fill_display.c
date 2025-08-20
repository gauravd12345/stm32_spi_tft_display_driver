#include <stdio.h>
#include "./ili9488.c"

int main(void){
    gpio_init();
    spi_config();
    ili9488_init();

    fill_screen();
    while(1){
         
    }
}