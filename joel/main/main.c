#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "soundSensor.h"
#include "rotary_encoder.h"

#define TAG "MAIN"
#define ROT_ENC_A_GPIO 4
#define ROT_ENC_B_GPIO 16

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

int somDigitalValue;
int rotaryPosition;
int previousRotaryPosition;

rotary_encoder_info_t info = {0};
#define ENABLE_HALF_STEPS false // Set to true to enable tracking of rotary encoder at half step resolution
#define RESET_AT 0              // Set to a positive non-zero number to reset the position if this value is exceeded
#define FLIP_DIRECTION false    // Set to true to reverse the clockwise/counterclockwise sense

void setup(){

    // esp32-rotary-encoder requires that the GPIO ISR service is installed before calling rotary_encoder_register()
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    // Initialise the rotary encoder device with the GPIOs for A and B signals
    ESP_ERROR_CHECK(rotary_encoder_init(&info, ROT_ENC_A_GPIO, ROT_ENC_B_GPIO));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, ENABLE_HALF_STEPS));
#ifdef FLIP_DIRECTION
    ESP_ERROR_CHECK(rotary_encoder_flip_direction(&info));
#endif
  
  som_setup();

}

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while(true)
    {
      sprintf(mensagem, "{\"somDigital\": %d}", somDigitalValue);
      mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);
      sprintf(mensagem, "{\"somDigital\": %d, \"rotaryPosition\": %d}", somDigitalValue, rotaryPosition);
      mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);
      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void somReader(void *params){
  while (true)
  {
    somDigitalValue =  som_get(); 
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}

void rotaryReader(void *params)
{
    QueueHandle_t event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));
    while (true)
    {
        // Wait for incoming events on the event queue.
        rotary_encoder_event_t event = {0};
        if (xQueueReceive(event_queue, &event, 1000 / portTICK_PERIOD_MS) == pdTRUE)
        {
            rotaryPosition = event.state.position;
            //ESP_LOGI(TAG, "Event: position %d, direction %s", event.state.position,
            //         event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
        }
    }
}

void app_main(void)
{
    setup();
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
    //ESP_LOGI(TAG, "WIFI-Inciado");

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    //ESP_LOGI(TAG, "MQTT-Inciado");
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    //ESP_LOGI(TAG, "Comunicacao com o Broker-Inciado");
    xTaskCreate(&somReader, "Leitura do som",4096, NULL, 1, NULL );
    //ESP_LOGI(TAG, "Leitura do som-Inciado");
    xTaskCreate(&rotaryReader, "Leitura do Rotary",4096, NULL, 1, NULL );
    //ESP_LOGI(TAG, "Leitura do Rotary Enconder-Inciado");    
}
