
//Add libraries
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/uart.h"
#include "driverlib/uart.c"
#include "driverlib/interrupt.h"
#include "driverlib/interrupt.c"
//Define Variables
#define RX1_PIN         GPIO_PIN_0
#define TX1_PIN         GPIO_PIN_1
char data;
char Buffer[100];
int8_t i=0;
char data_recv;
// UART INTERRUPT HANDLER

void Receive_ISR(void)
{
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART1_BASE, true); //get interrupt status
                                                  //value true because they're masked
                                                  //interrupt (they are enabled by code)
    UARTIntClear(UART1_BASE, ui32Status);         //clear the asserted interrupts
    while(UARTCharsAvail(UART1_BASE)==1)
    {
    data=UARTCharGet(UART1_BASE);
//    UARTCharPut(UART1_BASE, data); //If dont get the char, FIFO data stay and will not escape the WHILE
    Buffer[i]=data;
    i++;
    }
}
//MAIN
int main(void){
    // Clock configure at 50Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // UART configure
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_U1RX);            //PIN mux for UART
    GPIOPinConfigure(GPIO_PB1_U1TX);            //PIN mux for UART
    GPIOPinTypeUART(GPIO_PORTB_BASE, RX1_PIN |TX1_PIN);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,(UART_CONFIG_WLEN_8| UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    // Interrupt configure
//    IntMasterEnable();
 //   IntEnable(INT_UART1);
    //UARTIntEnable(UART1_BASE, UART_INT_RX| UART_INT_RT); //Receive and receive timeout in case of data don't come after 30 cycles
    UARTIntRegister(UART1_BASE, Receive_ISR);            //If don't have timeout, system will interrupt after receive 8 bits data
    // LED CONFIG
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE ,GPIO_PIN_1);
while (1)
    {
        UARTCharPut(UART1_BASE,'A');
        UARTCharPut(UART1_BASE,'T');
        UARTCharPut(UART1_BASE,'\r');
        UARTCharPut(UART1_BASE,'\n');
        while(UARTCharsAvail(UART1_BASE))
        {
            data_recv=UARTCharGet(UART1_BASE);
            if(data_recv!=0)
                data=1;
        }
    }
}


