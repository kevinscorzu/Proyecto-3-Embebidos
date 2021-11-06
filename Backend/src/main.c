#include <uart.h>
#include <server.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * Main program function
 */
int main()
{
    printf("Initializing UART\n");
    initializeUart();
    printf("UART initialized\n");

    startServer();

    return 0;
}