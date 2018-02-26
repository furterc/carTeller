#include <string.h>
#include <stdbool.h>

#include "terminal.h"
#include "commands.h"

#define CMD_BUFFER_SIZE         128

#define ESCAPE_CHAR_BACKSPACE   1
#define ESCAPE_CHAR_ESCAPE      2

uint8_t escapeNumber = 0;

bool terminal_validFrame = false;
bool terminal_active = false;

/* Uart Handle */
static UART_HandleTypeDef hUart;

char cmdBuffer[CMD_BUFFER_SIZE];
uint8_t cmdIndex = 0;

/*!
 * Private functions
 */
uint8_t parseCommand(char *command, uint8_t argc, char **argv)
{
    uint8_t count = 0;
    argc -= 2;
    argv[count] = command;
    char *ptr = strchr(argv[count], ' ');

    while (ptr && (count++ < argc))
    {
        ptr[0] = 0;
        ptr++;

        argv[count] = ptr;

        ptr = strchr(argv[count], ' ');
    }

    count++;

    return count;
}

void handleCommand(char *cmd)
{
    uint8_t processed = 0;
    char* argv[5];
    uint8_t argc = 0;
    uint8_t cmdIndex = 0;

    argc = parseCommand(cmd, 5, argv);
    if (argc)
    {
        const sTermEntry_t *entry = term_entries[cmdIndex++];
        while (entry)
        {
            if (!strcmp(entry->cmd, cmd))
            {
                entry->cmdFunc(argc, argv);
                processed = 1;
            }
            entry = term_entries[cmdIndex++];
        }
    }

    if (!processed && (strlen(cmd) > 0))
    {
        printf(RED("Unknown command '%s' : 0x%02X, try help\n"), cmd,
                (unsigned int) *cmd);
    }

    printf(GREEN("$: "));
    fflush(stdout);
}

int _write(int file, char *data, int len)
{
    char uartBuff[128];
    char *uartPtr = uartBuff;
    for (uint8_t k = 0; k < len; k++)
    {
        uartPtr[0] = data[0];

        if (data[0] == '\n')
        {
            uartPtr++;
            uartPtr[0] = '\r';
        }

        uartPtr++;
        data++;
    }

    uint16_t uartLen = uartPtr - uartBuff;
    if (HAL_UART_Transmit(&hUart, (uint8_t *) uartBuff, uartLen, 300) != HAL_OK)
        return -1;

    return len;
}
/*!
 * Debug entries
 */

void help(uint8_t argc, char **argv)
{
    uint8_t cmdIndex = 0;
    const sTermEntry_t *entry = term_entries[cmdIndex++];

    printf(YELLOW("Available commands:\n"));
    while (entry)
    {
        printf("%s\t- %s\n", entry->cmd, entry->description);
        entry = term_entries[cmdIndex++];
    }
}
sTermEntry_t hEntry =
{ "h", "This help", help };
sTermEntry_t helpEntry =
{ "help", "This help", help };

void reboot(uint8_t argc, char **argv)
{
    printf(RED("Rebooting...\n"));
    NVIC_SystemReset();
}
sTermEntry_t rebootEntry =
{ "r", "Reboot the device", reboot };

/*!
 * Public functions
 */

void terminal_init()
{
    // sit printf \n af
    setbuf(stdout, 0);

    /*-- Configure the UART peripheral --*/
    hUart.Instance = TERM_USART;
    hUart.Init.BaudRate = 115200;
    hUart.Init.WordLength = UART_WORDLENGTH_8B;
    hUart.Init.StopBits = UART_STOPBITS_1;
    hUart.Init.Parity = UART_PARITY_NONE;
    hUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart.Init.Mode = UART_MODE_TX_RX;

    if (HAL_UART_Init(&hUart) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    TERM_USART->CR1 |= USART_CR1_RXNEIE;

    HAL_NVIC_SetPriority(TERM_USART_IRQn, 0x1, 0);
    HAL_NVIC_EnableIRQ(TERM_USART_IRQn);

    terminal_active = true;
}

void terminal_deInit()
{
    terminal_active = false;

#if 1
    HAL_UART_DeInit(&hUart);
#endif
}

void terminal_ioInit()
{
    /* Enable USART2 clock */
    TERM_USART_CLK_ENABLE()
    ;

    GPIO_InitTypeDef GPIO_InitStruct =
    { 0 };
    /* Enable GPIO TX/RX clock */
    TERM_USART_TX_GPIO_CLK_ENABLE()
    ;
    TERM_USART_RX_GPIO_CLK_ENABLE()
    ;
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;

    HAL_GPIO_Init(TERM_USART_GPIO_PORT, &GPIO_InitStruct);
}

void terminal_ioDeInit()
{
    GPIO_InitTypeDef GPIO_InitStructure =
    { 0 };

    TERM_USART_TX_GPIO_CLK_ENABLE()
    ;
    TERM_USART_RX_GPIO_CLK_ENABLE()
    ;

    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    GPIO_InitStructure.Pin = TERM_USART_TX_PIN | TERM_USART_RX_PIN;
    HAL_GPIO_Init( TERM_USART_GPIO_PORT, &GPIO_InitStructure);
}

uint8_t terminal_run()
{
    if (!terminal_active)
        return 0;

    if (terminal_validFrame)
    {
        handleCommand(cmdBuffer);
        terminal_validFrame = false;
        return 1;
    }

    switch (escapeNumber)
    {
    case 0:
        break;
    case ESCAPE_CHAR_BACKSPACE:
    {
        escapeNumber = 0;
        if (cmdIndex == 0)
        {
            printf(" ");
            return 1;
        }

        printf(" %c", 0x08);
        cmdBuffer[cmdIndex--] = 0;
    }
        break;
    case ESCAPE_CHAR_ESCAPE:
        escapeNumber = 0;
        break;
    }
    return 1;
}

void terminal_handleByte(uint8_t byte, uint8_t escaped)
{
    if (!terminal_active)
        return;

    if (escaped)
    {
        escapeNumber = ESCAPE_CHAR_ESCAPE;
        return;
    }

    if (byte == 0x08)
    {
        escapeNumber = ESCAPE_CHAR_BACKSPACE;
        return;
    }

    if (terminal_validFrame)
        return;

    if (byte == '\r') // || (byte == '\n'))
    {
        cmdBuffer[cmdIndex] = 0;
        terminal_validFrame = true;
        cmdIndex = 0;
        return;
    }

    cmdBuffer[cmdIndex++] = byte;

    if (cmdIndex > CMD_BUFFER_SIZE)
        cmdIndex = 0;
}

