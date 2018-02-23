#include "mainwindow.h"


#include <QApplication>
#include <QtWidgets/QTextBrowser>



using namespace std;





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
