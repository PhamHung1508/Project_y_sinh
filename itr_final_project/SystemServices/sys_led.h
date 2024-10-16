/**
 * @file        sys_led.h
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
#ifndef __SYS_LED_H
#define __SYS_LED_H

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include "sys_data_mng.h"
#include "sys_data_mng_conn_define.h"
#include "sys_data_mng_msg_define.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define SYS_LED_SUCCESS (0x00000000)
#define SYS_LED_ERROR   (0xFFFFFFFF)

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
uint32_t sys_led_init(TIM_HandleTypeDef *htim);

/**
 * @brief           System led loop() function
 */
uint32_t sys_led_loop();

#endif // __SYS_LED_H

/* End of file -------------------------------------------------------- */
