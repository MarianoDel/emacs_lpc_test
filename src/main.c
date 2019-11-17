/*
  File: Blinky.c

  To blink the onboard LED (D3) of the Phytec PCM-023 board
  for every 65ms.

*/

#include "timer.h"
#include "lpc2294.h"
#include "lpc2294_reg.h"

// Module Externals -----------------------------------------------------------
volatile unsigned short global_timer = 0;

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

    // First disable interrupts.
    DisableInterrupts;

    // Setup interrupt controller.
    LPC2294InitVIC();
    LPC2294InitTimerInterrupt();
    // LPC2294InitTimerInterruptNonVectored();

    // Periodic timer initialization.
    LPC2294InitTimer();

    // Enable interrupts.
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

    unsigned char a_enviar = 2;
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
            a_enviar++;
            break;

        case 1:
            LPC2294UART0TxString((char *)s2);
            a_enviar++;            
            break;

        case 2:
            LPC2294UART0TxString((char *)s3);
            a_enviar = 0;
            break;
        }
        LED3_OFF;

        // Wait_ms(50);
        
        // if (T0IR & 0x01)    //hubo match
        // {
        //     T0IR |= 0x01;    //blank int line
        //     if (LED1)
        //         LED1_OFF;
        //     else
        //         LED1_ON;
        // }

        // if (VICRawIntr & 0x10)    //hubo match en el VIC
        // {
        //     if (VICIRQStatus & 0x10)
        //     {
        //         if (LED7)
        //             LED7_OFF;
        //         else
        //             LED7_ON;

        //         //para que lo que sigue funcione, necesito quitar static
        //         //en lpc2294.c declaration & definition
        //         if (VICVectAddr == (unsigned int) &DefDummyInterrupt)
        //         {
        //             if (LED6)
        //                 LED6_OFF;
        //             else
        //                 LED6_ON;

        //             VICVectAddr = 0;
        //         }
        //     }

        //     if (LED1)
        //         LED1_OFF;
        //     else
        //         LED1_ON;

        //     T0IR |= 0x01;    //blank int line
        // }
        
        // if (T0TC < (PCLKFREQ >> 1))
        //     LED7_ON;
        // else
        //     LED7_OFF;

        // while (1)
        // {
        //     //disable int
        //     VICIntEnClear |= VIC_TIMER0;
        //     SimpleDelay2();
        //     VICIntEnable |= VIC_TIMER0;
        //     SimpleDelay2();
        // }
            
            
            

        //apagar y prender ints en el core cada 1 segundo
#define WITH_CORE_INTS    0
#define ONLY_TIMER    1
        
        // unsigned char int_state = WITH_CORE_INTS;

        // while (1)
        // {
        //     switch (int_state)
        //     {
        //     case WITH_CORE_INTS:
        //         if (global_timer > 1000)
        //         {
        //             DisableInterrupts;
        //             // VICIntEnClear |= VIC_TIMER0;
        //             LED3_OFF;
        //             int_state = ONLY_TIMER;
        //             global_timer = 0;
        //         }
        //         break;

        //     case ONLY_TIMER:
        //         if (global_timer < 1000)
        //         {
        //             if (T0IR & 0x01)    //hubo match
        //             {
        //                 T0IR |= 0x01;    //blank int line
        //                 if (LED1)
        //                     LED1_OFF;
        //                 else
        //                     LED1_ON;

        //                 global_timer++;
        //             }
        //         }
        //         else
        //         {
        //             LED1_OFF;
        //             int_state = WITH_CORE_INTS;
        //             global_timer = 0;
        //             EnableInterrupts;
        //             // VICIntEnable |= VIC_TIMER0;
        //         }
        //         break;
        //     }
        // }
        
    }
        
    // Loop forever.
    while (1)
    {
        // LPC2294LedSet();
        LED1_ON;
        LED3_ON;
        LED4_ON;
        LED6_ON;
        LED7_ON;
        LED8_ON;
        Sleep(1000);
        // SimpleDelay();
        LED1_OFF;
        LED3_OFF;
        LED4_OFF;
        LED6_OFF;
        LED7_OFF;
        LED8_OFF;
        Sleep(3000);
        // SimpleDelay();
        
    }

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
