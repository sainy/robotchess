#include "neuralnetwork.h"
#include <iostream>
#include <QTime>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <map>
#include <QDebug>

using namespace std;

NeuralNetwork::NeuralNetwork(int in, int hidden, int out, QObject *parent):
    QObject(parent),nInput(in), nHidden(hidden), nOutput(out),
    epoch(0), logResolution(10),
    trainingSetAccuracy(0), validationSetAccuracy(0), generalizationSetAccuray(0),
    trainingSetMSE(0), generalizationSetMSE(0),istraining(true)
{
    //create neuron lists
    //--------------------------------------------------------------------------------------------------------
    inputNeurons = new double[in + 1];
    for ( int i=0; i < in; i++ )
        inputNeurons[i] = 0;
    //create bias neuron
    inputNeurons[in] = -1;

    // create hidden neurons
    hiddenNeurons = new double[hidden + 1];
    for ( int i=0; i < hidden; i++ )
        hiddenNeurons[i] = 0;
    //create bias neuron
    hiddenNeurons[hidden] = -1;

    // create output neurons
    outputNeurons = new double[out + 1];
    for( int i=0;i<out;++i )
        outputNeurons[i] = 0;
    // create output bias neuron
    outputNeurons[1] = -1.0;

    //create weight lists (include bias neuron weights)
    //--------------------------------------------------------------------------------------------------------
    wInputHidden = new double*[in + 1];
    for ( int i=0; i <= in; i++ )
    {
        wInputHidden[i] = new double[hidden];
        for ( int j=0; j < hidden; j++ )
            wInputHidden[i][j] = 0;
    }

    wHiddenOutput = new double*[hidden + 1];
    for ( int i=0; i <= hidden; i++ )
    {
        wHiddenOutput[i] = new double[out];
        for( int j=0; j < out; ++j )
            wHiddenOutput[i][j] = 0;
    }

    //create delta lists
    //--------------------------------------------------------------------------------------------------------
    deltaInputHidden = new double*[in + 1];
    for ( int i=0; i <= in; i++ )
    {
        deltaInputHidden[i] = new double[hidden];
        for ( int j=0; j < hidden; j++ )
            deltaInputHidden[i][j] = 0;
    }

    deltaHiddenOutput = new double*[hidden + 1];
    for ( int i=0; i <= hidden; i++ )
    {
        deltaHiddenOutput[i] = new double[out];
        for( int j=0;j<out;++j )
        {
            deltaHiddenOutput[i][j] = 0;
        }
    }

    //create error gradient storage
    //--------------------------------------------------------------------------------------------------------
    hiddenErrorGradients = new double[hidden + 1];
    for ( int i=0; i <= hidden; i++ )
        hiddenErrorGradients[i] = 0;

    // bias i==nOutput
    outputErrorGradients = new double[out + 1];
    for( int i=0; i<=out; ++i )
    {
        outputErrorGradients[i] = 0.0;
    }

    //initialize weights -- random values
    //--------------------------------------------------------------------------------------------------------
    initializeWeights();

    //default learning parameters
    //--------------------------------------------------------------------------------------------------------
    learningRate = LEARNING_RATE;
    momentum = MOMENTUM;

    //stop conditions
    //--------------------------------------------------------------------------------------------------------
    maxEpochs = MAX_EPOCHS;
    desiredAccuracy = DESIRED_ACCURACY;
    m_readInput.clear();
    m_output.clear();
}

NeuralNetwork::~NeuralNetwork()
{
    //delete neurons
    delete[] inputNeurons;
    delete[] hiddenNeurons;
    delete[] outputNeurons;

    //delete weight storage
    for (int i=0; i <= nInput; i++)
        delete[] wInputHidden[i];
    delete[] wInputHidden;

    for (int j=0; j <= nHidden; j++)
        delete[] wHiddenOutput[j];
    delete[] wHiddenOutput;

    //delete delta storage
    for (int i=0; i <= nInput; i++)
        delete[] deltaInputHidden[i];
    delete[] deltaInputHidden;

    for (int j=0; j <= nHidden; j++)
        delete[] deltaHiddenOutput[j];
    delete[] deltaHiddenOutput;

    //delete error gradients
    delete[] hiddenErrorGradients;
    delete[] outputErrorGradients;
}

//set learning parameters
void NeuralNetwork::setLearningParameters(double lr, double m)
{
    learningRate = lr;
    momentum = m;
}

//set max epoch
void NeuralNetwork::setMaxEpochs(int max)
{
    maxEpochs = max;
}

//set desired accuracy
void NeuralNetwork::setDesiredAccuracy(float d)
{
    desiredAccuracy = d;
}

