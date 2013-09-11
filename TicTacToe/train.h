#ifndef TRAIN_H
#define TRAIN_H

#include <QObject>
#include "egine.h"

class Train : public QObject
{
    Q_OBJECT
public:
    explicit Train(QObject *parent = 0, int gameRound = 5000);
    ~Train();
    void play();

    Egine *egine1,*egine2;
    
private:
    int playRound;
    int gameplayed;
    int gameInfo[3][3];

    void reset();
signals:
    
public slots:
    
};

#endif // TRAIN_H
