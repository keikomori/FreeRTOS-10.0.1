#ifndef TAREFAS_H_
#define TAREFAS_H_

#include "stdint.h"
#include "FreeRTOS.h"

typedef struct _leds_t_{
        unsigned long clock_port;
        unsigned long pin;
        unsigned long port_base;
        int           timeout;
}leds_t;

void exec(void *param);
void execUart(void *param);
void vTerminalTask(void *pvParameters);
void vCommandConsoleTask( void *pvParameters);


#endif /* TAREFAS_H_ */
