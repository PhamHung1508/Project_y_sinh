/**
 * @file        sys_data_mng_msg_define.h
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
#ifndef __SYS_DATA_MNG_MSG_DEFINE_H
#define __SYS_DATA_MNG_MSG_DEFINE_H

/* Includes ----------------------------------------------------------- */
#include "bsp_led.h"
#include "sys_data_mng_conn_define.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief   Message frame for connection from ADC to MNG
 *
 */
typedef struct __attribute__((packed))
{
  uint16_t data;
} sys_data_mng_conn_adc_to_mng_msg_t;


/**
 * @brief   Message frame for connection from MNG to UART
 *
 */
typedef struct __attribute__((packed))
{
  int16_t data    : 15;
  uint16_t is_peak : 1;
  uint8_t  heart_rate;
} sys_data_mng_conn_mng_to_uart_msg_t;

/**
 * @brief   GUI command
 *
 */
typedef uint8_t sys_data_mng_conn_uart_to_mng_command_t;
#define SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_START_DATA \
  ((sys_data_mng_conn_uart_to_mng_command_t)0x00)
#define SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_STOP_DATA \
  ((sys_data_mng_conn_uart_to_mng_command_t)0x01)
#define SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_STOP_WARNING \
  ((sys_data_mng_conn_uart_to_mng_command_t)0x02)

/**
 * @brief   Message frame for connection from UART to MNG
 *
 */
typedef struct __attribute__((packed))
{
  sys_data_mng_conn_uart_to_mng_command_t cmd;
} sys_data_mng_conn_uart_to_mng_msg_t;

/**
 * @brief   LED event
 *
 */
typedef uint8_t sys_data_mng_conn_mng_to_led_event_t;
#define SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_OFF \
  ((sys_data_mng_conn_mng_to_led_event_t)0x00)
#define SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_GREEN_ON \
  ((sys_data_mng_conn_mng_to_led_event_t)0x01)
#define SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_RED_BLINK \
  ((sys_data_mng_conn_mng_to_led_event_t)0x02)

/**
 * @brief   Message frame for connection from MNG to LED
 *
 */
typedef struct __attribute__((packed))
{
  sys_data_mng_conn_mng_to_led_event_t event;
  bsp_led_t                            led;
} sys_data_mng_conn_mng_to_led_msg_t;


/**
 * @brief   Buzzer event
 *
 */
typedef uint8_t sys_data_mng_conn_mng_to_buzz_event_t;
#define SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_OFF \
  ((sys_data_mng_conn_mng_to_buzz_event_t)0x00)
#define SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_ON \
  ((sys_data_mng_conn_mng_to_buzz_event_t)0x01)

/**
 * @brief   Message frame for connection from MNG to BUZZ
 *
 */
typedef struct __attribute__((packed))
{
  sys_data_mng_conn_mng_to_buzz_event_t event;
} sys_data_mng_conn_mng_to_buzz_msg_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

#endif // __SYS_DATA_MNG_MSG_DEFINE_H

/* End of file -------------------------------------------------------- */
