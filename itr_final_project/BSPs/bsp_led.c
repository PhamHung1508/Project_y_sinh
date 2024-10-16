/**
 * @file        bsp_led.c
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
#include "bsp_led.h"
#include "common.h"
#include "main.h"
#include <stdint.h>

/* Private defines ---------------------------------------------------- */
#define TIMX_CR1_CEN_MASK     (0x00000001) // Counter enable bit
#define TIMX_CCMR1_OC1PE_MASK (0x00000008) // Output compare 1 preload enable
#define TIMX_CCMR1_OC2PE_MASK (0x00000800) // Output compare 2 preload enable
#define TIMX_CCMR2_OC3PE_MASK (0x00000008) // Output compare 3 preload enable

/* CCR register value to make duty cycle 50% */
#define BSP_LED_CCR_50_DUTY_VALUE (31249)

/* Private enumerate/structure ---------------------------------------- */
typedef struct
{
  GPIO_TypeDef      *gpio_x;
  uint16_t           gpio_pin;
  uint32_t           active_state;
  TIM_HandleTypeDef *htim;
  uint32_t           tim_channel;
  uint32_t           blink_enable;
} bsp_led_data_t;

/* Private macros ----------------------------------------------------- */
/**
 * @brief           Get interrupt enable bit mask in TIMx_DIER register
 *
 * @param[in]       CHANNEL PWM channel
 *
 * @return          Enable bit mask in TIMx_DIER register
 *
 */
#define TIMX_DIER_CCXIE_MASK(CHANNEL) (0x00000002 << (CHANNEL))

/**
 * @brief           Get capture/compare output enable bit mask in TIMx_CCER register
 *
 * @param[in]       CHANNEL PWM channel
 *
 * @return          Enable bit mask in TIMx_DIER register
 *
 */
#define TIMX_CCER_CCXE_MASK(CHANNEL) (0x00000001 << ((CHANNEL) * 4))

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static bsp_led_data_t bled_data[] = {
  { GPIOD, GPIO_PIN_13, 1, NULL, TIM_CHANNEL_1, 0 },
  { GPIOD, GPIO_PIN_14, 1, NULL, TIM_CHANNEL_2, 0 }
};

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
uint32_t bsp_led_init(TIM_HandleTypeDef *htim)
{
  bled_data[0].htim = htim;
  bled_data[1].htim = htim;

  /* Set value for CCR1 register */
  // Clear OC1PE bit to directly set value for CCR1 register
  htim->Instance->CCMR1 &= ~TIMX_CCMR1_OC1PE_MASK;
  // Set value for CCR1 register
  bled_data[0].htim->Instance->CCR1 = BSP_LED_CCR_50_DUTY_VALUE;
  // Set OC1PE bit to enable updating CCR1 value at each rising edge
  htim->Instance->CCMR1 |= TIMX_CCMR1_OC1PE_MASK;

  /* Set value for CCR2 register */
  htim->Instance->CCMR1 &= ~TIMX_CCMR1_OC2PE_MASK;
  bled_data[1].htim->Instance->CCR2 = BSP_LED_CCR_50_DUTY_VALUE;
  htim->Instance->CCMR1 |= TIMX_CCMR1_OC2PE_MASK;

  return BSP_LED_SUCCESS;
}

uint32_t bsp_led_on(bsp_led_t led)
{
  // Check input
  ASSERT((led >= 0) && (led <= (sizeof(bled_data) / sizeof(bsp_led_data_t))), BSP_LED_ERROR);

  HAL_GPIO_WritePin(bled_data[led].gpio_x, bled_data[led].gpio_pin,
                    bled_data[led].active_state);
  return BSP_LED_SUCCESS;
}

uint32_t bsp_led_off(bsp_led_t led)
{
  // Check input
  ASSERT((led >= 0) && (led <= (sizeof(bled_data) / sizeof(bsp_led_data_t))), BSP_LED_ERROR);

  HAL_GPIO_WritePin(bled_data[led].gpio_x, bled_data[led].gpio_pin,
                    !bled_data[led].active_state);
  return BSP_LED_SUCCESS;
}

