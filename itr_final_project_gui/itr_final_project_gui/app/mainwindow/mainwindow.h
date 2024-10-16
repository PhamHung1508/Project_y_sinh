#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"
#include "uart.h"
#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  signals:
  void process_raw_data_done(double);

  private slots:
  void process_raw_data(qint16 data, bool is_peak, quint8 heart_rate);

  void plot_data();


  private slots:
  void on_btn_refresh_clicked();

  void on_btn_connect_clicked();

  void on_btn_run_clicked();

  void on_btn_clear_clicked();

  void on_btn_stop_warning_clicked();

  private:
  Ui::MainWindow *ui;
  QVector<double> x_val;
  QVector<double> ecg_data;
  QVector<bool>   r_peak_anotation;
  bool            is_ready_plot;
  QTimer          timer;
  Uart            uart;
  quint8          heart_rate;

  // QCPItemStraightLine *denoisedSignalUpdateLine;
  // QCPItemStraightLine *algorithmSignalUpdateLine;
  // bool isReadyPlot;
  // bool isFirstTimePlotDenoised;
};
#endif // MAINWINDOW_H
