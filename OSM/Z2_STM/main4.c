/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ll_spi_ili9341.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* Definitions for LCD_Task */
osThreadId_t LCD_TaskHandle;
const osThreadAttr_t LCD_Task_attributes = {
    .name = "LCD_Task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for LED1_task */
osThreadId_t LED1_taskHandle;
const osThreadAttr_t LED1_task_attributes = {
    .name = "LED1_task",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for LED2_tack */
osThreadId_t LED2_tackHandle;
const osThreadAttr_t LED2_tack_attributes = {
    .name = "LED2_tack",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal,
};
/* Definitions for BinarySem01 */
osSemaphoreId_t BinarySem01Handle;
const osSemaphoreAttr_t BinarySem01_attributes = {
    .name = "BinarySem01"};
/* USER CODE BEGIN PV */
uint8_t DmaSpiCnt = 1;

int x = 120, y = 160; // Skladowe x,y polozenia kwadratu
int x_old = 120, y_old = 160;
int dx = 1, dy = 1; // Skladowe wektora predkosci dx, dy

unsigned char *ptr;
unsigned char napis[] = {"Press the blue button..."};
int dl_n = 24;
unsigned int nx = 0;
unsigned int ny = 280;
unsigned int key;

// ------------------------------------------------------------------------------------------------
int Tim = 0; // Licznik uzytkownika
int TimPrev = 0;
unsigned int preScale = 0;       // Preskaler licznika uzytkownika
volatile char EnableRefresh = 0; // Zezwolenie na odswiezenie zawartosci pamieci graficznej
// Screen
const int X = 320;
const int Y = 240;
const int t_a[] = {X / 2 - 20, X - 20};
const int t_H[] = {Y / 2 - 10, Y - 10};
const int t_x0[] = {0, X / 2 + 5};
const int t_y0[] = {0, Y / 2 + 5};
int screenNumber = 1;
int currentScreenNumber;

// Tp
const unsigned int Tp_ms = 2000;
const float Tp = (float)Tp_ms / 1000;

// UAR
const float H_MAX = 100;
float yZad = H_MAX / 2;

// PID
const float c_kp = 0.1;
const float c_Ti = 1;
const float c_Td = 0.2;

const float Smin = 0;
const float t_yZad[] = {0, H_MAX * 0.25, H_MAX * 0.5, H_MAX * 0.75, H_MAX * 1};
const float t_kp[] = {0.5 * c_kp, c_kp, 2 * c_kp};
const float t_Ti[] = {2 * c_Ti, c_Ti, 0.5 * c_Ti};
const float t_Td[] = {0.5 * c_Td, c_Td, 2 * c_Td};
float kp = t_kp[1];
float Ti = t_Ti[1];
float Td = t_Td[1];
const float t_z[] = {0, H_MAX / 20, H_MAX / 10};
const float Smax = H_MAX;

// G
float pole = 0.1;

// Signals
float e = 0;
float u = 0;
float yWy = 0;
float yWyPrev = 0;
float z = 10;
float ki;
float kd;

// v
const int nv = 40;
int iv = 0;

char block = 0;

unsigned char ch_u[] = "u(n)";
unsigned char ch_e[] = "e(n)";
unsigned char ch_y[] = "y(n)";
unsigned char ch_kp[] = "kp =     ";
unsigned char ch_Ti[] = "Ti =     ";
unsigned char ch_Td[] = "Td =     ";

int *v_u;
int *v_e;
int *v_yWy;

double temp = 0;
unsigned char ch_yZad[] = "yZad =    ";
// ------------------------------------------------------------------------------------------------
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RNG_Init(void);
static void MX_SPI5_Init(void);
void StartLCDTask(void *argument);
void StartTaskLED1(void *argument);
void StartTaskLED2(void *argument);

/* USER CODE BEGIN PFP */
void move_square_C(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void move_square_C()
{
  // Aktualizacja po│ozenia na podstawie wektora predkosci
  x += dx;
  y += dy;
  // Ograniczenie obszaru poruszania kwadratem w obrebie LCD
  if (x <= 1 || x >= 240 - 34)
    dx = -dx;
  if (y <= 1 || y >= 320 - 34)
    dy = -dy;
}

void clearArray(int *ar, int n)
{
  for (int i = 0; i < n; i++)
  {
    ar[i] = 0;
  }
}

float PID_Calculate(float in, char clear)
{
  static float sum = 0;
  static float inPrev = 0;
  float out;
  if (clear)
  {
    inPrev = 0;
    sum = 0;
    return 0;
  }
  sum += in * ki;
  if (sum > Smax)
    sum = Smax;
  if (sum < Smin)
    sum = Smin;
  out = sum + in * kp + (in - inPrev) * kd;
  inPrev = in;
  return out;
}

float InercModel_Calculate(float in)
{
  float out = 0;
  out = out * pole + (1 - pole) * in;
  return out;
}

void FunctionRealTime(int a, int H, int x0, int y0, int *v, float H_MAX, char clear)
{
  DrawAxes(a, H, x0, y0, clear);
  int dx = a / nv;
  int x = x0;
  int y = 0;
  for (int i = 0; i < nv; i++)
  {
    if (v[i] != 0)
    {
      y = v[i];
      y = y * H / H_MAX;
      y = H + y0 - y;
      if (clear)
      {
        TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
      }
      else
      {
        TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_YELLOW);
      }
    }
    x = x + dx;
  }
}

void DrawAxes(int a, int H, int x0, int y0, char clear)
{
  for (int y = y0; y <= y0 + H; y++)
  {
    for (int x = x0; x <= x0 + a; x++)
    {
      if (y == y0 + H || x == x0)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
  }
}

void Cross(char clear)
{
  for (int y = 0; y < Y; y++)
  {
    for (int x = 0; x < X; x++)
    {
      if (y == Y / 2 || x == X / 2)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
  }
}

void WaterTank_Pipes(int a, int H, int x0, int y0, float yWy, float yWyPrev, float H_MAX, float z, char clear)
{
  int a_P = a / 4;
  int a_WT = a - 2 * a_P;

  int H_WT = H;
  int H_P = H / 10;
  int H_WA = H - H_P;

  int x0_WT = x0 + a_P;
  int y0_WT = y0;
  int x0_P1 = x0;
  int y0_P1 = y0;
  int x0_P2 = x0 + a_P + a_WT;
  int y0_P2 = y0 + H - H_P;
  int x0_WA = x0 + a_P;
  int y0_WA = y0 + H_P;

  float h_WT = yWy * H / H_MAX;
  int h_P1 = 0;
  float h_P2 = z * H / H_MAX;

  float dh_WT = (float)(yWy - yWyPrev) * H / H_MAX;

  WaterTank(a_WT, H_WT, x0_WT, y0_WT, h_WT, clear);
  h_P1 = Pipe_In(a_P, H_P, x0_P1, y0_P1, dh_WT, H_WT, clear);
  WaterAnimation(a_WT, H_WA, x0_WA, y0_WA, dh_WT, H_WT, h_P1, clear);
  Pipe_Out(a_P, H_P, x0_P2, y0_P2, h_P2, h_WT, H_MAX, clear);
}

void WaterAnimation(int a, int H, int x0, int y0, float dh_WT, int H_WT, int h_P1, char clear)
{
  const float vMax = (float)H / (a * a);
  float v;
  if (dh_WT > 0 && dh_WT <= H_WT / 100)
  {
    v = 50 * vMax;
  }
  if (dh_WT > H_WT / 100 && dh_WT <= H_WT / 90)
  {
    v = 45 * vMax;
  }
  if (dh_WT > H_WT / 90 && dh_WT <= H_WT / 80)
  {
    v = 40 * vMax;
  }
  if (dh_WT > H_WT / 80 && dh_WT <= H_WT / 70)
  {
    v = 35 * vMax;
  }
  if (dh_WT > H_WT / 70 && dh_WT <= H_WT / 60)
  {
    v = 30 * vMax;
  }
  if (dh_WT > H_WT / 60 && dh_WT <= H_WT / 50)
  {
    v = 25 * vMax;
  }
  if (dh_WT > H_WT / 50 && dh_WT <= H_WT / 40)
  {
    v = 20 * vMax;
  }
  if (dh_WT > H_WT / 40 && dh_WT <= H_WT / 30)
  {
    v = 15 * vMax;
  }
  if (dh_WT > H_WT / 30 && dh_WT <= H_WT / 20)
  {
    v = 10 * vMax;
  }
  if (dh_WT > H_WT / 20 && dh_WT <= H_WT / 10)
  {
    v = 5 * vMax;
  }
  if (dh_WT > H_WT / 10 && dh_WT <= H_WT)
  {
    v = 1 * vMax;
  }

  int y;
  if (dh_WT > 0)
  {
    for (int x = x0; x <= x0 + a; x++)
    {
      for (int i = 0; i <= h_P1; i++)
      {
        y = y0 + v * (x - x0) * (x - x0) - i;
        if (y < y0 + H && x < x0 + a)
        {
          if (clear)
          {
            TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
          }
          else
          {
            TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
          }
        }
      }
    }
  }
}

void WaterTank(int a, int H, int x0, int y0, float h, char clear)
{
  for (int y = y0; y <= y0 + H; y++)
  {
    if (y == y0 + H)
    {
      for (int x = x0; x <= x0 + a; x++)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
    for (int x = x0; x <= x0 + a; x++)
    {
      if ((x == x0) || (x == x0 + a))
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
  }

  for (int y = y0; y <= y0 + H; y++)
  {
    if (y <= y0 + H && y >= y0 + H - h)
    {
      for (int x = x0; x <= x0 + a; x++)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
        }
      }
    }
  }
}

int Pipe_In(int a, int H, int x0, int y0, float dh_WT, int H_WT, char clear)
{
  for (int y = y0; y <= y0 + H; y++)
  {
    if (y == y0 + H || y == y0)
    {
      for (int x = x0; x <= x0 + a; x++)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
    for (int x = x0; x <= x0 + a; x++)
    {
      if ((x == x0) || (x == x0 + a))
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
  }

  float h;
  if (dh_WT > 0 && dh_WT <= H_WT / 100)
    h = 0.1 * H;
  if (dh_WT > H_WT / 100 && dh_WT <= H_WT / 90)
    h = 0.1 * H;
  if (dh_WT > H_WT / 90 && dh_WT <= H_WT / 80)
    h = 0.2 * H;
  if (dh_WT > H_WT / 80 && dh_WT <= H_WT / 70)
    h = 0.3 * H;
  if (dh_WT > H_WT / 70 && dh_WT <= H_WT / 60)
    h = 0.4 * H;
  if (dh_WT > H_WT / 60 && dh_WT <= H_WT / 50)
    h = 0.5 * H;
  if (dh_WT > H_WT / 50 && dh_WT <= H_WT / 40)
    h = 0.6 * H;
  if (dh_WT > H_WT / 40 && dh_WT <= H_WT / 30)
    h = 0.7 * H;
  if (dh_WT > H_WT / 30 && dh_WT <= H_WT / 20)
    h = 0.8 * H;
  if (dh_WT > H_WT / 20 && dh_WT <= H_WT / 10)
    h = 0.9 * H;
  if (dh_WT > H_WT / 10 && dh_WT <= H_WT)
    h = H;

  for (int y = y0; y <= y0 + H; y++)
  {
    if (y <= y0 + H && y >= y0 + H - h)
    {
      for (int x = x0; x <= x0 + a; x++)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
        }
      }
    }
  }

  return h;
}

void Pipe_Out(int a, int H, int x0, int y0, float h, float h_WT, float H_MAX, char clear)
{
  for (int y = y0; y <= y0 + H; y++)
  {
    if (y == y0 + H || y == y0)
    {
      for (int x = x0; x <= x0 + a; x++)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
    for (int x = x0; x <= x0 + a; x++)
    {
      if ((x == x0) || (x == x0 + a))
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLACK);
        }
      }
    }
  }
  float hSaturated;
  if (h_WT < h && h != 0)
  {
    hSaturated = h_WT;
  }
  else if (h != 0)
  {
    hSaturated = h;
  }

  for (int y = y0; y <= y0 + H; y++)
  {
    if (y <= y0 + H && y >= y0 + H - hSaturated)
    {
      for (int x = x0; x <= x0 + a; x++)
      {
        if (clear)
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_RED);
        }
        else
        {
          TFTDisplay_ILI9341_DrawPixel(x, y, TFT_COLOR_ILI9341_BLUE);
        }
      }
    }
  }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */
  currentScreenNumber = screenNumber;
  ptr = napis;
  ki = kp * Tp / Ti;
  kd = kp * Td / Tp;
  v_u = malloc(nv * sizeof(int));
  v_e = malloc(nv * sizeof(int));
  v_yWy = malloc(nv * sizeof(int));

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
  NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));
  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RNG_Init();
  MX_SPI5_Init();
  /* USER CODE BEGIN 2 */
  LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_4);
  LL_DMA_ClearFlag_TC4(DMA2);
  LL_DMA_ClearFlag_TE4(DMA2);
  LL_SPI_EnableDMAReq_TX(SPI5);
  LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_4);
  LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_4);
  LL_SPI_Enable(SPI5);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BinarySem01 */
  BinarySem01Handle = osSemaphoreNew(1, 1, &BinarySem01_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LCD_Task */
  LCD_TaskHandle = osThreadNew(StartLCDTask, NULL, &LCD_Task_attributes);

  /* creation of LED1_task */
  LED1_taskHandle = osThreadNew(StartTaskLED1, NULL, &LED1_task_attributes);

  /* creation of LED2_tack */
  LED2_tackHandle = osThreadNew(StartTaskLED2, NULL, &LED2_tack_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  free(v_u);
  free(v_e);
  free(v_yWy);
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_PWR_EnableOverDriveMode();
  LL_RCC_HSE_Enable();

  /* Wait till HSE is ready */
  while (LL_RCC_HSE_IsReady() != 1)
  {
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 180, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 180, LL_RCC_PLLQ_DIV_8);
  LL_RCC_PLL_Enable();

  /* Wait till PLL is ready */
  while (LL_RCC_PLL_IsReady() != 1)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  /* Wait till System clock is ready */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }
  LL_Init1msTick(180000000);
  LL_SetSystemCoreClock(180000000);
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/**
 * @brief RNG Initialization Function
 * @param None
 * @retval None
 */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* Peripheral clock enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  LL_RNG_Enable(RNG);
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */
}

