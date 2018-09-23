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

#define NEURAL_NETWORK_DEPTH		4
#define NEURAL_NETWORK_HEIGHT		4
#define NEURAL_NETWORK_FEATURES		4

enum Label
{
	GREEN,	//0
	RED,	//1
	BLUE,	//2
	OTHER,	//3
};

class Data
{
public:
	int* m_red;
	int* m_green;
	int* m_blue;
	int* m_alpha;

	int* m_features[NEURAL_NETWORK_FEATURES];

	Label* m_label;//"green"

	Data()
	{
		m_red = new int(); 
		m_red = 0;
		m_green = new int();
		m_green = 0;
		m_blue = new int();
		m_blue = 0;
		m_alpha = new int();
		m_alpha = 0;

		m_label = NULL;

		for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
		{
			m_features[f] = new int();
		}
	}

	Data(int* red, int* green, int* blue, int* alpha, Label* label)
	{
		m_red = new int();
		m_red = red;
		
		m_green = new int();
		m_green = green;
		
		m_blue = new int();
		m_blue = blue;
		
		m_alpha = new int();
		m_alpha = alpha;
		
		m_label = new Label();
		m_label = label;

		for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
		{
			m_features[f] = new int();
		}
	}

	void Copy(Data data)
	{
		m_red = data.m_red;
		m_green = data.m_green;
		m_blue = data.m_blue;
		m_alpha = data.m_alpha;
		m_label = data.m_label;
	}

	void GetFeatures()
	{
		m_features[0] = m_red;
		m_features[1] = m_green;
		m_features[2] = m_blue;
		m_features[3] = m_alpha;
	}
};

class Neuron
{
public:
	Neuron();

	Data m_data;
	float m_weight[NEURAL_NETWORK_HEIGHT];
};

class NeuralNetwork
{
public:

	NeuralNetwork();
	~NeuralNetwork();

	Neuron* m_input_layer[NEURAL_NETWORK_FEATURES];
	Neuron* m_output_layer[NEURAL_NETWORK_FEATURES];
	Neuron* m_network[NEURAL_NETWORK_HEIGHT][NEURAL_NETWORK_DEPTH];
	vector<Data> m_labelled_dataset;
	vector<Data> m_unlabelled_dataset;

	bool AddData(Data data);
	bool Train();
	void DoNothing(){};
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