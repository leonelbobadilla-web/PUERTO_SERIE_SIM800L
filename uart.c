#include "uart.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

static uint8_t rx_buffer[BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

static void on_uart_rx(void) {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        uint16_t next_head = (rx_head + 1) % BUFFER_SIZE;
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = ch;
            rx_head = next_head;
        }
    }
}

void uart_driver_init(uint32_t baudrate) {
    uart_init(UART_ID, baudrate);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, false);

    int irq_num = (UART_ID == uart0) ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(irq_num, on_uart_rx);
    irq_set_enabled(irq_num, true);
    uart_set_irq_enables(UART_ID, true, false);
}

bool uart_driver_read_byte(uint8_t *byte) {
    if (rx_head == rx_tail) return false;
    *byte = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % BUFFER_SIZE;
    return true;
}

void uart_driver_write_string(const char *str) {
    uart_puts(UART_ID, str);
}