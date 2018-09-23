#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//Loading scripts
	//LoadCSVFile(SHIP_CSV_FILE);


	//Input labelled data
	int* red = new int();
	*red = 0;
	int* green = new int();
	*green = 255;
	int* blue = new int();
	*blue = 0;
	int* alpha = new int();
	*alpha = 0;
	Label* label = new Label();
	*label = GREEN;

	Data labelled_data(red, green, blue, alpha, label);
	m_NeuralNetwork.AddData(labelled_data);

	int* red2 = new int();
	*red2 = 0;
	int* green2 = new int();
	*green2 = 120;
	int* blue2 = new int();
	*blue2 = 0;
	int* alpha2 = new int();
	*alpha2 = 0;

	Data unlabelled_data(red2, green2, blue2, alpha2, NULL);
	m_NeuralNetwork.AddData(unlabelled_data);

	//Train on labelled data
	printf("\nTraining neural network\n");
	m_NeuralNetwork.Train();
	
	//Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	//(*CurrentGame).m_playerShip = playerShip;
	//(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);
	//
	////Load saved file
	//if (!Ship::LoadShip(playerShip))
	//{
	//	//or create a new save file
	//	Ship::SaveShip(playerShip);
	//}
	//
	
	//
	//GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	//(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);
	//
	//(*CurrentGame).m_map_size = background->m_size;
	//(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	//(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);
}

void InGameState::Update(sf::Time deltaTime)
{
	//(*CurrentGame).updateScene(deltaTime);
	//
	////move camera
	//UpdateCamera(deltaTime);
	//
	////Create and destroy HUD panels
	////case 1: destroying a panel
	//if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel == SFPanel_None && (*CurrentGame).m_playerShip->m_SFTargetPanel)
	//{
	//	DestroySFPanel((*CurrentGame).m_playerShip);
	//}
	//else if ((*CurrentGame).m_playerShip->m_is_asking_SFPanel != SFPanel_None)
	//{
	//	//case 2: creating a panel
	//	if (!(*CurrentGame).m_playerShip->m_SFTargetPanel)
	//	{
	//		CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
	//	}
	//	//case 3: changing panel
	//	else if ((*CurrentGame).m_playerShip->m_SFTargetPanel->m_panel_type != (*CurrentGame).m_playerShip->m_is_asking_SFPanel)
	//	{
	//		DestroySFPanel((*CurrentGame).m_playerShip);
	//		CreateSFPanel((*CurrentGame).m_playerShip->m_is_asking_SFPanel, (*CurrentGame).m_playerShip);
	//	}
	//}
	//
	//this->mainWindow->clear();

	m_NeuralNetwork.DoNothing();
}

void InGameState::Draw()
{
	//(*CurrentGame).drawScene();
}

void InGameState::Release()
{
	//TODO
}

void InGameState::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

