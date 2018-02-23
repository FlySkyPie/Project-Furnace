#include "linechart.h"

#include <QPainter>

LineChart::LineChart(QWidget *w)
{
    //set size
    Width = w->width();
    Height = w->height();


    //add QGraphicsView
    Scene = new QGraphicsScene();
    View = new QGraphicsView(w);
    View->setFixedSize(Width,Height);
    View->setScene(Scene);

    //disable scroll
    View->setSceneRect(0, 0, Width,Height);
    View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //first draw
    reDraw();
    //Scene->clear();
    //QPixmap pic(Width,Height);
    //pic.fill( QColor(255,255,255));
    //Scene->addPixmap( pic );
}


void LineChart::addData(uint64_t _time,float _data)
{
    if (Time.size() +1 > Width)
    {
         Time.erase( Time.begin());
         Data.erase( Data.begin());
    }
    Time.push_back(_time);
    Data.push_back(_data);
}

unsigned int LineChart::getLenght()
{
    return Time.size();
}

 void LineChart::reDraw()
 {
    //fill background to white
    Scene->clear();
    QPixmap pic(Width,Height);
    pic.fill( QColor(255,255,255));

    QPainter painter(&pic);
    painter.setPen(Qt::black);

    uint FrameWidth,FrameHeight,MaxElement,MaxTemperature;
    float StepWidth,StepHeight;
    FrameWidth = Width - 20;        //frame's width
    FrameHeight = Height - 20;      //frame's height
    MaxElement = Width;               //how many element(data) that want show
    MaxTemperature = 800;           //maximum value of y axis
    StepWidth = 1.0 * FrameWidth / MaxElement;          // (x/element) ratio
    StepHeight = 1.0 * FrameHeight / MaxTemperature;    // (y/temerature) ratio

    //prepare draw axis line
    painter.setPen(Qt::gray);
    uint DistantX,DistantY;
    DistantX = 60;//FrameWidth / 10;
    DistantY = 100;

    //draw vertical line
    uint v_lines = MaxElement / DistantX;
    for(uint i=1;i<v_lines;i++)
    {
        uint x,y1,y2;
        x = 10 + DistantX * i;
        y1 = Height - 10;
        y2 = Height - 10 - FrameHeight;
        painter.drawLine(x,y1,x,y2);
    }

    //draw horizontal line
    uint lines = MaxTemperature / DistantY;
    for(uint i=1; i<=lines ;i++)
    {
        uint x1,x2,y;
        x1 = 10 ;
        x2 = 10 + FrameWidth;
        y = Height - 10 -  DistantY * i * StepHeight;
        painter.drawLine(x1,y,x2,y);
    }

    //draw frame
    painter.setPen(Qt::black);
    painter.drawRect(10,10, FrameWidth, FrameHeight);

    //draw data line
    if( getLenght()>0 )
    {
        painter.setPen(Qt::blue);
        uint StartIndex,EndIndex;       //start index, maximum of i

        if(getLenght()<=MaxElement)
        {
            StartIndex = 0;
            EndIndex = getLenght()-1;
        }

/*
        if(getLenght()>MaxElement)
        {
            StartIndex = getLenght() - MaxElement-1 ;
            EndIndex = MaxElement;
        }
        else
        {
            StartIndex = 0;
            EndIndex = getLenght()-1;
        }*/

        for(uint i=0; i<EndIndex ;i++)      //(StartIndex + EndIndex -1) are real end of index
        {
            uint index = StartIndex + i;
            uint x1,y1,x2,y2;

            x1 = 10 + StepWidth * i;
            y1 = Height - 10 - (StepHeight * getData(index));
            x2 = 10 + StepWidth * (i+1);
            y2 = Height - 10 -(StepHeight * getData(index+1));
            painter.drawLine(x1,y1,x2,y2);
        }
    }

    Scene->addPixmap( pic );
 }

 float LineChart::getData(uint index)
 {
    return Data.at(index);
 }