//enable logging of training results
void NeuralNetwork::enableLogging(const char *filename)
{
    //create log file
    QFile out(filename);
    out.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream o(&out);
    map<QString,vector<double> >::iterator p;
    //qDebug()<<"Q's size is "<<Q.size();
    for(p=m_output.begin();p!=m_output.end();p++)
    {
        //qDebug()<<p->first;
        o<<p->first<<" ";
        for( int i=0; i<p->second.size(); ++i )
            o<<(*p).second[i];
        o<<endl;
    }
    out.close();
}

// read train data set
void NeuralNetwork::readTrainData(const char *filename)
{
    QStringList lines;
    QString line;

    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if( file.isOpen() )
    {
        QTextStream stream(&file);
        do //while( !file )
        {
            vector<double> temp;
            line = stream.readLine();
            lines = line.split(" ");
            if( lines.size()== nOutput + 1 )    // pattern plus out number of values
            {
                temp.clear();
                for( int i=0; i< nOutput; ++i )
                    temp.push_back( lines.at(i+1).toDouble() );
                m_readInput.push_back( make_pair(lines.at(0),temp) );
            }
            else if( lines.size()>0 )
            {
                // add warning dialog
                cout<<"file format error!"<<endl;
                break;
            }
        }while( !line.isEmpty());
        file.close();
    }
    else
        std::cout<<"open file error!";

    // initiale out put data size
    m_output.clear();
}

//resets the neural network
void NeuralNetwork::resetWeights()
{
    initializeWeights();
}

//feed data through network
double* NeuralNetwork::feedInput(QString s)
{
    feedForward(s);

    return outputNeurons;
}

//train the network
void NeuralNetwork::trainNetwork()
{
    // read train data
    readTrainData("1.txt");

    //saveXML();
    // reset epoch
    epoch = 0;
    //train network using training dataset for training and generalization dataset for testing
    //--------------------------------------------------------------------------------------------------------
    while( (trainingSetAccuracy < desiredAccuracy || generalizationSetAccuray < desiredAccuracy )
           && epoch < maxEpochs )
    {
        // use training set to train network
        runTrainingEpoch();

        // get generalization set accuracy and MES
        generalizationSetAccuray = getSetAccuracy();
        generalizationSetMSE = getSetMSE();

        // once training set is complete increase epoch
        epoch++;
    }
    cout<<"epoch "<<epoch<<" Accuray "<<trainingSetAccuracy<<" MSE "<<trainingSetMSE<<endl;
}

//initialize weights and weight changes
void NeuralNetwork::initializeWeights()
{
    //init random number generator
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //set weights between input and hidden to a random value between -05 and 0.5
    //--------------------------------------------------------------------------------------------------------
    for(int i = 0; i <= nInput; i++)
    {
        for(int j = 0; j < nHidden; j++)
        {
            //set weights to random values    -0.5 ~ 0.5
            wInputHidden[i][j] = (double)qrand() / (RAND_MAX + 1) - 0.5;

            //create blank delta
            deltaInputHidden[i][j] = 0.0;
        }
    }

    //set weights between input and hidden to a random value between -0.5 and 0.5
    //--------------------------------------------------------------------------------------------------------
    for(int i = 0; i <= nHidden; i++)
    {
        for( int j=0; j<nOutput; ++j )
        {
            //set weights to random values
            wHiddenOutput[i][j] = (double)rand() / (RAND_MAX + 1) - 0.5;

            //create blank delta
            deltaHiddenOutput[i][j] = 0.0;
        }
    }
}

//run training epoches
void NeuralNetwork::runTrainingEpoch()
{
    // check the input
    if( m_readInput.size()==0 )
    {
        std::cout<<"input value is invalidate!";
        return;
    }

    // start training
    double incorrectPatterns = 0;       //incorrect patterns
    double mse = 0;

    for( int i=0; i<m_readInput.size(); ++i )
    {
        feedForward( m_readInput[i].first );
        backpropagate( m_readInput[i].second );

        // pattern correct flag
        bool patternCorrect = true;

        //check all outputs from neural network against desired values
        for ( int k = 0; k < nOutput; k++ )
        {
            //pattern incorrect if desired and output differ
            //cout<<m_readInput[i].second[k]<<" "<<outputNeurons[k]<<" "<<endl;
            if ( (outputNeurons[k]-m_readInput[i].second[k])>0.001
                 || (outputNeurons[k]-m_readInput[i].second[k])<-0.001 )        // principle for judging the correct pattern
                patternCorrect = false;

            //calculate MSE
            mse += pow((outputNeurons[k] - m_readInput[i].second[k]), 2);
        }

        // add the incorrect pattern number
        if( !patternCorrect )
            incorrectPatterns++;
    }

    trainingSetAccuracy = 100 - ( incorrectPatterns / m_readInput.size() * 100 );
    trainingSetMSE = mse / ( nOutput * m_readInput.size() );
}

