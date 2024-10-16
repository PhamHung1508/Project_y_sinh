/**
 * @file        sys_led.c
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
#include "sys_led.h"
#include "bsp_led.h"
#include "common.h"
#include "sys_data_mng.h"
#include "sys_data_mng_msg_define.h"
#include "sys_mng.h"

/* Private defines ---------------------------------------------------- */
#define SYS_LED_CBUFFER_SIZE (20) /* Size of circular buffer */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static cbuffer_t sled_cb;
static uint8_t   sled_cb_buf[SYS_LED_CBUFFER_SIZE];

static sys_data_mng_conn_mng_to_led_msg_t sled_msg_to_led;

/* Private function prototypes ---------------------------------------- */
/**
 * @brief           Process data from system manager
 *
 * @param[in]       none
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
static uint32_t sys_led_process_data_from_sys_mng();

/* Function definitions ----------------------------------------------- */
uint32_t sys_led_init(TIM_HandleTypeDef *htim)
{
  uint32_t ret;
  // Init BSP LED
  ret = bsp_led_init(htim);
  ASSERT(ret == BSP_LED_SUCCESS, SYS_LED_ERROR);

  // Initialize circular buffer
  ret = cb_init(&sled_cb, sled_cb_buf, SYS_LED_CBUFFER_SIZE, 1);
  ASSERT(ret == CB_SUCCESS, SYS_LED_ERROR);

  // Initialize connection to system manager
  ret = sys_data_mng_reg_node(SYS_DATA_MNG_CONN_MNG_TO_LED, &sled_cb);
  ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_LED_ERROR);

  return SYS_LED_SUCCESS;
}

uint32_t sys_led_loop()
{
  uint32_t ret;

  // Process data from sys_mng
  ret = sys_led_process_data_from_sys_mng();
  ASSERT(ret == SYS_LED_SUCCESS, SYS_LED_ERROR);

  return SYS_LED_SUCCESS;
}

/* Private definitions ----------------------------------------------- */
static uint32_t sys_led_process_data_from_sys_mng()
{
  uint32_t num_avail;

  // Check if data is available
  num_avail = cb_data_count(&sled_cb);
  if ((num_avail / sizeof(sys_data_mng_conn_mng_to_led_msg_t)))
  {
    // Receive data from System Manager
    num_avail =
    cb_read(&sled_cb, &sled_msg_to_led, sizeof(sys_data_mng_conn_mng_to_led_msg_t));

    // Process message
    if (sled_msg_to_led.event == SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_OFF)
    {
      if (sled_msg_to_led.led == BSP_ALL_LED)
      {
        bsp_led_off(BSP_LED_GREEN);
        bsp_led_stop_blink(BSP_LED_RED);
      }
      else
      {
        bsp_led_stop_blink((bsp_led_t)sled_msg_to_led.led);
      }
    }
    else if (sled_msg_to_led.event == SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_GREEN_ON)
    {
      if (sled_msg_to_led.led == BSP_ALL_LED)
      {
        bsp_led_on(BSP_LED_GREEN);
        bsp_led_off(BSP_LED_RED);
      }
      else
      {
        bsp_led_on((bsp_led_t)sled_msg_to_led.led);
      }
    }
    else if (sled_msg_to_led.event == SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_RED_BLINK)
    {
      if (sled_msg_to_led.led == BSP_ALL_LED)
      {
        bsp_led_off(BSP_LED_GREEN);
        bsp_led_start_blink(BSP_LED_RED);
      }
      else
      {
        bsp_led_start_blink((bsp_led_t)sled_msg_to_led.led);
      }
    }
  }

  return SYS_LED_SUCCESS;
}

/* End of file -------------------------------------------------------- */
