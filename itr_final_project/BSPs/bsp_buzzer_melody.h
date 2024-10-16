/**
 * @file        bsp_buzzer_melody.h
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
#ifndef __BSP_BUZZER_MELODY_H
#define __BSP_BUZZER_MELODY_H

/* Includes ----------------------------------------------------------- */
#include "bsp_buzzer_tone.h"
#include "stdint.h"

/* Public defines ----------------------------------------------------- */
#define MELODY_LENGTH 4 // Cập nhật độ dài đoạn nhạc

/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
static const uint16_t melody_notes[MELODY_LENGTH] = { NOTE_E5, 0, NOTE_A4, 0 };

static const uint16_t melody_durations[MELODY_LENGTH] = { 7, 8, 7, 8 };

#endif // __BSP_BUZZER_MELODY_H

/* End of file -------------------------------------------------------- */
