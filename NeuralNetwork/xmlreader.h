#ifndef XMLREADER_H
#define XMLREADER_H

#include <QObject>

class XMLReader : public QObject
{
    Q_OBJECT
public:
    explicit XMLReader(QObject *parent = 0);
    ~XMLReader();
    void setNeuronsNum(int input, int hidden, int output);
    void setWeigths(double **InputHidden, double **HiddenOutput);
    void loadXML();
    void saveXML();

signals:
    
public slots:
    
public:
    int nInput;
    int nHidden;
    int nOutput;
    double **wInputHidden;
    double **wHiddenOutput;

};

#endif // XMLREADER_H
