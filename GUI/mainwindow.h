#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QTextBrowser>

//#include <cstring>

#include "linechart.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void SerialReceived();

    void on_pushButton_clicked();

    void on_pushButton_port_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_close_clicked();

private:
    Ui::MainWindow *ui;
    LineChart *Chart;

    QString LogName;

    void ShowCOMPorts(void);

    void SetPort(QString str);
    void SendSerial(QString str);
};

#endif // MAINWINDOW_H
