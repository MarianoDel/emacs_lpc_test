#ifndef __LPC2294_H__
#define __LPC2294_H__

//XTAL frequency in Hz
#define XTALFREQ 10000000

#define PCLKFREQ (XTALFREQ/4)



//Defines some know GPIOs
//LED6 on P0.22
#define LED6    (IO0PIN & 0x00400000)
#define LED6_ON    (IO0SET |= 0x00400000)
#define LED6_OFF    (IO0CLR |= 0x00400000)

//LED7 on P0.23
#define LED7    (IO0PIN & 0x00800000)
#define LED7_ON    (IO0SET |= 0x00800000)
#define LED7_OFF    (IO0CLR |= 0x00800000)

//LED4 on P0.24
#define LED4    (IO0PIN & 0x01000000)
#define LED4_ON    (IO0SET |= 0x01000000)
#define LED4_OFF    (IO0CLR |= 0x01000000)

//LED1 on P1.16
#define LED1    (IO1PIN & 0x00010000)
#define LED1_ON    (IO1SET |= 0x00010000)
#define LED1_OFF    (IO1CLR |= 0x00010000)

//LED3 on P1.18
#define LED3    (IO1PIN & 0x00040000)
#define LED3_ON    (IO1SET |= 0x00040000)
#define LED3_OFF    (IO1CLR |= 0x00040000)

//LED8 on P1.19
#define LED8    (IO1PIN & 0x00080000)
#define LED8_ON    (IO1SET |= 0x00080000)
#define LED8_OFF    (IO1CLR |= 0x00080000)



void LPC2294InitTimerInterruptNonVectored (void);
void Wait_ms (unsigned short);

void LPC2294SystemInit (void);
void LPC2294InitPIO (void);
void LPC2294InitTimer (void);
void LPC2294StartTimer (void);
void LPC2294InitVIC (void);
void LPC2294InitTimerInterrupt (void);
void LPC2294InitUART0Interrupt (void);

void LPC2294LedSet(void);
void LPC2294LedClear(void);

void LPC2294InitUART0(void);
void LPC2294UART0TxByte(unsigned char byte);
void LPC2294UART0TxString (char *);
void Usart0_Reset (void);

#endif  /* __LPC2294_H__ */
