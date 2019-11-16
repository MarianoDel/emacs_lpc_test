/*
  File: LPC2294.c

  To provide the lpc2294 specific uart, i/o port and timer interface
  function.

*/

#include "lpc2294.h"
#include "lpc2294_reg.h"


#define BAUDRATE    9600
#define BAUDRATEDIVISOR (PCLKFREQ/(BAUDRATE*16))

// Module Globals --------------------------------------------------------------
volatile unsigned short wait_ms_counter = 0;


// Module Private Functions ----------------------------------------------------
// Pointers to interrupt callback functions.
static void (*uart0rx_function)(unsigned char);
static void (*uart0tx_function)(void);

static void TimerInterruptHandler(void) __attribute__ ((interrupt ("IRQ")));
static void DefDummyInterrupt(void) __attribute__ ((interrupt ("IRQ")));
// void DefDummyInterrupt(void) __attribute__ ((interrupt ("IRQ")));

//
// Interrupt handlers.
//

//Dummy interrupt handler, called as default in irqHandler() if no other
//vectored interrupt is called.
static void DefDummyInterrupt(void)
// void DefDummyInterrupt(void)    
{
    if (LED6)
        LED6_OFF;
    else
        LED6_ON;

    T0IR = 0xff; // Clear timer 0 interrupt line.
    VICVectAddr = 0;    // Reset VIC logic
}

// Timer interrupt handler
static void TimerInterruptHandler(void)
{
    T0IR = 0xff; // Clear timer 0 interrupt line.
    // if (LED3)
    //     LED3_OFF;
    // else
    //     LED3_ON;

    if (wait_ms_counter)
        wait_ms_counter--;
    
    VICVectAddr = 0;    // Reset VIC logic
}


void Wait_ms (unsigned short tms)
{
    wait_ms_counter = tms;
    while (wait_ms_counter);
}


//UART0 interrupt handler
static void UART0Interrupt(void)
{
    unsigned char status = 0;
    status = (unsigned char) (U0IIR >> 1);
    status &= 0x07;
    
  // switch(U0IIR_bit.IID)
  switch(status)      
  {
  case 0x1:  //THRE interrupt
    (*uart0tx_function)(); //Call tx buffer empty callback function
    break;
  case 0x2:  //Receive data available
    (*uart0rx_function)(U0RBR);    //Call received byte callback function
    break;
  case 0x0:  //Modem interrupt
  case 0x3:  //Receive line status interrupt (RDA)
  case 0x6:  //Character time out indicator interrupt (CTI)
  default:
    break;
  }
}

// IRQ exception handler. Calls the interrupt handlers.
// #pragma vector=0x18
// __irq __arm void irq_handler(void)
// {
//   void (*interrupt_function)();
//   unsigned int vector;

//   vector = VICVectAddr;   // Get interrupt vector.
//   interrupt_function = (void(*)())vector;
//   (*interrupt_function)();  // Call vectored interrupt function.

//   VICVectAddr = 0;        // Clear interrupt in VIC.
// }


//
// System initialization.
//
void LPC2294SystemInit(void)
{
    MEMMAP = 1;
}

//
// Interrupt controller initalization.
//
#define VICIntEnClear VICIntEnClr

// Reset all interrupts
void LPC2294InitVIC()
{
  // Setup interrupt controller.
  VICProtection = 0;
  // Disable all interrupts
  VICIntEnClear = 0xffffffff;
  VICDefVectAddr = (unsigned int)&DefDummyInterrupt;
}


void LPC2294InitTimerInterruptNonVectored (void)
{
    VICIntSelect &= ~VIC_TIMER0;    // IRQ on timer 0 line.
    VICIntEnable |= VIC_TIMER0;    // Enable timer 0 interrupt.    

    //dummy addr already setted
    VICVectAddr = 0;    // Reset VIC logic    
}


// Setup Timer interrupt
void LPC2294InitTimerInterrupt(void)
{
    VICIntSelect &= ~VIC_TIMER0; // IRQ on timer 0 line.
    VICVectAddr1 = (unsigned int)&TimerInterruptHandler;
    VICVectCntl1 = VIC_VEC_TIMER0 | VIC_VEC_ENABLE_MASK;    // Enable vector interrupt for timer 0.
    VICIntEnable |= VIC_TIMER0;    // Enable timer 0 interrupt.
    VICVectAddr = 0;    // Reset VIC logic    
}

