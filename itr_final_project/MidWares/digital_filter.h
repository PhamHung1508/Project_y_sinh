/**
 * @file        digital_filter.h
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
#ifndef __DIGITAL_FILTER_H
#define __DIGITAL_FILTER_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define DIGITAL_FILTER_SUCCESS (0x00000000)
#define DIGITAL_FILTER_ERROR   (0xFFFFFFFF)

#define DIGITAL_FILTER_LOWPASS_ORDER  (7)
#define DIGITAL_FILTER_HIGHPASS_ORDER (4)
#define DIGITAL_FILTER_NOTCH_ORDER    (7)

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief
 *
 */
typedef struct
{
  double low_in[DIGITAL_FILTER_LOWPASS_ORDER];
  double low_out[DIGITAL_FILTER_LOWPASS_ORDER];
  double high_in[DIGITAL_FILTER_HIGHPASS_ORDER];
  double high_out[DIGITAL_FILTER_HIGHPASS_ORDER];
  double notch_in[DIGITAL_FILTER_NOTCH_ORDER];
  double notch_out[DIGITAL_FILTER_NOTCH_ORDER];
} digital_filter_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief           Clear filter data
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t digital_filter_clear(digital_filter_t *filter);

/**
 * @brief           Filter data
 *
 * @param[in]       data_input    data input
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t digital_filter_process(digital_filter_t *filter, uint16_t input, int16_t *output);

#endif // __DIGITAL_FILTER_H

/* End of file -------------------------------------------------------- */
