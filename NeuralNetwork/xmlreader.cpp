#include "xmlreader.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>

XMLReader::XMLReader(QObject *parent) :
    QObject(parent)
{
    wInputHidden = NULL;
    wHiddenOutput = NULL;
}

XMLReader::~XMLReader()
{

}

void XMLReader::loadXML()
{
    QFile f("weight.xml");
    if( !f.open(QFile::ReadOnly | QFile::Text) )
    {
        // open failed
        return;
    }
    QXmlStreamReader xsr;
    xsr.setDevice( &f );

    // read version
    while ( !xsr.atEnd() )
    {
        xsr.readNext();
        if( xsr.isStartElement() )
        {
            if( xsr.name() == "LAYER" )
            {
                while( !xsr.atEnd() )
                {
                    xsr.readNext();
                    if( xsr.name() == "InputLayerNum" )
                        nInput = xsr.readElementText().toInt();
                    else if( xsr.name() == "HiddenLayerNum" )
                        nHidden = xsr.readElementText().toInt();
                    else if( xsr.name() == "HiddenLayerNum" )
                        nOutput = xsr.readElementText().toInt();
                    else
                        break;
                }
                //create weight lists (include bias neuron weights)
                //--------------------------------------------------------------------------------------------------------
                wInputHidden = new double*[nInput + 1];
                for ( int i=0; i <= nInput; i++ )
                {
                    wInputHidden[i] = new double[nHidden];
                    for ( int j=0; j < nHidden; j++ )
                        wInputHidden[i][j] = 0;
                }

                wHiddenOutput = new double*[nHidden + 1];
                for ( int i=0; i <= nHidden; i++ )
                {
                    wHiddenOutput[i] = new double[nOutput];
                    for( int j=0; j < nOutput; ++j )
                        wHiddenOutput[i][j] = 0;
                }
            }
            else
                return;
        }
    }

    // read neuron weights
    while ( !xsr.atEnd() ){
        xsr.readNext();

        if( xsr.isStartElement() && xsr.name() == "INPUT" )
        {
            xsr.readNext();
            while( !xsr.atEnd() && xsr.name()=="InputHidden" )
            {
                // add input neuron weight to variable
                //v = xsr.readElementText().toDouble();
            }
        }
        else if( xsr.isStartElement() && xsr.name() == "HIDDEN")
        {
            xsr.readNext();
            while( !xsr.atEnd() && xsr.name()=="HiddenOutput" )
            {
                // add input neuron weight to variable
                //v = xsr.readElementText().toDouble();
            }
        }
        else if( xsr.isEndElement() )
            continue;
        else
            break;
    }
}

void XMLReader::saveXML()
{
    if( wInputHidden==NULL || wHiddenOutput==NULL )
        return;

    QFile f("weight.xml");
    if( !f.open(QFile::WriteOnly | QFile::Text) )
    {
        // open failed
        return;
    }

    QXmlStreamWriter xsw;
    xsw.setDevice( &f );
    xsw.setAutoFormatting(true);

    xsw.writeStartDocument();
    xsw.writeDTD("<!DOCTYPE ANNWeights>");
    xsw.writeStartElement("Profile");
    xsw.writeAttribute("version","1.0");

    // ----------------------- Layer number ------------------------
    xsw.writeStartElement("LAYER");
    xsw.writeTextElement( "InputLayerNum", QString::number(nInput) );
    xsw.writeTextElement( "HiddenLayerNum", QString::number(nHidden) );
    xsw.writeTextElement( "OutputLayerNum", QString::number(nOutput) );
    xsw.writeEndElement();      // end layer

    // -----------INPUT------------------------------------------
    xsw.writeStartElement("INPUT");

    // write input-hidden weights
    for ( int i=0; i<nInput; ++i )
    {
        for ( int j=0; j < nHidden; ++j )
            xsw.writeTextElement( "InputHidden", QString::number( wInputHidden[i][j]) );
    }
    xsw.writeEndElement();      // end input-hidden

    // -----------HIDDEN------------------------------------------
    xsw.writeStartElement("HIDDEN");

    // write hidden-output weights
    for ( int i=0; i<nHidden; ++i )
    {
        for ( int j=0; j < nOutput; ++j )
            xsw.writeTextElement( "HiddenOutput", QString::number( wHiddenOutput[i][j]) );
    }
    xsw.writeEndElement();      // end hidden-output

    xsw.writeEndElement();      // end profile
    xsw.writeEndDocument();     // end document
}

void XMLReader::setNeuronsNum(int input, int hidden, int output)
{
    nInput = input;
    nHidden = hidden;
    nOutput = output;
}

void XMLReader::setWeigths(double **InputHidden, double **HiddenOutput)
{
    wInputHidden = InputHidden;
    wHiddenOutput = HiddenOutput;
}
