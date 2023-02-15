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
#include "GPIO/gpio.h"
#include "ENCODER/rotary_encoder.h"

#define ENABLE_HALF_STEPS false 
#define RESET_AT 0              
#define FLIP_DIRECTION false

rotary_encoder_info_t info = {0};
SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

int somDigitalValue;
int rotaryPosition;
int previousRotaryPosition;

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

            nvsWriteValue("led_vermelho", lvm);
            nvsWriteValue("led_verde", lvd);
            nvsWriteValue("led_placa", lp);

            sprintf(mensagem, "{ \"led_vermelho\": \"%d\", \"led_verde\": \"%d\" }", lvm, lvd);
            mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

            sprintf(mensagem, "{ \"botao\": \"%d\", \"led_placa\": \"%d\" }", botao, lp);
            mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);
            
            sprintf(mensagem, "{\"somDigital\": %d}", somDigitalValue);
            mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

            sprintf(mensagem, "{\"somDigital\": %d, \"rotaryPosition\": %d}", somDigitalValue, rotaryPosition);
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

void setupRotary(){
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Initialise the rotary encoder device with the GPIOs for A and B signals
    ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif
  
  som_setup();

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
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            counter++;
        }else{
            status = -1;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void somReader(void *params){
    while (true){
        somDigitalValue =  0 + gpio_get_level(SOUNDSENSOR); 
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void rotaryReader(void *params){
    QueueHandle_t event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));

    while (true){
        // Wait for incoming events on the event queue.
        rotary_encoder_event_t event = {0};
        if (xQueueReceive(event_queue, &event, 1000 / portTICK_PERIOD_MS) == pdTRUE){
            rotaryPosition = event.state.position;
        }
    }
}

void app_main(void){
    nvsInit();
    
    setupGPIO();
    config_leds();
    setupRotary();

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    wifi_start();

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&coletaTemp, "Leitura DHT11", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(&controlaLeds, "Controle dos LEDS", 4096, NULL, 1, NULL);
    xTaskCreate(&somReader, "Leitura do som",4096, NULL, 1, NULL );
    xTaskCreate(&rotaryReader, "Leitura do Rotary",4096, NULL, 1, NULL );
}