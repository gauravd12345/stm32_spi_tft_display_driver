#define IVT_SIZE (97 + 16)

extern int main(void);
extern unsigned int _stack_pointer;


extern unsigned int _sdata, _edata, _sidata;
extern unsigned int _sbss, _ebss;

void _reset(void){
    /* 
    src holds the starting memory address of where .data is stored in flash.
    dst holds the starting memory address of where .data is to be stored in RAM
    */
    unsigned int* src = &_sidata;
    unsigned int* dst = &_sdata;

    /* Copying the .data from flash to RAM until the end memory address in RAM */
    while (dst < &_edata) 
        *dst++ = *src++;

    /* Setting dst to the start of .bss memory in ??. Initializing all values to 0 until the end memory address in RAM */
    dst = &_sbss;
    while (dst < &_ebss) 
        *dst++ = 0;

    /* Calling the main function */
    main();

    while(1);
}


/* 
Defining the Interrupt Vector Table
First 2 values: address of the stack pointer and the reset function(what to do after MCU boots up)

Array size is the # of interrupts as specified in the datasheet(IVT_SIZE)
*/
__attribute__((section(".vectors"))) const void* ivt[IVT_SIZE] = {
    (void*)&_stack_pointer,
    (void*)&_reset

};


