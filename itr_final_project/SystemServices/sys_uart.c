/**
 * @file        sys_uart.c
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

/* Includes ----------------------------------------------------------- */
#include "sys_uart.h"
#include "bsp_uart.h"
#include "common.h"
#include "sys_data_mng.h"
#include "sys_data_mng_conn_define.h"
#include "sys_data_mng_msg_define.h"

/* Private defines ---------------------------------------------------- */
#define SYS_UART_CBUFFER_SIZE (20) /*Size of circular buffer*/
#define SYS_UART_TIMEOUT_TICK 10   /*Timeout tick - 10ms*/

/* Private enumerate/structure ---------------------------------------- */
/**
 * @brief   TLV message frame transmit to GUI from UART
 *
 */
typedef struct __attribute__((packed))
{
  uint8_t                             type;
  uint8_t                             length;
  sys_data_mng_conn_mng_to_uart_msg_t value;
} sys_uart_tlv_msg_to_gui_t;

typedef enum
{
  SYS_UART_STATE_WAIT_TYPE,
  SYS_UART_STATE_WAIT_LEN
} sys_uart_state_t;

typedef uint8_t sys_uart_type_t;
#define UART_TYPE_START_DATA   ((sys_uart_type_t)0x01)
#define UART_TYPE_STOP_DATA    ((sys_uart_type_t)0x02)
#define UART_TYPE_STOP_WARNING ((sys_uart_type_t)0x03)

typedef struct __attribute__((packed))
{
  uint8_t type;
  uint8_t length;
  // TLV message from GUI does not include data
} sys_uart_tlv_msg_from_gui_t;

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static bsp_uart_t suart;
static cbuffer_t  suart_cb;
static uint8_t    suart_cb_buf[SYS_UART_CBUFFER_SIZE];

static sys_uart_state_t suart_state      = SYS_UART_STATE_WAIT_TYPE;
static uint32_t         suart_start_tick = 0;

static sys_data_mng_conn_uart_to_mng_msg_t suart_msg_to_mng;
static sys_uart_tlv_msg_from_gui_t         suart_msg_from_gui;

/* Private function prototypes ---------------------------------------- */
/**
 * @brief           Receive and process data from uart bsp
 *
 * @param[in]       none
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
static uint32_t sys_uart_process_data_from_uart();

/**
 * @brief           Receive and process data from system manager
 *
 * @param[in]       none
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
static uint32_t sys_uart_process_data_from_sys_mng();

/* Function definitions ----------------------------------------------- */
uint32_t sys_uart_init(UART_HandleTypeDef *huart)
{
  uint32_t ret;

  // Initialize UART
  ret = bsp_uart_init(&suart, huart);
  ASSERT(ret == BSP_UART_SUCCESS, SYS_UART_ERROR);

  // Initialize circular buffer
  ret = cb_init(&suart_cb, suart_cb_buf, SYS_UART_CBUFFER_SIZE, 1);
  ASSERT(ret == CB_SUCCESS, SYS_UART_ERROR);

  // Initialize connection to system mangager
  ret = sys_data_mng_reg_node(SYS_DATA_MNG_CONN_MNG_TO_UART, &suart_cb);
  ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_UART_ERROR);

  return SYS_UART_SUCCESS;
}

uint32_t sys_uart_loop()
{
  uint32_t ret;

  // Process data from UART
  ret = sys_uart_process_data_from_uart();
  ASSERT(ret == SYS_UART_SUCCESS, SYS_UART_ERROR);

  // Process data from sys_mng
  ret = sys_uart_process_data_from_sys_mng();
  ASSERT(ret == SYS_UART_SUCCESS, SYS_UART_ERROR);

  return SYS_UART_SUCCESS;
}

void sys_uart_rx_exti_handle(UART_HandleTypeDef *huart)
{
  bsp_uart_rx_exti_handle(&suart, huart);
}

