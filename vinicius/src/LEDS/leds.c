#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LED_PLACA 2
#define LED_VERMELHO 4
#define LED_VERDE 16

extern int led_verd;
extern int led_verm;
extern int led_placa;

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
        .gpio_num = LED_PLACA,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config);

    // Configuração do Canal 1 - led vermelho
    ledc_channel_config_t channel_config_1 = {
        .gpio_num = LED_VERMELHO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config_1);

    // Configuração do Canal 2 - led verde
    ledc_channel_config_t channel_config_2 = {
        .gpio_num = LED_VERDE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_config_2);
    ledc_fade_func_install(0);
}

void set_brightness(int led){
    switch (led){
        case LED_PLACA:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, led_placa);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            break;
        case LED_VERMELHO:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, led_verm);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
            break;
        case LED_VERDE:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, led_verd);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
            break;
        default:
            break;
    }
}
