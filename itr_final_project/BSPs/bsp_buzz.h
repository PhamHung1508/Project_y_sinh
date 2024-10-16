/**
 * @file        bsp_buzz.h
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
#ifndef __BSP_BUZZ_H
#define __BSP_BUZZ_H

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>


/* Public defines ----------------------------------------------------- */
#define BSP_BUZZ_ERROR   0xFFFFFFFF
#define BSP_BUZZ_SUCCESS 0x00000000

/* Public enumerate/structure ----------------------------------------- */
typedef struct
{
  TIM_HandleTypeDef *htim;
  GPIO_TypeDef      *port;
  uint16_t           pin;
} bsp_buzzer_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief         Initialize the Buzzer
 *
 * @param[in]     buzzer       Pointer to bsp_buzzer_t structure
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_buzzer_init(bsp_buzzer_t *buzzer);

/**
 * @brief           The buzzer alarm state
 *
 * @param[in]     buzzer       Pointer to bsp_buzzer_t structure
 *
 * @return          None
 */
void bsp_buzzer_alarm(bsp_buzzer_t *buzzer);

/**
 * @brief         Turn on the Buzzer
 *
 * @param[in]     buzzer       Pointer to bsp_buzzer_t structure
 *
 * @return        None
 */
void bsp_buzzer_on(bsp_buzzer_t *buzzer);

/**
 * @brief         Turn off the Buzzer
 *
 * @param[in]     buzzer       Pointer to bsp_buzzer_t structure
 *
 * @return        None
 */
void bsp_buzzer_off(bsp_buzzer_t *buzzer);

/**
 * @brief         Toggle the Buzzer
 *
 * @param[in]     buzzer       Pointer to bsp_buzzer_t structure
 *
 * @return        None
 */
void bsp_buzzer_toggle(bsp_buzzer_t *buzzer);

#endif // __BSP_BUZZ_H

/* End of file -------------------------------------------------------- */
