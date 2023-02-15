#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#include "WIFI/wifi.h"
#include "MQTT/mqtt.h"
#include "leds.h"
#include "DHT11/dht11.h"
#include "GPIO/gpio_setup.h"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

#define LED_PLACA 2
#define LED_VERMELHO 4     
#define LED_VERDE 16    
#define BOTAO 0
#define DHT_GPIO 18

float temperatura = 0, totalTemp = 0;
float umidade = 0, totalUmid = 0;
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
    if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)){
        while(true){
            int led_verm = 1;
            int led_verd = 0;

            sprintf(mensagem, "{ \"led_vermelho\": \"%d\", \"led_verde\": \"%d\" }", led_verm, led_verd);
            mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

            if(status == 0 && temperatura != 0 && umidade != 0){
                //ledPower = nvsGetValue("led");
                sprintf(mensagem, "{ \"temperatura\": \"%.2f\", \"umidade\": \"%.2f\" }", temperatura, umidade);
                mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void alterna_leds(){
    config_leds();

    brighten(LED_VERMELHO);
    // while(true){
    //     dimmer(LED_VERMELHO);

    //     brighten(LED_VERDE);
    //     dimmer(LED_VERDE);
    // }
}

void trataDht(void *params){
    float temp;
    float umid;
    while(true) {
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

// void comunicaTemperaturaUmidade(void *params)
// {
//     char mensagem[50];
//     char tag[] = "DHT";
//     while (true)
//     {
//         xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY);
//         struct dht11_reading leitura = DHT11_read();
//         if (leitura.status == 0)
//         {
//             // DHT
//             sprintf(mensagem, "{\"temperatura1\": \"%d\"}", leitura.temperature);
//             mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
//             ESP_LOGD(tag, "Temperatura enviada:%d", leitura.temperature);
            
//             sprintf(mensagem, "{\"umidade1\": \"%d\"}", leitura.humidity);
//             mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
//             ESP_LOGD(tag, "Umidade enviada:%d", leitura.humidity);
//         }
//         xSemaphoreGive(conexaoMQTTSemaphore);
//         vTaskDelay(10000 / portTICK_PERIOD_MS);
//     }
    
// }

void app_main(void){
    // adc_init(ADC_UNIT_1);

    //pinMode(TEMP_PIN, GPIO_ANALOG);
    //pinMode(LED_2, GPIO_INPUT_OUTPUT);
    //pinMode(LED_3, GPIO_INPUT_OUTPUT);
    
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    setupGPIO();
    alterna_leds();

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataDht, "Leitura DHT11", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);

    // xTaskCreate(&alterna_leds, "Pisca leds", 4096, NULL, 1, NULL);
}