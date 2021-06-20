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

#define NN_LEARNING_RATE				0.1
#define NN_MOMENTUM						0.0
#define NN_ACTIVATION_FUNCTION			SIGMOID

#define NN_ERROR_MARGIN					0.02f
#define NN_MAX_ATTEMPTS					500
#define NN_MAX_OVERALL_ATTEMPTS			200000

#define DATASET_SIZE					500
#define DATASET_SUPERVISED_LOT			200
#define DATASET_TESTING_LOT				(DATASET_SIZE - DATASET_SUPERVISED_LOT)

#define PRINT_ALL						false
#define PRINT_FF						false//feed forward
#define PRINT_EC						false//erorr caculation
#define PRINT_BP						false//gradient back propagation
#define PRINT_WU						false//weights update
#define PRINT_FB						false//feed backward
#define PRINT_TR						false//training
#define PRINT_TE						false//testing
#define PRINT_LO						true//result of the loop

#define DATASET_FILE					"Saves/DataSet.txt"//"Saves/DataSetYellow.txt"
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
	IdleMode,				//0: wait input
	CreateDataSet,			//1: run one time with given hyperparameters and default weights
	RestoreRandomWeights,	//2: run in loop with default weights, tuning hyperparameters every time
	Learn,					//3
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
	~NeuralNetwork();

	void AddLayer(int nb_neuron, LayerType type);
	double GetTargetValue(const Label label);
	vector<Layer*> m_layers;
	int m_nb_layers;
	vector<Data> m_dataset;
	struct tm timer;
	Data CreateDataWithManualInputs();
	string DecodingData(Data& data);

	void Run(NeuralNetworkMode mode);
	void Training();
	void Testing();
	void Creating();
	Label TestSample(Data &data);
	bool DoNothing(){ return true; };
	void Display();

	void InitInputLayer(const Data &data);
	void FeedForward();
	float ErrorCalculation(const Data &data);
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
	bool m_use_bias;

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
	void BalanceDataset();

	//Save and Load files
	bool SaveDatasetIntoFile();
	bool LoadDatasetFromFile();
	bool SaveStartingWeightsIntoFile();
	bool LoadWeightsFromFile(string filename);
	bool LoadHyperParametersFromFile(string filename);
	bool SavePerfIntoFile();
	bool SaveBestPerfIntoFile();
	bool IsBetterPerfThanSaveFile(Performance &perf);

	int m_weightLoadIndex;

	static string GetLabelString(Label label)
	{
		if (label == IS_ROMAIN)
			return "IS_ROMAIN";
		if (label == IS_NOT_ROMAIN)
			return "IS NOT ROMAIN";
		if (label == UNLABELLED)
			return "UNLABELLED";
	};

	//display
	sf::RenderWindow m_renderWindow;
	void AdjustNeuronDisplayPositions();

	sf::Clock m_display_timer;
	sf::Text m_general_info_text;
	int m_current_data_index;
};

#endif // STARFIGHTER_H_INCLUDED