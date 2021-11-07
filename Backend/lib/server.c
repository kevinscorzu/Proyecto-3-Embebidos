#include <config.h>

#include <gpio.h>
#include <uart.h>

#include <ulfius.h>
#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

#define sensor1GPIO 12
#define sensor2GPIO 17
#define sensor3GPIO 27
#define sensor4GPIO 22

#define greenLEDGPIO 24
#define redLEDGPIO 23

#define PORT 9090

pthread_t sensorThread;
pthread_mutex_t stopMutex;
pthread_mutex_t sensorMutex;
sem_t *semStop;

int getParkingState(const struct _u_request *request, struct _u_response *response, void *user_data);
int stopServer(const struct _u_request *request, struct _u_response *response, void *user_data);
int allowCORS(const struct _u_request *request, struct _u_response *response, void *user_data);
void enablePins();
void disablePins();
void *readSensors(void *args);
void readSensorsAux();
void readSensorsAux2();
void readSensorsAux3(int sensorGPIO);
void flushUart();

int sensor1State = 1;
int sensor2State = 1;
int sensor3State = 1;
int sensor4State = 1;

int greenLEDState = 1;
int redLEDState = 0;

int stopThread = 0;

int assignedSpace = 0;

/**
 * Function that starts the server
 */
void startServer()
{
    enablePins();

    semStop = sem_open("semStop", O_CREAT, 0644, 0);

    if (semStop == SEM_FAILED)
    {
        printf("Failed to initialize semaphore\n");
        exit(1);
    }

    if (pthread_mutex_init(&sensorMutex, NULL) != 0)
    {
        printf("Failed to initialize the sensor mutex\n");
        exit(1);
    }

    if (pthread_mutex_init(&stopMutex, NULL) != 0)
    {
        printf("Failed to initialize the thread mutex\n");
        exit(1);
    }

    pthread_create(&sensorThread, NULL, readSensors, NULL);
    pthread_detach(sensorThread);

    struct _u_instance instance;

    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK)
    {
        printf("Error ulfius_init_instance, abort\n");
        return;
    }

    ulfius_add_endpoint_by_val(&instance, "OPTIONS", NULL, "*", 0, &allowCORS, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/Api/ParkingState", NULL, 0, &getParkingState, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/Api/Close", NULL, 0, &stopServer, NULL);

    if (ulfius_start_framework(&instance) == U_OK)
    {
        printf("Start server on port: %d\n", instance.port);

        if (sem_wait(semStop) < 0)
        {
            printf("Error while waiting for the stop semaphore\n");
            exit(1);
        }

        sleep(1);
    }
    else
    {
        printf("Error starting server\n");
    }

    printf("Server ended\n");

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    pthread_mutex_destroy(&sensorMutex);
    pthread_mutex_destroy(&stopMutex);

    if (sem_close(semStop) != 0)
    {
        printf("Error while closing the stop semaphore\n");
        exit(1);
    }

    if (sem_unlink("semStop") < 0)
    {
        printf("Error while unlinking the stop semaphore\n");
        exit(1);
    }

    disablePins();

    return;
}

/**
 * Function that returns the states of the parking lot
 */
int getParkingState(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    json_t *parkingJson = json_object();

    pthread_mutex_lock(&sensorMutex);

    json_object_set_new(parkingJson, "space1", json_integer(sensor1State));
    json_object_set_new(parkingJson, "space2", json_integer(sensor2State));
    json_object_set_new(parkingJson, "space3", json_integer(sensor3State));
    json_object_set_new(parkingJson, "space4", json_integer(sensor4State));

    json_object_set_new(parkingJson, "greenLed", json_integer(greenLEDState));
    json_object_set_new(parkingJson, "redLed", json_integer(redLEDState));

    json_object_set_new(parkingJson, "assignedSpace", json_integer(assignedSpace));

    pthread_mutex_unlock(&sensorMutex);

    char *response_body = json_dumps(parkingJson, JSON_ENCODE_ANY);

    ulfius_set_string_body_response(response, 200, response_body);

    json_decref(parkingJson);
    free(response_body);

    return U_CALLBACK_CONTINUE;
}

/**
 * Function that stops the server
 */
int stopServer(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    printf("Closing server\n");
    ulfius_set_string_body_response(response, 200, "Ok");

    if (sem_post(semStop) < 0)
    {
        printf("Error while posting the stop semaphore\n");
        exit(1);
    }

    pthread_mutex_lock(&stopMutex);

    stopThread = 1;

    pthread_mutex_unlock(&stopMutex);

    return U_CALLBACK_CONTINUE;
}

/**
 * Function that handles CORS of the server
 */
