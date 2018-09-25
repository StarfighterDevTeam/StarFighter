#include "InGameState.h"

extern Game* CurrentGame;

void NeuralNetwork::FillWithData()
{
	for (int i = 0; i < DATASET_SIZE; i++)
	{
		int red, green, blue = 0;
		Label label;

		//labelled examples
		if (i < DATASET_SUPERVISED_LOT)
		{
			if (i % 2 == 0)
			{
				//green examples
				red = RandomizeIntBetweenValues(0, (int)(NEURAL_NETWORK_ERROR_MARGIN * 255));
				green = RandomizeIntBetweenValues((int)(255 * (1 - NEURAL_NETWORK_ERROR_MARGIN)), 255);
				blue = RandomizeIntBetweenValues(0, (int)(NEURAL_NETWORK_ERROR_MARGIN * 255));

				label = IS_GREEN;
			}
			else
			{
				//not green examples
				red = RandomizeIntBetweenValues(0, 255);
				green = RandomizeIntBetweenValues(0, (int)((1 - NEURAL_NETWORK_ERROR_MARGIN) * 255));
				blue = RandomizeIntBetweenValues(0, 255);
				label = NOT_GREEN;
			}
		}
		else //unlabelled examples
		{
			if (i % 2 == 0)
			{
				//green examples
				red = RandomizeIntBetweenValues(0, (int)(NEURAL_NETWORK_ERROR_MARGIN * 255));
				green = RandomizeIntBetweenValues((int)(255 * (1 - NEURAL_NETWORK_ERROR_MARGIN)), 255);
				blue = RandomizeIntBetweenValues(0, (int)(NEURAL_NETWORK_ERROR_MARGIN * 255));
			}
			else
			{
				//not green examples
				red = RandomizeIntBetweenValues(0, 255);
				green = RandomizeIntBetweenValues(0, (int)((1 - NEURAL_NETWORK_ERROR_MARGIN) * 255));
				blue = RandomizeIntBetweenValues(0, 255);
			}

			label = UNLABELLED;
		}

		Data data(red, green, blue, label);

		if (label == UNLABELLED)
		{
			m_unlabelled_dataset.push_back(data);
			printf("Add Unlabbeled Data: SUCCESS (%d unlabelled data).\n", m_unlabelled_dataset.size());
		}
		else
		{
			m_labelled_dataset.push_back(data);
			printf("Add Labbeled Data: SUCCESS (%d labelled data).\n", m_labelled_dataset.size());
		}
	}
}

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);

	//Loading scripts
	//LoadCSVFile(SHIP_CSV_FILE);


	//Input labelled data
	m_NeuralNetwork.FillWithData();
	
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
		m_weight[j] = RandomizeFloatBetweenValues(sf::Vector2f(0.f, 1.f));
	}

}

bool Neuron::CorrectWeight(int index)
{
	if (m_weight[index] == 1.f)
	{
		return false;
	}
	else
	{
		m_weight[index] += NEURAL_NETWORK_LEARNING_RATE;
		if (m_weight[index] > 1.f)
		{
			m_weight[index] = 1.f;
		}

		return true;
	}
}

NeuralNetwork::NeuralNetwork()
{
	//Input layer
	for (int f = 0; f < NB_FEATURES + 1; f++)
	{
		Neuron* neuron = new Neuron();
		m_input_layer[f] = neuron;
	}

	//Hidden layers
	for (int i = 0; i < NEURAL_NETWORK_DEPTH; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
		{
			Neuron* neuron = new Neuron();
			m_hidden_layers[i][j] = neuron;
		}
	}

	//Output layer
	for (int k = 0; k < NB_LABELS; k++)
	{
		Neuron* neuron = new Neuron();
		m_output_layer[k] = neuron;
	}

	printf("Welcome. Neural network created:\n- input layer of %d features\n- %d hidden layers of %d neurons.\n- output layer of %d labels\n\n", NB_FEATURES + 1, NEURAL_NETWORK_DEPTH, NEURAL_NETWORK_HEIGHT, NB_LABELS);
}

NeuralNetwork::~NeuralNetwork()
{
	for (int f = 0; f < NB_FEATURES + 1; f++)
	{
		delete m_input_layer[f];
		m_input_layer[f] = NULL;
	}

	for (int i = 0; i < NEURAL_NETWORK_HEIGHT; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_DEPTH; j++)
		{
			delete m_hidden_layers[i][j];
			m_hidden_layers[i][j] = NULL;
		}
	}

	for (int k = 0; k < NB_LABELS; k++)
	{
		delete m_output_layer[k];
		m_output_layer[k] = NULL;
	}
}

