#define GPIO(port) ((struct gpio *) (0x40020000 + (0x0400 * (port - 'A'))))
#define RCC ((struct rcc *) 0x40023800)
#define SPI ((struct spi *) 0x40013000)

#define PA10 10 // Pin D2 on MCU -> CS line
#define PB5 5 // Pin D4 on MCU -> RESET line
#define PA8 8 // Pin D7 on MCU -> DC/RS line

#define PA5 5 // SPI1_SCK
#define PA6 6 // SPI1_MISO
#define PA7 7 // SPI1_MOSI


#define SOFTWARE_RESET (uint8_t) (0x01)
#define SLEEP_IN (uint8_t) (0x10)
#define SLEEP_OUT (uint8_t) (0x11)
#define ALL_PIXELS_OFF (uint8_t) (0x22)
#define DISPLAY_OFF (uint8_t) (0x28)
#define DISPLAY_ON (uint8_t) (0x29)
#define COLUMN_ADDRESS_SET (uint8_t) (0x2A)
#define PAGE_ADDRESS_SET (uint8_t) (0x2B)
#define MEMORY_WRITE (uint8_t) (0x2C)
#define INTERFACE_PIXEL_FORMAT (uint8_t) (0x3A)
#define MEMORY_ACCESS_CONTROL (uint8_t) (0x36)


struct rgb_color{
    uint8_t r, g, b;

 };
typedef struct rgb_color rgb;

#define RED (rgb) {0xFF, 0x00, 0x00}
#define GREEN (rgb) {0x00, 0xFF, 0x00}
#define BLUE (rgb) {0x00, 0x00, 0xFF}
#define WHITE (rgb) {0xFF, 0xFF, 0xFF}
#define BLACK (rgb) {0x00, 0x00, 0x00}