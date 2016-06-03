/*	STM8 EXAMPLE PROGRAM
 *
 */
#include <iostm8s.h>
#include "defines.h"

#define SIZE	64		/* buffer size */
#define TRDE	0x80		/* transmit ready bit */

/*	Authorize interrupts.
 */
#define rim() _asm("rim")

/*	Some variables
 */
char buffer[SIZE];		/* reception buffer */
char *ptlec;			/* read pointer */
char *ptecr;			/* write pointer */
char led;

_Bool _LED PORTD:4;

/*	Character reception.
 *	Loops until a character is received.
 */
char getch(void)
	{
	char c;				/* character to be returned */

	while (ptlec == ptecr)		/* equal pointers => loop */
		;
	c = *ptlec++;			/* get the received char */
	if (ptlec >= &buffer[SIZE])	/* put in in buffer */
		ptlec = buffer;
	return (c);
	}

/*	Send a char to the SCI.
 */
void outch(char c)
	{
	while (!(USART1_SR & TRDE))	/* wait for READY */
		;
	USART1_DR = c;			/* send it */
	}

/*	Timer1 overflow routine.
 *	This routine is called on interrupt.
 */	
	
@interrupt void tim1_ovf(void)
{
    led = !led;
    _LED = led;
}

/*	Character reception routine.
 *	This routine is called on interrupt.
 *	It puts the received char in the buffer.
 */
@interrupt void recept(void)
	{
	USART1_SR;			/* clear interrupt */
	*ptecr++ = USART1_DR;		/* get the char */
	if (ptecr >= &buffer[SIZE])	/* put it in buffer */
		ptecr = buffer;
	}

/*	Main program.
 *	Sets up the SCI and starts an infinite loop
 *	of receive transmit.
 */
void main(void)
	{
	ptecr = ptlec = buffer;		/* initialize pointers */
	USART1_BRR1 = 0xc9;		/* parameter for baud rate */
	USART1_CR1 = 0x00;		/* parameter for word length */
	USART1_CR2 = 0x2c;		/* parameters for interrupt */
	
	PD_DDR = 0b00010000;		/* PORTD.4 - output */
	CLK_PCKENR1 |= 0b10000000;	/* Enable Fmaster clock to Timer1 */
	TIM1_PSCRH = 0;
	TIM1_PSCRL = 0;
	TIM1_CR2 = 0;
	// Синхронизация как ведомый с периферией отключена
	TIM1_SMCR = 0;
	// Внешнее тактирование отключено
	TIM1_ETR = 0;
	// Прерывание по обновлению счетного регистра разрешено
	TIM1_IER = 0b00000001;
    
	TIM1_CR1 = 0b00000101;

	rim();				/* authorize interrupts */
	for (;;)			/* loop */
		outch(getch());		/* get and put a char */
	}
