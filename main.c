/*	STM8 EXAMPLE PROGRAM
 *
 */
#include <iostm8s.h>

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
	rim();				/* authorize interrupts */
	for (;;)			/* loop */
		outch(getch());		/* get and put a char */
	}
