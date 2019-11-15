/*
  File: Blinky.c

  To blink the onboard LED (D3) of the Phytec PCM-023 board
  for every 65ms.

*/

#include "timer.h"
#include "lpc2294.h"
#include "LPC2294_reg.h"

extern void __disable_interrupts ();
extern void __enable_interrupts ();

// Module Functions Declarations -----------------------------------------------
void SimpleDelay (void);
void DisableInterrupts (void);

#define DisableInterrupts    __disable_interrupts()
#define EnableInterrupts   __enable_interrupts()


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
    // LPC2294InitTimerInterrupt(TimerBeat);
    LPC2294InitTimerInterruptNonVectored();

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
    
    while (1)
    {
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
        //     }

        //     if (LED1)
        //         LED1_OFF;
        //     else
        //         LED1_ON;

        //     T0IR |= 0x01;    //blank int line
        // }

        if (T0TC < (PCLKFREQ >> 1))
            LED7_ON;
        else
            LED7_OFF;
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
