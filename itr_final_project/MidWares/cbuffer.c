/**
 * @file       cbuffer.h
 * @copyright
 * @license
 * @version    1.0.0
 * @date       26/06/2024
 * @author     Minh Phung Nhat
 *             Hung Pham Duc
 *             Khoi Nguyen Thanh
 * @brief      Circular Buffer
 *             This Circular Buffer is safe to use in IRQ with single reader,
 *             single writer. No need to disable any IRQ.
 *
 *             Capacity = <size> - 1
 * @note       None
 */
/* Define to prevent recursive inclusion ------------------------------ */
#include "cbuffer.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief           Write 1 byte to circular buffer
 *
 * @param[in]       cb      Pointer to a cbuffer_t structure
 * @param[in]       byte    Data to write
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
static uint32_t cb_write_byte(cbuffer_t *cb, uint8_t byte);

/**
 * @brief           Read 1 byte from circular buffer
 *
 * @param[in]       cb      Pointer to a cbuffer_t structure
 * @param[in]       byte    Pointer to data buffer
 *
 * @return
 *  - (0) : Success
 *  - (-1): Error
 */
static uint32_t cb_read_byte(cbuffer_t *cb, uint8_t *byte);

/* Function definitions ----------------------------------------------- */
uint32_t cb_init(cbuffer_t *cb, void *buf, uint32_t size, uint8_t unit_size)
{
  if (cb == NULL)
    return CB_ERROR;
  if (buf == NULL)
    return CB_ERROR;
  if (size >= CB_MAX_SIZE)
    return CB_ERROR;

  cb->data      = buf;
  cb->size      = size;
  cb->writer    = 0;
  cb->reader    = 0;
  cb->overflow  = 0;
  cb->active    = 1;
  cb->unit_size = unit_size;

  return CB_SUCCESS;
}

uint32_t cb_clear(cbuffer_t *cb)
{
  if (cb == NULL)
    return CB_ERROR;

  cb->writer   = 0;
  cb->reader   = 0;
  cb->overflow = 0;

  return CB_SUCCESS;
}

uint32_t cb_read(cbuffer_t *cb, void *buf, uint32_t nbytes)
{
  uint32_t data_count      = 0;
  uint32_t num_avail_bytes = 0;
  if (cb == NULL)
    return CB_ERROR;

  if (buf == NULL)
    return CB_ERROR;

  data_count = cb_data_count(cb);
  if (data_count >= nbytes)
    num_avail_bytes = nbytes;
  else
    num_avail_bytes = data_count;

  for (int i = 0; i < num_avail_bytes; i++)
  {
    cb_read_byte(cb, (uint8_t *)buf + i);
  }

  return num_avail_bytes;
}

uint32_t cb_write(cbuffer_t *cb, void *buf, uint32_t nbytes)
{
  uint32_t space_count     = 0;
  uint32_t num_avail_bytes = 0;
  if (cb == NULL)
    return CB_ERROR;

  if (buf == NULL)
    return CB_ERROR;

  space_count = cb_space_count(cb);
  if (space_count >= nbytes)
  {
    num_avail_bytes = nbytes;
    cb->overflow    = 0;
  }
  else
  {
    num_avail_bytes = space_count;
    cb->overflow    = nbytes - space_count;
  }

  for (int i = 0; i < num_avail_bytes; i++)
  {
    cb_write_byte(cb, *((uint8_t *)buf + i));
  }

  return num_avail_bytes;
}

uint32_t cb_data_count(cbuffer_t *cb)
{
  uint32_t res = 0;

  if (cb == NULL)
    return CB_ERROR;

  if (cb->writer >= cb->reader)
    res = cb->writer - cb->reader;
  else
    res = cb->size - cb->reader + cb->writer;

  return res;
}

uint32_t cb_space_count(cbuffer_t *cb)
{
  uint32_t res = 0;

  if (cb == NULL)
    return CB_ERROR;

  if (cb->reader > cb->writer)
    res = cb->reader - cb->writer - cb->unit_size;
  else if (cb->reader < cb->writer)
    res = cb->size - cb->writer + cb->reader - cb->unit_size;
  else
    res = cb->size - cb->unit_size;

  return res;
}

/* Private definitions ----------------------------------------------- */
static uint32_t cb_write_byte(cbuffer_t *cb, uint8_t byte)
{
  uint32_t next = cb->writer + 1;

  if (next == cb->size)
    next = 0;

  if (next == cb->reader)
    return CB_ERROR;

  *(cb->data + cb->writer) = byte;
  cb->writer               = next;
  return CB_SUCCESS;
}

static uint32_t cb_read_byte(cbuffer_t *cb, uint8_t *byte)
{
  uint32_t next = cb->reader + 1;

  if (cb->reader == cb->writer)
    return CB_ERROR;

  if (next == cb->size)
    next = 0;

  *byte      = *(cb->data + cb->reader);
  cb->reader = next;
  return CB_SUCCESS;
}

/* End of file -------------------------------------------------------- */
