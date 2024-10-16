/**
 * @file        bsp_led.h
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
#ifndef __BSP_LED_
#define __BSP_LED_

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define BSP_LED_ERROR     0xFFFFFFFF
#define BSP_LED_SUCCESS   0x00000000
#define BSP_LED_STATE_ON  0x00000001
#define BSP_LED_STATE_OFF 0x00000000

/* Public enumerate/structure ----------------------------------------- */

typedef enum
{
  BSP_LED_GREEN = 0,
  BSP_LED_RED,
  BSP_ALL_LED = 0xFF,
} bsp_led_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief           Init LEDs
 *
 * @param[in]       htim    Pointer to TIM_HandleTypeDef structure
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_init(TIM_HandleTypeDef *htim);

/**
 * @brief           Turn LED on
 *
 * @param[in]       led     bsp_led_t want to turn on
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_on(bsp_led_t led);

/**
 * @brief           Turn LED off
 *
 * @param[in]       led     bsp_led_t want to turn off
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_off(bsp_led_t led);

/**
 * @brief           Toggle LED
 *
 * @param[in]       led     bsp_led_t want to turn toggle
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_toggle(bsp_led_t led);

/**
 * @brief           Get current state of LED's GPIO pin
 *
 * @param[in]       led     bsp_led_t want to turn toggle
 *
 * @return
 *  - (1 - BSP_LED_STATE_ON): LED's GPIO pin state is high
 *  - (0 - BSP_LED_STATE_OFF): LED's GPIO pin state is low
 */
uint32_t bsp_led_get_state(bsp_led_t led);

/**
 * @brief           Start blink LED
 *
 * @param[in]       led     bsp_led_t want to turn toggle
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_start_blink(bsp_led_t led);

/**
 * @brief           Stop blink LED
 *
 * @param[in]       led     bsp_led_t want to turn toggle
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_stop_blink(bsp_led_t led);

/**
 * @brief           Handle led in interrupt callback function
 *
 * @param[in]       led     bsp_led_t want to turn toggle
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t bsp_led_int_handle(TIM_HandleTypeDef *htim);

#endif // __BSP_LED_

/* End of file -------------------------------------------------------- */
