# Frameworks para o ESP32

Um framework (estrutura em inglês) é um software com finalidade genérica, que disponibiliza diversas bibliotecas e ferramentas para desenvolver outros softwares com finalidade específica, baseados no framework.

Em microcontroladores, o framework é o conjunto de ferramentas que transformam um código de alto nível, (Python, C++, C) em instruções para o microcontrolador, cabendo ao framework a compilação e comunicação com o microcontrolador. O framework também é responsável por definir os parâmetros internos do MCU e os disponibilizar para serem acessados pelo código.

O ESP32 apresenta 2 frameworks principais:
## ESP-IDF
O ESP-IDF é o framework oficial de desenvolvimento para o ESP32
[Documentação](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

## arduino-esp32
Um framework baseado em arduino que o torna compatível com as bibliotecas desenvolvidas para arduino, seu nível de abstração é superior à do ESP-IDF.  
[Documentação](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
## Outros
A espressif possui diversos frameworks para o ESP32 além dos citados acima, cada possui uma aplicação específica e sua documentação está no GitHub e no Site da Espressif.
