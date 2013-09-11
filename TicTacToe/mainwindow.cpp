#include "mainwindow.h"
#include <QCursor>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include <QDebug>
#include <vector>
#include <QTime>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    image0 = ui->widget0;
    image1 = ui->widget1;
    image2 = ui->widget2;
    image3 = ui->widget3;
    image4 = ui->widget4;
    image5 = ui->widget5;
    image6 = ui->widget6;
    image7 = ui->widget7;
    image8 = ui->widget8;
    image0->setAutoFillBackground(true);
    image1->setAutoFillBackground(true);
    image2->setAutoFillBackground(true);
    image3->setAutoFillBackground(true);
    image4->setAutoFillBackground(true);
    image5->setAutoFillBackground(true);
    image6->setAutoFillBackground(true);
    image7->setAutoFillBackground(true);
    image8->setAutoFillBackground(true);

    redTurn = true;
    for( int i=0;i<3;i++ )
        for( int j=0;j<3;j++ )
            hasPix[i][j] = 0;

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    palette1.setBrush(QPalette::Background,QBrush(QPixmap("1.jpg").scaled(55,55,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    palette2.setBrush(QPalette::Background,QBrush(QPixmap("2.jpg").scaled(55,55,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));

    my_egine = NULL;
    opponent_egine = NULL;
    round = 0;
    winNum = 0;
    loseNum = 0;

    connect(ui->Exit,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionTraining,SIGNAL(triggered()),this,SLOT(play()));
}

MainWindow::~MainWindow()
{
    if( my_egine!=NULL )
        delete my_egine;
    if( opponent_egine!=NULL )
        delete opponent_egine;
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<this->x()<<" "<<y();
}

void MainWindow::setPic(int id, int placeNum)
{
    if( id==-1 )
    {
        //QMessageBox::critical(this,tr("Game Over"),tr("draw!"));
        qDebug()<<"draw";
        reset();
        return;
    }
    if( id<0 || id>8 )
        return ;
    QPalette p;
    if( redTurn )
        p = palette1;
    else
        p = palette2;

    if( hasPix[id%3][id/3] )
    {
        qDebug()<<"override value,this should not happen!";
        return;
    }
    hasPix[id%3][id/3] = redTurn ? 1:2;
    redTurn = !redTurn;

    switch(id)
    {
    case 0:
        image0->setPalette(p);
        break;
    case 1:
        image1->setPalette(p);
        break;
    case 2:
        image2->setPalette(p);
        break;
    case 3:
        image3->setPalette(p);
        break;
    case 4:
        image4->setPalette(p);
        break;
    case 5:
        image5->setPalette(p);
        break;
    case 6:
        image6->setPalette(p);
        break;
    case 7:
        image7->setPalette(p);
        break;
    case 8:
        image8->setPalette(p);
        break;
    default:
        qDebug()<<"error!";
        break;
    }
}

void MainWindow::play()
{
    if( my_egine==NULL )
    {
        my_egine = new Egine(NULL,tr("1.txt"),'1');
        connect(my_egine,SIGNAL(setPic(int,int)),this,SLOT(setPic(int,int)));
    }
    if( opponent_egine == NULL )
    {
        opponent_egine = new Egine(NULL,tr("2.txt"),'2');
        connect(opponent_egine,SIGNAL(setPic(int,int)),this,SLOT(setPic(int,int)));
    }
    while( 1 )
    {
        if( redTurn )
        {
            my_egine->setInput(hasPix);
            if( my_egine->check(hasPix,my_egine->gameSide) )
            {
                opponent_egine->check(hasPix,opponent_egine->oppenont);
                winOrLose = 1;
                //qDebug()<<"win";
                break;
            }
        }
        else
        {
            opponent_egine->setInput(hasPix);
            if( opponent_egine->check(hasPix,opponent_egine->gameSide) )
            {
                my_egine->check(hasPix,my_egine->oppenont);
                winOrLose = 2;
                //qDebug()<<"lose";
                break;
            }
            // random value
            vector<int> list;
            for(int i=0;i<9;i++)
            {
                if( hasPix[i%3][i/3]==0 )
                {
                    list.push_back( i );
                }
            }
            if( list.size()==0 )
            {
                // update the Q value
                my_egine->setInput(hasPix);
                winOrLose = 0;
                break;
            }
//            else
//            {
//                int id = list[ qrand()%(list.size()) ];
//                setPic(id,list.size());
//                if( my_egine->check(hasPix,my_egine->oppenont) )
//                {
//                    winOrLose = 2;
//                    qDebug()<<"lose";
//                    break;
//                }
//            }
        }
    }
    round++;
    if( round>7000 )
    {
        if( my_egine )
        {
            delete my_egine;
            my_egine = NULL;
        }
        qDebug()<<"win  times "<<winNum;
        qDebug()<<"lose times "<<loseNum;
        round = 0;
        winNum = 0;
        loseNum = 0;
        winOrLose = 0;
        reset();
    }
    else
    {
        if( winOrLose==1 )
            winNum++;
        else if( winOrLose==2 )
            loseNum++;
        winOrLose = 0;
        reset();
        play();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if( my_egine==NULL )
    {
        my_egine = new Egine(NULL,tr("1.txt"),'1');
        connect(my_egine,SIGNAL(setPic(int,int)),this,SLOT(setPic(int,int)));
    }

    QPoint p;
    p.setX(QCursor::pos().x() - this->geometry().x());
    p.setY(QCursor::pos().y() - this->geometry().y());

    int id_x,id_y;
    id_x = (p.x()-50)/60;       // !!!!!!!!!!!!!!!!!!!!!!!!  0--->x
    id_y = (p.y()-60)/60;       // !!!!!!!!!!!!!!!!!!!!!!!!  |
                                // !!!!!!!!!!!!!!!!!!!!!!!!  |
                                // !!!!!!!!!!!!!!!!!!!!!!!!  y
    if( id_x<0 || id_x>2 || id_y<0 || id_y>2 )
    {
        //qDebug()<<"out of range!";
        return ;
    }
    if( hasPix[id_x][id_y]==0 )
    {
        setPic( id_x+id_y*3,1 );
        if( my_egine->check(hasPix,my_egine->oppenont) )
        {
            //qDebug()<<"you win";
            reset();
        }
        else
        {
            my_egine->setInput(hasPix);
            if( my_egine->check(hasPix,my_egine->gameSide) )
            {
                //qDebug()<<"you win";
                reset();
            }
        }
    }
}

void MainWindow::reset()
{
    for( int i=0;i<3;i++ )
    {
        for( int j=0;j<3;j++ )
        {
            hasPix[i][j] = 0;
        }
    }
    image0->setPalette(empty);
    image1->setPalette(empty);
    image2->setPalette(empty);
    image3->setPalette(empty);
    image4->setPalette(empty);
    image5->setPalette(empty);
    image6->setPalette(empty);
    image7->setPalette(empty);
    image8->setPalette(empty);
    if( qrand()%2==0 )
        redTurn = true;
    else
        redTurn = false;
}
