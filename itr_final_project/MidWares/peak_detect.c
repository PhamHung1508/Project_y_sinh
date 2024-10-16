/**
 * @file        peak_detect.c
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

/* Includes ----------------------------------------------------------- */
#include "peak_detect.h"
#include "common.h"

/* Private defines ---------------------------------------------------- */
#define AMPLITUDE_THRESHOLD    (512) // 0.25 * DATA_MAX_VAL = 0.25 * 2048
#define INTERVAL_THRESHOLD     (300) // 0.6(s) * Fs
#define BUFFER_MIN_SIZE        (4097)
#define OUTPUT_AVAIL_SIZE      (4096)
#define OUTPUT_INDEX_FROM_HEAD (2048)
#define STARTUP_SIZE           (1024)
#define CBUFFER_UNIT_SIZE      (2)
#define SAMPLING_FREQ          (500)

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief   Get pointer to a data unit in cbuffer
 *
 * @param[in]   cb          Pointer to cbuffer_t strucure
 * @param[in]   index       Index start from reader (reader <=> index = 0)
 * @param[in]   unit_size   Each unit size in cbuffer
 *
 * @return  Pointer to value
 *
 */
static void *cb_get_ptr(cbuffer_t *cb, uint32_t index, uint8_t unit_size);

/* Function definitions ----------------------------------------------- */
uint32_t peak_detect_init(peak_detect_t *peak_detect,
                          int16_t       *filtered_data_buff,
                          uint32_t       filtered_data_buff_size,
                          uint8_t       *r_peaks_ann_buff,
                          uint32_t       r_peaks_ann_buff_size)
{
  // Check input
  ASSERT(peak_detect != NULL, PEAK_DETECT_ERROR);
  ASSERT(filtered_data_buff != NULL, PEAK_DETECT_ERROR);
  ASSERT(filtered_data_buff_size >= BUFFER_MIN_SIZE, PEAK_DETECT_ERROR);
  ASSERT(r_peaks_ann_buff != NULL, PEAK_DETECT_ERROR);
  ASSERT(filtered_data_buff_size >= BUFFER_MIN_SIZE, PEAK_DETECT_ERROR);

  // Init data
  peak_detect->temp_peak_value         = 0;
  peak_detect->temp_peak_index         = 0;
  peak_detect->peak_index_last         = 0;
  peak_detect->heart_rate              = 0;
  peak_detect->is_first_time           = 1;
  peak_detect->is_output_available     = 0;
  peak_detect->filtered_data_buff      = filtered_data_buff;
  peak_detect->filtered_data_buff_size = filtered_data_buff_size;
  peak_detect->r_peaks_ann_buff        = r_peaks_ann_buff;
  peak_detect->r_peaks_ann_buff_size   = r_peaks_ann_buff_size;

  cb_init(&peak_detect->filtered_data_cbuffer, peak_detect->filtered_data_buff,
          peak_detect->filtered_data_buff_size, CBUFFER_UNIT_SIZE);
  cb_init(&peak_detect->r_peaks_ann_cbuffer, peak_detect->r_peaks_ann_buff,
          peak_detect->r_peaks_ann_buff_size, 1);

  return PEAK_DETECT_SUCCESS;
}

uint32_t peak_detect_clear(peak_detect_t *peak_detect)
{
  // Check input
  ASSERT(peak_detect != NULL, PEAK_DETECT_ERROR);

  // Clear data
  peak_detect->temp_peak_value     = 0;
  peak_detect->temp_peak_index     = 0;
  peak_detect->is_first_time       = 1;
  peak_detect->is_output_available = 0;
  peak_detect->peak_index_last     = 0;
  peak_detect->heart_rate          = 0;

  cb_clear(&peak_detect->filtered_data_cbuffer);

  return PEAK_DETECT_SUCCESS;
}

