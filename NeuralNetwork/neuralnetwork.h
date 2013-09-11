/*******************************************************************************************************************
 *	NEURAL NETWORK CLASS
 *	----------------------------------------------------------------------------------------------------------------
 *	Classic Back-propagation Neural Network ( makes use of gradient descent )
 *	Can toggle between stochastic and batch learning
 *	----------------------------------------------------------------------------------------------------------------
 *	Author: Bobby Anguelov (banguelov@cs.up.ac.za)
 *	Downloaded From: takinginitiative.wordpress.co.za
 *******************************************************************************************************************/


#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QObject>
#include <map>
#include <fstream>
#include <vector>
#include <math.h>

#define LEARNING_RATE 0.09      // learning rate
#define MOMENTUM 0.90           //
#define MAX_EPOCHS 600000         // max iteractive time
#define DESIRED_ACCURACY 90     // desired accurancy
#define MAXOUTPUT 1             // used to adjust output value

class NeuralNetwork : public QObject
{
    Q_OBJECT

private:
    // learning rate
    double learningRate;		// adjusts the step size of the weight update
    double momentum;            // improves performance of stochastic learning (don't use for batch)

    // number of neurons
    int nInput;
    int nHidden;
    int nOutput;

    // neurons out value
    double *inputNeurons;
    double *hiddenNeurons;
    double *outputNeurons;

    // weights
    double **wInputHidden;
    double **wHiddenOutput;

    // epoch counter
    long epoch;             // particular point in time used as a reference point
    long maxEpochs;

    // accuracy required
    double desiredAccuracy;

    // change to weights
    double **deltaInputHidden;
    double **deltaHiddenOutput;

    // error gradients
    double *hiddenErrorGradients;
    double *outputErrorGradients;

    // accuracy stats per epoch
    double trainingSetAccuracy;
    double validationSetAccuracy;
    double generalizationSetAccuray;
    double generalizationSetMSE;
    double trainingSetMSE;

    // log file handle
    std::vector< std::pair< QString, std::vector<double> > > m_readInput;
    std::map<QString, std::vector<double> > m_output;
    int logResolution;

    // training or play mode
    bool istraining;

public:
    explicit NeuralNetwork(int in, int hidden, int out, QObject *parent = 0);
    ~NeuralNetwork();
    void setLearningParameters(double lr, double m);
    void setMaxEpochs(int max);
    void setDesiredAccuracy(float d);
    void enableLogging(const char* filename);
    void readTrainData(const char* filename);
    void resetWeights();
    double *feedInput(QString s);
    void trainNetwork( );
    void runTrainingEpoch( );

private:
    void loadXML();
    void saveXML();
    void initializeWeights();
    void feedForward(QString inputString);
    void backpropagate(std::vector<double>& desiredValues);
    void updateWeights();
    //activation function
    inline double activationFunction( double x )
    {
        //sigmoid function
        return 2.0*(1.0/(1.0+exp(-x))-0.5);
        //return sin(x);
    }

    //get error gradient for ouput layer
    inline double getOutputErrorGradient(double desiredValue, double outputValue)
    {
        //return error gradient
        return ( (outputValue+1) * ( 1.0 - (outputValue+1) / 2.0 ) * ( desiredValue - outputValue ) );
        //return ( outputValue * (1.0 - outputValue) * ( desiredValue - outputValue ) );
    }
    double getHiddenErrorGradient(std::vector<double> &desiredValues, int j);
    //feed forward set of patterns and return error
    double getSetAccuracy();
    //feed forward set of patterns and return MSE
    double getSetMSE ();

signals:

public slots:

};

#endif // NEURALNETWORK_H
