# Trabalho 3 - ESP32 e Sensores

Video de apresentação: 
https://youtu.be/ibE4ysmXA-Q

## 1. Instalação

1. Instale a extensão do PlatformIO v3.0 para VS Code.
2. Ligue os sensores nos respectivos pinos da ESP32:
    - DHT11: pino D18
    - 2-color LED: pino do meio no D4, pino da direita no RX2
    - Rotary Encoder: pino da esquerda no D19, pino da direita no D21
    - Sound Sensor: pino TX2
3. Instale os [drivers necessários](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip) caso esteja no Windows, e conecte a placa ESP32 ao computador via cabo USB.
4. Importe o projeto e configure a rede de wifi no `menuconfig`.
5. Clique em `build` seguido de `upload`.
6. Utilize o [Dashboard](http://164.41.98.25:443/dashboards/f91ab8b0-a700-11ed-8436-09bf763c9306) para interagir com a placa.

---
Integrantes: Joel Jefferson e Vinícius Roriz