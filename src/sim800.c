#include "sim800.h"
#include "uart.h"
#include "pico/stdlib.h"
#include <stdio.h>

void sim800_init(void) {
    uart_driver_init(9600);
    sleep_ms(1000);
    sim800_send_cmd("AT");
    sleep_ms(100);
    sim800_send_cmd("AT+CMGF=1"); // Configurar SMS en modo texto
}

void sim800_send_cmd(const char *cmd) {
    uart_driver_write_string(cmd);
    uart_driver_write_string("\r\n");
}

void sim800_get_signal(void) {
    sim800_send_cmd("AT+CSQ");
}

void sim800_get_network_status(void) {
    sim800_send_cmd("AT+CREG?");
}

void sim800_get_antenna_info(void) {
    sim800_send_cmd("AT+CENG=1,1"); // Habilita reporte de ingeniería
    sim800_send_cmd("AT+CENG?");
}

void sim800_make_call(const char *number) {
    char buf[32];
    snprintf(buf, sizeof(buf), "ATD%s;", number);
    sim800_send_cmd(buf);
}

void sim800_answer_call(void) {
    sim800_send_cmd("ATA");
}

void sim800_hangup_call(void) {
    sim800_send_cmd("ATH");
}

void sim800_send_sms(const char *number, const char *text) {
    char buf[48];
    snprintf(buf, sizeof(buf), "AT+CMGS=\"%s\"", number);
    sim800_send_cmd(buf);
    sleep_ms(200);
    uart_driver_write_string(text);
    
    // Envía Ctrl+Z (ASCII 26) para confirmar el SMS
    char ctrl_z[2] = {26, 0};
    uart_driver_write_string(ctrl_z);
}

// Procesa bytes entrantes y los reenvía directamente a la consola USB
void sim800_process_rx_stream(void) {
    uint8_t ch;
    while (uart_driver_read_byte(&ch)) {
        putchar(ch);
    }
}