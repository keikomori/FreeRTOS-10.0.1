#include "tarefas.h"
#include "uart.h"

//includes c
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

//includes FreeRTOS
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "FreeRTOS_IO.h"
#include "IOUtils_Common.h"
#include "FreeRTOS_uart.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "semphr.h"

#include "FreeRTOS_IO.h"
#include "FreeRTOS_DriverInterface.h"

//includes tiva
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

//INCLUDES UART E OUTROS
#include "FreeRTOS_uart.h"
#include "FreeRTOS_CLI.h"

#define MAX_INPUT_LENGTH    50
#define MAX_OUTPUT_LENGTH   512

static const char *const pcWelcomeMessage = "\r\nBoas Vindas\r\n";


leds_t LED4;

xTaskHandle procId3;

//PISCA LED
void exec(void *param)
{
    leds_t *led = (leds_t *)param;
    SysCtlPeripheralEnable(led->clock_port);
    GPIOPadConfigSet(led->port_base, led->pin, GPIO_STRENGTH_8MA_SC, GPIO_PIN_TYPE_STD);
    GPIOPinTypeGPIOOutput(led->port_base, led->pin);

    while(1)
    {
        GPIOPinWrite(led->port_base, led->pin, 0);
        vTaskDelay(led->timeout);
        GPIOPinWrite(led->port_base, led->pin, led->pin);
        vTaskDelay(led->timeout);
    }
}

// Tarefa UART
void execUart(void *param)
{
    uint8_t data;
    Peripheral_Descriptor_t xConsoleUART;

    xConsoleUART = FreeRTOS_open(boardCOMMAND_CONSOLE_UART, 0);

    FreeRTOS_write(xConsoleUART, (uint8_t*)"\033[2J\033[H",sizeof("\033[2J\033[H"));
    FreeRTOS_write(xConsoleUART, (uint8_t*)"FreeRTOS started!\n\r",sizeof("FreeRTOS started!\n\r"));

    while(1)
    {
        FreeRTOS_read(xConsoleUART, &data,sizeof(data));
        FreeRTOS_write(xConsoleUART, (uint8_t*)"Foi inserida a tecla ",sizeof("Foi inserida a tecla "));
        if((char)data=='\r')
            FreeRTOS_write(xConsoleUART, (uint8_t*)"r",sizeof("r"));
        if((char)data=='\n')
            FreeRTOS_write(xConsoleUART, (uint8_t*)"n",sizeof("n"));
        if((char)data=='\b')
            FreeRTOS_write(xConsoleUART,(uint8_t*)"b",sizeof("b"));
        FreeRTOS_write(xConsoleUART, &data,sizeof(data));
        FreeRTOS_write(xConsoleUART, (uint8_t*)"!\n\r",sizeof("!\n\r"));
    }
}

// Tarefa LED_ON/LED_OFF
static BaseType_t prvLEDOnOffCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString){
    char *pcParameter1;
    BaseType_t xParameter1StringLength;

    pcParameter1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &xParameter1StringLength);

    char *head = "\r\nLED mudou de estado para: ";
    strcpy(pcWriteBuffer, head);
    if(!strcmp(pcParameter1, "on")){
        strcpy(&pcWriteBuffer[strlen(head)], "ON\r\n\r\n");
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
    }else{
        strcpy(&pcWriteBuffer[strlen(head)], "OFF\r\n\r\n");
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
    }
    return pdFALSE;
}

static BaseType_t prvTaskStatsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString){
    // texto de cabeçlho das informações impressas
    char *head = "Name      State Priority Stack Number\n\r";

    // assume que o tamanho do buffer é grande o suficiente para armazenar o texto gerado ao executar a função vTaskList()
    (void) xWriteBufferLen;

    // utiliza o buffer de saída como parametro da função vTaskList(). Assim, a função escreve a lista de tarefas diretamente no buffer de saída
    strcpy(pcWriteBuffer, head);

    vTaskList(&pcWriteBuffer[strlen(head)]);

    return pdFALSE;

}

