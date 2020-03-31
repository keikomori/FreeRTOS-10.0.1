#include "uart.h"

xQueueHandle qUART0;
xSemaphoreHandle sUART0;
xSemaphoreHandle mutexTx0;
xSemaphoreHandle mutexRx0;
QueueHandle_t qUART0Tx;

extern void UART_IRQHandler(void *param){
    (void)param;
    uint32_t ui32Status = MAP_UARTIntStatus(UART0_BASE, true);
    char uartchar;
    signed portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;
    signed portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;

    UARTIntClear(UART0_BASE, ui32Status);
    if(UART_INT_RX & ui32Status){
        while(MAP_UARTCharsAvail(UART0_BASE)){
            uartchar = (char)MAP_UARTCharGetNonBlocking(UART0_BASE);
            xQueueSendToBackFromISR(qUART0,&uartchar,&pxHigherPriorityTaskWokenRX);
        }
    }
    if(UART_INT_TX & ui32Status){
        MAP_UARTIntDisable(UART0_BASE, UART_INT_TX);
        xSemaphoreGiveFromISR(sUART0,&pxHigherPriorityTaskWokenTX);
    }
    if((pxHigherPriorityTaskWokenTX == pdTRUE)||(pxHigherPriorityTaskWokenRX == pdTRUE)){
        portYIELD();
    }
}

void vUartInit(void *param){
   (void)param;

   sUART0 = xSemaphoreCreateBinary();

    if( sUART0 == NULL ){
        /* There was insufficient FreeRTOS heap available for the semaphore to
        be created successfully. */
        vTaskSuspend(NULL);
    }
    else
    {
        mutexTx0 = xSemaphoreCreateMutex();
        if( mutexTx0 == NULL ){
            /* There was insufficient FreeRTOS heap available for the semaphore to
            be created successfully. */
            vSemaphoreDelete( sUART0);
            vTaskSuspend(NULL);
        }else{
            qUART0 = xQueueCreate(128, sizeof(char));
            if( qUART0 == NULL ){
                /* There was insufficient FreeRTOS heap available for the queue to
                be created successfully. */
                vSemaphoreDelete( sUART0);
                vSemaphoreDelete( mutexTx0);
                vTaskSuspend(NULL);
            }else
            {
               taskENTER_CRITICAL();
               /*#if UART_STRING == UART_QUEUE
               qUART0Tx = xQueueCreate(128, sizeof(char));
               #endif
               *///
               // Enable the peripherals used by this example.
               MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
               MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

               //
               // Set GPIO A0 and A1 as UART pins.
               MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
               MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
               MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

               //
               // Configure the UART for 115,200, 8-N-1 operation.
               MAP_UARTConfigSetExpClk(UART0_BASE,  configCPU_CLOCK_HZ, 115200,
                                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                                        UART_CONFIG_PAR_NONE));

               MAP_UARTFIFODisable(UART0_BASE);

               //
               // Enable the UART interrupt.
               MAP_IntPrioritySet(INT_UART0, 0xC0);
               MAP_IntEnable(INT_UART0);
               MAP_UARTIntEnable(UART0_BASE, UART_INT_RX);
               taskEXIT_CRITICAL();
            }
        }
    }
}


void FreeRTOS_write(uint8_t *pui8Data, uint32_t ui32Size){
    // espera indefinidamente pelo recurso
    if(xSemaphoreTake(mutexRx0, portMAX_DELAY) == pdTRUE){

        while(1){

            if(xQueueReceive(qUART0, &pui8Data, portMAX_DELAY) == pdTRUE){
                ui32Size--;
            }
        }
        xSemaphoreGive(mutexRx0);
    }
}

void UARTIntHandler(void){
    unsigned int ui32Status;
    portBASE_TYPE pxHigherPriorityTaskWokenRX = pdFALSE;
    portBASE_TYPE pxHigherPriorityTaskWokenTX = pdFALSE;

    char data;

    // le flags de interrupcao
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    // limpa flags das interrupcoes acionadas
    UARTIntClear(UART0_BASE, ui32Status);

    // Se for interrupcao de recepcao
    if ((ui32Status&UART_INT_RX) == UART_INT_RX){
        // le o caractere recebido
        //data = UART_DATA_R;
        xQueueSendToBackFromISR(qUART0, &data, &pxHigherPriorityTaskWokenRX);
    }

    // Se for interrupcao de transmissao
    if ((ui32Status&UART_INT_TX) == UART_INT_TX){
        ROM_UARTIntDisable(UART0_BASE, UART_INT_TX);

        /* Informa ao driver que o buffer de transmissao esta vazio
         * e pronto para o proximo caractere */
        xSemaphoreGiveFromISR(sUART0, &pxHigherPriorityTaskWokenRX);
    }
    /* Verifica se o semaforo ou a fila acordam uma tarefa de maior
     * prioridade que a tarefa sendo executada atualmente*/
    if ((pxHigherPriorityTaskWokenRX == pdTRUE) || (pxHigherPriorityTaskWokenTX == pdTRUE)){
        // Se acordou, chama interrupcao de software para a troca de contexto
        portYIELD();
    }
}