uint32_t bsp_led_get_state(bsp_led_t led)
{
  // Check input
  ASSERT((led >= 0) && (led <= (sizeof(bled_data) / sizeof(bsp_led_data_t))), BSP_LED_ERROR);

  if ((bled_data[led].gpio_x->IDR & bled_data[led].gpio_pin) != 0)
    return BSP_LED_STATE_ON;
  else
    return BSP_LED_STATE_OFF;
}

uint32_t bsp_led_start_blink(bsp_led_t led)
{
  // Check input
  ASSERT((led >= 0) && (led <= (sizeof(bled_data) / sizeof(bsp_led_data_t))), BSP_LED_ERROR);

  // Enable blink
  bled_data[led].blink_enable = 1;

  // Check whether timer has started counting
  if (!(bled_data[led].htim->Instance->CR1 & TIMX_CR1_CEN_MASK))
  {
    bled_data[led].htim->Instance->CNT = 0;
    bled_data[led].htim->Instance->DIER |= TIMX_DIER_CCXIE_MASK(led);
    bled_data[led].htim->Instance->CCER |= TIMX_CCER_CCXE_MASK(led);
    bled_data[led].htim->Instance->CR1 |= TIMX_CR1_CEN_MASK;
  }

  return BSP_LED_SUCCESS;
}

uint32_t bsp_led_stop_blink(bsp_led_t led)
{
  // Check input
  ASSERT((led >= 0) && (led < (sizeof(bled_data) / sizeof(bsp_led_data_t))), BSP_LED_ERROR);

  // Disable blink
  bled_data[led].blink_enable = 0;

  // Check whether timer has started counting
  if (bled_data[led].htim->Instance->CR1 & TIMX_CR1_CEN_MASK)
  {
    // Check if any other LEDs are still blinking
    int any_led_blinking = 0;
    for (int i = 0; i < (sizeof(bled_data) / sizeof(bsp_led_data_t)); i++)
    {
      if (bled_data[i].blink_enable)
      {
        any_led_blinking = 1;
        break;
      }
    }

    // If no other LEDs are blinking, stop the timer
    if (!any_led_blinking)
    {
      bled_data[led].htim->Instance->DIER &= ~TIMX_DIER_CCXIE_MASK(led);
      bled_data[led].htim->Instance->CCER &= ~TIMX_CCER_CCXE_MASK(led);
      bled_data[led].htim->Instance->CR1 &= ~TIMX_CR1_CEN_MASK;
    }
    else
    {
      // Ensure that the timer interrupt for this channel is disabled
      bled_data[led].htim->Instance->DIER &= ~TIMX_DIER_CCXIE_MASK(led);
      bled_data[led].htim->Instance->CCER &= ~TIMX_CCER_CCXE_MASK(led);
    }
  }

  return BSP_LED_SUCCESS;
}

uint32_t bsp_led_int_handle(TIM_HandleTypeDef *htim)
{
  uint32_t is_working = 0;

  for (int i = 0; i < (sizeof(bled_data) / sizeof(bsp_led_data_t)); i++)
  {
    // Check TIMx interrupt
    if (bled_data[i].htim != htim)
      continue;

    // Start/Stop blink
    if (bled_data[i].blink_enable)
    {
      bled_data[i].htim->Instance->DIER |= TIMX_DIER_CCXIE_MASK(i);
      bled_data[i].htim->Instance->CCER |= TIMX_CCER_CCXE_MASK(i);
      is_working = 1;
    }
    else
    {
      bled_data[i].htim->Instance->DIER &= ~TIMX_DIER_CCXIE_MASK(i);
      bled_data[i].htim->Instance->CCER &= ~TIMX_CCER_CCXE_MASK(i);
    }
  }

  if (!is_working)
  {
    bled_data[0].htim->Instance->CR1 &= ~TIMX_CR1_CEN_MASK;
    bled_data[0].htim->Instance->CNT = 0;
  }

  return BSP_LED_SUCCESS;
}

/* Private definitions ----------------------------------------------- */

/* End of file -------------------------------------------------------- */