//static BaseType_t prvHelpCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString){
//
//    static const xCommandLineInputListItem *pxCommand = NULL;
//    BaseType_t xReturn;
//
//    if(pxCommand == NULL){
//        pxCommand = &xRegisteredCommands;
//    }
//
//    strncpy(pcWriteBuffer, pxCommand->pxCommandLineDefinition->pcHelpString, xWriteBufferLen);
//
//    pxCommand = pxCommand->pxNext;
//
//    if(pxCommand == NULL){
//        xReturn pdFALSE;
//    }else{
//        xReturn pdTRUE;
//    }
//    return xReturn;
//}

// Tarefa Terminal
//void Terminal(void *param){
//    char buffer[512];
//
//    Peripheral_Descriptor_t xConsoleUART = FreeRTOS_open(boardCOMMAND_CONSOLE_UART, 0);
//
//    //(void)param;
//    vUartInit(param);
//
//        while(1){
//            FreeRTOS_write(xConsoleUART, (uint8_t*) "Name     State Priority Stack Number\n\r", sizeof("Name     State Priority Stack Number\n\r"));
//            FreeRTOS_write(xConsoleUART, (uint8_t*) "***************\n\r", sizeof("***************\n\r"));
//            vTaskList(buffer);
//            FreeRTOS_write(xConsoleUART, (uint8_t*) buffer, sizeof(buffer));
//            FreeRTOS_write(xConsoleUART, (uint8_t*) "\n\r", sizeof("\n\r"));
//            vTaskDelay(10000);
//        }
//}


static const CLI_Command_Definition_t xTasksCommand = {"tasks", "\r\ntasks:\r\n Lista todas as tarefas instaladas\r\n\r\n", prvTaskStatsCommand, 0};
static const CLI_Command_Definition_t xLEDCommand = {"led", "\r\nEstado LED:\r\n ON or OFF\r\n\r\n", prvLEDOnOffCommand, 1};

//static const CLI_Command_Definition_t xHelpCommand = {"help", "\r\ntasks:\r\n Lista Help:\r\n\r\n", prvHelpCommand, 0};

void vTerminalTask(void *pvParameters){

    Peripheral_Descriptor_t xConsoleUART = FreeRTOS_open(boardCOMMAND_CONSOLE_UART, 0);

    int8_t cRxedChar, cInputIndex = 0;
    BaseType_t xMoreDataToFollow;
//    CLI_Command_Definition_t xTasksCommand;



    // Os buffers de entrada e saida sao declarados estaticos para mantelos fora da pilha
    static char pcOutputString[MAX_OUTPUT_LENGTH], pcInputString[MAX_INPUT_LENGTH];

    // Este codigo assume que o periferico sendo utilizado como terminal
    // ja foi configurado e foi passado para a tarefa como um argumento
//    xConsole = (Peripheral_Descriptor_t) pvParameters;

//     registra um comando que retorna a lista de tarefas instaladas
    FreeRTOS_CLIRegisterCommand(&xTasksCommand);
    FreeRTOS_CLIRegisterCommand(&xLEDCommand);

//     envia mensagem de boas vindas
    FreeRTOS_write(xConsoleUART, pcWelcomeMessage, strlen(pcWelcomeMessage));

    for(;;){
        // lê um caracter por vez, espera com a tarefa bloqueada até que um caractere seja recebido
        FreeRTOS_read(xConsoleUART, &cRxedChar, sizeof(cRxedChar));
        if(cRxedChar == '\r'){

            // nova linha recebido, comando esta completo e pode ser processado
            FreeRTOS_write(xConsoleUART, "\r\n", strlen("\r\n"));

            do{

                xMoreDataToFollow = FreeRTOS_CLIProcessCommand(pcInputString, pcOutputString, MAX_INPUT_LENGTH);

                FreeRTOS_write(xConsoleUART, pcOutputString, strlen(pcOutputString));

            }while(xMoreDataToFollow != pdFALSE);

            cInputIndex=0;
            memset(pcInputString, 0x00, MAX_INPUT_LENGTH);
        }else{
            // executa processamento de qualquer caractere que nao seja nova linha
            if(cRxedChar == '\n'){

            }else if (cRxedChar == '\b'){
                    if(cInputIndex>0){
                        cInputIndex--;
                        pcInputString[cInputIndex] = '\0';
                    }
                    // implementa o eco do caractere no terminal
                    FreeRTOS_write(xConsoleUART, &cRxedChar, 1);
            }else{
                /* um novo caractere foi digitado e não é nenhum dos caracteres especiais do terminal. Portanto,
                será adicionado à string de entrada para formar o comando digitado e será processado quando
                o caractere nova linha for detectado */
                if(cInputIndex < MAX_INPUT_LENGTH){
                    pcInputString[cInputIndex] = cRxedChar;
                    cInputIndex++;
                }
                // implementa o eco do caractere no terminal
                FreeRTOS_write(xConsoleUART, &cRxedChar, 1);
            }
        }
    }
}