uint32_t peak_detect_process(peak_detect_t *peak_detect, int16_t input)
{
  uint32_t data_point_cnt;

  // Check input
  ASSERT(peak_detect != NULL, PEAK_DETECT_ERROR);

  // Store data
  data_point_cnt = cb_data_count(&peak_detect->filtered_data_cbuffer) / CBUFFER_UNIT_SIZE;

  if (data_point_cnt == OUTPUT_AVAIL_SIZE)
  {
    peak_detect->is_output_available = 1;
  }

  if (data_point_cnt == peak_detect->filtered_data_buff_size / CBUFFER_UNIT_SIZE - 1)
  {
    int16_t temp_data_int16;
    uint8_t temp_data_uint8;
    cb_read(&peak_detect->filtered_data_cbuffer, &temp_data_int16, sizeof(temp_data_int16));
    cb_write(&peak_detect->filtered_data_cbuffer, &input, sizeof(input));

    cb_read(&peak_detect->r_peaks_ann_cbuffer, &temp_data_uint8, sizeof(temp_data_uint8));
    temp_data_uint8 = 0;
    cb_write(&peak_detect->r_peaks_ann_cbuffer, &temp_data_uint8, sizeof(temp_data_uint8));

    // Update BOI region location
    if (peak_detect->temp_peak_index > 0)
      peak_detect->temp_peak_index--;
    if (peak_detect->peak_index_last > 0)
      peak_detect->peak_index_last--;
  }
  else
  {
    uint8_t temp_data_uint8 = 0;
    cb_write(&peak_detect->filtered_data_cbuffer, &input, sizeof(input));
    cb_write(&peak_detect->r_peaks_ann_cbuffer, &temp_data_uint8, sizeof(temp_data_uint8));
    data_point_cnt++;
  }

  // Check number of date in buffer
  if (data_point_cnt >= STARTUP_SIZE)
  {
    // Detect peak
    if (input >= AMPLITUDE_THRESHOLD)
    {
      if (peak_detect->is_first_time == 1)
      {
        peak_detect->temp_peak_value = input;
        peak_detect->temp_peak_index = data_point_cnt - 1;
        peak_detect->is_first_time   = 0;
      }
      else
      {
        if ((data_point_cnt - peak_detect->temp_peak_index) <= INTERVAL_THRESHOLD)
        {
          if (peak_detect->temp_peak_value <= input)
          {
            peak_detect->temp_peak_value = input;
            peak_detect->temp_peak_index = data_point_cnt - 1;
          }
        }
        else
        {
          uint8_t *r_peak_ann_ptr;
          r_peak_ann_ptr = (uint8_t *)cb_get_ptr(&peak_detect->r_peaks_ann_cbuffer,
                                                 peak_detect->temp_peak_index, 1);
          *r_peak_ann_ptr            = 1;
          peak_detect->is_first_time = 1;
          if (peak_detect->peak_index_last == 0)
          {
            peak_detect->peak_index_last = peak_detect->temp_peak_index;
          }
          else
          {
            double temp_val = 1.0 *
            (peak_detect->temp_peak_index - peak_detect->peak_index_last) / SAMPLING_FREQ;
            peak_detect->heart_rate      = (uint8_t)(60 / temp_val);
            peak_detect->peak_index_last = peak_detect->temp_peak_index;
          }
        }
      }
    }
  }

  return PEAK_DETECT_SUCCESS;
}

uint32_t peak_detect_get_output(peak_detect_t *peak_detect,
                                int16_t       *ecg_val,
                                uint8_t       *is_peak,
                                uint8_t       *heart_rate)
{
  ASSERT(peak_detect != NULL, PEAK_DETECT_ERROR);
  ASSERT(ecg_val != NULL, PEAK_DETECT_ERROR);
  ASSERT(is_peak != NULL, PEAK_DETECT_ERROR);
  ASSERT(heart_rate != NULL, PEAK_DETECT_ERROR);

  *ecg_val = *(int16_t *)cb_get_ptr(&peak_detect->filtered_data_cbuffer, 0, 2);
  *is_peak = *(uint8_t *)cb_get_ptr(&peak_detect->r_peaks_ann_cbuffer, 0, 1);
  *heart_rate = peak_detect->heart_rate;

  return PEAK_DETECT_SUCCESS;
}

uint32_t peak_detect_output_avalable(peak_detect_t *peak_detect)
{
  ASSERT(peak_detect != NULL, PEAK_DETECT_ERROR);
  return peak_detect->is_output_available;
}

/* Private definitions ------------------------------------------------ */
void *cb_get_ptr(cbuffer_t *cb, uint32_t index, uint8_t unit_size)
{
  uint32_t arr_index = cb->reader + index * unit_size;
  if (arr_index >= cb->size)
  {
    arr_index -= cb->size;
  }
  //  printf("cb->reader = %d\tindex = %d\t arr_index = %d\n", cb->reader, index, arr_index);
  return cb->data + arr_index;
}

/* End of file -------------------------------------------------------- */
