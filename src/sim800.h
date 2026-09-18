#ifndef SIM800_H
#define SIM800_H
#include "uart.h"

#include <stdbool.h>

void sim800_init(void);
void sim800_send_cmd(const char *cmd);
void sim800_get_signal(void);
void sim800_get_network_status(void);
void sim800_get_antenna_info(void);

void sim800_make_call(const char *number);
void sim800_answer_call(void);
void sim800_hangup_call(void);

void sim800_send_sms(const char *number, const char *text);
void sim800_process_rx_stream(void);

#endif