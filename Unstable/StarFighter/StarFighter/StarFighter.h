#ifndef STARFIGHTER_H_INCLUDED
#define STARFIGHTER_H_INCLUDED

using namespace std;

#define _USE_MATH_DEFINES
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */

#include "Globals.h"

#define NEURAL_NETWORK_DEPTH						2
#define NEURAL_NETWORK_HEIGHT						4
#define NEURAL_NETWORK_ERROR_MARGIN					0.002
#define NEURAL_NETWORK_LEARNING_RATE				0.01
#define NEURAL_NETWORK_MOMENTUM						0.01

#define DATASET_SIZE									500
#define DATASET_SUPERVISED_LOT						300

enum Label
{
	IS_GREEN,	//0
	NB_LABELS,	//1
	NOT_GREEN,	//2
	UNLABELLED,	//3
};

enum Features
{
	RED,				//0
	GREEN,			//1
	BLUE,			//2
	NB_FEATURES,		//3
};

enum FunctionType
{
	LINEAR,		//0
	SIGMOID,		//1
	THRESHOLD,	//2
	TANH,		//3
};

enum LayerType
{
	InputLayer,		//0
	HiddenLayer,		//1
	OutpuLayer,		//2
};

class Data
{
public:
	Data(vector<double> features, Label label);
	Data(Label label);

	vector<double> m_features;
	Label m_label;
};

class Neuron
{
public:
	double m_value;
	double m_input_value;
	vector <double> m_weights;
	vector <double> m_previous_weights;
	double m_error;
	double m_gradient;

	static float RandomizeWeight();
};

class Layer
{
public:
	Layer(int nb_neuron, LayerType type);

	LayerType m_type;
	int m_nb_neurons;
	vector<Neuron> m_neurons;
};

class NeuralNetwork
{
public:
	NeuralNetwork();

	void AddLayer(int nb_neuron, LayerType type);
	vector<Layer> m_layers;
	int m_nb_layers;
	vector<Data> m_dataset;

	void Training();

	void InitInputLayer(const Data &data);
	void FeedForward();
	void ErrorCalculation(const Data &data);
	void BackPropagation();

	double m_error;//error
	double m_learning_rate;
	double m_momentum;
	FunctionType m_function;

	double TransferFunction(double x, FunctionType function);
	double TransferFunctionDerivative(double x, FunctionType function);
	void CreateDataset();
	//void Train();
	void RestorePreviousWeight();
	void DoNothing(){};
	static string GetLabelString(Label label)
	{
		if (label == IS_GREEN)
			return "IS_GREEN";
		if (label == NOT_GREEN)
			return "NOT GREEN";
		if (label == UNLABELLED)
			return "UNLABELLED";
	};
};

#endif // STARFIGHTER_H_INCLUDED