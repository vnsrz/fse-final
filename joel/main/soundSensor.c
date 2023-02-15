#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_setup.h"

#define SOUDNSENSOR 15 

void som_setup() {
    pinMode(SOUDNSENSOR, GPIO_INPUT_PULLDOWN);
}

int som_get() {
    int som = 0 + digitalRead(SOUDNSENSOR);
    return som;
}