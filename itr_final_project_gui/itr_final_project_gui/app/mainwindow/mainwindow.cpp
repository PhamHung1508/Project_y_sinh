#include "mainwindow.h"
#include "app_config.h"
#include "ui_mainwindow.h"


#include <QDebug>
#include <QPalette>
#include <QSerialPort>
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // Config window
  setFixedSize(width(), height());
  setWindowTitle("ECG Plotting");

  // Init value of members
  this->is_ready_plot = false;
  this->heart_rate    = 0;

  // Connect signal to slot
  connect(&timer, &QTimer::timeout, this, &MainWindow::plot_data);
  connect(&uart, &Uart::read_done, this, &MainWindow::process_raw_data);

  // Serial port init
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  QList<QString>         str_ports;
  for (int i = 0; i < ports.size(); i++)
  {
    str_ports.append(ports.at(i).portName() + " (" + ports.at(i).description() + ")");
  }
  ui->cb_port->addItems(str_ports);

  // Disable run button
  ui->btn_run->setDisabled(true);

  // Find baudrate and set initial baudrate
  QList<qint32>  baudrates = QSerialPortInfo::standardBaudRates();
  QList<QString> str_baudrates;
  for (int i = 0; i < baudrates.size(); i++)
  {
    str_baudrates.append(QString::number(baudrates.at(i)));
  }
  ui->cb_baud->addItems(str_baudrates);
  ui->cb_baud->setCurrentText("115200");

  // Init LCD number
  QPalette palette = ui->lcd_heart_rate->palette();
  palette.setColor(palette.WindowText, Qt::black);
  palette.setColor(palette.Light, Qt::black);
  palette.setColor(palette.Dark, Qt::black);

  // Init raw signal graph
  ui->cp_raw_signal->addGraph();
  ui->cp_raw_signal->graph(0)->setScatterStyle(QCPScatterStyle::ScatterShape::ssNone);
  ui->cp_raw_signal->graph(0)->setLineStyle(QCPGraph::lsLine);
  ui->cp_raw_signal->addGraph();
  ui->cp_raw_signal->graph(1)->setScatterStyle(QCPScatterStyle::ScatterShape::ssDisc);
  ui->cp_raw_signal->graph(1)->setLineStyle(QCPGraph::lsNone);
  ui->cp_raw_signal->graph(1)->setPen(QColor(255, 0, 0));
  ui->cp_raw_signal->xAxis->setRange(0, DATA_ARR_SIZE);
  ui->cp_raw_signal->yAxis->setRange(-2000, 2000);

  // Init x axis for graph
  for (int i = 0; i < DATA_ARR_SIZE; i++)
  {
    this->x_val.append(i);
  }
}


MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::plot_data()
{
  QVector<double> r_peak_x;
  QVector<double> r_peak_y;

  // Plot ECG data
  ui->cp_raw_signal->graph(0)->setData(this->x_val, this->ecg_data);

  for (int i = 0; i < this->r_peak_anotation.size(); i++)
  {
    if (this->r_peak_anotation.at(i) == 1)
    {
      r_peak_x.append(i);
      r_peak_y.append(this->ecg_data.at(i));
    }
  }
  ui->cp_raw_signal->graph(1)->setData(r_peak_x, r_peak_y);

  ui->cp_raw_signal->replot();
  ui->cp_raw_signal->update();

  // Display heart rate
  ui->lcd_heart_rate->display(this->heart_rate);
}

void MainWindow::process_raw_data(qint16 data, bool is_peak, quint8 heart_rate)
{
  // qInfo() << "preprocessRawData";
  if (ui->btn_run->text() != "Run")
  {
    // Store ecg data
    double val = data * 1.0;
    if (this->ecg_data.size() < DATA_ARR_SIZE)
    {
      this->ecg_data.append(val);
      this->r_peak_anotation.append(is_peak);
    }
    else
    {
      this->ecg_data.pop_front();
      this->ecg_data.append(val);
      this->r_peak_anotation.pop_front();
      this->r_peak_anotation.append(is_peak);
    }

    // Store heart rate
    this->heart_rate = heart_rate;

    // qInfo() << "data = " << data;
    // qInfo() << "is_peak = " << is_peak;
    // qInfo() << "heart_rate = " << heart_rate;

    this->is_ready_plot = true;
  }
}
