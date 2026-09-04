#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define UART_ID uart1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define BUFFER_SIZE 512

void uart_driver_init(uint32_t baudrate);
bool uart_driver_read_byte(uint8_t *byte);
void uart_driver_write_string(const char *str);

#endif