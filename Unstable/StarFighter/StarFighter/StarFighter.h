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
#include <iostream>		/* cin */

#include "Globals.h"

#define NEURAL_NETWORK_ERROR_MARGIN					0.02
#define NEURAL_NETWORK_LEARNING_RATE				0.05
#define NEURAL_NETWORK_MOMENTUM						0.5
#define NEURAL_NETWORK_MAX_ATTEMPTS					1000

#define DATASET_SIZE									300
#define DATASET_SUPERVISED_LOT						200
#define DATASET_TESTING_LOT							(DATASET_SIZE - DATASET_SUPERVISED_LOT)

#define PRINT_ALL				false
#define PRINT_FF					false//feed forward
#define PRINT_EC					true//erorr caculation
#define PRINT_BP					false//gradient back propagation
#define PRINT_WU					false//weights update

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
	TANSIG,		//4
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

struct Performance
{
public:
	double m_average_error;
	double m_learning_rate;
	double m_momentum;
	FunctionType m_function;
	int m_overall_attempts;
	int m_loops;
	vector<int> m_hidden_layers;
};

class Neuron
{
public:
	double m_value;
	double m_input_value;
	vector <double> m_weights;
	vector <double> m_deltaWeights;
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
	double GetTargetValue(const Data &data);
	vector<Layer> m_layers;
	int m_nb_layers;
	vector<Data> m_dataset;

	void Run();
	void Training();
	void Testing();
	void Creating();
	Label TestSample(Data &data);

	void InitInputLayer(const Data &data);
	void FeedForward();
	void ErrorCalculation(const Data &data);
	void GradientBackPropagation(const Data &data);
	void WeightsUpdate();

	double m_error;
	double m_average_error;

	double m_learning_rate;
	double m_momentum;
	FunctionType m_function;
	int m_attempts;
	int m_overall_attempts;
	int m_success;
	double m_success_rate;
	int m_loops;

	vector<Performance> m_perf_records;
	bool RecordPerf();

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