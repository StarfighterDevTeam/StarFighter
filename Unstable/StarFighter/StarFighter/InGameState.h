#ifndef METAGAME_H_INCLUDED
#define METAGAME_H_INCLUDED

#include "GameState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Game.h"
#include "Ship.h"
#include "SFPanelSpecific.h"
#include "FileLoadUtils.h"

class GameObject;

#define NEURAL_NETWORK_DEPTH				2
#define NEURAL_NETWORK_HEIGHT				4
#define NEURAL_NETWORK_ERROR_MARGIN			0.02
#define NEURAL_NETWORK_LEARNING_RATE		0.5

#define DATASET_SIZE						100
#define DATASET_SUPERVISED_LOT				75

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
	GREEN,		//1
	BLUE,		//2
	NB_FEATURES,	//3
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
	bool CorrectWeight(int index);

	float m_value;
	float m_weight[NEURAL_NETWORK_HEIGHT];
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
	void DoNothing(){};
	string GetLabelString(Label label)
	{
		if (label == IS_GREEN)
			return "IS_GREEN";
		if (label == NOT_GREEN)
			return "NOT GREEN";
	};
};

class InGameState : public GameState
{
public:
	GameObject* playerShip;

	void Initialize(Player player);
	void Update(sf::Time deltaTime);
	void Draw();
	void Release();
	void UpdateCamera(sf::Time deltaTime);
	static void CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip);
	static void DestroySFPanel(Ship* playerShip);
	static void LoadCSVFile(string scenes_file);

	NeuralNetwork m_NeuralNetwork;

private:
	sf::RenderWindow* mainWindow;
	sf::Text* framerate;
};

#endif //METAGAME_H_INCLUDED