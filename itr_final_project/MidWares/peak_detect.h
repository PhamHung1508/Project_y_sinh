/**
 * @file        peak_detect.h
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
#ifndef __PEAK_DETECT_H
#define __PEAK_DETECT_H

/* Includes ----------------------------------------------------------- */
#include "cbuffer.h"
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
#define PEAK_DETECT_ERROR         (0xFFFFFFFF)
#define PEAK_DETECT_SUCCESS       (0x00000000)
#define PEAK_DETECT_PEAK_DETECTED (0x00000001)

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief   TERMA data typedef struct
 *
 */
typedef struct
{
  //  int16_t   debug_sig;
  int16_t   temp_peak_value;
  uint32_t  temp_peak_index;
  uint32_t  peak_index_last;
  uint8_t   heart_rate;
  uint8_t   is_first_time;
  uint8_t   is_output_available;
  int16_t  *filtered_data_buff;
  uint32_t  filtered_data_buff_size;
  cbuffer_t filtered_data_cbuffer;
  uint8_t  *r_peaks_ann_buff;
  uint32_t  r_peaks_ann_buff_size;
  cbuffer_t r_peaks_ann_cbuffer;
} peak_detect_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief           Init peak_detect algorithm data
 *
 * @param[in]       peak_detect                     Pointer to peak_detect_t
 * @param[in]       filtered_data_buff        Data buffer
 * @param[in]       filtered_data_buff_size   Size of data buffer
 * @param[in]       r_peaks_ant_buff          Buffer to store R peak annotation
 * @param[in]       r_peaks_ant_buff_size     R peak annotation buffer size
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t peak_detect_init(peak_detect_t *peak_detect,
                          int16_t       *filtered_data_buff,
                          uint32_t       filtered_data_buff_size,
                          uint8_t       *r_peaks_ann_buff,
                          uint32_t       r_peaks_ann_buff_size);

/**
 * @brief           Clear peak_detect algorithm data
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t peak_detect_clear(peak_detect_t *peak_detect);

/**
 * @brief           peak_detect algorithm process
 *
 * @param[in]       peak_detect       Pointer to peak_detect_t
 * @param[in]       input       Data input
 *
 * @return
 *  - (1) : Peak is detected
 *  - (0) : Peak is not detected
 *  - (-1): Error
 */
uint32_t peak_detect_process(peak_detect_t *peak_detect, int16_t input);

/**
 * @brief           Get output data
 *
 * @param[in]       peak_detect       Pointer to peak_detect_t
 * @param[out]      ecg_val     ECG data
 * @param[out]      is_peak     Is output data R peak or not
 * @param[out]      heart_rate  Heart rate
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
uint32_t peak_detect_get_output(peak_detect_t *peak_detect,
                                int16_t       *ecg_val,
                                uint8_t       *is_peak,
                                uint8_t       *heart_rate);

/**
 * @brief           Return is output available or not
 *
 * @param[in]       peak_detect       Pointer to peak_detect_t
 *
 * @return
 *  - (1) : Available
 *  - (0) : Not available
 *  - (-1): Error
 */
uint32_t peak_detect_output_avalable(peak_detect_t *peak_detect);

#endif // __PEAK_DETECT_H

/* End of file -------------------------------------------------------- */
