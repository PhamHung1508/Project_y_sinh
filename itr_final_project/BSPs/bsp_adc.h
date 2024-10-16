/**
 * @file        bsp_adc.h
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
#ifndef __BSP_ADC_H
#define __BSP_ADC_H

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define BSP_ADC_SUCCESS (0x00000000)
#define BSP_ADC_ERROR   (0xFFFFFFFF)

/* Public enumerate/structure ----------------------------------------- */
typedef struct
{
  ADC_HandleTypeDef *hadc;
  TIM_HandleTypeDef *htim;
  uint32_t           htim_channel;
} bsp_adc_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief           Initialize BSP ADC
 *
 * @param[in]     hadc         Pointer to ADC_HandleTypeDef structure
 * @param[in]     htim         Pointer to TIM_HandleTypeDef structure
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_adc_init(bsp_adc_t *badc, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief           Start ADC convert
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_adc_start(bsp_adc_t *badc);

/**
 * @brief           Stop converting ADC
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_adc_stop(bsp_adc_t *badc);

/**
 * @brief         ADC convert complete interrupt handling function
 *
 * @param[in]     hadc         Pointer to ADC_HandleTypeDef structure
 *
 * @return        none
 *
 */
void bsp_adc_exti_handle(bsp_adc_t *badc, ADC_HandleTypeDef *hadc);

#endif // __BSP_ADC_H

/* End of file -------------------------------------------------------- */
