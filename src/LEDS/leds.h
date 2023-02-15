#ifndef LEDS_H
#define LEDS_H

#define LED_PLACA       2
#define LED_VERMELHO    4
#define LED_VERDE       16

void config_leds();

void set_brightness(int led);

#endif