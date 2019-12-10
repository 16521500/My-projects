#include "main.h"
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "queue.h"

uint32_t volatile *PDout;
queue_t Queue_send;
queue_t Queue_receive;
char *input = "\r\n Input: ";

struct node
{
  int data;
  struct node *pLeft;
  struct node *pRight;
};
typedef struct node *Node;

Node CreateNode(int data)
{
  Node temp = (Node)malloc(sizeof(struct node));
  temp->pLeft = temp;
  temp->pRight = temp;
  temp->data = data;
  return temp;
}

Node LED_East;
Node LED_North;
Node LED_West;
Node LED_South;
Node begin;
Node cursor;
bool direction = true;
char charInput;
bool isChanged = false;
void LED_board_create()
{
  LED_East = CreateNode(GPIO_Pin_4);
  LED_North = CreateNode(GPIO_Pin_5);
  LED_West = CreateNode(GPIO_Pin_6);
  LED_South = CreateNode(GPIO_Pin_7);

  LED_East->pRight = LED_North;
  LED_North->pRight = LED_West;
  LED_West->pRight = LED_South;
  LED_South->pRight = LED_East;

  LED_East->pLeft = LED_South;
  LED_North->pLeft = LED_East;
  LED_West->pLeft = LED_North;
  LED_South->pLeft = LED_West;
}
void Led_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_SetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}

void NVIC_init()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  /* Enable the SPI5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SPI5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void Spi_init()
{
  // Enable GPIO clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  // Enable SPI5 clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct;

  // Configure Port PF6, PF7, PF8 & PF9 as SPI5's NSS, SCK, MISO & MOSI
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_SPI5);

  SPI_InitTypeDef SPI_InitStruct;

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
  SPI_I2S_ITConfig(SPI5, SPI_I2S_IT_RXNE, ENABLE);

  // Enable SPI4 & SPI5
  SPI_Cmd(SPI5, ENABLE);
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

void SPI5_IRQHandler()
{
  if (SPI_I2S_GetITStatus(SPI5, SPI_I2S_IT_RXNE) == SET)
  {
    charInput = SPI_I2S_ReceiveData(SPI5);
  }
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
Node setLed()
{
  // while (!SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_RXNE))
  //   ;
  //  queue_push_string(&Queue_send, (char *)input, strlen(input));
  //  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  //  while (queue_is_empty(&Queue_receive))
  //    ;
  while (!isChanged)
    ;
  isChanged = true;
  charInput = queue_pop(&Queue_receive);

  switch (charInput)
  {
  case 'e':
  {
    return LED_East;
  }
  break;
  case 'n':
  {
    return LED_North;
  }
  break;
  case 'w':
  {
    return LED_West;
  }
  break;
  case 's':
  {
    return LED_South;
  }
  break;
  default:
    return LED_East;
  }
}

void setDirection()
{
  // while (!SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_RXNE))
  //   ;
  //  queue_push_string(&Queue_send, (char *)input, strlen(input));
  //  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
  //  while (queue_is_empty(&Queue_receive))
  //    ;
  //  charInput = queue_pop(&Queue_receive);
  while (!isChanged)
    ;
  isChanged = true;
  switch (charInput)
  {
  case 'c':
  {
    if (!direction)
    {
      if (cursor != begin)
        cursor = cursor->pRight;
    }
    direction = true;
  }
  break;
  case 'a':
  {
    if (direction)
    {
      if (cursor != begin)
        cursor = cursor->pLeft;
    }
    direction = false;
  }
  break;
  }
}

void singleLedOn()
{
  GPIO_SetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

  GPIO_ResetBits(GPIOD, cursor->data);
  if (direction)
    cursor = cursor->pRight;
  else
    cursor = cursor->pLeft;
}

void start()
{
  cursor = begin;
  GPIO_SetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
  while (charInput != 's')
  {
    //  while (queue_is_empty(&Queue_receive))
    //  {

    for (int i = 0; i < 3000000; i++)
      ;
    GPIO_ToggleBits(GPIOD, cursor->data);
    if (direction)
      cursor = cursor->pRight;
    else
      cursor = cursor->pLeft;
  }
}

int main()
{
  RCC_DeInit();
  PDout = &GPIOD->ODR;
  //  queue_init(&Queue_send);
  //  queue_init(&Queue_receive);
  Led_init();
  //  GPIO_ResetBits(GPIOD, GPIO_Pin_4);
  NVIC_init();
  //  Uart_init();
  Spi_init();
  LED_board_create();
  begin = LED_East;
  cursor = LED_East;
  //  GPIO_SetBits(GPIOD, GPIO_Pin_4);
  while (1)
  {
    // while (!SPI_I2S_GetFlagStatus(SPI5, SPI_I2S_FLAG_RXNE))
    //   ;
    //    queue_push_string(&Queue_send, (char *)input, strlen(input));
    //    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    //    while (queue_is_empty(&Queue_receive))
    //      ;
    //    charInput = queue_pop(&Queue_receive);
    while (!isChanged)
      ;
    isChanged = true;
    switch (charInput)
    {
    case 'a':
    {
      begin = setLed();
      cursor = begin;
    }
    break;
    case 'b':
    {
      setDirection();
    }
    break;
    case 'c':
    {
      start();
    }
    break;
    case 't':
    {
      singleLedOn();
    }
    break;
    case 'q':
    {
      GPIO_SetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    }
    break;
    }
  }
}