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
#include <fstream>		/* ofstream */
#include <sstream>		/*istringstream */

#include "Globals.h"

#define NN_LEARNING_RATE				1.0
#define NN_MOMENTUM						0.5
#define NN_ACTIVATION_FUNCTION			TANH

#define NN_ERROR_MARGIN					0.02
#define NN_MAX_ATTEMPTS					500
#define NN_MAX_OVERALL_ATTEMPTS			100000

#define DATASET_SIZE					300
#define DATASET_SUPERVISED_LOT			200
#define DATASET_TESTING_LOT				(DATASET_SIZE - DATASET_SUPERVISED_LOT)

#define PRINT_ALL						false
#define PRINT_FF						false//feed forward
#define PRINT_EC						true//erorr caculation
#define PRINT_BP						false//gradient back propagation
#define PRINT_WU						false//weights update
#define PRINT_TR						true//training
#define PRINT_TE						true//testing

#define DATASET_FILE					"Saves/DataSet.txt"
#define RANDOM_WEIGHTS_FILE				"Saves/RandomWeights.txt"
#define PERF_RECORDS_FILE				"Saves/PerfRecords.csv"
#define PERF_BEST_FILE					"Saves/PerfBest.csv"
#define PERF_BEST_WEIGHTS_FILE			"Saves/PerfBestWeights.txt"

enum Label
{
	IS_GREEN,		//0
	NB_LABELS,		//1
	NOT_GREEN,		//2
	UNLABELLED,		//3
};

enum Features
{
	RED,			//0
	GREEN,			//1
	BLUE,			//2
	NB_FEATURES,	//3
};

enum FunctionType
{
	LINEAR,			//0
	SIGMOID,		//1
	THRESHOLD,		//2
	TANSIG,			//3
	TANH,			//4
	RELU,			//5
	LEAKY_RELU,		//6
	NB_FUNCTIONS,	//7
};

enum LayerType
{
	InputLayer,		//0
	HiddenLayer,	//1
	OutpuLayer,		//2
};

enum NeuralNetworkMode
{
	PerfFromScratch,		//0: run one time with given hyperparameters and default weights
	LearnHyperparameters,	//1: run in loop with default weights, tuning hyperparameters every time
	ImproveWeights,			//2: load best-known weights and hyperparameters and iterate to improve weights
	Prod,					//3: load best-known weights and hyperparameters and get ready to produce results
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
	vector<double> m_weights;
	double m_success_rate;
	int m_index;
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

	void Run(NeuralNetworkMode mode);
	void Training();
	void Testing();
	void Creating();
	Label TestSample(Data &data);
	bool DoNothing(){ return true; };

	void InitInputLayer(const Data &data);
	void FeedForward();
	void ErrorCalculation(const Data &data);
	void BackPropagationGradient(const Data &data);
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

	//Record perfs
	vector<Performance> m_perf_records;
	bool RecordPerf();
	bool UpdateBestPerf();
	vector<double> m_weightsStart;
	void RestoreWeights(vector<double>& weights);
	void CopyWeightsInto(vector<double>& weights);
	void LoadBestWeights();
	Performance m_best_perf;

	double TransferFunction(double x, FunctionType function);
	double TransferFunctionDerivative(double x, FunctionType function);

	//Dataset
	void CreateDataset();

	//Save and Load files
	bool SaveDatasetIntoFile();
	bool LoadDatasetFromFile();
	bool SaveWeightsIntoFile();
	bool LoadWeightsFromFile(string filename);
	bool LoadHyperParametersFromFile(string filename);
	bool SavePerfIntoFile();
	bool SaveBestPerfIntoFile();

	int m_weightLoadIndex;

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