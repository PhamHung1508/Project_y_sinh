#ifndef __UART_H
#define __UART_H

#include <QObject>
#include <QSerialPort>

class Uart : public QObject
{
  Q_OBJECT

  enum rx_state_t
  {
    WAITING_TYPE,
    WAITING_LENGTH,
    WAITING_VALUE,
  };

  typedef struct __attribute__((packed))
  {
    int16_t  data    : 15;
    uint16_t is_peak : 1;
    uint8_t  heart_rate;
  } uart_value_t;

  typedef struct __attribute__((packed))
  {
    uint8_t type;
    uint8_t len;
  } uart_msg_t;

  public:
  enum uart_cmd_t
  {
    DATA_RX,
    START_GETTING_DATA,
    STOP_GETTING_DATA,
    STOP_WARNING
  };

  explicit Uart(QObject *parent = nullptr);
  void connect(const QString &name, int baudrate);
  void disconnect();
  bool is_open();
  void send_cmd(uart_cmd_t cmd);

  signals:
  void read_done(qint16, bool, quint8);

  private slots:
  void read_data();

  private:
  void handle_byte(unsigned char data);

  QSerialPort   serial;
  unsigned char rx_len;
  unsigned char rx_buff[8];
  unsigned char num_byte;
  rx_state_t    state;
};

#endif // __UART_H