// Setup UART interrupt
void LPC2294InitUART0Interrupt(void(*uart0rx_func)(unsigned char),
                               void(*uart0tx_func)())
{
  // Setup uart1 callback functions.
  uart0rx_function = uart0rx_func;
  uart0tx_function = uart0tx_func;

  VICIntSelect &= ~VIC_UART0;  // IRQ on UART0.
  VICVectAddr5 = (unsigned int)&UART0Interrupt;
  VICVectCntl5 = VIC_VEC_UART0 | VIC_VEC_ENABLE_MASK; // Enable vector interrupt for UART0.
  VICIntEnable |= VIC_UART0;    // Enable UART 0 interrupt.
}

//
// Timer functions.
//

// Setup Timer
void LPC2294InitTimer()
{
  T0TCR = 0; // Disable timer 0.
  T0TCR = 2; // Reset timer 0.
  T0TCR = 0;
  T0IR = 0xff; // Clear timer 0 interrupt line.
  T0PR = 0; // Prescaler is set to no division.
  T0MR0 = PCLKFREQ / 1000; // Count up to this value. To generate 1000KHz.
  T0MCR = 3; // Reset and interrupt on MR0 (match register 0).
  T0CCR = 0; // Capture is disabled.
  T0EMR = 0; // No external match output.
}

// Start Timer
void LPC2294StartTimer()
{
  T0TCR = 1; // Enable timer 0.
}

//
// Parallel I/O functions.
//

#define PINSEL2_NA_MASK    0x0FF3E9FC
void LPC2294InitPIO()
{
    //P1.31-26 gpio
    // PINSEL2 &= (0xFFFFFFFB & PINSEL2_NA_MASK);   //esto me traba el programa 
    //P3.22 .. 2 to gpio
    // PINSEL2 &= (0xF1FFFFFF & PINSEL2_NA_MASK);

    // PINSEL0_bit.P0_8 = 0x00;    //arranca en 0
    // IO0DIR = 0x00000100;      /* P0.8 defined as Output */

    //inicializo P0.27 y P1.16 como outputs
    // IO0DIR = 0x08000001;    //P0.0 y P0.27 output
    // IO1DIR = 0x80010000;    //P1.16 y P1.31 output
    // IO3DIR = 0x00600000;    //P3.21 y P3.22 output

    //inicializo pines de los LEDs
    IO0DIR = 0x01C00000;    //P0.22, P0.23, P0.24 output
    IO1DIR = 0x000D0000;    //P1.16, P1.18, P1.19 output
}

//
// LED output drivers.
//

void LPC2294LedClear(void)
{
    IO0CLR |= 0x01C00000;
    IO1CLR |= 0x000D0000; 
}

void LPC2294LedSet(void)
{
    IO0SET |= 0x01C00000;
    IO1SET |= 0x000D0000; 
}

//
// UART functions
//

void LPC2294InitUART0()
{
  //Set pins for use with UART
  // PINSEL0_bit.P0_0 = 0x01;                  /* Enable RxD0 and TxD0              */
  // PINSEL0_bit.P0_1 = 0x01;                  /* Enable RxD0 and TxD0              */

  PINSEL0 |= 0x01;    //UART0 TXD
  PINSEL0 |= 0x04;    //UART0 RXD

  //Set the FIFO enable bit in the FCR register. This bit must be set for
  //proper UART operation.
  U0FCR = 1;

  //Set baudrate
  // U0LCR_bit.DLAB = 1;
  U0LCR |= 0x80;
  U0DLL = BAUDRATEDIVISOR & 0x00ff;
  U0DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
  // U0LCR_bit.DLAB = 0;
  U0LCR &= 0x7F;

  //Set mode
  // U0LCR_bit.WLS = 0x3;   //8 bit word length
  U0LCR |= 0x03;
  // U0LCR_bit.SBS = 0x0;   //1 stop bit
  U0LCR &= 0xFB;
  // U0LCR_bit.PE  = 0x0;   //No parity
  U0LCR &= 0xCF;

  //Enable UART0 interrupts
  // U0IER_bit.RDAIE  = 1;  //Enable byte received interrupt
  U0IER |= 0x01;
  // U0IER_bit.THREIE = 1;  //Enable tx buf empty interrupt
  U0IER |= 0x02;
}

//Transmits one byte via UART0
void LPC2294UART0TxByte(unsigned char byte)
{
  // while(U0LSR_bit.THRE != 1);
    while(!(U0LSR & 0x10));
    U0THR = byte;
}

