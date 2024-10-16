#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSerialPortInfo>


void MainWindow::on_btn_refresh_clicked()
{
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  QList<QString>         str_ports;
  while (ui->cb_port->count() > 0)
  {
    ui->cb_port->removeItem(0);
  }
  for (int i = 0; i < ports.size(); i++)
  {
    str_ports.append(ports.at(i).portName() + " (" + ports.at(i).description() + ")");
  }
  ui->cb_port->addItems(str_ports);
}


void MainWindow::on_btn_connect_clicked()
{
  if (ui->btn_connect->text() == "Connect")
  {
    int space_index = ui->cb_port->currentText().indexOf(' ');
    uart.connect(ui->cb_port->currentText().mid(0, space_index),
                 ui->cb_baud->currentText().toInt());
    if (!uart.is_open())
    {
    }
    else
    {
      ui->btn_connect->setText("Disconnect");
      ui->cb_port->setDisabled(true);
      ui->cb_baud->setDisabled(true);
      ui->btn_refresh->setDisabled(true);
      ui->btn_run->setDisabled(false);
    }
  }
  else
  {
    uart.disconnect();
    ui->btn_connect->setText("Connect");
    ui->cb_port->setDisabled(false);
    ui->cb_baud->setDisabled(false);
    ui->btn_refresh->setDisabled(false);
    ui->btn_run->setDisabled(true);
  }
}


void MainWindow::on_btn_run_clicked()
{
  if (ui->btn_run->text() == "Run")
  {
    this->timer.start(10);
    ui->btn_run->setText("Stop");
    ui->btn_connect->setDisabled(true);
    ui->btn_clear->setDisabled(true);
    ui->cp_raw_signal->replot();
    ui->cp_raw_signal->update();

    ui->btn_stop_warning->setText("Stop\nWarning");

    this->uart.send_cmd(Uart::uart_cmd_t::START_GETTING_DATA);
  }
  else
  {
    this->uart.send_cmd(Uart::uart_cmd_t::STOP_GETTING_DATA);
    this->timer.stop();
    ui->btn_run->setText("Run");
    ui->btn_connect->setDisabled(false);
    ui->btn_clear->setDisabled(false);
    this->is_ready_plot = false;
  }
}


void MainWindow::on_btn_clear_clicked()
{
  this->ecg_data.clear();
  this->r_peak_anotation.clear();
  this->heart_rate = 0;

  ui->cp_raw_signal->graph(0)->data()->clear();
  ui->cp_raw_signal->graph(1)->data()->clear();
  ui->cp_raw_signal->replot();
  ui->cp_raw_signal->update();
  ui->lcd_heart_rate->display(this->heart_rate);
}


void MainWindow::on_btn_stop_warning_clicked()
{
  if (ui->btn_run->text() == "Stop")
  {
    if (ui->btn_stop_warning->text() == "Stop\nWarning")
    {
      ui->btn_stop_warning->setText("Allow\nWarning");
      this->uart.send_cmd(Uart::uart_cmd_t::STOP_WARNING);
    }
    else
    {
      ui->btn_stop_warning->setText("Stop\nWarning");
      this->uart.send_cmd(Uart::uart_cmd_t::STOP_WARNING);
    }
  }
}