void NeuralNetwork::Train()
{
	for (int d = 0; d < DATASET_SUPERVISED_LOT; d++)
	{
		printf("\nTraining supervised on labelled data: %d/%d.\n", d, DATASET_SUPERVISED_LOT);
		
		Label input_label = m_labelled_dataset[d].m_label;
		float error = -1;
		float previous_error = -1;
		float learning_rate = NEURAL_NETWORK_LEARNING_RATE;
		int attempt = 0;

		while (abs(error) > NEURAL_NETWORK_ERROR_MARGIN)
		{
			if (attempt > 10)
			{
				//return;
			}

			attempt++;
			previous_error = error;
			printf("\nAttempt %d\n", attempt);

			//Convert data into Input features between 0 and 1
			for (int f = 0; f < NB_FEATURES; f++)
			{
				//clamping values on entropy
				m_input_layer[f]->m_value = Lerp(m_labelled_dataset[d].m_features[f], 0.f, 255.f, 0.f, 1.f);
			}
			m_input_layer[NB_FEATURES]->m_value = 1.f;//"1" channel

			//Input layer -> Hidden layers
			printf("Feed-forward|Input->Hidden 0:\n");
			for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
			{
				printf("Neuron %d: ", j);
				m_hidden_layers[0][j]->m_value = 0;
				for (int f = 0; f < NB_FEATURES + 1; f++)
				{
					m_hidden_layers[0][j]->m_value += m_input_layer[f]->m_value * m_input_layer[f]->m_weight[j];
					printf("%.3f * %.3f", m_input_layer[f]->m_value, m_input_layer[f]->m_weight[j]);
					if (f < NB_FEATURES)
						printf(" + ");
				}
				printf(" = %.3f", m_hidden_layers[0][j]->m_value);

				//m_hidden_layers[0][j]->m_value *= (1.f / (NB_FEATURES + 1));
				m_hidden_layers[0][j]->m_value = ActivationFunction(m_hidden_layers[0][j]->m_value);
				printf(" |->activation: %.3f\n", m_hidden_layers[0][j]->m_value);
			}

			//Hidden layers
			for (int i = 1; i < NEURAL_NETWORK_DEPTH; i++)
			{
				printf("Feed-forward|Hidden %d->Hidden %d:\n", i, i+1);
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					printf("Neuron %d: ", j);
					m_hidden_layers[i][j]->m_value = 0;
					for (int k = 0; k < NEURAL_NETWORK_HEIGHT; k++)
					{
						m_hidden_layers[i][j]->m_value += m_hidden_layers[i-1][k]->m_value * m_hidden_layers[i-1][k]->m_weight[j];
						printf("%.3f * %.3f", m_hidden_layers[i-1][k]->m_value, m_hidden_layers[i-1][k]->m_weight[j]);
						if (k < NEURAL_NETWORK_HEIGHT - 1)
							printf(" + ");
					}
					printf(" = %.3f", m_hidden_layers[i][j]->m_value);

					//m_hidden_layers[i][j]->m_value *= (1.f / NEURAL_NETWORK_HEIGHT);
					m_hidden_layers[i][j]->m_value = ActivationFunction(m_hidden_layers[i][j]->m_value);
					printf(" |->activation: %.3f\n", m_hidden_layers[i][j]->m_value);
				}
			}

			//Hidden layers -> output layer
			printf("Feed-forward|Hidden %d->Output:\n", NEURAL_NETWORK_DEPTH-1);
			m_output_layer[0]->m_value = 0;
			for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
			{
				m_output_layer[0]->m_value += m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_value * m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0];
				printf("%.3f * %.3f", m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_value, m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0]);
			}
			printf(" = %.3f", m_output_layer[0]->m_value);

			//m_output_layer[0]->m_value *= (1.f / NEURAL_NETWORK_HEIGHT);
			m_output_layer[0]->m_value = ActivationFunction(m_output_layer[0]->m_value);
			printf(" |->activation: %.3f\n", m_output_layer[0]->m_value);
			
			//Label recognition
			if (input_label == IS_GREEN)
			{
				error = 1 - m_output_layer[0]->m_value;
			}
			else if (input_label == NOT_GREEN)
			{
				error = m_output_layer[0]->m_value;
			}
			m_output_layer[0]->m_error = error;

			//Success?
			if (abs(error) < NEURAL_NETWORK_ERROR_MARGIN)
			{
				printf("Data %d successfully recognized (%d attempt(s)).\n", d, attempt);
				break;
			}
			//Error backpropagation
			else
			{
				//Learning rate moderation
				float learning_rate = NEURAL_NETWORK_LEARNING_RATE;
				if (abs(error) < 1 && abs(error) > abs(previous_error))
				{
					learning_rate *= 0.1;
				}
				printf("Error: %f (previous error: %f). Learning rate: %f\n", error, previous_error, learning_rate);

				//Hidden layers -> output layer
				printf("Backpropagation|Output->Hidden %d:\n", NEURAL_NETWORK_DEPTH - 1);
				float total_weight = 0;
				float previous_weight[NEURAL_NETWORK_HEIGHT];
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					previous_weight[j] = m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0];
					total_weight += previous_weight[j];
				}
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					float synaptic_weight = total_weight == 0 ? 0 : previous_weight[j] / total_weight;

					m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_error = m_output_layer[0]->m_error * synaptic_weight * learning_rate;

					float new_weight = previous_weight[j] * (1 + m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_error);
					new_weight = Bound(new_weight, 0.f, 1.f);
					m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0] = new_weight;
					printf("weight: %d, neuron output, old_weight: %.3f | synaptic: %.1f | parent_error: %.3f | error: %.3f | new_weight: %.3f\n", j, previous_weight[j], synaptic_weight, m_output_layer[0]->m_error, m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_error, new_weight);
				}

				//Hidden layers
				for (int i = NEURAL_NETWORK_DEPTH - 1; i > 0; i--)
				{
					printf("Backpropagation|Hidden %d->Hidden %d:\n", i, i-1);
					for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
					{
						total_weight = 0;
						for (int k = 0; k < NEURAL_NETWORK_HEIGHT; k++)
						{
							previous_weight[k] = m_hidden_layers[i - 1][k]->m_weight[j];
							total_weight += previous_weight[k];
						}
						for (int k = 0; k < NEURAL_NETWORK_HEIGHT; k++)
						{
							float synaptic_weight = total_weight == 0 ? 0 : previous_weight[k] / total_weight;

							m_hidden_layers[i - 1][k]->m_error = m_hidden_layers[i][j]->m_error * synaptic_weight * learning_rate;

							float new_weight = previous_weight[k] * (1 + m_hidden_layers[i - 1][j]->m_error);
							new_weight = Bound(new_weight, 0.f, 1.f);
							m_hidden_layers[i - 1][k]->m_weight[j] = new_weight;
							printf("weight %d, neuron %d, old_weight:%.3f | synaptic: %.1f | parent_error: %.3f | error: %.3f | new_weight: %.3f\n", k, j, previous_weight[j], synaptic_weight, m_hidden_layers[i][j]->m_error, m_hidden_layers[i - 1][k]->m_error, new_weight);
						}
					}
				}

				//Hidden layers -> input_layer
				printf("Backpropagation|Hidden 0->Input:\n");
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					total_weight = 0;
					for (int f = 0; f < NB_FEATURES + 1; f++)
					{
						previous_weight[f] = m_input_layer[f]->m_weight[j];
						total_weight += previous_weight[j];
					}
					for (int f = 0; f < NB_FEATURES + 1; f++)
					{
						float synaptic_weight = total_weight == 0 ? 0 : previous_weight[f] / total_weight;

						m_input_layer[f]->m_error = m_hidden_layers[0][j]->m_error * synaptic_weight * learning_rate;

						float new_weight = previous_weight[f] * (1 + m_input_layer[f]->m_error);
						new_weight = Bound(new_weight, 0.f, 1.f);
						m_input_layer[f]->m_weight[j] = new_weight;
						printf("weight %d, neuron: %d, old_weight:%.3f | synaptic: %.1f | parent_error: %.3f | error: %.3f | new_weight: %.3f\n", f, j, previous_weight[j], synaptic_weight, m_hidden_layers[0][j]->m_error, m_input_layer[f]->m_error, new_weight);
					}
				}
			}
		}
	}

	printf("");
}

