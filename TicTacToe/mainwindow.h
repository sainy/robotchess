#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "egine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void setPic(int id,int placeNum);
    void play();

private:
    Ui::MainWindow *ui;
    //void keyPressEvent(QKeyEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);

    void reset();

    QWidget *image0;
    QWidget *image1;
    QWidget *image2;
    QWidget *image3;
    QWidget *image4;
    QWidget *image5;
    QWidget *image6;
    QWidget *image7;
    QWidget *image8;

    QPalette palette1;
    QPalette palette2;
    QPalette empty;

    bool redTurn;
    int hasPix[3][3];
    int round;

    Egine *my_egine;
    Egine *opponent_egine;

    int winNum;
    int loseNum;
    int  winOrLose;     // 0 draw 1 win 2 lose
};

#endif // MAINWINDOW_H
