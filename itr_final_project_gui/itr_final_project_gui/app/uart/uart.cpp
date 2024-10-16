#include "uart.h"
#include <QDebug>

Uart::Uart(QObject *parent) : QObject{ parent }
{
  QObject::connect(&serial, &QSerialPort::readyRead, this, &Uart::read_data);

  this->serial.setDataBits(QSerialPort::Data8);
  this->serial.setStopBits(QSerialPort::OneStop);
  this->serial.setParity(QSerialPort::NoParity);

  this->state = rx_state_t::WAITING_TYPE;
}


void Uart::connect(const QString &name, int baudrate)
{
  this->serial.setBaudRate(baudrate);
  this->serial.setPortName(name);
  this->serial.open(QIODevice::ReadWrite);
}


void Uart::disconnect()
{
  this->serial.close();
}


bool Uart::is_open()
{
  return this->serial.isOpen();
}

void Uart::send_cmd(uart_cmd_t cmd)
{
  if (cmd != uart_cmd_t::START_GETTING_DATA &&
      cmd != uart_cmd_t::STOP_GETTING_DATA && cmd != uart_cmd_t::STOP_WARNING)
    return;

  uart_msg_t msg = { (uint8_t)cmd, 0 };
  this->serial.write((char *)&msg, sizeof(msg));
}

void Uart::read_data()
{
  // qInfo() << "Receive data";
  QByteArray rxData = serial.readAll();
  for (int i = 0; i < rxData.size(); i++)
  {
    this->handle_byte(rxData.at(i));
  }
}


void Uart::handle_byte(unsigned char data)
{
  // qInfo() << data << '\n';
  switch (this->state)
  {
  case rx_state_t::WAITING_TYPE:
    if (data == uart_cmd_t::DATA_RX)
    {
      this->state    = rx_state_t::WAITING_LENGTH;
      this->num_byte = 0;
    }
    break;
  case rx_state_t::WAITING_LENGTH:
    this->rx_len = data;
    if (this->rx_len == 0)
      this->state = rx_state_t::WAITING_TYPE;
    else
      this->state  = rx_state_t::WAITING_VALUE;
    break;
  case rx_state_t::WAITING_VALUE:
    this->rx_buff[this->num_byte++] = data;
    if (this->num_byte == this->rx_len)
    {
      this->state       = rx_state_t::WAITING_TYPE;
      uart_value_t *val = (uart_value_t *)(&this->rx_buff);
      emit          read_done(val->data, val->is_peak, val->heart_rate);
    }
    break;
  }
}