void vCommandConsoleTask( void *pvParameters )
{
Peripheral_Descriptor_t xConsole;
int8_t cRxedChar, cInputIndex = 0;
BaseType_t xMoreDataToFollow;
/* The input and output buffers are declared static to keep them off the stack. */
static char pcOutputString[ MAX_OUTPUT_LENGTH ], pcInputString[ MAX_INPUT_LENGTH ];

    /* This code assumes the peripheral being used as the console has already
    been opened and configured, and is passed into the task as the task
    parameter.  Cast the task parameter to the correct type. */
    xConsole = ( Peripheral_Descriptor_t ) pvParameters;

    /* Send a welcome message to the user knows they are connected. */
    FreeRTOS_write( xConsole, pcWelcomeMessage, strlen( pcWelcomeMessage ) );

    for( ;; )
    {
        /* This implementation reads a single character at a time.  Wait in the
        Blocked state until a character is received. */
        FreeRTOS_read( xConsole, &cRxedChar, sizeof( cRxedChar ) );

        if( cRxedChar == '\n' )
        {
            /* A newline character was received, so the input command string is
            complete and can be processed.  Transmit a line separator, just to
            make the output easier to read. */
            FreeRTOS_write( xConsole, "\r\n", strlen( "\r\n" ));

            /* The command interpreter is called repeatedly until it returns
            pdFALSE.  See the "Implementing a command" documentation for an
            exaplanation of why this is. */
            do
            {
                /* Send the command string to the command interpreter.  Any
                output generated by the command interpreter will be placed in the
                pcOutputString buffer. */
                xMoreDataToFollow = FreeRTOS_CLIProcessCommand
                              (
                                  pcInputString,   /* The command string.*/
                                  pcOutputString,  /* The output buffer. */
                                  MAX_OUTPUT_LENGTH/* The size of the output buffer. */
                              );

                /* Write the output generated by the command interpreter to the
                console. */
                FreeRTOS_write( xConsole, pcOutputString, strlen( pcOutputString ) );

            } while( xMoreDataToFollow != pdFALSE );

            /* All the strings generated by the input command have been sent.
            Processing of the command is complete.  Clear the input string ready
            to receive the next command. */
            cInputIndex = 0;
            memset( pcInputString, 0x00, MAX_INPUT_LENGTH );
        }
        else
        {
            /* The if() clause performs the processing after a newline character
            is received.  This else clause performs the processing if any other
            character is received. */

            if( cRxedChar == '\r' )
            {
                /* Ignore carriage returns. */
            }
            else if( cRxedChar == '\b' )
            {
                /* Backspace was pressed.  Erase the last character in the input
                buffer - if there are any. */
                if( cInputIndex > 0 )
                {
                    cInputIndex--;
                    pcInputString[ cInputIndex ] = '\0';
                }
            }
            else
            {
                /* A character was entered.  It was not a new line, backspace
                or carriage return, so it is accepted as part of the input and
                placed into the input buffer.  When a n is entered the complete
                string will be passed to the command interpreter. */
                if( cInputIndex < MAX_INPUT_LENGTH )
                {
                    pcInputString[ cInputIndex ] = cRxedChar;
                    cInputIndex++;
                }
            }
        }
    }
}

