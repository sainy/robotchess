#include "train.h"

Train::Train(QObject *parent, int gameRound) :
    QObject(parent)
{
    playRound = gameRound;
    gameplayed = 0;
    egine1 = new Egine(NULL,"train1.txt",'1');
    egine2 = new Egine(NULL,"train2.txt",'2');


    play();
}

Train::~Train()
{
    if( egine1 )
        delete egine1;
    if( egine2 )
        delete egine2;
}

void Train::play()
{
    if( gameplayed<playRound)
    {
        gameplayed++;
        while( egine1->placeAvailable( egine1->arrayToQString(gameInfo) ) )
        {

        }
        reset();
        play();
    }
}

void Train::reset()
{
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
            gameInfo[i][j] = 0;
    }
}
