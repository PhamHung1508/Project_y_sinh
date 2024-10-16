/**
 * @file        bsp_int_callback.c
 * @copyright
 * @license
 * @version     0.0.0
 * @date
 * @author      Minh Phung Nhat
 *              Hung Pham Duc
 *              Khoi Nguyen Thanh
 * @brief       none
 *
 * @note        none
 *
 * @example     none
 *
 */
/* Define to prevent recursive inclusion ------------------------------ */

/* Includes ----------------------------------------------------------- */
#include "bsp_int_callback.h"
#include "bsp_led.h"
#include "sys_mng.h"
#include "sys_uart.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */

/* Private definitions ----------------------------------------------- */

/* Weak definitions ----------------------------------------------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  sys_uart_rx_exti_handle(huart);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  bsp_led_int_handle(htim);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  sys_mng_adc_exti_handle(hadc);
}

/* End of file -------------------------------------------------------- */
