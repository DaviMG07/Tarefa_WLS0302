# Tarefa_WLS0302

## Descrição Geral

Este projeto demonstra a integração de múltiplos periféricos em um microcontrolador RP2040 (por exemplo, Raspberry Pi Pico). Ele utiliza uma matriz de LEDs WS2812 (configurada em 5x5) para exibir dígitos, um display OLED SSD1306 para mostrar mensagens e caracteres, além de um LED RGB e botões de entrada para interação. O sistema integra hardware e software, empregando interrupções para tratamento dos botões e leitura de entrada via USB para atualizar o display e os LEDs.

## Objetivos do Projeto

- **Interrupções e Debouncing:**  
  - Implementar rotinas de interrupção para detectar o acionamento dos botões.
  - Utilizar um mecanismo de debouncing (delay de 300ms) para evitar leituras incorretas dos botões.

- **Controle de LEDs WS2812 e LED RGB:**  
  - Gerar e atualizar uma matriz 5x5 de LEDs WS2812 para representar dígitos (0 a 9) a partir de arrays pré-definidos.
  - Alternar os estados dos LEDs GREEN e BLUE (do LED RGB) em resposta aos eventos dos botões.

- **Interface com Display OLED SSD1306:**  
  - Inicializar a comunicação I2C para configurar e atualizar o display OLED.
  - Exibir uma mensagem inicial e atualizar o display conforme a entrada via USB.

- **Processamento de Entrada via USB:**  
  - Capturar caracteres digitados pelo usuário.
  - Se o caractere for um dígito, desenhar o número correspondente na matriz WS2812 com uma coloração definida; caso contrário, limpar a exibição.

## Componentes Utilizados

- **Microcontrolador RP2040 (ex: Raspberry Pi Pico):**  
  Responsável por gerenciar as operações, executar rotinas de interrupção e integrar os periféricos.

- **Matriz de LEDs WS2812 (5x5):**  
  Utilizada para a exibição de dígitos de 0 a 9. Cada dígito é representado por um array de valores que definem a intensidade de cada LED.

- **Display OLED SSD1306:**  
  Comunicação via I2C (com pinos configurados no GPIO 14 e 15) para exibição de mensagens e caracteres.

- **LED RGB:**  
  Utilizado para indicar eventos. Os pinos RED (GPIO 13), GREEN (GPIO 11) e BLUE (GPIO 12) são configurados para saída, com GREEN e BLUE alternando seu estado conforme a interação.

- **Botões de Acionamento:**  
  - **Botão A (GPIO 5):** Ao ser pressionado, alterna o estado do LED GREEN.
  - **Botão B (GPIO 6):** Ao ser pressionado, alterna o estado do LED BLUE.

## Funcionalidades do Projeto

1. **Inicialização e Configuração dos Periféricos:**  
   - Configuração do clock do sistema e inicialização do ambiente de desenvolvimento.
   - Setup dos LEDs RGB, botões (com pull-up interno e interrupções) e matriz de LEDs WS2812 por meio do PIO.
   - Inicialização do display SSD1306 via I2C, com exibição de uma mensagem inicial.

2. **Exibição de Dígitos na Matriz WS2812:**  
   - Arrays pré-definidos armazenam os padrões dos números de 0 a 9.
   - Ao receber um dígito via USB, a função responsável converte os valores para o formato RGB e envia os dados para os LEDs, atualizando o display.

3. **Interface via USB e Atualização do Display OLED:**  
   - Através da entrada padrão (USB), o sistema captura um caractere.
   - Se o caractere for um dígito, o display OLED é atualizado para exibir o dígito recebido, e a matriz de LEDs é redesenhada com a cor definida (escala de roxo com intensidade baixa no canal vermelho e azul).
   - Para caracteres não numéricos, a matriz é limpa (desenha o dígito 0 sem cor).

4. **Tratamento de Botões com Interrupções:**  
   - As interrupções geradas pelos botões (na borda de descida) acionam uma função callback que alterna o estado dos LEDs GREEN e BLUE.
   - Um intervalo de 300ms é empregado para evitar múltiplos acionamentos devido ao efeito de bouncing.

## Descrição do Funcionamento

- **Inicialização do Sistema:**  
  No `main()`, são chamadas as funções de inicialização para configurar os LEDs RGB, botões (com interrupções), a matriz WS2812 (via PIO) e o display OLED. Também é definida uma mensagem inicial para o display.

- **Processamento de Entrada:**  
  Um loop infinito verifica a conexão USB e, ao detectar a entrada de um caractere, a função `process_input()` atualiza o display e, se o caractere for um dígito, redesenha o número correspondente na matriz WS2812 com a coloração definida.

- **Interrupção dos Botões:**  
  A função `button_pressed()` é chamada sempre que ocorre um evento nos botões, registrando o tempo do último evento e alternando o estado dos LEDs indicativos (GREEN ou BLUE), garantindo assim uma resposta visual à interação do usuário.

## Requisitos Técnicos e Organização do Código

- **Utilização de Bibliotecas:**  
  - **pico/stdlib.h, hardware/i2c.h, hardware/pio.h, hardware/timer.h:** Para configuração e controle dos periféricos.
  - **ssd1306.h e font.h:** Para manipulação do display OLED.
  - **ws2812.pio.h:** Para controle da matriz de LEDs WS2812 via PIO.

- **Estrutura Modular:**  
  O código está organizado em funções específicas para:
  - Inicialização de LEDs, botões, display e matriz WS2812.
  - Processamento de entrada via USB.
  - Desenho e atualização dos dígitos na matriz de LEDs.
  - Tratamento de interrupções com mecanismo de debouncing.

## Compilação e Execução

1. **Pré-requisitos:**  
   - Instalação e configuração do SDK do Raspberry Pi Pico.
   - Certificação de que todas as bibliotecas (como `ssd1306`, `ws2812.pio.h`, etc.) estejam corretamente instaladas e referenciadas.

2. **Compilação:**  
   - Utilize a ferramenta de compilação (por exemplo, via VSCode com a extensão para Raspberry Pi Pico) para compilar o firmware.
   - Configure o projeto conforme as instruções específicas do ambiente de desenvolvimento utilizado.

3. **Upload para o Microcontrolador:**  
   - Conecte o dispositivo ao computador via USB.
   - Faça o upload do firmware (por exemplo, copiando o arquivo UF2 para o dispositivo montado ou utilizando a opção de “run” no VSCode, se configurado).

## Considerações Finais

Este projeto integra conceitos essenciais de sistemas embarcados, como a utilização de interrupções, debouncing, comunicação I2C e controle de dispositivos LED. Ao combinar o display OLED SSD1306, a matriz de LEDs WS2812 e a interação via USB e botões, o projeto serve como um excelente exemplo prático para o desenvolvimento de aplicações interativas com microcontroladores, demonstrando a sinergia entre hardware e software.
