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

#define DATASET_SIZE								500
#define DATASET_SUPERVISED_LOT						300
#define DATASET_TESTING_LOT							100

enum Label
{
	IS_GREEN,	//0
	NB_LABELS,	//1
	NOT_GREEN,	//2
	UNLABELLED,	//3
};

enum Features
{
	RED,			//0
	GREEN,			//1
	BLUE,			//2
	NB_FEATURES,	//3
};

enum ActivationFunctions
{
	LINEAR,		//0
	SIGMOID,	//1
	THRESHOLD,	//2
	TAHN,		//3
};

class Data
{
public:
	int m_features[NB_FEATURES];
	Label m_label;

	Data(int red, int green, int blue, Label label)
	{
		m_features[RED] = red;
		m_features[GREEN] = green;
		m_features[BLUE] = blue;

		m_label = label;
	}

	void Copy(Data data)
	{
		for (int f = 0; f < NB_FEATURES; f++)
		{
			m_features[f] = data.m_features[f];
		}
		m_label = data.m_label;
	}
};

class Neuron
{
public:
	Neuron();

	float m_value;
	float m_weight[NEURAL_NETWORK_HEIGHT];
	float m_previous_weight[NEURAL_NETWORK_HEIGHT];
	float m_error;
};

class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();

	Neuron* m_input_layer[NB_FEATURES + 1];
	Neuron* m_output_layer[NB_LABELS];
	Neuron* m_hidden_layers[NEURAL_NETWORK_HEIGHT][NEURAL_NETWORK_DEPTH];
	vector<Data> m_labelled_dataset;
	vector<Data> m_unlabelled_dataset;

	float ActivationFunction(float x);
	void FillWithData();
	void Train();
	void RestorePreviousWeight();
	void DoNothing(){};
	string GetLabelString(Label label)
	{
		if (label == IS_GREEN)
			return "IS_GREEN";
		if (label == NOT_GREEN)
			return "NOT GREEN";
	};
};

#endif // STARFIGHTER_H_INCLUDED