/*
  File: Blinky.c

  To blink the onboard LED (D3) of the Phytec PCM-023 board
  for every 65ms.

*/

#include "timer.h"
#include "lpc2294.h"
#include "lpc2294_reg.h"
#include <stdio.h>
#include "hard.h"

// Module Externals -----------------------------------------------------------
#ifdef HARD_TEST_MODE_IRQ_CORE_AND_VIC
volatile unsigned short global_timer = 0;
#endif

extern void __disable_interrupts ();
extern void __enable_interrupts ();
extern void DefDummyInterrupt ();

// Module Functions Declarations -----------------------------------------------
void SimpleDelay (void);
void SimpleDelay2 (void);
void DisableInterrupts (void);

#define DisableInterrupts    __disable_interrupts()
#define EnableInterrupts   __enable_interrupts()

const char s1 [] = {"Primero\n"};
const char s2 [] = {"Segundo\n"};
const char s3 [] = {"Tercero\n"};
    
// Module Functions Definitions ------------------------------------------------
int main (void)
{
    // System initialization, this will map the exception vectors.
    LPC2294SystemInit();

    // Set up Gpio registers.  
    LPC2294InitPIO();

    // First disable interrupts (at core level)
    DisableInterrupts;

    // Setup interrupt controller.
    LPC2294InitVIC();
    LPC2294InitTimerInterrupt();
    // LPC2294InitTimerInterruptNonVectored();

    // Periodic timer initialization.
    LPC2294InitTimer();

    // Enable interrupts at core level
    EnableInterrupts;

    LED1_OFF;
    LED3_OFF;
    LED4_OFF;
    LED6_OFF;
    LED7_OFF;
    LED8_OFF;
    
    // Start periodic timer.
    LPC2294StartTimer();

    //Start the UART0
    LPC2294InitUART0();

    // Hard Test Starts from here. ---------------------------------------------
#ifdef HARD_TEST_MODE_USART0_RX
    //enciendo usart1
    Usart1Config();
    char buff_local [128] = { 0 };
    unsigned char readed = 0;

    while(1)
    {
        Wait_ms(3000);
        if (usart1_have_data)
        {
            readed = ReadUsart1Buffer(buff_local, 127);
            *(buff_local + readed) = '\n';    //cambio el '\0' por '\n' antes de enviar
            *(buff_local + readed + 1) = '\0';    //ajusto el '\0'
            Usart1Send(buff_local);
        }
    }    
#endif // HARD_TEST_MODE_USART0_RX


#ifdef HARD_TEST_MODE_USART0_TX    
    unsigned char a_enviar = 2;
    char seq [10] = { 0 };
    LPC2294UART0TxString("Empiezo con 2\n");
    
    while (1)
    {
        Wait_ms(330);
        
        LED3_ON;
        // LPC2294UART0TxByte('M');
        // LPC2294UART0TxString("Marianito\n");
        switch (a_enviar)
        {
        case 0:
            LPC2294UART0TxString((char *)s1);
            Wait_ms(10);
            sprintf(seq, "-> %d\n", a_enviar);
            LPC2294UART0TxString(seq);
            a_enviar++;
            break;

        case 1:
            LPC2294UART0TxString((char *)s2);
            Wait_ms(10);
            sprintf(seq, "-> %d\n", a_enviar);
            LPC2294UART0TxString(seq);
            a_enviar++;            
            break;

        case 2:
            LPC2294UART0TxString((char *)s3);
            Wait_ms(10);
            sprintf(seq, "-> %d\n", a_enviar);
            LPC2294UART0TxString(seq);
            a_enviar = 0;
            break;
        }
        LED3_OFF;
    }
#endif    // HARD_TEST_MODE_USART0_TX


#ifdef HARD_TEST_MODE_IRQ_CORE_AND_VIC
    //apagar y prender ints en el core cada 1 segundo
#define WITH_CORE_INTS    0
#define ONLY_TIMER    1
        
    unsigned char int_state = WITH_CORE_INTS;
    
    while (1)
    {
        switch (int_state)
        {
        case WITH_CORE_INTS:
            if (global_timer > 1000)
            {
                DisableInterrupts;
                // VICIntEnClear |= VIC_TIMER0;
                LED3_OFF;
                int_state = ONLY_TIMER;
                global_timer = 0;
            }
            break;

        case ONLY_TIMER:
            if (global_timer < 1000)
            {
                if (T0IR & 0x01)    //hubo match
                {
                    T0IR |= 0x01;    //blank int line
                    if (LED1)
                        LED1_OFF;
                    else
                        LED1_ON;

                    global_timer++;
                }
            }
            else
            {
                LED1_OFF;
                int_state = WITH_CORE_INTS;
                global_timer = 0;
                EnableInterrupts;
                // VICIntEnable |= VIC_TIMER0;
            }
            break;
        }
    }
#endif    // HARD_TEST_MODE_IRQ_CORE_AND_VIC


#ifdef HARD_TEST_MODE_LED
    // Loop forever.
    while (1)
    {
        LED1_ON;
        LED3_ON;
        LED4_ON;
        LED6_ON;
        LED7_ON;
        LED8_ON;
        Wait_ms(1000);
        // SimpleDelay();
        LED1_OFF;
        LED3_OFF;
        LED4_OFF;
        LED6_OFF;
        LED7_OFF;
        LED8_OFF;
        Wait_ms(3000);
        // SimpleDelay();
    }
#endif // HARD_TEST_MODE_LED

    
    return 0;
}

void IRQHandler (void)
{
}

void sysInit (void)
{
}

void SimpleDelay (void)
{
    for (unsigned char i = 0; i < 255; i++)
    {
        asm (	"nop \n\t"
                "nop \n\t"
                "nop \n\t" );
    }
}


void SimpleDelay2 (void)
{
    for (unsigned short i = 0; i < 65000; i++)
    {
        asm (	"nop \n\t"
                "nop \n\t"
                "nop \n\t" );
    }
}


/*
There is no such instruction, you have to change a bit in a cpsr
register. You have to be in a privileged state to do so.
And you have to make sure that the change took place, in case another
task is enabling interrupts at the 'same' time.

Here's the functions that I use:

disable_interrupts:
dint:
critical_Begin:
mrs	r0, cpsr
orr	r1, r0, #IRQ_DISABLED // keep r0 to return	
msr	cpsr_c, r1
mrs	r2, cpsr
and	r2, r2, #IRQ_DISABLED
cmp	r2, #IRQ_DISABLED
bne	=critical_Begin
bx	lr

enable_interrupts:
eint:
critical_End:
stmfd	sp!, {r0}
mrs	r0, cpsr
bic	r0,r0,#IRQ_DISABLED
msr	cpsr_fsxc, r0
ldmfd	sp!, {r0}
bx	lr
*/
