/**
 * @file        sys_mng.c
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
#include "sys_mng.h"
#include "bsp_adc.h"
#include "common.h"
#include "digital_filter.h"
#include "main.h"
#include "peak_detect.h"
#include "sys_buzz.h"
#include "sys_data_mng.h"
#include "sys_data_mng_conn_define.h"
#include "sys_data_mng_msg_define.h"
#include "sys_led.h"
#include "sys_uart.h"

/* Private defines ---------------------------------------------------- */
#define SYS_MNG_ADC_CBUFFER_SIZE \
  (48) /*Size of circular buffer containing ADC value*/
#define SYS_MNG_UART_CBUFFER_SIZE \
  (20) /*Size of circular buffer containing UART command*/
#define SYS_MNG_ECG_DATA_BUFF_SIZE \
  (4097) /* Size of buffer to store ECG data and buffer to store R peaks annotation */
#define SYS_MNG_HIGH_HEART_RATE (100) /* Heart rate warning threshold */

/* Private enumerate/structure ---------------------------------------- */
typedef enum
{
  SYS_MNG_STATE_IDLE,
  SYS_MNG_STATE_PROCESS
} sys_mng_state_t;

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static cbuffer_t smng_adc_cb;
static uint8_t   smng_adc_cb_buff[SYS_MNG_ADC_CBUFFER_SIZE];

static cbuffer_t smng_uart_cb;
static uint8_t   smng_uart_cb_buff[SYS_MNG_UART_CBUFFER_SIZE];

static bsp_adc_t        smng_adc;
static digital_filter_t smng_filter;
static peak_detect_t    smng_peak_detect;
static int16_t          smng_filtered_data_buff[SYS_MNG_ECG_DATA_BUFF_SIZE];
static uint8_t          smng_r_peaks_ann_buff[SYS_MNG_ECG_DATA_BUFF_SIZE];

static sys_mng_state_t smng_state;
volatile static bool   smng_warning_enable_flag = true;
volatile static bool   smng_already_warning = false;

static sys_data_mng_conn_adc_to_mng_msg_t  smng_msg_from_adc;
static sys_data_mng_conn_uart_to_mng_msg_t smng_msg_from_uart;
static sys_data_mng_conn_mng_to_uart_msg_t smng_msg_to_uart;

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
uint32_t sys_mng_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint32_t channel)
{
  uint32_t ret;

  // Initialize circular buffer containing ADC value
  ret = cb_init(&smng_adc_cb, smng_adc_cb_buff, SYS_MNG_ADC_CBUFFER_SIZE, 1);
  ASSERT(ret == CB_SUCCESS, SYS_MNG_ERROR);

  // Initialize circular buffer containing UART command
  ret = cb_init(&smng_uart_cb, smng_uart_cb_buff, SYS_MNG_UART_CBUFFER_SIZE, 1);
  ASSERT(ret == CB_SUCCESS, SYS_MNG_ERROR);

  // Initialize BSP ADC
  bsp_adc_init(&smng_adc, hadc, htim, channel);

  // Initialize connection to BSP ADC
  ret = sys_data_mng_reg_node(SYS_DATA_MNG_CONN_ADC_TO_MNG, &smng_adc_cb);
  ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_MNG_ERROR);

  // Initialize connection to System UART
  ret = sys_data_mng_reg_node(SYS_DATA_MNG_CONN_UART_TO_MNG, &smng_uart_cb);
  ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_MNG_ERROR);

  // Initialize peak detection algorithm data
  ret = peak_detect_init(&smng_peak_detect, smng_filtered_data_buff, sizeof(smng_filtered_data_buff),
                         smng_r_peaks_ann_buff, sizeof(smng_r_peaks_ann_buff));
  ASSERT(ret == PEAK_DETECT_SUCCESS, SYS_MNG_ERROR);

  return SYS_MNG_SUCCESS;
}

