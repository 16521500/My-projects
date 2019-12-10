#include "main.h"
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "queue.h"

#define STR_MAX_LENGTH 12

int OP1 = 0, OP2 = 0, result = 0;

char *newLineStr = "\r\n";

static char TempStr[10] = "\0\0\0\0\0\0\0\0\0\0";
uint8_t length = 0;

queue_t Queue_send;
queue_t Queue_receive;

bool isAdvanceLed = false;
bool isTimer = false;

RCC_ClocksTypeDef RCC_Clocks;
TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
int LED_Period = 10000;
//functions definition
void mainMenu()
{
  char *comp = "Choose your option(1,2 ..): \r\n 1. Student info \r\n 2. Basic operation \r\n 3. Simple led \r\n 4. Advance led \r\n 5. Timer \r\n\r\n";

  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    switch (queue_pop(&Queue_receive))
    {
    case 49:
    {
      studentInfoMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 50:
    {
      basicOperationMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 51:
    {
      simpleLedMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 52:
    {
      isAdvanceLed = true;
      advanceLedMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 53:
    {
      isTimer = true;
      timerMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    }
  }
}

void studentInfoMenu()
{
  char *comp = " 1.Student info \r\n ID_1: 16521500 \r\n Full name: PHAM THANH DAT \r\n\r\n ID_2: 16520969 \r\n Full name: NGUYEN NAM PHUONG \r\n ESC: return previous menu \r\n\r\n";
  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    if (queue_pop(&Queue_receive) == 8)
      return;
  }
}

void basicOperationMenu()
{
  char *comp = " 2.Basic Operation(a,b,..): \r\n a. Plus \r\n b. Subtract \r\n c. Multiple \r\n d. Divide \r\n e. Module \r\n ESC: return previous menu \r\n\r\n";
  char *OP_str;
  char Operator;

  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    //    if (queue_peek(&Queue_receive) == 8)
    //    {
    //      queue_pop(&Queue_receive);
    //      return;
    //    }
    switch (queue_pop(&Queue_receive))
    {
    case 97:
    {
      OP_str = " a.Plus \r\n";
      Operator = '+';
      operationMenu(OP_str, Operator);
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 98:
    {
      OP_str = " b.Subtract \r\n";
      Operator = '-';
      operationMenu(OP_str, Operator);
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 99:
    {
      OP_str = " c.Multiply \r\n";
      Operator = '*';
      operationMenu(OP_str, Operator);
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 100:
    {
      OP_str = " d.Divide \r\n";
      Operator = '/';
      operationMenu(OP_str, Operator);
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 101:
    {
      OP_str = " e.Module \r\n";
      Operator = '%';
      operationMenu(OP_str, Operator);
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 8:
    {
      return;
    }
    }
  }
}

void operationMenu(char *OP_str, char Operator)
{
  char *comp1 = " Operand 1: ";
  char *comp2 = " Operand 2: ";
  char *comp3 = " Warning: Division by zero! \r\n\r\n";
  char *comp4 = " Result:        ";
  char *comp5 = " ESC return previous menu \r\n\r\n";

  queue_push_string(&Queue_send, (char *)OP_str, strlen(OP_str));
  queue_push_string(&Queue_send, (char *)comp1, strlen(comp1));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  UARTReceive(10);
  queue_push_string(&Queue_send, (char *)TempStr, length);
  queue_push_string(&Queue_send, (char *)newLineStr, strlen(newLineStr));
  OP1 = str2int(TempStr);

  queue_push_string(&Queue_send, (char *)comp2, strlen(comp2));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  UARTReceive(10);
  queue_push_string(&Queue_send, (char *)TempStr, length);
  queue_push_string(&Queue_send, (char *)newLineStr, strlen(newLineStr));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  OP2 = str2int(TempStr);

  // Calculation
  switch (Operator)
  {
  case 43:
    result = OP1 + OP2;
    break;
  case 45:
    result = OP1 - OP2;
    break;
  case 42:
    result = OP1 * OP2;
    break;
  case 47:
  {
    if (!OP2)
      queue_push_string(&Queue_send, (char *)comp3, strlen(comp3));
    result = OP1 / OP2;
  }
  break;
  case 37:
  {
    if (!OP2)
      queue_push_string(&Queue_send, (char *)comp3, strlen(comp3));
    result = OP1 % OP2;
  }
  break;
  }

  // Convert result from integer to string and send
  length = int2str(TempStr, result);

  queue_push_string(&Queue_send, (char *)comp4, strlen(comp4));
  queue_push_string(&Queue_send, (char *)TempStr, strlen(TempStr));
  queue_push_string(&Queue_send, (char *)newLineStr, strlen(newLineStr));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  // Wait for ESC key
  queue_push_string(&Queue_send, (char *)comp5, strlen(comp5));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    if (queue_pop(&Queue_receive) == 8)
      return;
  }
}

void simpleLedMenu()
{
  char *comp = " 3.Simple led(a,b,c,d) \r\n a.Green On \r\n b.Green Off \r\n c.Red On \r\n d.Red Off \r\n ESC return previous menu \r\n\r\n";
  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    //    if (queue_peek(&Queue_receive) == 8)
    //    {
    //      queue_pop(&Queue_receive);
    //      return;
    //    }
    switch (queue_pop(&Queue_receive))
    {
    case 97:
      GPIO_SetBits(GPIOG, GPIO_Pin_13);
      break;
    case 98:
      GPIO_ResetBits(GPIOG, GPIO_Pin_13);
      break;
    case 99:
      GPIO_SetBits(GPIOG, GPIO_Pin_14);
      break;
    case 100:
      GPIO_ResetBits(GPIOG, GPIO_Pin_14);
      break;
    case 8:
      return;
    }
  }
}

void advanceLedMenu()
{
  char *comp = " 4.Advance led (a,b,c): \r\n a.Set led \r\n b.Set direction \r\n c.Start \r\n ESC return previous menu \r\n\r\n";
  GPIO_ResetBits(GPIOE, GPIO_Pin_11);
  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    switch (queue_pop(&Queue_receive))
    {
    case 'a':
    {
      SPI_Tx('a');
      setLedMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 'b':
    {
      SPI_Tx('b');
      setDirectionMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 'c':
    {
      SPI_Tx('c');
      startMenu();
      queue_push_string(&Queue_send, (char *)comp, strlen(comp));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
    break;
    case 8:
    {
      isAdvanceLed = false;
      GPIO_SetBits(GPIOE, GPIO_Pin_11);
      return;
    }
    break;
    }
  }
}

void setLedMenu()
{
  char *comp1 = " a.Set led(e,n,w,s): ";
  char *comp2 = "ESC return previous menu \r\n\r\n";
  queue_push_string(&Queue_send, (char *)comp1, strlen(comp1));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    switch (queue_peek(&Queue_receive))
    {
    case 'e':
    case 'n':
    case 'w':
    case 's':
    {
      SPI_Tx(queue_peek(&Queue_receive));
      queue_push(&Queue_send, queue_pop(&Queue_receive));
      queue_push_string(&Queue_send, (char *)newLineStr, strlen(newLineStr));
      queue_push_string(&Queue_send, (char *)comp2, strlen(comp2));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
      while (1)
      {
        while (queue_is_empty(&Queue_receive))
          ;
        if (queue_pop(&Queue_receive) == 8)
          return;
      }
    }
    break;
    default:
      queue_pop(&Queue_receive);
    }
  }
}

void setDirectionMenu()
{
  char *comp1 = " b.Set direction(c,a): ";
  char *comp2 = " ESC return previous menu \r\n\r\n";
  queue_push_string(&Queue_send, (char *)comp1, strlen(comp1));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    switch (queue_peek(&Queue_receive))
    {
    case 'c':
    case 'a':
    {
      SPI_Tx(queue_peek(&Queue_receive));
      queue_push(&Queue_send, queue_pop(&Queue_receive));
      queue_push_string(&Queue_send, (char *)newLineStr, strlen(newLineStr));
      queue_push_string(&Queue_send, (char *)comp2, strlen(comp2));
      USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
      while (1)
      {
        while (queue_is_empty(&Queue_receive))
          ;
        if (queue_pop(&Queue_receive) == 8)
          return;
      }
    }
    break;
    default:
      queue_pop(&Queue_receive);
    }
  }
}

void startMenu()
{
  char *comp = " c.Running... \r\n ESC return previous menu \r\n\r\n";
  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    if (queue_pop(&Queue_receive) == 8)
    {
      SPI_Tx('s');
      return;
    }
  }
}
void timerMenu()
{
  char *comp = "Blinking... \r\n Press UserButton: double speed \r\n Press & hold UserButton: halve speed \r\n ESC return previous menu \r\n\r\n";
  queue_push_string(&Queue_send, (char *)comp, strlen(comp));
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
  // Wait for ESC key
  while (1)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    if (queue_pop(&Queue_receive) == 8)
    {
      GPIO_ResetBits(GPIOG, GPIO_Pin_13 | GPIO_Pin_14);
      TIM_Cmd(TIM2, DISABLE);
      TIM_Cmd(TIM3, DISABLE);
      isTimer = false;
      return;
    }
  }
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  /* Enable the EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIMER2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //  NVIC_InitStructure.NVIC_IRQChannel = SPI5_IRQn;
  //  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIMER3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Led_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_SetBits(GPIOG, GPIO_Pin_13);
  GPIO_SetBits(GPIOG, GPIO_Pin_14);
}

void Button_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
void Uart_init()
{
  //Preparations
  GPIO_InitTypeDef gpio_init;
  USART_InitTypeDef usart_init;

  //Initialize
  GPIO_StructInit(&gpio_init);
  USART_StructInit(&usart_init);

  //Enable peripheral clock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  //According to the USART mode, enable the GPIO clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  //Connect the pin to the desired peripherals's Alternate function
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

  //Configure the desired pin in alternate function
  //Select the type pull-up/pull-down and output speed
  //Call GPIO_Init() function
  gpio_init.GPIO_Mode = GPIO_Mode_AF;
  gpio_init.GPIO_OType = GPIO_OType_PP;
  gpio_init.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
  gpio_init.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &gpio_init);

  //Program the Baud Rate, Word Length , Stop Bit, Parity, Hardware
  //flow control and Mode(Receiver/Transmitter) using the USART_Init()
  //function.
  usart_init.USART_BaudRate = 9600;
  usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  usart_init.USART_Parity = USART_Parity_No;
  usart_init.USART_StopBits = USART_StopBits_1;
  usart_init.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART3, &usart_init);

  //Enable the NVIC and the corresponding interrupt using the function
  //USART_ITConfig() if you need to use interrupt mode.
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  //Enable USART3
  USART_Cmd(USART3, ENABLE);
}

void UARTReceive(uint8_t strLen)
{
  uint8_t index = 0;

  while (index < strLen && index < 10)
  {
    while (queue_is_empty(&Queue_receive))
      ;
    if (queue_peek(&Queue_receive) == 8)
    {
      if (index != 0)
      {
        queue_pop(&Queue_receive);
        index -= 2;
      }
    }
    else
    {
      if (queue_peek(&Queue_receive) == 13)
      {
        queue_pop(&Queue_receive);
        TempStr[index] = '\0';
        length = index;
        return;
      }
      else
      {
        if (index == 0 && queue_peek(&Queue_receive) == 45)
        {
          TempStr[index] = queue_pop(&Queue_receive);
        }
        else if (queue_peek(&Queue_receive) >= 48 && queue_peek(&Queue_receive) <= 57)
          TempStr[index] = queue_pop(&Queue_receive);
        else
        {
          queue_pop(&Queue_receive);
          index--;
        }
      }
    }
    index++;
  }
  length = index;
}

void Spi_init()
{
  // Enable GPIO clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);

  // Enable SPI4 & SPI5 clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct;

  // Configure Port PE11 as SPI4's NSS
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOE, &GPIO_InitStruct);

  // Configure Port PE12, PE13 & PE14 as SPI4's SCK, MISO & MOSI
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOE, &GPIO_InitStruct);

  // Configure Port PF6, PF7, PF8 & PF9 as SPI5's NSS, SCK, MISO & MOSI
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStruct);

  // Connect the pin to the desired peripherals' Alternate Function
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_SPI4);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_SPI4);

  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_SPI5);

  SPI_InitTypeDef SPI_InitStruct;

  // Configure SPI4 as Master
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI4, &SPI_InitStruct);

  // Configure SPI5 as Slave
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;
  SPI_Init(SPI5, &SPI_InitStruct);

  // Enable Slave interrupt
  //SPI_I2S_ITConfig(SPI5, SPI_I2S_IT_RXNE, ENABLE);

  // Enable SPI4 & SPI5
  SPI_Cmd(SPI4, ENABLE);
  SPI_Cmd(SPI5, ENABLE);
}

//Tx SPI param: data
void SPI_Tx(uint8_t data)
{
  //GPIO_ResetBits(GPIOE, GPIO_Pin_11);
  while (!SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_TXE))
    ;
  SPI_I2S_SendData(SPI4, data);
  //  while (!SPI_I2S_GetFlagStatus(SPI4, SPI_I2S_FLAG_RXNE))
  //    ;
  //  SPI_I2S_ReceiveData(SPI4);
  //GPIO_SetBits(GPIOE, GPIO_Pin_11);
}

//Rx SPI param: return data
uint8_t SPI_Rx(void)
{
  uint8_t data;

  //GPIO_ResetBits(GPIOE, GPIO_Pin_11);
  //  while (!SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_TXE))
  //    ;
  //  SPI_I2S_SendData(SPI5, 0);
  while (!SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_RXNE))
    ;
  data = (uint8_t)SPI_I2S_ReceiveData(SPI5);
  //GPIO_SetBits(GPIOE, GPIO_Pin_11);

  return data;
}
void Timer_init()
{
  // Enable clock for TIM2 & TIM3
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

  TIM_TimeBaseInitStruct.TIM_Prescaler = ((RCC_Clocks.PCLK1_Frequency / 10000) - 1);
  TIM_TimeBaseInitStruct.TIM_Period = LED_Period - 1;
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

  // TIM2 initialize
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

  TIM_TimeBaseInitStruct.TIM_Period = (LED_Period / 2) - 1;

  // TIM3 initialize
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

  // Enable TIM2 & TIM3 interrupt
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void USART3_IRQHandler(void)
{
  if (USART_GetITStatus(USART3, USART_IT_TXE) == SET)
  {

    if (!queue_is_empty(&Queue_send))
    {
      USART_SendData(USART3, queue_pop(&Queue_send));
    }
    else
    {
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      //            USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    }
  }

  if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
  {
    //        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
    queue_push(&Queue_receive, (char)USART_ReceiveData(USART3));
  }
}

void SPI5_IRQHandler()
{
  if (SPI_I2S_GetITStatus(SPI5, SPI_I2S_IT_RXNE) == SET)
  {
    //slave.receive = SPI_I2S_ReceiveData(SPI5);
  }
}
void TIM2_IRQHandler()
{
  // Checks whether the TIM2 interrupt has occurred or not
  if (TIM_GetITStatus(TIM2, TIM_IT_Update))
  {
    GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
    // Clears the TIM2 interrupt pending bit
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

void TIM3_IRQHandler()
{
  // Checks whether the TIM2 interrupt has occurred or not
  if (TIM_GetITStatus(TIM3, TIM_IT_Update))
  {
    GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
    // Clears the TIM2 interrupt pending bit
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
}

void EXTI0_IRQHandler()
{
  if (isTimer && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
  {
    for (int i = 0; i < 300000; i++)
    {
      if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
      {
        LED_Period /= 2;
        TIM_TimeBaseInitStruct.TIM_Period = LED_Period - 1;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
        TIM_TimeBaseInitStruct.TIM_Period = (LED_Period / 2) - 1;
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

        EXTI_ClearITPendingBit(EXTI_Line0);
        return;
      }
    }
    LED_Period *= 2;
    TIM_TimeBaseInitStruct.TIM_Period = LED_Period - 1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Period = (LED_Period / 2) - 1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    EXTI_ClearITPendingBit(EXTI_Line0);
  }
  if (isAdvanceLed && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
  {
    SPI_Tx('t');
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

int str2int(char a[])
{
  int c, sign, offset, n;
  if (a[0] == '-')
  { // Handle negative integers
    sign = -1;
  }
  if (sign == -1)
  { // Set starting position to convert
    offset = 1;
  }
  else
  {
    offset = 0;
  }
  n = 0;
  for (c = offset; a[c] != '\0'; c++)
  {
    n = n * 10 + a[c] - '0';
  }
  if (sign == -1)
  {
    n = -n;
  }
  return n;
}

uint8_t int2str(char str[], int num)
{
  int index = 0, scale = 1, tmp = num, c;
  if (num < 0)
  {
    str[index++] = '-';
    tmp *= -1;
  }

  while (num / 10)
  {
    scale *= 10;
    num /= 10;
  }

  while (scale != 0)
  {
    c = tmp / scale;
    tmp %= scale;
    scale /= 10;
    str[index++] = (char)(c + 48);
  }
  if (index < 9)
    str[index] = '\0';
  return length = index;
}

int main()
{
  RCC_DeInit();
  RCC_GetClocksFreq(&RCC_Clocks);

  // Initialize Queue
  queue_init(&Queue_send);
  queue_init(&Queue_receive);

  // Initialize NVIC
  NVIC_Configuration();
  // Initialize UART
  Uart_init();
  // Initialize LED
  Led_init();
  // Initialize Button
  Button_init();
  // Initialize
  Spi_init();
  // Initialize Timer
  Timer_init();

  mainMenu();

  return 0;
}