/* Private definitions ----------------------------------------------- */
static uint32_t sys_uart_process_data_from_uart()
{
  uint32_t ret;
  uint32_t num_avail;
  uint8_t  rx_data;

  // Receive data from BSP UART
  num_avail = bsp_uart_receive(&suart, &rx_data, sizeof(rx_data));

  // Process data
  switch (suart_state)
  {
  case SYS_UART_STATE_WAIT_TYPE:
  {
    if (num_avail == sizeof(rx_data))
    {
      if (rx_data == UART_TYPE_START_DATA)
      {
        suart_msg_from_gui.type = UART_TYPE_START_DATA;
        suart_start_tick        = HAL_GetTick();
        suart_state             = SYS_UART_STATE_WAIT_LEN;
      }
      else if (rx_data == UART_TYPE_STOP_DATA)
      {
        suart_msg_from_gui.type = UART_TYPE_STOP_DATA;
        suart_start_tick        = HAL_GetTick();
        suart_state             = SYS_UART_STATE_WAIT_LEN;
      }
      else if (rx_data == UART_TYPE_STOP_WARNING)
      {
        suart_msg_from_gui.type = UART_TYPE_STOP_WARNING;
        suart_start_tick        = HAL_GetTick();
        suart_state             = SYS_UART_STATE_WAIT_LEN;
      }
    }
    break;
  }
  case SYS_UART_STATE_WAIT_LEN:
  {
    uint32_t duration = HAL_GetTick() - suart_start_tick;
    if (duration < SYS_UART_TIMEOUT_TICK)
    {
      if (num_avail == sizeof(rx_data))
      {
        if (rx_data == 0)
        {
          switch (suart_msg_from_gui.type)
          {
          case UART_TYPE_START_DATA:
          {
            // Send start data message to System Manager
            suart_msg_to_mng.cmd = SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_START_DATA;
            ret = sys_data_mng_send(SYS_DATA_MNG_CONN_UART_TO_MNG, (uint8_t *)&suart_msg_to_mng,
                                    sizeof(sys_data_mng_conn_uart_to_mng_msg_t));
            ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_UART_ERROR);
            break;
          }
          case UART_TYPE_STOP_DATA:
          {
            // Send stop data message to System Manager
            suart_msg_to_mng.cmd = SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_STOP_DATA;
            ret = sys_data_mng_send(SYS_DATA_MNG_CONN_UART_TO_MNG, (uint8_t *)&suart_msg_to_mng,
                                    sizeof(sys_data_mng_conn_uart_to_mng_msg_t));
            ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_UART_ERROR);
            break;
          }
          case UART_TYPE_STOP_WARNING:
          {
            // Send stop warning message to System Manager
            suart_msg_to_mng.cmd = SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_STOP_WARNING;
            ret = sys_data_mng_send(SYS_DATA_MNG_CONN_UART_TO_MNG, (uint8_t *)&suart_msg_to_mng,
                                    sizeof(sys_data_mng_conn_uart_to_mng_msg_t));
            ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_UART_ERROR);
            break;
          }
          }
        }
        else
        {
          suart_state = SYS_UART_STATE_WAIT_TYPE;
        }
      }
    }
    else
    {
      suart_state = SYS_UART_STATE_WAIT_TYPE;
    }

    break;
  }
  }
  return SYS_UART_SUCCESS;
}

static uint32_t sys_uart_process_data_from_sys_mng()
{
  uint32_t ret;
  uint32_t num_avail;

  // Check if data is available
  num_avail = cb_data_count(&suart_cb);
  if ((num_avail / sizeof(sys_data_mng_conn_mng_to_uart_msg_t)))
  {
    sys_uart_tlv_msg_to_gui_t suart_tlv_msg_to_gui;
    suart_tlv_msg_to_gui.type   = (0);
    suart_tlv_msg_to_gui.length = sizeof(suart_tlv_msg_to_gui.value);
    // Receive data from System Manager
    num_avail = cb_read(&suart_cb, &(suart_tlv_msg_to_gui.value),
                        sizeof(sys_data_mng_conn_mng_to_uart_msg_t));

    // Send message to GUI
    ret = bsp_uart_transmit(&suart, (uint8_t *)&suart_tlv_msg_to_gui,
                            sizeof(sys_uart_tlv_msg_to_gui_t));
    ASSERT(ret == BSP_UART_SUCCESS, SYS_UART_ERROR);
  }

  return SYS_UART_SUCCESS;
}
/* End of file -------------------------------------------------------- */