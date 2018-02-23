#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDateTime>
#include <QFile>
#include <QTextStream>


#include <iostream>
#include <algorithm>
#include <string>

#include <unistd.h>

using namespace std;

QSerialPort *SerialCOM;

bool SerialConnected = false;
bool Online = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800,600);        //set windows size

    //"Enter" in lineEdit equal press button
    connect(ui->lineEdit, SIGNAL(returnPressed()),ui->pushButton,SIGNAL(clicked()));

    //setup line chart
    Chart = new LineChart(ui->widget);

    //setup log file name
    QDateTime currentDateTime (QDateTime::currentDateTime());
    LogName = "log/" + currentDateTime.toString("yyyy-MM-dd_hh:mm:ss") ;

    ShowCOMPorts();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete Chart;   //release chart
}

void MainWindow::SerialReceived()
{
    if(!SerialCOM->canReadLine())
        return;

    QString RxString = SerialCOM->readLine();

    //remove \n
    string RxCString = RxString.toStdString();  //.toUtf8().constData();
    RxCString.erase(remove(RxCString.begin(), RxCString.end(), '\n'), RxCString.end());
    RxString = QString::fromStdString(RxCString);

    if (RxString!="")
    {
        ui->plainTextEdit->appendPlainText(RxString);

        QStringList Datas = RxString.split(":");
        if(Datas.length()!=2)
            return;

        if(Datas[0]=="T")
        {
            //get UNIX time of current time
            QDateTime currentDateTime (QDateTime::currentDateTime());
            uint unixtime = currentDateTime.toTime_t() ;

            //write log
            if(Online)
            {
                QFile file(LogName);
                if (file.open(QFile::WriteOnly | QFile::Append))
                {
                    QTextStream stream(&file);
                    stream << unixtime << "," <<  Datas[1].toFloat() << endl;
                    file.close();
                }
            }

            //refresh line chart
            Chart->addData(unixtime,Datas[1].toFloat());    //add data
            Chart->reDraw();

            //update info to label
            ui->Tlabel1->setText( "當前溫度：" + Datas[1] + "°C");

            return;
        }
        if(Datas[0]=="W")
        {
            /*
            //write log
            QFile file(LogName);
            if (file.open(QFile::WriteOnly | QFile::Append))
            {
                QTextStream stream(&file);
                stream << Datas[1] << endl;
                file.close();
            }
            //change string
            RxString =  Datas[1];*/

        }
        if(Datas[0]=="O")
        {
            ui->Tlabel2->setText( "PWM強度：" + Datas[1]);
        }

    }
}

//execute command
void MainWindow::on_pushButton_clicked()
{
    QString cmd = ui->lineEdit->text();
    ui->lineEdit->setText("");              //clear input box

    SendSerial(cmd);

}

//send data to arduino
void MainWindow::SendSerial(QString str)
{
    if(!SerialConnected)
    {
        ui->plainTextEdit->appendPlainText("COM port hadn't set!");
        return;
    }
    //cout << "[DeBUG]" << str.toStdString() << "\n";
    usleep(200000);             //waitting UART stabilize
    SerialCOM->write(str.toStdString().c_str(),str.size());
}

//setting serial port
void  MainWindow::SetPort(QString str)
{
    if(SerialCOM != NULL)   //release old serial
    {
        delete SerialCOM;
    }

    //setup SerialCOM
    SerialCOM = new QSerialPort(this);
    SerialCOM->setPortName(str);
    SerialCOM->setBaudRate(QSerialPort::Baud9600);
    SerialCOM->setDataBits(QSerialPort::Data8);
    SerialCOM->setParity(QSerialPort::NoParity);
    SerialCOM->setStopBits(QSerialPort::OneStop);
    SerialCOM->setFlowControl(QSerialPort::NoFlowControl);
    SerialCOM->open(QIODevice::ReadWrite);
    connect(SerialCOM,SIGNAL(readyRead()),this,SLOT(SerialReceived()));

    SerialConnected = true;
}

//show all COM ports
void MainWindow::ShowCOMPorts(void)
{
    QString s = "SerialPort : ";
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        s += info.portName() + ", ";

    }
    ui->plainTextEdit->appendPlainText(s);
}

void MainWindow::on_pushButton_port_clicked()
{
    QString p = ui->lineEdit_port->text();
    ui->lineEdit_port->setEnabled(false);
    ui->pushButton_port->setEnabled(false);

    SetPort(p);

    ui->pushButton_start->setEnabled(true);
}

void MainWindow::on_pushButton_start_clicked()
{
    //disable GUI objects
    ui->lineEdit_Kp->setEnabled(false);
    ui->lineEdit_Ki->setEnabled(false);
    ui->lineEdit_Kd->setEnabled(false);
    ui->lineEdit_Target->setEnabled(false);
    ui->pushButton_start->setEnabled(false);

    ui->pushButton_close->setEnabled(true);

    //get PID const
    QString Kp,Ki,Kd,Target;
    Kp = ui->lineEdit_Kp->text();
    Ki = ui->lineEdit_Ki->text();
    Kd = ui->lineEdit_Kd->text();
    Target =  ui->lineEdit_Target->text();

    //setup log file name
    QDateTime currentDateTime (QDateTime::currentDateTime());
    LogName = "log/" + currentDateTime.toString("yyyy-MM-dd_hh:mm:ss") ;

    //write first info
    QFile file(LogName);
    if (file.open(QFile::WriteOnly | QFile::Append))
    {
        QTextStream stream(&file);
        stream << "Kp," << Kp << "\n";
        stream << "Ki," << Ki << "\n";
        stream << "Kd," << Kd << "\n";
        stream << "Target," << Target << "\n";
        file.close();
    }

    //send command to arduino
    SendSerial("Kp:" + Kp + "\n");
    SendSerial("Ki:" + Ki + "\n");
    SendSerial("Kd:" + Kd + "\n");
    SendSerial("Target:" + Target + "\n");
    SendSerial("start\n");

    Online = true;

}

void MainWindow::on_pushButton_close_clicked()
{
    //enable GUI objects
    ui->lineEdit_Kp->setEnabled(true);
    ui->lineEdit_Ki->setEnabled(true);
    ui->lineEdit_Kd->setEnabled(true);
    ui->lineEdit_Target->setEnabled(true);
    ui->pushButton_start->setEnabled(true);

    ui->pushButton_close->setEnabled(false);



    Online = false;

    SendSerial("close\n");

    ui->Tlabel2->setText( "PWM強度：--" );
}