//feed input forward
void NeuralNetwork::feedForward(QString inputString)
{
    //set input neurons with input values
    for(int i = 0; i < nInput; i++)
    {
        inputNeurons[i] = static_cast<double>(inputString.at(i).digitValue());  //inputString.toDouble();   //
    }

    //Calculate Hidden Layer values - include bias neuron
    //--------------------------------------------------------------------------------------------------------
    for(int j=0; j < nHidden; j++)
    {
        //clear value
        hiddenNeurons[j] = 0;

        //get weighted sum of inputs and bias neuron
        for( int i=0; i <= nInput; i++ )
            hiddenNeurons[j] += inputNeurons[i] * wInputHidden[i][j];

        //set to result of sigmoid
        hiddenNeurons[j] = activationFunction( hiddenNeurons[j] );
    }

    //Calculating Output Layer values - include bias neuron
    //--------------------------------------------------------------------------------------------------------
    std::vector<double> temp;
    temp.clear();
    for( int k=0; k<nOutput; ++k )
    {
        //clear value
        outputNeurons[k] = 0;

        //get weighted sum of inputs and bias neuron
        for( int j=0; j <= nHidden; j++ )
            outputNeurons[k] += hiddenNeurons[j] * wHiddenOutput[j][k];

        //set to result of sigmoid
        //cout<<outputNeurons[k]<<" ";
        outputNeurons[k] = activationFunction( outputNeurons[k] );
        //cout<<outputNeurons[k]<<" ";
        temp.push_back( outputNeurons[k] );
    }
    //cout<<endl;
    // set output data
    m_output[inputString].swap(temp);
}

//modify weights according to ouput
void NeuralNetwork::backpropagate(vector<double> &desiredValues)
{
    //modify deltas between hidden and output layers
    //--------------------------------------------------------------------------------------------------------
    for (int k = 0; k < nOutput; k++)
    {
        //get error gradient for every output node
        outputErrorGradients[k] = getOutputErrorGradient( desiredValues[k], outputNeurons[k] );

        //for all nodes in hidden layer and bias neuron
        for (int j = 0; j <= nHidden; j++)
        {
            //calculate change in weight
            //cout<<deltaHiddenOutput[j][k]<<" ";
            deltaHiddenOutput[j][k] = learningRate * hiddenNeurons[j] * outputErrorGradients[k]
                    + momentum * deltaHiddenOutput[j][k];
            //cout<<deltaHiddenOutput[j][k]<<" "<<endl;
        }
    }

    //modify deltas between input and hidden layers
    //--------------------------------------------------------------------------------------------------------
    for (int j = 0; j < nHidden; j++)
    {
        //get error gradient for every hidden node
        hiddenErrorGradients[j] = getHiddenErrorGradient( desiredValues, j );

        //for all nodes in input layer and bias neuron
        for (int i = 0; i <= nInput; i++)
        {
            //calculate change in weight
            deltaInputHidden[i][j] = learningRate * inputNeurons[i] * hiddenErrorGradients[j]
                    + momentum * deltaInputHidden[i][j];
        }
    }

    updateWeights();
}

// update weights
void NeuralNetwork::updateWeights()
{
    //input -> hidden weights
    //--------------------------------------------------------------------------------------------------------
    for (int i = 0; i <= nInput; i++)
    {
        for (int j = 0; j < nHidden; j++)
        {
            //update weight
            //cout<<deltaInputHidden[i][j]<<" ";
            wInputHidden[i][j] += deltaInputHidden[i][j];
            //cout<<wInputHidden[i][j]<<" ";
        }
    }

    //hidden -> output weights
    //--------------------------------------------------------------------------------------------------------
    for (int j = 0; j <= nHidden; j++)
    {
        for (int k = 0; k < nOutput; ++k )
        {
            //update weight
            //cout<<deltaHiddenOutput[j][k]<<" ";
            wHiddenOutput[j][k] += deltaHiddenOutput[j][k];
            //cout<<wHiddenOutput[j][k]<<" ";
        }
    }
}

//get error gradient for hidden layer
double NeuralNetwork::getHiddenErrorGradient( vector<double> &desiredValues, int j )
{
    //get sum of hidden->output weights * output error gradients
    double weightedSum = 0;
    for( int k=0; k<nOutput; ++k )
    {
        weightedSum += wHiddenOutput[j][k] *(desiredValues[k]-outputNeurons[k]);   // outputErrorGradients[k];      //
    }

    //return error gradient
    return ( (hiddenNeurons[j]+1) * ( 1 - (hiddenNeurons[j]+1) / 2.0 ) * weightedSum );
}

//feed forward set of patterns and return error
double NeuralNetwork::getSetAccuracy()
{
    return trainingSetAccuracy;
}

//feed forward set of patterns and return MSE
double NeuralNetwork::getSetMSE ()
{
    return trainingSetMSE;
}
