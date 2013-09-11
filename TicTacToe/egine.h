#ifndef EGINE_H
#define EGINE_H

#include <QObject>
#include <map>

using namespace std;

class Egine : public QObject
{
    Q_OBJECT
public:
    explicit Egine(QObject *parent, QString filen, char side='1');
    ~Egine();

    void setInput(const int (*a)[3]);
    pair<int, double> generateMove(const QString &str, const int (*a)[3]);
    bool check(const QString &str, char side);
    bool check(const int(*a)[3],char side);

    void updateQValue(const QString &str, const pair<int, double> max);

    QString arrayToQString(const int (*a)[3]);
    int placeAvailable(const QString &str);
    char gameSide;
    char oppenont;

signals:
    void setPic(int id,int placeNum);

public slots:
    
private:

    std::map<QString,double> Q;
    const double alpha;    // ѧϰ�ٶ�
    const double gamma;    // �ۿ���
    double reward;

    QString lastmove;
    QString filename;

};

#endif // EGINE_H
