/*
  File: LPC2294.c

  To provide the lpc2294 specific uart, i/o port and timer interface
  function.

*/

#include "lpc2294.h"
#include "lpc2294_reg.h"
#include "hard.h"

#define BAUDRATE    9600
#define BAUDRATEDIVISOR (PCLKFREQ/(BAUDRATE*16))

// Module Externals -----------------------------------------------------------
#ifdef HARD_TEST_MODE_IRQ_CORE_AND_VIC
extern volatile unsigned short global_timer;
#endif

extern volatile unsigned char usart0_have_data;
extern volatile unsigned char * prx0;

// Module Globals --------------------------------------------------------------
volatile unsigned short wait_ms_counter = 0;

#define SIZEOF_DATA    80
volatile unsigned char rx0buff[SIZEOF_DATA];


// Module Private Functions ----------------------------------------------------
static void TimerInterruptHandler (void) __attribute__ ((interrupt ("IRQ")));
static void DefDummyInterrupt (void) __attribute__ ((interrupt ("IRQ")));
static void UART0InterruptHandler (void) __attribute__ ((interrupt ("IRQ")));

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

#ifdef HARD_TEST_MODE_IRQ_CORE_AND_VIC
    if (LED3)
        LED3_OFF;
    else
        LED3_ON;

    global_timer++;
#endif
    
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
static void UART0InterruptHandler (void)
{
    unsigned char status = 0;
    unsigned char dummy = 0;
    status = (unsigned char) (U0IIR >> 1);
    status &= 0x07;
    
    // switch(U0IIR_bit.IID)
    switch(status)      
    {
    case 0x1:  //THRE interrupt
        // (*uart0tx_function)(); //Call tx buffer empty callback function
        break;
    case 0x2:  //Receive data available
        dummy = U0RBR;
        
        if (prx0 < &rx0buff[SIZEOF_DATA])
        {
            if ((dummy == '\n') || (dummy == '\r') || (dummy == 26))		//26 es CTRL-Z
            {
                *prx0 = '\0';
                usart0_have_data = 1;
            }
            else
            {
                *prx0 = dummy;
                prx0++;
            }
        }
        else
            prx0 = rx0buff;    //soluciona problema bloqueo con garbage

        break;
    case 0x0:  //Modem interrupt
    case 0x3:  //Receive line status interrupt (RDA)
    case 0x6:  //Character time out indicator interrupt (CTI)
    default:
        break;
    }
    VICVectAddr = 0;    // Reset VIC logic
}


void Usart0_Reset (void)
{
    usart0_have_data = 0;
    prx0 = rx0buff;
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
void LPC2294InitTimerInterrupt (void)
{
    VICIntSelect &= ~VIC_TIMER0; // IRQ on timer 0 line.
    VICVectAddr1 = (unsigned int)&TimerInterruptHandler;
    VICVectCntl1 = VIC_VEC_TIMER0 | VIC_VEC_ENABLE_MASK;    // Enable vector interrupt for timer 0.
    VICIntEnable |= VIC_TIMER0;    // Enable timer 0 interrupt.
    VICVectAddr = 0;    // Reset VIC logic    
}


// Setup UART interrupt
void LPC2294InitUART0Interrupt (void)
{
    VICIntSelect &= ~VIC_UART0;  // IRQ on UART0.
    VICVectAddr5 = (unsigned int)&UART0InterruptHandler;
    VICVectCntl5 = VIC_VEC_UART0 | VIC_VEC_ENABLE_MASK; // Enable vector interrupt for UART0.
    VICIntEnable |= VIC_UART0;    // Enable UART 0 interrupt.
    VICVectAddr = 0;    // Reset VIC logic    
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
// UART functions
//

void LPC2294InitUART0()
{
    //Set pins for use with UART
    PINSEL0 |= 0x01;    //UART0 TXD
    PINSEL0 |= 0x04;    //UART0 RXD

    //Set the FIFO enable bit in the FCR register. This bit must be set for
    //proper UART operation.
    U0FCR = 1;

    //Set baudrate
    U0LCR |= 0x80;
    U0DLL = BAUDRATEDIVISOR & 0x00ff;
    U0DLM = (BAUDRATEDIVISOR >> 8) & 0x00ff;
    U0LCR &= 0x7F;

    //Set mode
    U0LCR |= 0x03;       //8 bit word length
    U0LCR &= 0xFB;     //1 stop bit
    U0LCR &= 0xCF;     //No parity

    prx0 = rx0buff;
    
    //Enable UART0 interrupts
    U0IER |= 0x01;    //Enable byte received interrupt
    // U0IER |= 0x02;    //Enable tx buf empty interrupt

}

//Transmits one byte via UART0
void LPC2294UART0TxByte(unsigned char byte)
{
  // while(U0LSR_bit.THRE != 1);
    while(!(U0LSR & 0x20));
    U0THR = byte;
}


//Transmits strings via UART0
void LPC2294UART0TxString (char * s_byte)
{
    while (*s_byte != '\0')
    {
        while(!(U0LSR & 0x20));
        U0THR = *s_byte;
        s_byte++;
    }
}



