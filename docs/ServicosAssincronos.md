# Serviços assíncronos

Quando se progrma para um MCU, 90% dos serviços são síncronos, ou seja, a leitura dos sensores, envio de pacotes e outros são programados para serem executados em determinada ordem e momento, entretanto quando se espera o recebimento de algum dado, ou alguma requisição, tal informação não é sempre enviada no momento que se deseja ou espera, então, ao programar-se um MCU como o ESP32, serviços de interrupação de caso seja recebida alguma informação é vital para o funcionamento correto do microcontrolador.

Com o Framework arduino-esp32, encontra-se diversas bibliotecas com esse fim:

## Bibliotecas Async:

- ESPAsyncWebServer 
- AsyncMqttClient
- Async