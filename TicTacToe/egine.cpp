#include "egine.h"
#include <iostream>
#include <fstream>
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QTime>
#include <QFile>

Egine::Egine(QObject *parent, QString filen, char side) :
    QObject(parent),alpha(0.05),gamma(0.99),
    filename(filen),gameSide(side)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    Q.clear();

    oppenont = (  gameSide=='1' ) ? '2' : '1';

    QStringList lines;
    QString line;

    QFile file(filename.toStdString().c_str());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&file);
    while(!file.atEnd())
    {
        line = stream.readLine();
        lines = line.split(" ");
        if( lines.size()==2 )
            Q[ lines.at(0)] = lines.at(1).toDouble();
    }
    file.close();
}

Egine::~Egine()
{
    QFile out(filename.toStdString().c_str());
    out.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream o(&out);
    std::map<QString,double>::iterator p;
    //qDebug()<<"Q's size is "<<Q.size();
    for(p=Q.begin();p!=Q.end();p++)
    {
        //qDebug()<<p->first;
        o<<p->first<<" "<<(*p).second<<endl;
    }
    qDebug()<<"write file done";
    out.close();
}

pair<int, double> Egine::generateMove(const QString& str,const int (*a)[3])
{
    pair<int ,double> max(-1,0.0);

    // find empty place
    vector<pair<int,double> > list;
    for(int i=0;i<str.size();i++)
    {
        if( str.at(i)=='0' )
        {
            QString s = str;
            s.replace(i,1,gameSide);
            list.push_back( make_pair(i,Q[s]) );
        }
    }

    // no room to place a pic
    if( list.size()==0 )
    {
        ;
    }
    else if( list.size()==1 )   // only one place to place pic
    {
        max = list.at(0);
    }
    else if( qrand()%10==1 )        // random value
    {
        max = list[ qrand()%(list.size()) ];
        QString s1 = str;
        s1.replace(max.first,1,gameSide);
        bool trigger = false;
        for(int i=0;i<s1.size();i++)
        {
            if( s1.at(i)=='0' )
            {
                QString s2 = s1;
                s2.replace(i,1,oppenont);
                if( trigger || max.second<Q[s2] )
                {
                    max.second = Q[s2];
                }
            }
        }
    }
    else
    {
        // find to max!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        max.first = -1;
        pair<int ,double> max_of_loop(-1,0.0);
        for(int i=0;i<str.size();i++)       // place a black chess
        {
            if( str.at(i)=='0')
            {
                QString s1 = str;
                s1.replace(i,1,gameSide);
                max_of_loop.first = -1;
                for(int j=0;j<str.size();j++)   // play a red chess
                {
                    if( s1.at(j)=='0' )
                    {
                        QString s2 = s1;
                        s2.replace(j,1,oppenont);
                        if(max_of_loop.first==-1 || max_of_loop.second>Q[s2])
                        {
                            max_of_loop.first = i;
                            max_of_loop.second = Q[s2];
                        }
                    }
                }   // end for
                if( max.first==-1 || max.second<max_of_loop.second)       // choose the best of the worst
                {
                    max = max_of_loop;
                }
            }
        }
    }

    return max;
}

// check is gameside win
bool Egine::check(const QString& str,char side)
{
    if( side=='0' )       // illegal input
        return false;

    reward = (gameSide==side) ? 1.0:-1.0;

    for(int i=0;i<3;i++)
    {
        if( str.at(i*3)==side && (str.at(i*3)==str.at(i*3+1)) && (str.at(i*3)==str.at(i*3+2)) )  // ÁÐ
        {
            Q[str] = reward;
            return true;
        }
        else if(str.at(i)==side && (str.at(i)==str.at(i+3)) && (str.at(i)==str.at(i+6)) )  // ÐÐ
        {
            Q[str] = reward;
            return true;
        }
    }
    if( str.at(0)==side && (str.at(0)==str.at(4))&&(str.at(0)==str.at(8)) )
    {
        Q[str] = reward;
        return true;
    }
    else if( str.at(2)==side && (str.at(2)==str.at(4))&&(str.at(2)==str.at(6)) )
    {
        Q[str] = reward;
        return true;
    }
    return false;
}

bool Egine::check(const int (*a)[3], char side)
{
    QString str = arrayToQString(a);
    return check(str,side);
}

void Egine::setInput(const int (*a)[3])
{
    QString str = arrayToQString(a);
    //qDebug()<<"str"<<str;

    // judge if it is the terminate state
//    if( Q.count(str)==0 )
//        Q[str]=0.0;
    if( check(str, oppenont) )      // Êä
    {
        Q[str] = -1.0;
        return ;
    }
    else if( check(str,gameSide) )
    {
        Q[str] = 1.0;
        return;
    }
    else if( placeAvailable(str)==0 )
    {
        setPic(-1,0);
        Q[str] = -0.01;
        return ;
    }

    // generate the next move
    pair<int,double> id;
//    do
//    {
        id = generateMove(str,a);
//        if( str.at(id)!='0' )
//            qDebug()<<"error! this should not happen!";
//    }while(a[id%3][id/3]!=0);

    // update Q value
    if( id.first!=-1 )
        updateQValue(str,id);

    // show the pic
    emit setPic(id.first,1);
}

int Egine::placeAvailable(const QString& str)
{
    int num = 0;
    for( int i=0;i<str.size();i++ )
    {
        if( str.at(i)=='0' )
            num++;
    }
    return num;
}

QString Egine::arrayToQString(const int (*a)[3])
{
    QString str;
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            str += QString::number(a[j][i]);
        }
    }
    return str;
}


void Egine::updateQValue(const QString &str, const pair<int,double> max)
{
    lastmove = str;
    lastmove.replace(max.first,1,gameSide);
    if( check(lastmove,gameSide) )
    {
        Q[lastmove] = 1.0;
        reward = 1.0;
    }
    else
        reward = -0.01;

    Q[str] = (1.0-alpha) * Q[str] + alpha * (reward + gamma*max.second);
}
