#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * Enables a pin in the Raspberry Pi
 * Pin is the GPIO pin
 */
int enablePin(int pin) {

    char pinC[5];
    sprintf(pinC, "%d", pin);

    FILE *fd = fopen("/sys/class/gpio/export", "wb");

    if (fd == NULL) {
        printf("Error enabling the pin %d\n", pin);
        exit(1);
    }

    fwrite(pinC, strlen(pinC), 1, fd);
    fclose(fd);

    return 0;

}

/**
 * Enables a pin in the Raspberry Pi
 * Pin is the GPIO pin
 */
int disablePin(int pin) {

    char pinC[5];
    sprintf(pinC, "%d", pin);

    FILE *fd = fopen("/sys/class/gpio/unexport", "wb");

    if (fd == NULL) {
        printf("Error disabling the pin %d\n", pin);
        exit(1);
    }

    fwrite(pinC, strlen(pinC), 1, fd);
    fclose(fd);

    return 0;

}

/**
 * Sets the pin to input or output
 */
int pinMode(int pin, char* mode) {

    char pinC[5];
    sprintf(pinC, "%d", pin);

    char* filePath = malloc(strlen("/sys/class/gpio/gpio") + strlen(pinC) + strlen("/direction") + 1);
    strcpy(filePath, "/sys/class/gpio/gpio");
    strcat(filePath, pinC);
    strcat(filePath, "/direction");

    FILE *fd = fopen(filePath, "wb");

    if (fd == NULL) {
        printf("Error changing the pin mode of pin %d\n", pin);
        exit(1);
    }

    fwrite(mode, strlen(mode), 1, fd);
    fclose(fd);

    free(filePath);

    return 0;
}

/**
 * Writes a value to a pin
 */
int digitalWrite(int pin, int value) {

    char valueC[5];
    sprintf(valueC, "%d", value);

    char pinC[5];
    sprintf(pinC, "%d", pin);

    char* filePath = malloc(strlen("/sys/class/gpio/gpio") + strlen(pinC) + strlen("/value") + 1);
    strcpy(filePath, "/sys/class/gpio/gpio");
    strcat(filePath, pinC);
    strcat(filePath, "/value");

    FILE *fd = fopen(filePath, "wb");

    if (fd == NULL) {
        printf("Error writing pin %d\n", pin);
        exit(1);
    }

    fwrite(valueC, strlen(valueC), 1, fd);
    fclose(fd);

    free(filePath);

    return 0;
    
}

/**
 * Reads a value from a pin
 */
int digitalRead(int pin) {

    char valueC[5];

    char pinC[5];
    sprintf(pinC, "%d", pin);

    char* filePath = malloc(strlen("/sys/class/gpio/gpio") + strlen(pinC) + strlen("/value") + 1);
    strcpy(filePath, "/sys/class/gpio/gpio");
    strcat(filePath, pinC);
    strcat(filePath, "/value");

    FILE *fd = fopen(filePath, "rb");

    if (fd == NULL) {
        printf("Error reading pin %d\n", pin);
        exit(1);
    }

    fread(valueC, sizeof(valueC), 1, fd);
    fclose(fd);

    free(filePath);

    return atoi(valueC);
}

/**
 * Makes a pin blink
 */
int blink(int pin, int freq, int duration) {

    double freqTime = 1000000.0 / freq;
    time_t startTime = time(0);

    while ((time(0) - startTime) < duration) {
        digitalWrite(pin, 1);
        usleep(freqTime);
        digitalWrite(pin, 0);
        usleep(freqTime);
    }

    return 0;

}