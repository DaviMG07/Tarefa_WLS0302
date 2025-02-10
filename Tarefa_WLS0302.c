#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#define I2C_PORT       i2c1
#define DISPLAY_SDA    14
#define DISPLAY_SCL    15
#define ADRESS         0x3C

#define RED            13
#define GREEN          11
#define BLUE           12

#define BUTTON_A       5
#define BUTTON_B       6

#define PIXELS         25
#define MATRIX         7

static volatile uint32_t last_event = 0;

/*
 * Matrizes que representam os dígitos de 0 a 9 para controle dos LEDs.
 */
double number_0[PIXELS] = {
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_1[PIXELS] = {
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0,
    0.0, 0.2, 0.2, 0.0, 0.0,
    0.0, 0.0, 0.2, 0.0, 0.0
};

double number_2[PIXELS] = {
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_3[PIXELS] = {
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_4[PIXELS] = {
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0
};

double number_5[PIXELS] = {
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_6[PIXELS] = {
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_7[PIXELS] = {
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_8[PIXELS] = {
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double number_9[PIXELS] = {
    0.0, 0.2, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0,
    0.0, 0.2, 0.0, 0.2, 0.0,
    0.0, 0.2, 0.2, 0.2, 0.0
};

double* numbers[] = { number_0, number_1, number_2, number_3, number_4,
                      number_5, number_6, number_7, number_8, number_9 };

/*
 * Converte valores de ponto flutuante (0 a 1) em um valor RGB formatado para os LEDs.
 */
uint32_t rgb_matrix(double r, double g, double b) {
    unsigned char R = (unsigned char)(r * 255);
    unsigned char G = (unsigned char)(g * 255);
    unsigned char B = (unsigned char)(b * 255);
    
    return (G << 24) | (R << 16) | (B << 8);
}

/*
 * Desenha o dígito representado pela matriz 'number' nos LEDs WS2812.
 */
void draw_number(double *number, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < PIXELS; i++) {
        uint32_t led_value = rgb_matrix(number[i] * r, number[i] * g, number[i] * b);
        pio_sm_put_blocking(pio, sm, led_value);
    }
}

/*
 * Callback de interrupção para botões. Alterna os LEDs (verde para BUTTON_A e azul para BUTTON_B)
 * com debounce de 300ms.
 */
void button_pressed(uint gpio, uint32_t events) {
    printf("Botão pressionado: GPIO %d\n", gpio);
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_event > 300000) {
        last_event = current_time;
        if (gpio == BUTTON_A) {
            gpio_put(GREEN, !gpio_get(GREEN));
        } else if (gpio == BUTTON_B) {
            gpio_put(BLUE, !gpio_get(BLUE));
        }
    }
}

/*
 * Inicializa os LEDs indicadores (RGB).
 */
void init_leds(void) {
    gpio_init(RED);
    gpio_set_dir(RED, GPIO_OUT);
    gpio_init(GREEN);
    gpio_set_dir(GREEN, GPIO_OUT);
    gpio_init(BLUE);
    gpio_set_dir(BLUE, GPIO_OUT);
}

/*
 * Configura os botões com pull-up e associa a função de interrupção.
 */
void init_buttons(void) {
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_pressed);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_pressed);
}

/*
 * Inicializa os LEDs WS2812 utilizando o PIO e desenha o dígito '0' como estado inicial.
 */
void init_ws2812_leds(PIO pio, uint *sm, uint *offset) {
    *offset = pio_add_program(pio, &ws2812_program);
    *sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, *sm, *offset, MATRIX, 800000, false);
    draw_number(numbers[0], pio, *sm, 0, 0, 0);
}

/*
 * Inicializa a comunicação I2C e configura o display SSD1306.
 */
void init_i2c_and_display(ssd1306_t *ssd) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(DISPLAY_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_SDA);
    gpio_pull_up(DISPLAY_SCL);

    ssd1306_init(ssd, WIDTH, HEIGHT, false, ADRESS, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

/*
 * Exibe uma mensagem inicial no display com uma borda retangular.
 */
void display_message(ssd1306_t *ssd, const char* msg) {
    bool cor = false;
    ssd1306_fill(ssd, cor);
    ssd1306_rect(ssd, 3, 3, 122, 58, cor, !cor);
    ssd1306_draw_string(ssd, msg, 0, 30);
    ssd1306_send_data(ssd);
}

/*
 * Processa a entrada via USB e atualiza o display e os LEDs conforme o caractere recebido.
 */
void process_input(ssd1306_t *ssd, PIO pio, uint sm) {
    char c;
    if (scanf("%c", &c) == 1) {
        printf("Recebido: '%c'\n", c);
        ssd1306_fill(ssd, false);
        ssd1306_draw_char(ssd, c, 60, 30);
        ssd1306_send_data(ssd);

        if (isdigit(c)) {
            draw_number(numbers[c - '0'], pio, sm, 0.1, 0.0, 0.3);
        } else {
            draw_number(numbers[0], pio, sm, 0.0, 0.0, 0.0);
        }
    }
}

/*
 * Função principal: inicializa os periféricos e executa o loop de processamento de entrada.
 */
int main(void) {
    stdio_init_all();
    PIO pio = pio0;
    uint offset, sm;

    /* Configuração do clock do sistema */
    set_sys_clock_khz(128000, false);

    /* Inicialização dos periféricos */
    init_leds();
    init_buttons();
    init_ws2812_leds(pio, &sm, &offset);

    ssd1306_t ssd;
    init_i2c_and_display(&ssd);
    display_message(&ssd, "Digite um char");

    /* Loop principal: aguarda e processa entrada via USB */
    while (true) {
        if (stdio_usb_connected()) {
            process_input(&ssd, pio, sm);
        }
    }
    return 0;
}
