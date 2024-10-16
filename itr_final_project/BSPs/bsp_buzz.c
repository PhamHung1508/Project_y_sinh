/**
 * @file        bsp_buzz.c
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
#include "bsp_buzz.h"
#include "bsp_buzzer_melody.h"
#include "common.h"

/* Private defines ---------------------------------------------------- */
#define BSP_BUZZER_SOUND          0
#define BSP_BUZZER_NO_SOUND       1
#define BSP_TIM_CHANNEL           TIM_CHANNEL_1
#define BSP_BUZZER_COUNTER_PERIOD 1000

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
static void bsp_play_melody(TIM_HandleTypeDef *htim);

/* Public function prototypes ---------------------------------------- */

uint32_t bsp_buzzer_init(bsp_buzzer_t *buzzer)
{
  ASSERT(buzzer->htim != NULL, BSP_BUZZ_ERROR);
  ASSERT(buzzer->port != NULL, BSP_BUZZ_ERROR);

  HAL_GPIO_WritePin(buzzer->port, buzzer->pin, GPIO_PIN_SET); // Set pin to high state by default

#ifdef USE_MELODY_VERSION
  HAL_TIM_PWM_Start(buzzer->htim, BSP_TIM_CHANNEL);
  __HAL_TIM_SET_COMPARE(buzzer->htim, BSP_TIM_CHANNEL, BSP_BUZZER_COUNTER_PERIOD); // Turn off the buzzer
#endif

  return BSP_BUZZ_SUCCESS;
}

void bsp_buzzer_alarm(bsp_buzzer_t *buzzer)
{
  bsp_play_melody(buzzer->htim);
}

void bsp_buzzer_on(bsp_buzzer_t *buzzer)
{
  HAL_GPIO_WritePin(buzzer->port, buzzer->pin, BSP_BUZZER_SOUND);
}

void bsp_buzzer_off(bsp_buzzer_t *buzzer)
{
  HAL_GPIO_WritePin(buzzer->port, buzzer->pin, BSP_BUZZER_NO_SOUND);
}

void bsp_buzzer_toggle(bsp_buzzer_t *buzzer)
{
  HAL_GPIO_TogglePin(buzzer->port, buzzer->pin);
}

/* Private function definitions --------------------------------------- */

static void bsp_play_melody(TIM_HandleTypeDef *htim)
{

  uint32_t current_time;

  for (int i = 0; i < MELODY_LENGTH; i++)
  {
    if (melody_notes[i] > 0)
    {
      __HAL_TIM_SET_COMPARE(htim, BSP_TIM_CHANNEL, melody_notes[i]);
      current_time = HAL_GetTick();
      while ((HAL_GetTick() - current_time) < ((1000 / melody_durations[i]) * 2))
      {
        // Delay time
      }
    }
    else
    {
      __HAL_TIM_SET_COMPARE(htim, BSP_TIM_CHANNEL, BSP_BUZZER_COUNTER_PERIOD);
      current_time = HAL_GetTick();
      while ((HAL_GetTick() - current_time) < ((1000 / melody_durations[i]) * 2))
      {
        // Delay time
      }
    }
  }

  __HAL_TIM_SET_COMPARE(htim, BSP_TIM_CHANNEL, BSP_BUZZER_COUNTER_PERIOD); // Turn off the buzzer
}

/* End of file -------------------------------------------------------- */