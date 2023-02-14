#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "leds.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#include "gpio_setup.h"
#include "adc_module.h"

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

#define LED_1 2
#define LED_2 4     // vermelho
#define LED_3 16    // verde
#define BOTAO 0

#define ADC_VREF_mV    3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0
#define TEMP_PIN       ADC_CHANNEL_3

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void * params){
    char mensagem[50];
    if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY)){
        while(true){
            // int Vo;
            // float R1 = 10000; // value of R1 on board
            // float logR2, R2, T;
            // float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741; //steinhart-hart coeficients for thermistor

            // Vo = analogRead(TEMP_PIN);
            // R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculate resistance on thermistor
            // logR2 = log(R2);
            // T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));

            //int adcVal = analogRead(TEMP_PIN);
            //float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION); // convert the ADC value to voltage in millivolt
            //float tempC = milliVolt / 10;

            // printf("Temperature: %.2f", T);
            // printf(" °C\n");

            float T = 20.0 + (float)rand()/(float)(RAND_MAX/10.0);
            sprintf(mensagem, "{\"temperatura\": \"%f\"}", T);
            mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

            sprintf(mensagem, "{\"led_vermelho\": \"%d\"}", gpio_get_level(LED_2));
            mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
    }
}

void alterna_leds(){
    config_leds();
    brighten(LED_2);
    // while(true)
    // {
    //     brighten(LED_2);
    //     dimmer(LED_2);

    //     brighten(LED_3);
    //     dimmer(LED_3);
    // }
}

void app_main(void){
    adc_init(ADC_UNIT_1);

    pinMode(TEMP_PIN, GPIO_ANALOG);
    pinMode(LED_2, GPIO_OUTPUT);

    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();
    alterna_leds();
    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);

    // xTaskCreate(&alterna_leds, "Pisca leds", 4096, NULL, 1, NULL);
}