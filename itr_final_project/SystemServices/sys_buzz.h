/**
 * @file        sys_buzz.h
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
#ifndef __SYS_BUZZ_H
#define __SYS_BUZZ_H

/* Includes ----------------------------------------------------------- */
#include "bsp_buzz.h"
#include "main.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define SYS_BUZZ_SUCCESS (0x00000000)
#define SYS_BUZZ_ERROR   (0xFFFFFFFF)

/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief           Initialize the buzzer system
 *
 * @param[in]       htim          Pointer to TIM handle for PWM
 * @param[in]       port          Pointer to GPIO port for the buzzer
 * @param[in]       pin           GPIO pin for the buzzer
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t sys_buzzer_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief           Control the buzzer alarm state
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t sys_buzzer_loop();

#endif // __SYS_BUZZ_H

/* End of file -------------------------------------------------------- */