/**
 * @brief SPI5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI5);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  /**SPI5 GPIO Configuration
  PF7   ------> SPI5_SCK
  PF8   ------> SPI5_MISO
  PF9   ------> SPI5_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* SPI5 DMA Init */

  /* SPI5_TX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_4, LL_DMA_CHANNEL_2);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_4, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_4, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_4, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_4, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_4);

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(SPI5, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI5, LL_SPI_PROTOCOL_MOTOROLA);
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

  /* Init with LL driver */
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* DMA interrupt init */
  /* DMA2_Stream4_IRQn interrupt configuration */
  NVIC_SetPriority(DMA2_Stream4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
  NVIC_EnableIRQ(DMA2_Stream4_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);

  /**/
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_2);

  /**/
  LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_12 | LL_GPIO_PIN_13);

  /**/
  LL_GPIO_ResetOutputPin(GPIOG, LL_GPIO_PIN_13 | LL_GPIO_PIN_14);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12 | LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
void DMA1_Stream4_TransferComplete(void)
{
  LL_DMA_ClearFlag_TC4(DMA2);
  DmaSpiCnt--;

  if (DmaSpiCnt == 0)
  {
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_4);
    DmaSpiCnt = 1;
    CS_DESELECT();
  }
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartLCDTask */
/**
 * @brief  Function implementing the LCD_Task thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLCDTask */
void StartLCDTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  TFTDisplay_ILI9341_Initialization(240, 320);
  TFTDisplay_ILI9341_FillScreen(TFT_COLOR_ILI9341_RED);
  TFTDisplay_ILI9341_SetRotation(1);
  for (;;)
  {
    TFTDisplay_ILI9341_SetTextColor(TFT_COLOR_ILI9341_YELLOW);
    if (GPIOA->IDR & 0x0001)
    {
      if (!block)
      {
        screenNumber++;
        if (screenNumber == 3 || screenNumber == 5 || screenNumber == 7)
        {
          yWy = 0;
          yWyPrev = 0;
          iv = 0;
          PID_Calculate(0, 1);
          clearArray(v_u, nv);
          clearArray(v_e, nv);
          clearArray(v_yWy, nv);
          if (screenNumber == 7)
            screenNumber = 1;
        }
        block = 1;
      }
    }
    else
    {
      block = 0;
    }
    if (Tim != TimPrev)
    {
      switch (screenNumber)
      {
      case 1:
        WaterTank_Pipes(t_a[1], t_H[1], t_x0[0], t_y0[0], yWy, yWyPrev, H_MAX, z, 0);

        temp = kp;
        ch_kp[5] = '0' + temp;
        ch_kp[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_kp[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_kp[8] = '0' + temp;

        temp = Ti;
        ch_Ti[5] = '0' + temp;
        ch_Ti[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_Ti[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_Ti[8] = '0' + temp;

        temp = Td;
        ch_Td[5] = '0' + temp;
        ch_Td[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_Td[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_Td[8] = '0' + temp;

        temp = yZad;
        ch_yZad[7] = '0' + (int)temp / 100;
        temp = temp - ((int)temp / 100) * 100;
        ch_yZad[8] = '0' + (int)temp / 10;
        temp = temp - ((int)temp / 10) * 10;
        ch_yZad[9] = '0' + (int)temp;

        TFTDisplay_ILI9341_String(210, 5, ch_kp);
        TFTDisplay_ILI9341_String(210, 20, ch_Ti);
        TFTDisplay_ILI9341_String(210, 35, ch_Td);
        TFTDisplay_ILI9341_String(210, 50, ch_yZad);
        break;

      case 2:
        FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[0], v_u, H_MAX, 0);
        FunctionRealTime(t_a[0], t_H[0], t_x0[1], t_y0[0], v_e, H_MAX, 0);
        FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[1], v_yWy, H_MAX, 0);
        WaterTank_Pipes(t_a[0], t_H[0], t_x0[1], t_y0[1], yWy, yWyPrev, H_MAX, z, 0);
        Cross(0);

        TFTDisplay_ILI9341_String(165, 5, ch_e);
        TFTDisplay_ILI9341_String(5, 125, ch_y);
        TFTDisplay_ILI9341_String(5, 5, ch_u);

        break;

      case 3:
        WaterTank_Pipes(t_a[1], t_H[1], t_x0[0], t_y0[0], yWy, yWyPrev, H_MAX, z, 0);

        temp = kp;
        ch_kp[5] = '0' + temp;
        ch_kp[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_kp[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_kp[8] = '0' + temp;

        temp = Ti;
        ch_Ti[5] = '0' + temp;
        ch_Ti[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_Ti[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_Ti[8] = '0' + temp;

        temp = Td;
        ch_Td[5] = '0' + temp;
        ch_Td[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_Td[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_Td[8] = '0' + temp;

        temp = yZad;
        ch_yZad[7] = '0' + (int)temp / 100;
        temp = temp - ((int)temp / 100) * 100;
        ch_yZad[8] = '0' + (int)temp / 10;
        temp = temp - ((int)temp / 10) * 10;
        ch_yZad[9] = '0' + (int)temp;

        TFTDisplay_ILI9341_String(210, 5, ch_kp);
        TFTDisplay_ILI9341_String(210, 20, ch_Ti);
        TFTDisplay_ILI9341_String(210, 35, ch_Td);
        TFTDisplay_ILI9341_String(210, 50, ch_yZad);
        break;

      case 4:
        FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[0], v_u, H_MAX, 0);
        FunctionRealTime(t_a[0], t_H[0], t_x0[1], t_y0[0], v_e, H_MAX, 0);
        FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[1], v_yWy, H_MAX, 0);
        WaterTank_Pipes(t_a[0], t_H[0], t_x0[1], t_y0[1], yWy, yWyPrev, H_MAX, z, 0);
        Cross(0);

        TFTDisplay_ILI9341_String(165, 5, ch_e);
        TFTDisplay_ILI9341_String(5, 125, ch_y);
        TFTDisplay_ILI9341_String(5, 5, ch_u);

        break;

      case 5:
        WaterTank_Pipes(t_a[1], t_H[1], t_x0[0], t_y0[0], yWy, yWyPrev, H_MAX, z, 0);

        temp = kp;
        ch_kp[5] = '0' + temp;
        ch_kp[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_kp[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_kp[8] = '0' + temp;

        temp = Ti;
        ch_Ti[5] = '0' + temp;
        ch_Ti[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_Ti[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_Ti[8] = '0' + temp;

        temp = Td;
        ch_Td[5] = '0' + temp;
        ch_Td[6] = '.';
        temp = (temp - (int)temp) * 10;
        ch_Td[7] = '0' + temp;
        temp = (temp - (int)temp) * 10;
        ch_Td[8] = '0' + temp;

        temp = yZad;
        ch_yZad[7] = '0' + (int)temp / 100;
        temp = temp - ((int)temp / 100) * 100;
        ch_yZad[8] = '0' + (int)temp / 10;
        temp = temp - ((int)temp / 10) * 10;
        ch_yZad[9] = '0' + (int)temp;

        TFTDisplay_ILI9341_String(210, 5, ch_kp);
        TFTDisplay_ILI9341_String(210, 20, ch_Ti);
        TFTDisplay_ILI9341_String(210, 35, ch_Td);
        TFTDisplay_ILI9341_String(210, 50, ch_yZad);
        break;

      case 6:
        FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[0], v_u, H_MAX, 0);
        FunctionRealTime(t_a[0], t_H[0], t_x0[1], t_y0[0], v_e, H_MAX, 0);
        FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[1], v_yWy, H_MAX, 0);
        WaterTank_Pipes(t_a[0], t_H[0], t_x0[1], t_y0[1], yWy, yWyPrev, H_MAX, z, 0);
        Cross(0);

        TFTDisplay_ILI9341_String(165, 5, ch_e);
        TFTDisplay_ILI9341_String(5, 125, ch_y);
        TFTDisplay_ILI9341_String(5, 5, ch_u);

        break;
      }
      currentScreenNumber = screenNumber;
      TimPrev = Tim;
    }
  }
}
/* USER CODE END 5 */

/* USER CODE BEGIN Header_StartTaskLED1 */
/**
 * @brief Function implementing the LED1_task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskLED1 */
void StartTaskLED1(void *argument)
{
  /* USER CODE BEGIN StartTaskLED1 */
  /* Infinite loop */
  for (;;)
  {
    TFTDisplay_ILI9341_FillScreen(TFT_COLOR_ILI9341_RED);
    //    switch (currentScreenNumber)
    //    {
    //    case 1:
    //      WaterTank_Pipes(t_a[1], t_H[1], t_x0[0], t_y0[0], yWy, yWyPrev, H_MAX, z, 1);
    //              TFTDisplay_ILI9341_String(210, 5, ch_kp);
    //      		TFTDisplay_ILI9341_String(210, 20, ch_Ti);
    //      		TFTDisplay_ILI9341_String(210, 35, ch_Td);
    //      kp=t_kp[1];
    //      Ti=t_Ti[1];
    //      Td=t_Td[1];
    //
    //      break;
    //
    //    case 2:
    //      FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[0], v_u, H_MAX, 1);
    //      FunctionRealTime(t_a[0], t_H[0], t_x0[1], t_y0[0], v_e, H_MAX, 1);
    //      FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[1], v_yWy, H_MAX, 1);
    //      WaterTank_Pipes(t_a[0], t_H[0], t_x0[1], t_y0[1], yWy, yWyPrev, H_MAX, z, 1);
    //      Cross(0);
    //
    //      TFTDisplay_ILI9341_String(165, 5, ch_e);
    //      TFTDisplay_ILI9341_String(5, 125, ch_y);
    //      TFTDisplay_ILI9341_String(5, 5, ch_u);
    //      break;
    //    case 3:
    //         WaterTank_Pipes(t_a[1], t_H[1], t_x0[0], t_y0[0], yWy, yWyPrev, H_MAX, z, 1);
    //         TFTDisplay_ILI9341_String(210, 5, ch_kp);
    //         		TFTDisplay_ILI9341_String(210, 20, ch_Ti);
    //         		TFTDisplay_ILI9341_String(210, 35, ch_Td);
    //         kp=t_kp[1];
    //         Ti=t_Ti[1];
    //         Td=t_Td[1];
    //    	kp=t_kp[0];
    //    	      Ti=t_Ti[0];
    //    	      Td=t_Td[0];
    //         break;
    //
    //   case 4:
    //	 FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[0], v_u, H_MAX, 1);
    //	 FunctionRealTime(t_a[0], t_H[0], t_x0[1], t_y0[0], v_e, H_MAX, 1);
    //	 FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[1], v_yWy, H_MAX, 1);
    //	 WaterTank_Pipes(t_a[0], t_H[0], t_x0[1], t_y0[1], yWy, yWyPrev, H_MAX, z, 1);
    //	 Cross(0);
    //
    //	 TFTDisplay_ILI9341_String(165, 5, ch_e);
    //	 TFTDisplay_ILI9341_String(5, 125, ch_y);
    //	 TFTDisplay_ILI9341_String(5, 5, ch_u);
    //	 break;
    //   	   case 5:
    //		WaterTank_Pipes(t_a[1], t_H[1], t_x0[0], t_y0[0], yWy, yWyPrev, H_MAX, z, 1);
    //	      TFTDisplay_ILI9341_String(210, 5, ch_kp);
    //	      		TFTDisplay_ILI9341_String(210, 20, ch_Ti);
    //	      		TFTDisplay_ILI9341_String(210, 35, ch_Td);
    //	      kp=t_kp[1];
    //	      Ti=t_Ti[1];
    //	      Td=t_Td[1];
    //   		kp=t_kp[2];
    //   		      Ti=t_Ti[2];
    //   		      Td=t_Td[2];
    //		break;
    //
    //	  case 6:
    //		FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[0], v_u, H_MAX, 1);
    //		FunctionRealTime(t_a[0], t_H[0], t_x0[1], t_y0[0], v_e, H_MAX, 1);
    //		FunctionRealTime(t_a[0], t_H[0], t_x0[0], t_y0[1], v_yWy, H_MAX, 1);
    //		WaterTank_Pipes(t_a[0], t_H[0], t_x0[1], t_y0[1], yWy, yWyPrev, H_MAX, z, 1);
    //		Cross(0);
    //
    //		TFTDisplay_ILI9341_String(165, 5, ch_e);
    //		TFTDisplay_ILI9341_String(5, 125, ch_y);
    //		TFTDisplay_ILI9341_String(5, 5, ch_u);
    //		break;
    //    }

    switch (screenNumber)
    {
    case 1:
      kp = t_kp[1];
      Ti = t_Ti[1];
      Td = t_Td[1];
      yZad = t_yZad[2];

      break;

    case 3:
      kp = t_kp[0];
      Ti = t_Ti[0];
      Td = t_Td[0];
      yZad = t_yZad[1];
      break;

    case 5:
      kp = t_kp[2];
      Ti = t_Ti[2];
      Td = t_Td[2];
      yZad = t_yZad[4];
      break;
    }
    ki = kp * Tp / Ti;
    kd = kp * Td / Tp;
    e = yZad - yWy;
    u = PID_Calculate(e, 0);
    yWyPrev = yWy;
    yWy = InercModel_Calculate(u) - z;
    if (yWy < 0)
    {
      yWy = 0;
    }
    if (iv == nv - 1)
    {
      for (int i = 0; i < nv - 1; i++)
      {
        v_u[i] = v_u[i + 1];
        v_e[i] = v_e[i + 1];
        v_yWy[i] = v_yWy[i + 1];
      }
      v_u[iv] = u;
      v_e[iv] = e;
      v_yWy[iv] = yWy;
    }
    else
    {
      v_u[iv] = u;
      v_e[iv] = e;
      v_yWy[iv] = yWy;
      iv++;
    }
    osDelay(Tp_ms);
    Tim++;
  }
  /* USER CODE END StartTaskLED1 */
}

/* USER CODE BEGIN Header_StartTaskLED2 */
/**
 * @brief Function implementing the LED2_tack thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskLED2 */
void StartTaskLED2(void *argument)
{
  /* USER CODE BEGIN StartTaskLED2 */
  /* Infinite loop */
  for (;;)
  {
    // LL_GPIO_TogglePin(GPIOG, LL_GPIO_PIN_14);
    osDelay(1000);
    // osSemaphoreRelease(BinarySem01Handle); // zwolnienie semafora i odblokowanie TaskLED1
  }
  /* USER CODE END StartTaskLED2 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
