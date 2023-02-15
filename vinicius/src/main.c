#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_http_client.h"
#include "esp_http_server.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "DHT11/dht11.h"
#include "WIFI/wifi.h"
#include "MQTT/mqtt.h"
#include "LEDS/leds.h"
#include "NVS/nvs.h"

#define BOTAO 0
#define DHT_GPIO 18

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

float temperatura = 0, totalTemp = 0;
float umidade = 0, totalUmid = 0;
int led_verm = 0, led_verd = 0;
int led_placa = 0, botao = 0;
int status, counter = 1;

void conectadoWifi(void * params){
    while(true){
        if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)){
            mqtt_start();
        }
    }
}

void trataComunicacaoComServidor(void * params){
    char mensagem[50];
    int lvm, lvd, lp;
    
    if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)){
        while(true){
            botao = !gpio_get_level(BOTAO);

            if(led_verm >= 1) lvm = 1;
            else lvm = 0;
        
            if(led_verd >= 1) lvd = 1;
            else lvd = 0;

            if(led_placa >= 1) lp = 1;
            else lp = 0;

            sprintf(mensagem, "{ \"led_vermelho\": \"%d\", \"led_verde\": \"%d\" }", lvm, lvd);
            mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

            sprintf(mensagem, "{ \"botao\": \"%d\", \"led_placa\": \"%d\" }", botao, lp);
            mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

            if(status == 0 && temperatura != 0 && umidade != 0){
                sprintf(mensagem, "{ \"temperatura\": \"%.2f\", \"umidade\": \"%.2f\" }", temperatura, umidade);
                mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void controlaLeds(void *params){
    while(true){
        // printf("led_placa = %d\nled_verm = %d\nled_verd = %d\n",led_placa,led_verm,led_verd);
        set_brightness(LED_PLACA);
        set_brightness(LED_VERDE);
        set_brightness(LED_VERMELHO);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void coletaTemp(void *params){
    float temp, umid;
    while(true){
        temp = DHT11_read().temperature;
        status = DHT11_read().status;
        umid = DHT11_read().humidity;

        if(temp > 1 && umid > 1){
            totalTemp += temp;
            totalUmid += umid;
            temperatura = totalTemp/counter;
            umidade = totalUmid/counter;
            printf("Temperature is %.2f \n", temperatura);
            printf("Humidity is %.2f\n", umidade);
            printf("Status code is %d\n", status);
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            counter++;
        }else{
            status = -1;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void setupGPIO(){
    DHT11_init(DHT_GPIO);
  
    esp_rom_gpio_pad_select_gpio(BOTAO);
    gpio_set_direction(BOTAO, GPIO_MODE_INPUT);

    esp_rom_gpio_pad_select_gpio(LED_PLACA);
    gpio_set_direction(LED_PLACA, GPIO_MODE_OUTPUT);

    esp_rom_gpio_pad_select_gpio(LED_VERDE);
    gpio_set_direction(LED_VERDE, GPIO_MODE_OUTPUT);

    esp_rom_gpio_pad_select_gpio(LED_VERMELHO);
    gpio_set_direction(LED_VERMELHO, GPIO_MODE_OUTPUT);
}

void app_main(void){
    nvsInit();
    
    setupGPIO();
    config_leds();

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&coletaTemp, "Leitura DHT11", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(&controlaLeds, "Controle dos LEDS", 4096, NULL, 1, NULL);
}