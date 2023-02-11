#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LED_0 2
#define LED_1 4
#define LED_2 16

void liga(int led){
    if(led == 1){
        gpio_set_level(LED_2, 0);
        gpio_set_level(LED_1, 1);
    } else{
        gpio_set_level(LED_1, 0);
        gpio_set_level(LED_2, 1);
    }
}

void tarefa(void *params){
    while(true){
        ESP_LOGI("MAIN", "LED 1 %s\n", (char *)params);
        liga(0);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_LOGI("MAIN", "LED 2 %s\n", (char *)params);
        liga(1);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void app_main(void){
    esp_rom_gpio_pad_select_gpio(LED_0);
    esp_rom_gpio_pad_select_gpio(LED_1);
    esp_rom_gpio_pad_select_gpio(LED_2);

    gpio_set_direction(LED_0, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_2, GPIO_MODE_OUTPUT);

    xTaskCreate(&tarefa, "1", 2048, "teste", 1, NULL);
}