void InGameState::DestroySFPanel(Ship* playerShip)
{
	if (playerShip->m_SFTargetPanel)
	{
		(*CurrentGame).removeFromFeedbacks(playerShip->m_SFTargetPanel);
		delete playerShip->m_SFTargetPanel;
		playerShip->m_SFTargetPanel = NULL;
	}
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Specific:
		{
			playerShip->m_SFTargetPanel = new SFPanelSpecific(sf::Vector2f(SFPANEL_SPECIFIC_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_Specific, playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::LoadCSVFile(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	vector<vector<string> > allConfigs = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allConfigVectorSize = allConfigs.size();
	for (size_t i = 0; i < allConfigVectorSize; i++)
	{
		(*CurrentGame).m_gameObjectsConfig.insert(std::map<string, vector<string> >::value_type(allConfigs[i][0], allConfigs[i]));
	}

	allConfigs.clear();
}


// NEURAL NETWORK
Neuron::Neuron()
{
	for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
	{
		m_weight[j] = RandomizeFloatBetweenValues(sf::Vector2f(0, 1));
	}
}

NeuralNetwork::NeuralNetwork()
{
	for (int i = 0; i < NEURAL_NETWORK_DEPTH; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
		{
			Neuron* neuron = new Neuron();
			m_network[i][j] = neuron;
		}
	}

	printf("Welcome. Neural network created: %d layers of %d neurons.\n", NEURAL_NETWORK_DEPTH, NEURAL_NETWORK_HEIGHT);
}

NeuralNetwork::~NeuralNetwork()
{
	for (int i = 0; i < NEURAL_NETWORK_HEIGHT; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_DEPTH; j++)
		{
			delete m_network[i][j];
			m_network[i][j] = NULL;
		}
	}
}

bool NeuralNetwork::AddData(Data data)
{
	//checking data integrity
	int feature_index = 0;
	if (data.m_red == NULL)
	{
		feature_index++;
		printf("Add Labbeled Data: FAIL (feature %d empty)\n", feature_index);
		return false;
	}
	if (data.m_green == NULL)
	{
		feature_index++;
		printf("Add Labbeled Data: FAIL (feature %d empty)\n", feature_index);
		return false;
	}
	if (data.m_blue == NULL)
	{
		feature_index++;
		printf("Add Labbeled Data: FAIL (feature %d empty)\n", feature_index);
		return false;
	}
	if (data.m_alpha == NULL)
	{
		feature_index++;
		printf("Add Labbeled Data: FAIL (feature %d empty)\n", feature_index);
		return false;
	}

	//Label
	if (data.m_label == NULL)
	{
		m_unlabelled_dataset.push_back(data);
		printf("Add Unlabbeled Data: SUCCESS (%d unlabelled data).\n", m_unlabelled_dataset.size());
		return false;
	}
	else
	{
		m_labelled_dataset.push_back(data);
		printf("Add Labbeled Data: SUCCESS (%d labelled data).\n", m_labelled_dataset.size());
		return true;
	}
}

bool NeuralNetwork::Train()
{
	Data input_data;
	input_data.Copy(m_unlabelled_dataset.front());
	input_data.GetFeatures();

	Data output_data;

	//Display input data
	printf("Input data:");
	for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
	{
		if (f < NEURAL_NETWORK_FEATURES - 1)
		{
			printf(" %d,", *input_data.m_features[f]);
		}
		else
		{
			printf(" %d.", *input_data.m_features[f]);
		}
	}
	
	//Input layer
	for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
	{
		for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
		{
			int output_value = (int)(*input_data.m_features[f] * m_network[0][j]->m_weight[j]);

			*m_network[0][j]->m_data.m_features[f] = output_value;
		}
	}
	
	//Hidden layers
	for (int i = 1; i < NEURAL_NETWORK_DEPTH; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
		{
			for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)	
			{
				int output_value = (int)(*m_network[i - 1][j]->m_data.m_features[f] * m_network[i - 1][j]->m_weight[j]);

				*m_network[i][j]->m_data.m_features[f] = output_value;
			}
		}
	}

	//Output layer
	for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
	{
		for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
		{
			int output_value = *m_network[NEURAL_NETWORK_DEPTH - 1][j]->m_data.m_features[f];

			*output_data.m_features[f] = output_value;
		}
	}

	//Display output data
	printf("\nOuput data:");
	for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
	{
		if (f < NEURAL_NETWORK_FEATURES - 1)
		{
			printf(" %d,", *output_data.m_features[f]);
		}
		else
		{
			printf(" %d.", *output_data.m_features[f]);
		}
	}

	//Error calculation
	float error_value = 0.f;
	float error_min = -1;
	int best_feature = -1;

	Data delta_data;

	Data ref_data;
	ref_data.Copy(m_labelled_dataset.front());
	ref_data.GetFeatures();

	for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
	{
		*delta_data.m_features[f] = *output_data.m_features[f] - *ref_data.m_features[f];
		float feature_error = *delta_data.m_features[f];
		error_value += feature_error;

		if (error_min < 0 || abs(feature_error) < error_min)
		{
			error_min = feature_error;
			best_feature = f;
		}
	}


	//Label identification
	if (abs(error_min) < 20.f)
	{
		printf("Unlabelled data identification: SUCCESS. Label: %d. Error margin: %f.\n", (Label)best_feature, error_min);
	}
	else
	{
		printf("Unlabelled data identification: FAIL. Best label: %d. Error margin: %f.\n", (Label)best_feature, error_min);
	}

	return true;
}