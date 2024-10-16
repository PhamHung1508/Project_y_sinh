/**
 * @file        sys_mng.h
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
#ifndef __SYS_MNG_H
#define __SYS_MNG_H

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define SYS_MNG_SUCCESS (0x00000000)
#define SYS_MNG_ERROR   (0xFFFFFFFF)

/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief           Initialize system manager
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t sys_mng_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief           System manager loop() function
 */
uint32_t sys_mng_loop();

/**
 * @brief         ADC convert complete interrupt handling function
 *
 * @param[in]     hadc         Pointer to ADC_HandleTypeDef structure
 *
 * @return        none
 *
 */
void sys_mng_adc_exti_handle(ADC_HandleTypeDef *hadc);

#endif // __SYS_MNG_H

/* End of file -------------------------------------------------------- */
