#ifndef LINECHART_H
#define LINECHART_H
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>

#include <iostream>
#include <vector>

using namespace std;

class LineChart
{
public:
    //LineChart();
    LineChart(QWidget *w);

    void addData(uint64_t,float);
    void reDraw();

    unsigned int getLenght();
    float getData(uint index);
private:
    uint Width;
    uint Height;

    QGraphicsScene *Scene;
    QGraphicsView *View;

    vector<uint64_t> Time;
    vector<float> Data;
};

#endif // LINECHART_H
