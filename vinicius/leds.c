#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LED_1 2
#define LED_2 4
#define LED_3 16

void config_leds(){
    // Configuração do Timer
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_config);

    // Configuração do Canal 0 - led da placa
    ledc_channel_config_t channel_config = {
        .gpio_num = LED_1,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config);

    // Configuração do Canal 1 - led vermelho
    ledc_channel_config_t channel_config_1 = {
        .gpio_num = LED_2,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config_1);

    // Configuração do Canal 2 - led verde
    ledc_channel_config_t channel_config_2 = {
        .gpio_num = LED_3,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config_2);
}

void brighten(int led){
    switch (led){
        case LED_1:
            for(int i = 0; i < 255; i++){
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
        case LED_2:
            for(int i = 0; i < 255; i++){
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
        case LED_3:
            for(int i = 0; i < 255; i++){
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
        default:
            break;
    }
}

void dimmer(int led){
    switch (led){
        case LED_1:
            for(int i = 255; i > 0; i--){
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
        case LED_2:
            for(int i = 255; i > 0; i--){
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
        case LED_3:
            for(int i = 255; i > 0; i--){
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, i);
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            break;
        default:
            break;
    }
}