int allowCORS(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");
    u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    u_map_put(response->map_header, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Bearer, Authorization");
    u_map_put(response->map_header, "Access-Control-Max-Age", "1800");
    return U_CALLBACK_COMPLETE;
}

/**
 * Function that initializes the pins of the Raspberry Pi
 */
void enablePins()
{
    printf("Initializing pins\n");

    enablePin(sensor1GPIO);
    enablePin(sensor2GPIO);
    enablePin(sensor3GPIO);
    enablePin(sensor4GPIO);

    enablePin(greenLEDGPIO);
    enablePin(redLEDGPIO);

    printf("Pins initialized successfully\n");

    printf("Configuring pins\n");

    pinMode(greenLEDGPIO, "out");
    pinMode(redLEDGPIO, "out");

    pinMode(sensor1GPIO, "in");
    pinMode(sensor2GPIO, "in");
    pinMode(sensor3GPIO, "in");
    pinMode(sensor4GPIO, "in");

    printf("Pins configured successfully\n");

    digitalWrite(greenLEDGPIO, 1);
    digitalWrite(redLEDGPIO, 0);

    return;
}

/**
 * Function that disables the pins of the Raspberry Pi
 */
void disablePins()
{
    printf("Disabling pins\n");

    disablePin(sensor1GPIO);
    disablePin(sensor2GPIO);
    disablePin(sensor3GPIO);
    disablePin(sensor4GPIO);

    disablePin(greenLEDGPIO);
    disablePin(redLEDGPIO);

    printf("Pins disabled successfully\n");

    return;
}

/**
 * Function that reads all the sensors in the parking lot
 */
void *readSensors(void *args)
{
    int readValue = 0;
    int carLeave = 0;

    while (1)
    {
        pthread_mutex_lock(&stopMutex);

        if (stopThread == 1)
        {
            break;
        }

        pthread_mutex_unlock(&stopMutex);

        readSensorsAux();

        carLeave = 0;

        readValue = digitalRead(sensor1GPIO);

        if (readValue == 1)
        {
            sensor1State = 1;
            carLeave = 1;
            readSensorsAux3(sensor1GPIO);
        }

        readValue = digitalRead(sensor2GPIO);

        if (readValue == 1)
        {
            sensor2State = 1;
            carLeave = 1;
            readSensorsAux3(sensor2GPIO);
        }

        readValue = digitalRead(sensor3GPIO);

        if (readValue == 1)
        {
            sensor3State = 1;
            carLeave = 1;
            readSensorsAux3(sensor3GPIO);
        }

        readValue = digitalRead(sensor4GPIO);

        if (readValue == 1)
        {
            sensor4State = 1;
            carLeave = 1;
            readSensorsAux3(sensor4GPIO);
        }

        if (carLeave == 1)
        {
            digitalWrite(greenLEDGPIO, 1);
            digitalWrite(redLEDGPIO, 0);

            greenLEDState = 1;
            redLEDState = 0;
        }

        pthread_mutex_unlock(&sensorMutex);
    }

    pthread_mutex_unlock(&stopMutex);

    return NULL;
}

/**
 * Auxiliary function
 */
void readSensorsAux()
{
    int readValue = 0;

    flushUart();

    uartSend(0x55);

    usleep(100000);

    readValue = uartReceive();

    pthread_mutex_lock(&sensorMutex);

    if (readValue < 70 && readValue != 0)
    {
        if (redLEDState == 1)
        {
            return;
        }

        if (sensor1State == 1)
        {
            assignedSpace = 1;
        }
        else if (sensor2State == 1)
        {
            assignedSpace = 2;
        }
        else if (sensor3State == 1)
        {
            assignedSpace = 3;
        }
        else if (sensor4State == 1)
        {
            assignedSpace = 4;
        }

        pthread_mutex_unlock(&sensorMutex);

        readSensorsAux2();

        pthread_mutex_lock(&sensorMutex);
    }

    return;
}

/**
 * Auxiliary function
 */
void readSensorsAux2()
{
    int readValue = 0;

    while (1)
    {
        pthread_mutex_lock(&sensorMutex);

        switch (assignedSpace)
        {
        case 1:
            readValue = digitalRead(sensor1GPIO);
            break;

        case 2:
            readValue = digitalRead(sensor2GPIO);
            break;

        case 3:
            readValue = digitalRead(sensor3GPIO);
            break;

        case 4:
            readValue = digitalRead(sensor4GPIO);
            break;
        }

        if (readValue == 1)
        {
            switch (assignedSpace)
            {
            case 1:
                sensor1State = 0;
                readSensorsAux3(sensor1GPIO);
                break;

            case 2:
                sensor2State = 0;
                readSensorsAux3(sensor2GPIO);
                break;

            case 3:
                sensor3State = 0;
                readSensorsAux3(sensor3GPIO);
                break;

            case 4:
                sensor4State = 0;
                readSensorsAux3(sensor4GPIO);
                break;
            }

            assignedSpace = 0;

            if (sensor1State == 0 && sensor2State == 0 && sensor3State == 0 && sensor4State == 0)
            {
                digitalWrite(greenLEDGPIO, 0);
                digitalWrite(redLEDGPIO, 1);

                greenLEDState = 0;
                redLEDState = 1;
            }

            break;
        }

        pthread_mutex_unlock(&sensorMutex);
    }

    pthread_mutex_unlock(&sensorMutex);

    return;
}

/**
 * Auxiliary function
 */
void readSensorsAux3(int sensorGPIO)
{
    int readValue = 0;

    while (1)
    {
        readValue = digitalRead(sensorGPIO);

        if (readValue == 0)
        {
            break;
        }
    }

    return;
}

void flushUart()
{
    int readValue = 0;
    int tries = 0;

    while (1)
    {
        uartSend(0x55);

        usleep(100000);

        readValue = uartReceive();

        if (90 < readValue && readValue < 150)
        {
            break;
        }
        else if (tries == 5)
        {
            restartUart();
            tries = 0;
        }
        else
        {
            tries += 1;
        }
    }

    return;
}