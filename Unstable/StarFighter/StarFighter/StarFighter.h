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
#include <ctime>			/* clock */

#include "Globals.h"
#include "Neuron.h"

#define NN_LEARNING_RATE					1.0//0.7
#define NN_MOMENTUM						0.5//0.6
#define NN_ACTIVATION_FUNCTION			TANH

#define NN_ERROR_MARGIN					0.02f
#define NN_MAX_ATTEMPTS					500
#define NN_MAX_OVERALL_ATTEMPTS			150000

#define DATASET_SIZE						100//300
#define DATASET_SUPERVISED_LOT			180//200
#define DATASET_TESTING_LOT				(DATASET_SIZE - DATASET_SUPERVISED_LOT)

#define PRINT_ALL					false
#define PRINT_FF						false//feed forward
#define PRINT_EC						true//erorr caculation
#define PRINT_BP						false//gradient back propagation
#define PRINT_WU						true//weights update
#define PRINT_FB						false//feed backward
#define PRINT_TR						true//training
#define PRINT_TE						true//testing
#define PRINT_LO						true//result of the loop

#define DATASET_FILE						"Saves/DataSetYellow_source.txt"//"Saves/DataSet.txt"//
#define RANDOM_WEIGHTS_FILE				"Saves/RandomWeights.txt"
#define PERF_RECORDS_FILE				"Saves/PerfRecords.csv"
#define PERF_BEST_FILE					"Saves/PerfBest.txt"
#define PERF_BEST_WEIGHTS_FILE			"Saves/PerfBestWeights.txt"

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

enum NeuralNetworkMode
{
	PerfFromScratch,			//0: run one time with given hyperparameters and default weights
	LearnHyperparameters,	//1: run in loop with default weights, tuning hyperparameters every time
	ImproveWeights,			//2: load best-known weights and hyperparameters and iterate to improve weights
	ManualInputs,			//3: load best-known weights and hyperparameters and get ready to produce results
	ListDataset,			//4: scroll one time through all unique examples of the dataset
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

class NeuralNetwork
{
public:
	NeuralNetwork();

	void AddLayer(int nb_neuron, LayerType type);
	double GetTargetValue(const Label label);
	vector<Layer> m_layers;
	int m_nb_layers;
	vector<Data> m_dataset;
	vector<Data> m_training_dataset;
	struct tm timer;
	Data CreateDataWithManualInputs();

	void Run(NeuralNetworkMode mode);
	void Training();
	void Testing();
	Label TestSample(Data &data);
	bool DoNothing(){ return true; };

	void InitInputLayer(const Data &data);
	void FeedForward();
	void ErrorCalculation(const Data &data);
	void BackPropagationGradient(const Data &data);
	void WeightsUpdate();
	void FeedBackward(Label label);

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
	void RestoreWeights();
	void CopyWeightsInto(vector<double>& weights);
	Performance m_best_perf;

	double TransferFunction(double x, FunctionType function);
	double TransferFunctionDerivative(double x, FunctionType function);
	double TransferFunctionInverse(double x, FunctionType function);

	//Dataset
	void CreateDataset();
	void MixDataSet();

	//Save and Load files
	bool SaveDatasetIntoFile();
	bool LoadDatasetFromFile();
	bool SaveStartingWeightsIntoFile();
	bool LoadWeightsFromFile(string filename);
	bool LoadHyperParametersFromFile(string filename);
	bool SavePerfIntoFile();
	bool SaveBestPerfIntoFile();
	bool IsBetterPerfThanSaveFile(Performance &perf);
	int m_datasetSize;
	int m_weightLoadIndex;

	static string GetLabelString(Label label)
	{
		if (label == IS_YELLOW)
			return "YELLOW";
		if (label == NB_LABELS)
			return "NOT YELLOW";
		if (label == NOT_GREEN)
			return "NOT GREEN";
		if (label == UNLABELLED)
			return "UNLABELLED";
	};
};

#endif // STARFIGHTER_H_INCLUDED