/**
 * @file        bsp_adc.c
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

/* Includes ----------------------------------------------------------- */
#include "bsp_adc.h"
#include "common.h"
#include "sys_data_mng.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
uint32_t bsp_adc_init(bsp_adc_t *badc, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint32_t channel)
{
  // Assert input
  ASSERT(badc != NULL, BSP_ADC_ERROR);
  ASSERT(hadc != NULL, BSP_ADC_ERROR);
  ASSERT(htim != NULL, BSP_ADC_ERROR);

  // Store hadc, htim pointer and PWM timer channel
  badc->hadc         = hadc;
  badc->htim         = htim;
  badc->htim_channel = channel;

  return BSP_ADC_SUCCESS;
}

uint32_t bsp_adc_start(bsp_adc_t *badc)
{
  // Assert input
  ASSERT(badc != NULL, BSP_ADC_ERROR);

  HAL_StatusTypeDef ret;

  // Start ADC interrupt
  ret = HAL_ADC_Start_IT(badc->hadc);
  ASSERT(ret == HAL_OK, BSP_ADC_ERROR);

  // Start triggering ADC convert using PWM. Frequency = 500 Hz
  ret = HAL_TIM_PWM_Start(badc->htim, badc->htim_channel);
  ASSERT(ret == HAL_OK, BSP_ADC_ERROR);

  return BSP_ADC_SUCCESS;
}

uint32_t bsp_adc_stop(bsp_adc_t *badc)
{
  // Assert input
  ASSERT(badc != NULL, BSP_ADC_ERROR);

  HAL_StatusTypeDef ret;

  // Start ADC interrupt
  ret = HAL_ADC_Stop_IT(badc->hadc);
  ASSERT(ret == HAL_OK, BSP_ADC_ERROR);

  // Start triggering ADC convert using PWM. Frequency = 500 Hz
  ret = HAL_TIM_PWM_Stop(badc->htim, badc->htim_channel);
  ASSERT(ret == HAL_OK, BSP_ADC_ERROR);

  return BSP_ADC_SUCCESS;
}

void bsp_adc_exti_handle(bsp_adc_t *badc, ADC_HandleTypeDef *hadc)
{
  if (badc->hadc == hadc)
  {
    uint16_t adc_val;

    // Get ADC value
    adc_val = (uint16_t)HAL_ADC_GetValue(badc->hadc);

    // Send ADC value to System Manager
    sys_data_mng_send(SYS_DATA_MNG_CONN_ADC_TO_MNG, (uint8_t *)&adc_val, sizeof(adc_val));
  }
}

/* Private definitions ----------------------------------------------- */

/* End of file -------------------------------------------------------- */