uint32_t sys_mng_loop()
{
  uint32_t num_avail;
  uint32_t ret;

  // Check message from System UART
  num_avail = cb_data_count(&smng_uart_cb);
  if ((num_avail / sizeof(smng_msg_from_uart)))
  {
    num_avail = cb_read(&smng_uart_cb, (uint8_t *)&smng_msg_from_uart,
                        sizeof(smng_msg_from_uart));
    ASSERT(num_avail == sizeof(smng_msg_from_uart), SYS_MNG_ERROR);
  }

  // FSM:
  switch (smng_state)
  {
  case SYS_MNG_STATE_IDLE:
  {
    if (num_avail == sizeof(smng_msg_from_uart))
    {
      // Check Start command
      if (smng_msg_from_uart.cmd == SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_START_DATA)
      {
        // Turn on ADC
        bsp_adc_start(&smng_adc);

        // Clear filter data
        digital_filter_clear(&smng_filter);

        // Clear peak detect algorithm data
        peak_detect_clear(&smng_peak_detect);

        // Send green led on command
        sys_data_mng_conn_mng_to_led_msg_t led_msg = { SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_GREEN_ON,
                                                       BSP_ALL_LED };
        ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_LED,
                                (uint8_t *)&led_msg, sizeof(led_msg));
        ASSERT(ret == SYS_LED_SUCCESS, SYS_MNG_ERROR);

        // Change state
        smng_state = SYS_MNG_STATE_PROCESS;
      }
    }
    break;
  }
  case SYS_MNG_STATE_PROCESS:
  {
    if (num_avail == sizeof(smng_msg_from_uart))
    { // Check Stop processing command
      if (smng_msg_from_uart.cmd == SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_STOP_DATA)
      {
        // Send stop led command
        sys_data_mng_conn_mng_to_led_msg_t led_msg = { SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_OFF,
                                                       BSP_ALL_LED };
        ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_LED,
                                (uint8_t *)&led_msg, sizeof(led_msg));
        ASSERT(ret == SYS_LED_SUCCESS, SYS_MNG_ERROR);

        // Send stop buzzer
        sys_data_mng_conn_mng_to_buzz_msg_t buzz_msg = { SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_OFF };
        ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_BUZZ,
                                (uint8_t *)&buzz_msg, sizeof(buzz_msg));
        ASSERT(ret == SYS_BUZZ_SUCCESS, SYS_MNG_ERROR);
        
        // Init flag data
        smng_warning_enable_flag = true;
        smng_already_warning = false;

        // Stop ADC
        bsp_adc_stop(&smng_adc);

        // Change state
        smng_state = SYS_MNG_STATE_IDLE;
      }
      // Check Stop warning command
      else if (smng_msg_from_uart.cmd == SYS_DATA_MNG_CONN_UART_TO_MNG_EVENT_STOP_WARNING)
      {
        sys_data_mng_conn_mng_to_buzz_msg_t buzz_msg = { SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_OFF };
        ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_BUZZ,
                                (uint8_t *)&buzz_msg, sizeof(buzz_msg));
        ASSERT(ret == SYS_BUZZ_SUCCESS, SYS_MNG_ERROR);

        if (smng_warning_enable_flag == false)
        {
          smng_warning_enable_flag = true;
          smng_already_warning = false;
        }
        else
        {
          smng_warning_enable_flag = false;
        }

      }
      break;
    }

    // Check message from ADC Circular buffer
    num_avail = cb_data_count(&smng_adc_cb);
    if ((num_avail / sizeof(smng_msg_from_adc)))
    {
      // Read ADC value
      num_avail =
      cb_read(&smng_adc_cb, (uint8_t *)&smng_msg_from_adc, sizeof(smng_msg_from_adc));
      ASSERT(num_avail == sizeof(smng_msg_from_adc), SYS_MNG_ERROR);

      // Filter the ADC value
      int16_t filter_output;
      ret = digital_filter_process(&smng_filter, smng_msg_from_adc.data, &filter_output);
      ASSERT(ret == DIGITAL_FILTER_SUCCESS, SYS_MNG_ERROR);

      // Detect peaks
      ret = peak_detect_process(&smng_peak_detect, filter_output);
      ASSERT(ret == PEAK_DETECT_SUCCESS, SYS_MNG_ERROR);

      // Checking if output data is available
      if (peak_detect_output_avalable(&smng_peak_detect))
      {
        int16_t temp_data;
        uint8_t temp_is_peak;
        uint8_t temp_heart_rate;
        ret = peak_detect_get_output(&smng_peak_detect, &temp_data,
                                     &temp_is_peak, &temp_heart_rate);
        ASSERT(ret == PEAK_DETECT_SUCCESS, SYS_MNG_ERROR);

        // Send data message to System UART
        smng_msg_to_uart.data       = temp_data;
        smng_msg_to_uart.is_peak    = temp_is_peak;
        smng_msg_to_uart.heart_rate = temp_heart_rate;

        // Send UART
        ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_UART, (uint8_t *)&smng_msg_to_uart,
                                sizeof(smng_msg_to_uart));
        ASSERT(ret == SYS_DATA_MNG_SUCCESS, SYS_MNG_ERROR);
      }
    }

    if (smng_warning_enable_flag == true && smng_already_warning == false && smng_msg_to_uart.heart_rate > SYS_MNG_HIGH_HEART_RATE)
    {
      // Send stop led command
      sys_data_mng_conn_mng_to_led_msg_t led_msg = { SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_OFF,
                                                      BSP_ALL_LED };
      ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_LED,
                              (uint8_t *)&led_msg, sizeof(led_msg));
      ASSERT(ret == SYS_LED_SUCCESS, SYS_MNG_ERROR);

      // Send red LED blink command
      led_msg.event = SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_RED_BLINK;
      led_msg.led = BSP_LED_RED;
      ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_LED, (uint8_t *)&led_msg,
                              sizeof(led_msg));
      ASSERT(ret == SYS_LED_SUCCESS, SYS_MNG_ERROR);

      // Send event message to System Buzzer
      sys_data_mng_conn_mng_to_buzz_msg_t buzz_msg = { SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_ON };
      ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_BUZZ,
                              (uint8_t *)&buzz_msg, sizeof(buzz_msg));
      ASSERT(ret == SYS_BUZZ_SUCCESS, SYS_MNG_ERROR);

      smng_already_warning = true;
    }
    else if (smng_already_warning == true && smng_msg_to_uart.heart_rate < SYS_MNG_HIGH_HEART_RATE)
    {
      // Send stop led command
      sys_data_mng_conn_mng_to_led_msg_t led_msg = { SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_OFF,
                                                      BSP_ALL_LED };
      ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_LED,
                              (uint8_t *)&led_msg, sizeof(led_msg));
      ASSERT(ret == SYS_LED_SUCCESS, SYS_MNG_ERROR);

      // Send red LED blink command
      led_msg.event = SYS_DATA_MNG_CONN_MNG_TO_LED_EVENT_GREEN_ON;
      led_msg.led = BSP_LED_GREEN;
      ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_LED, (uint8_t *)&led_msg,
                              sizeof(led_msg));
      ASSERT(ret == SYS_LED_SUCCESS, SYS_MNG_ERROR);

      // Send event message to System Buzzer
      sys_data_mng_conn_mng_to_buzz_msg_t buzz_msg = { SYS_DATA_MNG_CONN_MNG_TO_BUZZ_EVENT_OFF };
      ret = sys_data_mng_send(SYS_DATA_MNG_CONN_MNG_TO_BUZZ,
                              (uint8_t *)&buzz_msg, sizeof(buzz_msg));
      ASSERT(ret == SYS_BUZZ_SUCCESS, SYS_MNG_ERROR);

      smng_already_warning = false;
    }

    break;
  }
  default:
    break;
  }
  return SYS_MNG_SUCCESS;
}

void sys_mng_adc_exti_handle(ADC_HandleTypeDef *hadc)
{
  bsp_adc_exti_handle(&smng_adc, hadc);
}

/* Private definitions ----------------------------------------------- */

/* End of file -------------------------------------------------------- */