float NeuralNetwork::ActivationFunction(float x)
{
	//float output_value = 1.f / (1 + pow((1 / 2, 71828), x));
	float output_value = x;// = Bound(x, 0.f, 1.f);

	return output_value;

	if (output_value > 0.5f)
	{
		output_value = 1.f;
	}
	else
	{
		output_value = 0.f;
	}

	
}

		/*
		Data input_data;
		input_data.Copy(m_labelled_dataset[d]);
		Label input_label = *input_data.m_label;

		Data output_data;

		float error = -1;
		int attempt = 0;
		while (error < 0 || error > NEURAL_NETWORK_ERROR_MARGIN)
		{
			attempt++;
			printf("\nData: %d, attempt: %d\n", d, attempt);

			//Input layer
			printf("Input layer:");
			for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
			{
				m_input_layer[f]->m_value = *input_data.m_features[f];

				if (f < NEURAL_NETWORK_FEATURES - 1)
				{
					printf(" %d,", m_input_layer[f]->m_value);
				}
				else
				{
					printf(" %d.\n", m_input_layer[f]->m_value);
				}
			}

			//Input layer -> hidden layers
			printf("Hidden layers (1):");
			for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
			{
				int output_value = 0;
				float total_weight = 0.f;
				for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
				{
					output_value += (int)(m_input_layer[f]->m_value * m_input_layer[f]->m_weight[j]);
					total_weight += m_input_layer[f]->m_weight[j];
				}
				if (total_weight > 0)
				{
					output_value = (int)(output_value * 1.f / total_weight);
				}
				m_hidden_layers[0][j]->m_value = output_value;

				if (j < NEURAL_NETWORK_HEIGHT - 1)
				{
					printf(" %d,", m_hidden_layers[0][j]->m_value);
				}
				else
				{
					printf(" %d.\n", m_hidden_layers[0][j]->m_value);
				}
			}

			//Hidden layers
			for (int i = 1; i < NEURAL_NETWORK_DEPTH; i++)
			{
				printf("Hidden layers (%d):", i + 1);
				int output_value = 0;
				float total_weight = 0.f;
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					for (int h = 0; h < NEURAL_NETWORK_HEIGHT; h++)
					{
						output_value += (int)(m_hidden_layers[i - 1][h]->m_value * m_hidden_layers[i - 1][h]->m_weight[h]);
						total_weight += m_hidden_layers[i - 1][j]->m_weight[h];
					}

					if (total_weight > 0)
					{
						output_value = (int)(output_value * 1.f / total_weight);
					}

					m_hidden_layers[i][j]->m_value = output_value;

					if (j < NEURAL_NETWORK_HEIGHT - 1)
					{
						printf(" %d,", m_hidden_layers[i][j]->m_value);
					}
					else
					{
						printf(" %d.\n", m_hidden_layers[i][j]->m_value);
					}
				}
			}

			//Output layer
			printf("Output layer:");
			int output_value = 0;
			float total_weight = 0.f;
			for (int k = 0; k < NB_LABELS; k++)
			{
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					output_value += (int)(m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_value * m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[j]);
					total_weight += m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[j];
				}

				if (total_weight > 0)
				{
					output_value = (int)(output_value * 1.f / total_weight);
				}
				m_output_layer[k]->m_value = output_value;

				if (k < NB_LABELS - 1)
				{
					printf(" %d,", m_output_layer[k]->m_value);
				}
				else
				{
					printf(" %d.\n", m_output_layer[k]->m_value);
				}
			}

			//Output label
			int max_value = -1;
			int best_label = -1;
			for (int k = 0; k < NB_LABELS; k++)
			{
				if (max_value < 0 || m_output_layer[k]->m_value > max_value)
				{
					max_value = m_output_layer[k]->m_value;
					best_label = k;
				}
			}

			Label output_label = (Label)best_label;
			if (output_label == input_label)
			{
				printf("Training: label recognization: SUCCESS. Label: %s. Data: ", GetLabelString(best_label).c_str());
				for (int k = 0; k < NB_LABELS; k++)
				{
					if (k < NB_LABELS - 1)
					{
						printf(" %d,", m_output_layer[k]->m_value);
					}
					else
					{
						printf(" %d.\n", m_output_layer[k]->m_value);
					}
				}

				break;
			}
			else
			{
				printf("Training: label recognization: FAIL (Expected: %s, vs found: %s. Data: ", GetLabelString(input_label).c_str(), GetLabelString(best_label).c_str());
				for (int k = 0; k < NB_LABELS; k++)
				{
					if (k < NB_LABELS - 1)
					{
						printf(" %d,", m_output_layer[k]->m_value);
					}
					else
					{
						printf(" %d.)\n", m_output_layer[k]->m_value);
					}
				}
				
				//Backpropagation of the error: we want the correct label to weight at least NEURAL_NETWORK_ERROR_MARGIN more than any other label.
				float error_label[NB_LABELS];
				for (int k = 0; k < NB_LABELS; k++)
				{
					error_label[k] = 0.f;
					//Looking to weights all wrong labels at 0, and the correct label at the max value (or at least 1)
					if (k != input_label)
					{
						//positive error = needs to be decreased
						error_label[k] = m_output_layer[k]->m_value;
					}
					else
					{
						//negative error = needs to be increased
						error_label[k] = - (max(max_value, 1) - m_output_layer[k]->m_value);
					}
				}
				
				//Output layer
				printf("Output layer. ");
				
			}


			*/
				/*if (max_value == 0)
				{
					error = NEURAL_NETWORK_ERROR_MARGIN;//TO DO : what to do with this case? it's not necessarily an error.
				}
				else
				{
					error = (1.f - 1.f * m_output_layer[*input_data.m_label]->m_value / max_value);
				}
				
				if (error == 0.f)
				{
					error = NEURAL_NETWORK_ERROR_MARGIN;
				}
				printf("\nError backpropagation (%f)\n", error);


				//Output layer
				printf("Output layer. ");
				float max_weight = -1.f;
				int heaviest_neuron = -1;
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					if (m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[*input_data.m_label] < 1.f)
					{
						if (max_weight < 0 || m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[*input_data.m_label] > max_weight)
						{
							max_weight = m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[*input_data.m_label];
							heaviest_neuron = j;
						}
					}
				}

				printf("Neuron to blame: %d (weight: %f)", heaviest_neuron, max_weight);
				m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][heaviest_neuron]->CorrectWeight(*input_data.m_label);
				printf(" -> corrected weight: %f.\n", m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][heaviest_neuron]->m_weight[*input_data.m_label]);

				//Hidden layers
				int h = heaviest_neuron;
				if (h == -1)
				{
					h = RandomizeIntBetweenValues(0, NEURAL_NETWORK_HEIGHT - 1);
				}
				for (int i = NEURAL_NETWORK_DEPTH - 1; i > 0; i--)
				{	
					max_weight = -1.f;
					heaviest_neuron = -1;
					printf("Hidden layers (%d). ", i);
					for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
					{
						if (max_weight < 0 || m_hidden_layers[i - 1][h]->m_weight[j] > max_weight)
						{
							max_weight = m_hidden_layers[i - 1][h]->m_weight[j];
							heaviest_neuron = j;
						}
					}

					
					printf("Neuron to blame: %d (weight: %f)\n", heaviest_neuron, max_weight);
					m_hidden_layers[i - 1][heaviest_neuron]->CorrectWeight(h);
					printf(" -> corrected weight: %f.\n", m_hidden_layers[i - 1][heaviest_neuron]->m_weight[h]);

					h = heaviest_neuron;					
				}

				//Input layer
				printf("Input layer. ");
				if (h == -1)
				{
					h = RandomizeIntBetweenValues(0, NEURAL_NETWORK_HEIGHT - 1);
				}
				max_weight = -1.f;
				heaviest_neuron = -1;
				for (int f = 0; f < NEURAL_NETWORK_FEATURES; f++)
				{
					if (max_weight < 0 || m_input_layer[f]->m_weight[h] > max_weight)
					{
						max_weight = m_input_layer[f]->m_weight[h];
						heaviest_neuron = f;
					}
				}
				printf("Neuron to blame: %d (weight: %f)\n", heaviest_neuron, max_weight);
				m_input_layer[heaviest_neuron]->CorrectWeight();
				printf(" -> corrected weight: %f.\n", m_hidden_layers[i - 1][heaviest_neuron]->m_weight[h]);
			}
			*/
			//end of error retropropagation
	/*
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
	if (abs(error_min) < NEURAL_NETWORK_ERROR_MARGIN)
	{
		printf("Unlabelled data identification: SUCCESS. Label: %d. Error margin: %f.\n", (Label)best_feature, error_min);
	}
	else
	{
		printf("Unlabelled data identification: FAIL. Best label: %d. Error margin: %f.\n", (Label)best_feature, error_min);
	}

	return true;
	*/