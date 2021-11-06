#include <config.h>

#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

static unsigned GPIO_BASE = 0x3f200000;
static unsigned UART_BASE = 0x3f201000;
static unsigned FUARTCLK = 48000000;

volatile unsigned int *gpfsel0;
volatile unsigned int *gpfsel1;

volatile unsigned int *uartdr;
volatile unsigned int *uartcr;
volatile unsigned int *uartlcrh;
volatile unsigned int *uartlintbr;
volatile unsigned int *uartlfbr;
volatile unsigned int *uartris;
volatile unsigned int *uarticr;
volatile unsigned int *uartmis;
volatile unsigned int *uartimsc;
volatile unsigned int *uartfr;
volatile unsigned int *uartitop;

void initPtrs();
void gpioConfig();
void uartConfig(int BaudRate);

/**
 * Initializes the pointers of the UART driver
 */
void initPtrs()
{
	int fd = -1;

	if ((fd = open("/dev/mem", O_RDWR, 0)) == -1)
	{

		err(1, "Error opening /dev/mem");
	}

	gpfsel0 = (unsigned int *)mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);

	uartdr = (unsigned int *)mmap(0, getpagesize(), PROT_WRITE, MAP_SHARED, fd, UART_BASE);

	if (gpfsel0 == MAP_FAILED | uartdr == MAP_FAILED)
	{

		errx(1, "Error during mapping GPIO");
	}

	gpfsel1 = gpfsel0 + 0x1;

	uartcr = uartdr + 0xC;
	uartlcrh = uartdr + 0xB;
	uartlintbr = uartdr + 0x9;
	uartlfbr = uartdr + 0xA;
	uartris = uartdr + 0xF;
	uarticr = uartdr + 0x11;
	uartmis = uartdr + 0x10;
	uartimsc = uartdr + 0xE;
	uartfr = uartdr + 0x6;
	uartitop = uartdr + 0x22;

	return;
}

/**
 * Sets pins 14 and 15 as read and write by using alt mode 0
 */
void gpioConfig()
{
	*gpfsel1 = *gpfsel1 | 0x24000;

	return;
}

/**
 * Sets the necesary values to the UART pointers
 * BaudRate used in the communication
 */
void uartConfig(int BaudRate)
{
	*uartcr = *uartcr & 0xFFFFFFF0;

	*uartcr = *uartcr | 0x300;

	*uarticr = *uarticr | 0xFF;

	*uartlcrh = *uartlcrh & 0xffffff6f;

	*uartlintbr = (unsigned int)(FUARTCLK / (16 * BaudRate));
	*uartlfbr = (unsigned int)((FUARTCLK % (16 * BaudRate)) * 64 / (16 * BaudRate));

	*uartimsc = *uartimsc | 0x50;

	*uartcr = *uartcr | 0x1;

	return;
}

/**
 * Sends an 8 bit number to the UART terminal
 * data to send
 */
void uartSend(char data)
{
	*uartdr = data;

	return;
}

/**
 * Receives an 8 bit number from the UART terminal 
 */
int uartReceive()
{
	return *uartdr;
}

/**
 * Initializes the UART driver
 */
void initializeUart()
{
	initPtrs();
	gpioConfig();
	uartConfig(9600);
	*uartitop = *uartitop | 0x400;

	return;
}

void restartUart()
{
	gpioConfig();
	uartConfig(9600);
	*uartitop = *uartitop | 0x400;

	return;
}