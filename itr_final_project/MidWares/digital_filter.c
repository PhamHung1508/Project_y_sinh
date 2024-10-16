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

/* Includes ----------------------------------------------------------- */
#include "digital_filter.h"
#include "common.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */
const double num_low[DIGITAL_FILTER_LOWPASS_ORDER] = {
  0.126645382235659, 0.759872293413951, 1.899680733534879, 2.532907644713172,
  1.899680733534879, 0.759872293413951, 0.126645382235659
};

const double den_low[DIGITAL_FILTER_LOWPASS_ORDER] = {
  1.000000000000000, 2.120905816677038, 2.468548827441033, 1.657377603523223,
  0.684154573342103, 0.158265425198276, 0.016052216900477
};

const double num_high[DIGITAL_FILTER_HIGHPASS_ORDER] = {
  0.997087901678219, -2.991263705034658, 2.991263705034658, -0.997087901678219
};

const double den_high[DIGITAL_FILTER_HIGHPASS_ORDER] = {
  1.000000000000, -2.994167310674027, 2.988351619078653, -0.994184283673074
};

static const double num_notch[DIGITAL_FILTER_NOTCH_ORDER] = {
  0.982845203340365, -4.77092500802863, 10.6682068104887, -13.7054956643011,
  10.6682068104887,  -4.77092500802863, 0.982845203340365
};

const double den_notch[DIGITAL_FILTER_NOTCH_ORDER] = {
  1.000000000000000, -4.82620004413873, 10.7295914792704, -13.7050194884557,
  10.606527854659,   -4.716126147764,   0.965984693728759
};

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief           Filter data
 *
 * @param[in]       filter    data input
 *
 * @return
 *  data after filtered
 *
 */
void digital_filter_lowpass_process(digital_filter_t *filter);

/**
 * @brief           Filter data
 *
 * @param[in]       data_input    data input
 *
 * @return
 *  data after filtered
 *
 */
void digital_filter_highpass_process(digital_filter_t *filter);

/**
 * @brief           Filter data
 *
 * @param[in]       data_input    data input
 *
 * @return
 *  data after filtered
 *
 */
void digital_filter_notch_process(digital_filter_t *filter);

/* Function definitions ----------------------------------------------- */
uint32_t digital_filter_clear(digital_filter_t *filter)
{
  ASSERT(filter != NULL, DIGITAL_FILTER_ERROR);

  for (int i = 0; i < DIGITAL_FILTER_HIGHPASS_ORDER; i++)
  {
    filter->low_in[i]  = 0;
    filter->low_out[i] = 0;
  }
  for (int i = 0; i < DIGITAL_FILTER_LOWPASS_ORDER; i++)
  {
    filter->high_in[i]  = 0;
    filter->high_out[i] = 0;
  }
  for (int i = 0; i < DIGITAL_FILTER_NOTCH_ORDER; i++)
  {
    filter->notch_in[i]  = 0;
    filter->notch_out[i] = 0;
  }

  return DIGITAL_FILTER_SUCCESS;
}

uint32_t digital_filter_process(digital_filter_t *filter, uint16_t input, int16_t *output)
{
  ASSERT(filter != NULL, DIGITAL_FILTER_ERROR);

  filter->low_in[0] = (double)input;
  digital_filter_lowpass_process(filter);

  filter->high_in[0] = filter->low_out[0];
  digital_filter_highpass_process(filter);

  filter->notch_in[0] = filter->high_out[0];
  digital_filter_notch_process(filter);

  *output = (int16_t)filter->notch_out[0];

  return DIGITAL_FILTER_SUCCESS;
}

/* Private definitions ------------------------------------------------ */
void digital_filter_lowpass_process(digital_filter_t *filter)
{
  filter->low_out[0] = num_low[0] * filter->low_in[0];
  for (int i = 1; i < DIGITAL_FILTER_LOWPASS_ORDER; i++)
  {
    filter->low_out[0] += (num_low[i] * filter->low_in[i]);
  }
  for (int i = 1; i < DIGITAL_FILTER_LOWPASS_ORDER; i++)
  {
    filter->low_out[0] -= (den_low[i] * filter->low_out[i]);
  }
  for (int i = (DIGITAL_FILTER_LOWPASS_ORDER - 1); i > 0; i--)
  {
    filter->low_out[i] = filter->low_out[i - 1];
    filter->low_in[i]  = filter->low_in[i - 1];
  }
}

void digital_filter_highpass_process(digital_filter_t *filter)
{
  filter->high_out[0] = num_high[0] * filter->high_in[0];
  for (int i = 1; i < DIGITAL_FILTER_HIGHPASS_ORDER; i++)
  {
    filter->high_out[0] += (num_high[i] * filter->high_in[i]);
  }
  for (int i = 1; i < DIGITAL_FILTER_HIGHPASS_ORDER; i++)
  {
    filter->high_out[0] -= (den_high[i] * filter->high_out[i]);
  }
  for (int i = (DIGITAL_FILTER_HIGHPASS_ORDER - 1); i > 0; i--)
  {
    filter->high_out[i] = filter->high_out[i - 1];
    filter->high_in[i]  = filter->high_in[i - 1];
  }
}

void digital_filter_notch_process(digital_filter_t *filter)
{
  filter->notch_out[0] = num_notch[0] * filter->notch_in[0];
  for (int i = 1; i < DIGITAL_FILTER_NOTCH_ORDER; i++)
  {
    filter->notch_out[0] += (num_notch[i] * filter->notch_in[i]);
  }
  for (int i = 1; i < DIGITAL_FILTER_NOTCH_ORDER; i++)
  {
    filter->notch_out[0] -= (den_notch[i] * filter->notch_out[i]);
  }
  for (int i = (DIGITAL_FILTER_NOTCH_ORDER - 1); i > 0; i--)
  {
    filter->notch_out[i] = filter->notch_out[i - 1];
    filter->notch_in[i]  = filter->notch_in[i - 1];
  }
}

/* End of file -------------------------------------------------------- */
