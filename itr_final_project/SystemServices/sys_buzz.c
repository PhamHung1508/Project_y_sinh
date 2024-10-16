/**
 * @file        sys_buzz.c
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
#include "sys_buzz.h"
#include "common.h"
#include "stm32f4xx_hal.h"
#include "sys_data_mng.h"
#include "sys_data_mng_msg_define.h"

/* Private defines ---------------------------------------------------- */
#define SYS_BUZZ_CBUFFER_SIZE (20) /* Size of circular buffer */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static bsp_buzzer_t buzzer;
static cbuffer_t    sbuzz_cb;
static uint8_t      sbuzz_cb_buf[SYS_BUZZ_CBUFFER_SIZE];

static sys_data_mng_conn_mng_to_buzz_msg_t sbuzz_msg_to_buzz;

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
static uint32_t sys_buzz_process_data_from_sys_mng();

/* Function definitions ----------------------------------------------- */

uint32_t sys_buzzer_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin)
{
  uint32_t ret;

  ASSERT(htim != NULL, SYS_BUZZ_ERROR);
  ASSERT(port != NULL, SYS_BUZZ_ERROR);

  buzzer.htim = htim;
  buzzer.port = port;
  buzzer.pin  = pin;

  bsp_buzzer_init(&buzzer);

  // Initialize circular buffer
  ret = cb_init(&sbuzz_cb, sbuzz_cb_buf, SYS_BUZZ_CBUFFER_SIZE, 1);
  ASSERT(ret == CB_SUCCESS, SYS_BUZZ_ERROR);

  // Initialize connection to system manager
  ret = sys_data_mng_reg_node(SYS_DATA_MNG_CONN_MNG_TO_BUZZ, &sbuzz_cb);
  ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_BUZZ_ERROR);

  return SYS_BUZZ_SUCCESS;
}

uint32_t sys_buzzer_loop()
{
  uint32_t ret;

  // Process data from sys_mng
  ret = sys_buzz_process_data_from_sys_mng();
  ASSERT(ret == SYS_BUZZ_SUCCESS, SYS_BUZZ_ERROR);

  return SYS_BUZZ_SUCCESS;
}

/* Private function definitions --------------------------------------- */

/* Private definitions ------------------------------------------------ */
static uint32_t sys_buzz_process_data_from_sys_mng()
{
  uint32_t num_avail;

  // Check if data is available
  num_avail = cb_data_count(&sbuzz_cb);
  if ((num_avail / sizeof(sys_data_mng_conn_mng_to_buzz_msg_t)))
  {
    // Receive data from System Manager
    num_avail = cb_read(&sbuzz_cb, &sbuzz_msg_to_buzz,
                        sizeof(sys_data_mng_conn_mng_to_buzz_msg_t));
    if (num_avail == sizeof(sys_data_mng_conn_mng_to_buzz_msg_t))
    {
      // Process message
      if (sbuzz_msg_to_buzz.event == SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_ON)
      {
#ifdef USE_MELODY_VERSION
        bsp_buzzer_alarm(&buzzer);
#endif
        bsp_buzzer_on(&buzzer);
      }
      else if (sbuzz_msg_to_buzz.event == SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_OFF)
      {
        bsp_buzzer_off(&buzzer);
      }
    }
  }

  return SYS_BUZZ_SUCCESS;
}

/* End of file -------------------------------------------------------- */
