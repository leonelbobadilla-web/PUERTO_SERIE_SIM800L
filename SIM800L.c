#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "sim800.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#if 0 // esto esta definido en otro archvio
// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 9600 //115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif
// 
static void parse_usb_command(const char *cmd) {
    if (strcmp(cmd, "senal") == 0) {
        sim800_get_signal();
    } else if (strcmp(cmd, "red") == 0) {
        sim800_get_network_status();
    } else if (strcmp(cmd, "antena") == 0) {
        sim800_get_antenna_info();
    } else if (strcmp(cmd, "responder") == 0) {
        sim800_answer_call();
    } else if (strcmp(cmd, "colgar") == 0) {
        sim800_hangup_call();
    } else if (strncmp(cmd, "llamar ", 7) == 0) {
        sim800_make_call(cmd + 7);
    } else if (strncmp(cmd, "sms ", 4) == 0) {
        // Formato esperado: "sms +54911xxxx, Hola desde Pico"
        char num[20], msg[80];
        if (sscanf(cmd + 4, "%19[^,], %[^\n]", num, msg) == 2) {
            sim800_send_sms(num, msg);
        } else {
            printf("\n[Error] Usa el formato: sms +123456, Tu mensaje\n");
        }
    } else {
        // Si no es un comando de la CLI local, se envía directo como comando AT
        sim800_send_cmd(cmd);
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(3000); // Tiempo para abrir el monitor serie USB

    for(int aux_inicioDeAplicacion = 0; aux_inicioDeAplicacion<20; aux_inicioDeAplicacion++){
        printf("%d ", aux_inicioDeAplicacion);
        sleep_ms(500);
    }   printf("\n");

    // Inicializo el SIM800
    sim800_init();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
    
    #if 0
    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");
   #endif 
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart
    
    printf("     PICO SIM800L CONTROL LAYER v1.0    \n");
    printf("========================================\n");
    printf("Comandos:\n");
    printf(" - senal                : Consulta la intensidad (RSSI)\n");
    printf(" - red                  : Estado de registro celular\n");
    printf(" - antena               : Información de la celda\n");
    printf(" - llamar <numero>      : Realiza una llamada\n");
    printf(" - responder / colgar   : Gestión de llamadas\n");
    printf(" - sms <num>, <texto>   : Envía un mensaje corto\n");
    printf(" - <Comando AT>         : Envío directo al módem\n\n");

    char input_buf[128];
    int input_pos = 0;

    while (true) {
        // 1. Drenar datos recibidos del GSM a la consola USB
        sim800_process_rx_stream();

        // 2. Leer comandos desde el puerto USB serie
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            if (c == '\r' || c == '\n') {
                if (input_pos > 0) {
                    input_buf[input_pos] = '\0';
                    parse_usb_command(input_buf);
                    input_pos = 0;
                }
            } else if (input_pos < sizeof(input_buf) - 1) {
                input_buf[input_pos++] = (char)c;
                putchar(c); // Echo de escritura
            }
        }
        tight_loop_contents(); // ojo est es para 
    }
    return 0;
}
