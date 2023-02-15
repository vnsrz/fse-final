#include <stdio.h>
#include "driver/gpio.h"

#define BOTAO           0
#define LED_PLACA       2
#define LED_VERMELHO    4
#define LED_VERDE       16
#define SOUNDSENSOR     17
#define DHT_GPIO        18
#define ROT_ENC_A_GPIO  19
#define ROT_ENC_B_GPIO  21

void setupGPIO(){
    DHT11_init(DHT_GPIO);
  
    esp_rom_gpio_pad_select_gpio(BOTAO);
    gpio_set_direction(BOTAO, GPIO_MODE_INPUT);

    esp_rom_gpio_pad_select_gpio(SOUNDSENSOR);
    gpio_set_direction(SOUNDSENSOR, GPIO_MODE_INPUT);
    gpio_pull_mode(SOUNDSENSOR, GPIO_PULLDOWN_ONLY);

    esp_rom_gpio_pad_select_gpio(LED_PLACA);
    gpio_set_direction(LED_PLACA, GPIO_MODE_OUTPUT);

    esp_rom_gpio_pad_select_gpio(LED_VERDE);
    gpio_set_direction(LED_VERDE, GPIO_MODE_OUTPUT);

    esp_rom_gpio_pad_select_gpio(LED_VERMELHO);
    gpio_set_direction(LED_VERMELHO, GPIO_MODE_OUTPUT);
}