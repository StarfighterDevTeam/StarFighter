#include "StarFighter.h"

Resources* GlobalResources;

int main()
{
	srand(time(NULL));
	GlobalResources = new Resources();

	NeuralNetwork NeuralNetwork;

	//NeuralNetwork.LoadDatasetFromFile();
	NeuralNetwork.CreateDataset();
	NeuralNetwork.BalanceDataset();//cutting examples so that we have a perfect parity between green examples and not green examples, mixed alternatively
	NeuralNetwork.SaveDatasetIntoFile();

	//NeuralNetwork.Run(Learn);
	int mode = Learn;
	NeuralNetwork.m_current_data_index = 0;

	while ((*GlobalResources).m_renderWindow.isOpen())
	{
		sf::Event event;
		while ((*GlobalResources).m_renderWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				(*GlobalResources).m_renderWindow.close();
			}

			//run
			NeuralNetwork.Run((NeuralNetworkMode)mode);
			NeuralNetwork.Display();
		}
	}

	//while (mode >= 0)
	//{
	//	//printf("\nChoose run mode for the Neural Network:\n\n");
	//	//printf("0 = perform from scratch with given parameters\n");
	//	//printf("1 = loop to find the best hyper parameters\n");
	//	//printf("2 = load best-known weights and hyperparameters and iterate to improve weights\n");
	//	//printf("3 = input manual values to test the model\n\n");
	//
	//	//printf("0 = create new dataset\n");
	//	//printf("1 = reset weights\n");
	//
	//	NeuralNetwork.Run((NeuralNetworkMode)mode);
	//	NeuralNetwork.Display();
	//}
	
	printf("Exit program?\n");
	cin.get();

	delete GlobalResources;
	return 0;
}


//NEURAL NETWORK
NeuralNetwork::NeuralNetwork()
{
	//hyper parameters
	m_nb_layers = 0;
	m_success_rate = 0.f;
	m_loops = 0;
	m_overall_attempts = 0;
	m_weightLoadIndex = 0;

	//Use the same random weights every run
	LoadWeightsFromFile(RANDOM_WEIGHTS_FILE);

	m_learning_rate = NN_LEARNING_RATE;
	m_momentum = NN_MOMENTUM;
	m_function = NN_ACTIVATION_FUNCTION;
	m_use_bias = true;

	//Input layer
	AddLayer(NB_FEATURES, InputLayer);

	//Hidden layers
	AddLayer(5, HiddenLayer);
	//AddLayer(2, HiddenLayer);

	//Output layer
	AddLayer(1, OutpuLayer);

	//UI
	AdjustNeuronDisplayPositions();
}

NeuralNetwork::~NeuralNetwork()
{
	for (Layer* layer : m_layers)
		delete layer;
}

void NeuralNetwork::Run(NeuralNetworkMode mode)
{
	clock_t begin = clock();

	//Mode 0 - create new dataset
	switch (mode)
	{
		case IdleMode:
		{
			break;
		}
		case CreateDataSet:
		{
			CreateDataset();
			BalanceDataset();
			SaveDatasetIntoFile();

			//chrono
			clock_t end = clock();
			double elapsed = (double)((end - begin) / CLOCKS_PER_SEC);
			printf("Time elpased: %fs.\n", elapsed);

			break;
		}
		case RestoreRandomWeights:
		{
			//reset file
			remove(RANDOM_WEIGHTS_FILE);
			m_weightsStart.clear();

			//reinit weights
			for (Layer* layer : m_layers)
			{
				for (Neuron* neuron : layer->m_neurons)
				{
					double weight = RandomizeFloatBetweenValues(-1.f, 1.f);
					m_weightsStart.push_back(weight);
					if (neuron == layer->m_neurons.back())
						SaveStartingWeightsIntoFile();
				}
			}

			break;
		}

		case Learn:
		{
			if (m_display_timer.getElapsedTime().asSeconds() > 0.1)
			{
				int d = m_current_data_index;
				//for (int n = 0; n < m_dataset.size(); n++)
				InitInputLayer(m_dataset[d]);
				FeedForward();
				ErrorCalculation(m_dataset[d]);
				BackPropagationGradient(m_dataset[d]);
				WeightsUpdate();

				m_display_timer.restart();

				float val = m_layers.back()->m_neurons.front()->m_value;
				string str_info = "Dataset size: " + to_string(m_dataset.size()) + ", current item: " + to_string(int(trunc(m_dataset[d].m_features.front()))) + " (" + DecodingData(m_dataset[d]) + "), label: " + to_string(m_dataset[d].m_label) + ", estimated label: " + (val > 0.5 ? "ROMAIN" : "NOT_ROMAIN") + " (confidence : " + to_string(abs(val - 0.5)) + "), error : " + to_string(m_error);
				m_general_info_text.setString(str_info);

				m_current_data_index++;
				if (m_current_data_index == m_dataset.size())
					m_current_data_index = 0;
			}
			break;
		}
	}
}

void NeuralNetwork::Display()
{
	(*GlobalResources).m_renderWindow.clear();

	//mouse pointer
	sf::Vector2i mousepos2i = sf::Mouse::getPosition((*GlobalResources).m_renderWindow);
	sf::Vector2f mousepos = (*GlobalResources).m_renderWindow.mapPixelToCoords(mousepos2i, GlobalResources->m_view);

	//background
	sf::RectangleShape background;
	background.setSize(sf::Vector2f(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y));
	background.setFillColor(sf::Color(50, 50, 50, 255));
	background.setOrigin(0, 0);
	background.setPosition(0, 0);
	(*GlobalResources).m_renderWindow.draw(background);
	

	//neurons
	for (int i = 0; i < m_nb_layers; i++)
	{
		Layer* currentLayer = m_layers[i];
		for (Neuron* neuron : currentLayer->m_neurons)
		{
			//dot
			neuron->m_circle.setFillColor(neuron->m_is_bias == false ? sf::Color::Red : sf::Color::Magenta);
			(*GlobalResources).m_renderWindow.draw(neuron->m_circle);

			//lines
			for (sf::RectangleShape& line : neuron->m_lines)
				(*GlobalResources).m_renderWindow.draw(line);

			//weight texts' value update
			if (currentLayer->m_type != OutpuLayer)
			{
				Layer* nextLayer = m_layers[i + 1];
				int n = 0;
				for (Neuron* next_neuron : nextLayer->m_neurons)
				{
					if (next_neuron->m_is_bias == false && currentLayer->m_type != OutpuLayer)
					{
						sf::Text text;
						string str = to_string(neuron->m_weights[n]);
						neuron->m_weight_texts[n].setString(str.substr(0, 5));
					}

					n++;
				}
			}

			//weights text display if neuron is hovered
			if (neuron->IsHovered(mousepos) == true)
			{
				if (m_layers[i]->m_type != InputLayer)
				{
					int n = 0;
					for (Neuron* prev_neuron : m_layers[i - 1]->m_neurons)
					{
						(*GlobalResources).m_renderWindow.draw(prev_neuron->m_weight_texts[n]);
						n++;
					}
				}
			}
		}
	}

	for (int i = 0; i < m_nb_layers; i++)
	{
		int n = 0;
		for (Neuron* neuron : m_layers[i]->m_neurons)
		{
			

			//input value display
			string str;
			if (m_layers[i]->m_type != InputLayer && neuron->m_is_bias == false)
			{
				str = to_string(neuron->m_input_value);
				neuron->m_input_text.setString(str.substr(0, 5));
				(*GlobalResources).m_renderWindow.draw(neuron->m_input_text);
			}

			//ouput value display
			str = to_string(neuron->m_value);
			neuron->m_output_text.setString(str.substr(0, 5));
			(*GlobalResources).m_renderWindow.draw(neuron->m_output_text);

			n++;
		}
	}

	//general info
	(*GlobalResources).m_renderWindow.draw(m_general_info_text);

	//display
	(*GlobalResources).m_renderWindow.display();
}

void NeuralNetwork::RestoreWeights()
{
	int index = 0;

	for (Layer* layer : m_layers)
	{
		for (Neuron* neuron : layer->m_neurons)
		{
			int weightsSize = neuron->m_weights.size();
			for (int w = 0; w < weightsSize; w++)
			{
				neuron->m_weights[w] = m_weightsStart[index];
				neuron->m_deltaWeights[w] = 0;
				index++;
			}
		}
	}
}

void NeuralNetwork::CopyWeightsInto(vector<double>& weights)
{
	for (Layer* layer : m_layers)
	{
		for (Neuron* neuron : layer->m_neurons)
		{
			int weightsSize = neuron->m_weights.size();
			for (int w = 0; w < weightsSize; w++)
				weights.push_back(neuron->m_weights[w]);
		}
	}
}

bool NeuralNetwork::RecordPerf()
{
	//Get Perf
	Performance perf;
	perf.m_index = m_perf_records.size();
	perf.m_function = m_function;
	perf.m_momentum = m_momentum;
	perf.m_learning_rate = m_learning_rate;
	perf.m_overall_attempts = m_overall_attempts;
	perf.m_loops = m_loops;
	perf.m_success_rate = m_success_rate;
	perf.m_average_error = m_average_error;

	for (Layer* layer : m_layers)
		if (layer != m_layers.front() && layer != m_layers.back())
			perf.m_hidden_layers.push_back(layer->m_nb_neurons - 1);//-1 for the bias neuron

	CopyWeightsInto(perf.m_weights);

	//Save perf
	m_perf_records.push_back(perf);
	SavePerfIntoFile();
	if (UpdateBestPerf() && IsBetterPerfThanSaveFile(perf))
	{
		printf("New best perf: loops: %d, attempts: %d, success rate: %f, RMSE: %f (target: %f) [perf index: %d]\n", m_loops, m_overall_attempts, m_success_rate, m_average_error, NN_ERROR_MARGIN, m_perf_records.size() - 1);
		SaveBestPerfIntoFile();
		return true;
	}
	else
	{
		printf("Perf: loops: %d, attempts: %d, success rate: %f, RMSE: %f (target: %f) [perf index: %d]\n", m_loops, m_overall_attempts, m_success_rate, m_average_error, NN_ERROR_MARGIN, m_perf_records.size() - 1);
	}
	return false;
}

bool NeuralNetwork::UpdateBestPerf()
{
	assert(!m_perf_records.empty());
	
	int perfSize = m_perf_records.size();
	int best_perf = 0;
	int min_attempts = -1;
	double max_success_rate = -1.f;

	//Best perf = 1) best success rate 2) min overall attempts
	for (int i = 0; i < perfSize; i++)
	{
		if (min_attempts < 0 || m_perf_records[i].m_overall_attempts <= min_attempts)
		{
			if (max_success_rate < 0 || m_perf_records[i].m_success_rate >= max_success_rate)
			{
				if (!(m_perf_records[i].m_overall_attempts == min_attempts && m_perf_records[i].m_success_rate == max_success_rate))
				{
					min_attempts = m_perf_records[i].m_overall_attempts;
					max_success_rate = m_perf_records[i].m_success_rate;
					best_perf = i;
				}
			}
		}
	}

	m_best_perf = m_perf_records[best_perf];

	return best_perf == perfSize - 1;//return true if the latest perf is the best one
}

Data NeuralNetwork::CreateDataWithManualInputs()
{
	vector<double> features;
	int red;
	printf("Input red value between 0 and 255.\n");
	cin >> red;
	features.push_back((double)((1.f * red / 255 * 2) - 1));

	int green;
	printf("Input green value between 0 and 255.\n");
	cin >> green;
	features.push_back((double)((1.f * green / 255 * 2) - 1));

	int blue;
	printf("Input blue value between 0 and 255.\n");
	cin >> blue;
	features.push_back((double)((1.f * blue / 255 * 2) - 1));

	Label label = UNLABELLED;

	Data data(features, label);
	return data;
}

string NeuralNetwork::DecodingData(Data& data)
{
	int index = (int)data.m_features[0];

	switch (index)
	{
		case 1:
			return "A";
		case 2:
			return "B";
		case 3:
			return "C";
		case 4:
			return "D";
		case 5:
			return "E";
		case 6:
			return "F";
		case 7:
			return "G";
		case 8:
			return "H";
		case 9:
			return "I";
		case 10:
			return "J";
		case 11:
			return "K";
		case 12:
			return "L";
		case 13:
			return "M";
		case 14:
			return "N";
		case 15:
			return "O";
		case 16:
			return "P";
		case 17:
			return "Q";
		case 18:
			return "R";
		case 19:
			return "S";
		case 20:
			return "T";
		case 21:
			return "U";
		case 22:
			return "V";
		case 23:
			return "W";
		case 24:
			return "X";
		case 25:
			return "Y";
		case 26:
			return "Z";

		default:
			return "";
			
	}

	return "";
}

void NeuralNetwork::AddLayer(int nb_neuron, LayerType type)
{
	//Connnect previous layer's neurons
	if (!m_layers.empty())
	{
		Layer* previousLayer = m_layers.back();
		for (Neuron* neuron : previousLayer->m_neurons)
		{
			for (int j = 0; j < nb_neuron; j++)
			{
				double weight;
				//Load values from file if they exist
				if (m_weightLoadIndex < m_weightsStart.size())
					weight = m_weightsStart[m_weightLoadIndex];
				else
				{
					weight = RandomizeFloatBetweenValues(-1.f, 1.f);
					m_weightsStart.push_back(weight);

					if (j == nb_neuron - 1)
						SaveStartingWeightsIntoFile();
				}

				m_weightLoadIndex++;
				neuron->m_weights.push_back(weight);
				neuron->m_deltaWeights.push_back(0.f);
			}
		}
	}

	Layer* layer = new Layer(nb_neuron, type, m_use_bias);
	m_layers.push_back(layer);
	m_nb_layers++;
}

void NeuralNetwork::Training()
{
	if (PRINT_TR){ printf("\n*** Training. ***\n"); }

	m_success = 0;
	int training_attempts = 0;
	
	//Supervised training data
	int datasetSize = m_dataset.size();
	for (int d = 0; d < datasetSize; d++)
	{
		m_attempts = 0;
		m_error = NN_ERROR_MARGIN;

		if (PRINT_TR){ printf("\nInput data %d.\n", d); }
		InitInputLayer(m_dataset[d]);

		while (m_error >= NN_ERROR_MARGIN && m_attempts < NN_MAX_ATTEMPTS)
		{
			m_attempts++;
			m_overall_attempts++;
			if (PRINT_TR){ printf("\nAttempts: %d (overall attempts: %d).\n", m_attempts, m_overall_attempts); }
			FeedForward();

			ErrorCalculation(m_dataset[d]);

			if (m_error < NN_ERROR_MARGIN)
			{
				if (PRINT_TR){ printf("\nSuccess.\n"); }
				m_success++;
				break;
			}
			else
			{
				if (PRINT_TR){ printf("\nFail.\n"); }
			}

			BackPropagationGradient(m_dataset[d]);

			WeightsUpdate();
		}
	}
}

void NeuralNetwork::Testing()
{
	if (PRINT_TE){ printf("\n*** Testing. ***\n"); }

	m_average_error = 0.f;
	m_success = 0;
	m_attempts = 0;

	//Supervised training data
	int datasetSize = m_dataset.size();
	for (int d = 0; d < datasetSize; d++)
	{
		m_attempts++;
		if (PRINT_TE){ printf("\nInput data %d.\n", d); }

		InitInputLayer(m_dataset[d]);
		
		FeedForward();

		ErrorCalculation(m_dataset[d]);

		if (m_error < NN_ERROR_MARGIN)
		{
			if (PRINT_TE){ printf("Success.\n"); }
			m_success++;
		}
		else
		{
			if (PRINT_TE){ printf("Fail.\n"); }
		}
	}

	m_average_error = m_average_error / m_attempts;
	m_success_rate = 100.f * m_success / DATASET_TESTING_LOT;
	if (PRINT_LO){ printf("\nTesting data success: %d/%d (%.2f%%).\n", m_success, DATASET_TESTING_LOT, m_success_rate); }
	if (PRINT_LO){ printf("AVERAGE RMSE: %f (target: %f).\n", m_average_error, NN_ERROR_MARGIN); }
}

Label NeuralNetwork::TestSample(Data &data)
{
	InitInputLayer(data);
	FeedForward();
	
	double target_value = 1.f;

	m_error = 0.f;

	Layer* outputLayer = m_layers.back();
	for (Neuron* neuron : outputLayer->m_neurons)
	{
		double delta = target_value - neuron->m_value;
		m_error += delta * delta;
	}
	m_error *= 1.f / outputLayer->m_nb_neurons;
	m_error = sqrt(m_error);//Root Means Square Error

	Label label;
	if (m_error < 0.5f)
	{
		label = IS_ROMAIN;
	}
	else
	{
		label = IS_NOT_ROMAIN;
	}
	return label;

}

void NeuralNetwork::Creating()
{
	printf("\n*** Creating. ***\n");
	printf("Enter 0 for green, 1 for NOT-green.\n");
	int user_input;
	cin >> user_input;
	if (user_input == IS_ROMAIN)
	{

	}

}

void NeuralNetwork::InitInputLayer(const Data &data)
{
	Layer* inputLayer = m_layers.front();
	int index = 0;

	for (Neuron* neuron : inputLayer->m_neurons)
	{
		if (neuron != inputLayer->m_neurons.back())
		{
			neuron->m_value = data.m_features[index];
			neuron->m_input_value = neuron->m_value;
		}
		else
		{
			neuron->m_value = 1.f;//bias neuron
			neuron->m_input_value = neuron->m_value;
		}

		index++;
	}
}

void NeuralNetwork::FeedForward()
{
	//For display purposes
	if (PRINT_FF){ printf("Feed forward.\n"); }

	Layer* inputLayer = m_layers.front();
	int index = 0;
	for (Neuron* neuron : inputLayer->m_neurons)
	{
		if (PRINT_FF){ printf("Layer: %d, neuron: %d, input value: %f\n", 0, index, neuron->m_value); }
		index++;
	}
	
	//Feed forward
	for (int i = 0; i < m_nb_layers - 1; i++)//output layer doesn't feed forward
	{
		Layer* currentLayer = m_layers[i];
		Layer* nextLayer = m_layers[i +1];

		for (int n = 0; n < nextLayer->m_nb_neurons; n++)
		{
			if (n < nextLayer->m_nb_neurons - 1 || nextLayer->m_type == OutpuLayer)//Output layer doesn't have a bias neuron
			{
				if (PRINT_FF){ printf("\nSum calculation:\n");}
				double sum = 0;
				for (int w = 0; w < currentLayer->m_nb_neurons; w++)
				{
					sum += currentLayer->m_neurons[w]->m_value * currentLayer->m_neurons[w]->m_weights[n];
					if (PRINT_FF){ printf("   value * weight : %f * %f = %f\n", currentLayer->m_neurons[w]->m_value, currentLayer->m_neurons[w]->m_weights[n], sum);}
				}
				nextLayer->m_neurons[n]->m_input_value = sum;
				nextLayer->m_neurons[n]->m_value = TransferFunction(sum, m_function);
				if (PRINT_FF){ printf("Layer: %d, neuron: %d, input value: %f -> output value: %f\n", i + 1, n, nextLayer->m_neurons[n]->m_input_value, nextLayer->m_neurons[n]->m_value); }
			}
			else
			{
				nextLayer->m_neurons[n]->m_value = 1.f;//bias neuron
				nextLayer->m_neurons[n]->m_input_value = nextLayer->m_neurons[n]->m_value;
			}
		}
	}
}

double NeuralNetwork::GetTargetValue(const Label label)
{
	double target_value;
	if (label == IS_ROMAIN)
	{
		target_value = 1.f;
	}
	else
	{
		target_value = -1.f;
	}
	return target_value;
}

float NeuralNetwork::ErrorCalculation(const Data &data)
{
	//Error
	if (PRINT_EC){ printf("Error calculation.\n"); }
	double previous_error = m_attempts == 1 ? 0.f : m_error;
	m_error = 0.f;

	Layer* outputLayer = m_layers.back();
	for (int n = 0; n < outputLayer->m_nb_neurons; n++)
	{
		double delta = GetTargetValue(data.m_label) - outputLayer->m_neurons[n]->m_value;
		m_error += delta * delta;
		if (PRINT_EC){ printf("Neuron: %d, output: %f, target: %f (delta: %f).\n", n, outputLayer->m_neurons[n]->m_value, GetTargetValue(data.m_label), delta); }
	}
	m_error *= 1.f / outputLayer->m_nb_neurons;
	m_error = sqrt(m_error);//Root Means Square Error
	
	if (m_attempts <= 1)
	{
		if (PRINT_EC){ printf("RMSE: %f (target: %f)\n", m_error, NN_ERROR_MARGIN);}
	}
	else
	{
		if (PRINT_EC){ printf("RMSE: %f (previous RMSE: %f). Progression: %f (%f%%).\n", m_error, previous_error, previous_error - m_error, 100.f*(previous_error - m_error)) / previous_error; }
	}

	m_average_error += m_error;

	return m_error;
}

void NeuralNetwork::BackPropagationGradient(const Data &data)
{
	if (PRINT_BP){ printf("Back propagation.\n"); }

	Layer* outputLayer = m_layers.back();
	//Calcuation of the gradient in the output layer
	for (int n = 0; n < outputLayer->m_nb_neurons; n++)
	{
		double delta = GetTargetValue(data.m_label) - outputLayer->m_neurons[n]->m_value;
		double derivative = TransferFunctionDerivative(outputLayer->m_neurons[n]->m_value, m_function);
		outputLayer->m_neurons[n]->m_gradient = delta * derivative;

		if (PRINT_BP){ printf("Layer: %d, neuron: %d, gradient: %f (delta: %f, derivative: %f)\n", m_nb_layers - 1, n, outputLayer->m_neurons[n]->m_gradient, delta, derivative); }
	}

	//Propagation of the gradient
	for (int i = m_nb_layers - 2; i >= 0; i--)
	{
		Layer* currentLayer = m_layers[i];
		Layer* nextLayer = m_layers[i + 1];

		double sum = 0;
		for (int n = 0; n < currentLayer->m_nb_neurons; n++)
		{
				double delta = 0.f;
				for (int w = 0; w < nextLayer->m_nb_neurons; w++)//sum of the derivatives of the weighted errors
				{
					if (w < nextLayer->m_nb_neurons - 1 || nextLayer->m_type == OutpuLayer)
					{
						delta += currentLayer->m_neurons[n]->m_weights[w] * nextLayer->m_neurons[w]->m_gradient;
					}
				}

				double derivative = TransferFunctionDerivative(currentLayer->m_neurons[n]->m_value, m_function);
				currentLayer->m_neurons[n]->m_gradient = delta * derivative;
				if (PRINT_BP){ printf("Layer: %d, neuron: %d, gradient: %f (delta: %f, derivative: %f)\n", i, n, currentLayer->m_neurons[n]->m_gradient, delta, derivative); }
		}
	}
}

void NeuralNetwork::WeightsUpdate()
{
	//Weights update
	if (PRINT_WU){ printf("Weights update.\n"); }
	for (int i = m_nb_layers - 2; i >= 0; i--)
	{
		Layer* currentLayer = m_layers[i];
		Layer* nextLayer = m_layers[i + 1];

		for (int n = 0; n < currentLayer->m_nb_neurons; n++)
		{
			//update each weight towards the next layer's neurons
			for (int w = 0; w < nextLayer->m_nb_neurons; w++)
			{
				if (w < nextLayer->m_nb_neurons - 1 || nextLayer->m_type == OutpuLayer)
				{
					Neuron* currentNeuron = currentLayer->m_neurons[n];
					Neuron* connectedNeuron = nextLayer->m_neurons[w];

					double newDeltaWeight = currentNeuron->m_value * connectedNeuron->m_gradient * m_learning_rate;
					newDeltaWeight += currentNeuron->m_deltaWeights[w] * m_momentum;

					currentNeuron->m_deltaWeights[w] = newDeltaWeight;
					currentNeuron->m_weights[w] += newDeltaWeight;

					if (PRINT_WU){ printf("Layer: %d, neuron: %d, old weight: %f, new weight: %f (delta: %f).\n", i, n, currentNeuron->m_weights[w] - newDeltaWeight, currentNeuron->m_weights[w], newDeltaWeight); }
				}
			}
		}
	}
}


void NeuralNetwork::FeedBackward(Label label)
{
	if (PRINT_FB){ printf("Feed backward.\n"); }
	double output_value = GetTargetValue(label);

	//Output layer
	Layer* outputLayer = m_layers.back();
	for (int n = 0; n < outputLayer->m_nb_neurons; n++)
	{
		outputLayer->m_neurons[n]->m_input_value = output_value;
		if (PRINT_FB){ printf("Layer: %d, neuron: %d, input value: %f\n", 0, n, outputLayer->m_neurons[n]->m_input_value); }
	}
	
	//Feed Backward
	for (int i = 0; i < m_nb_layers - 1; i++)//output layer doesn't feed forward
	{
		Layer* currentLayer = m_layers[i];
		Layer* nextLayer = m_layers[i + 1];

		for (int n = 0; n < nextLayer->m_nb_neurons; n++)
		{
			double preactivation_value = TransferFunctionInverse(nextLayer->m_neurons[n]->m_input_value, m_function);
			if (n < currentLayer->m_nb_neurons - 1 || currentLayer->m_type == OutpuLayer)//No need to feed bias neuron backward
			{
				double sum_weights = 0;
				for (int w = 0; w < currentLayer->m_nb_neurons; w++)
				{
					sum_weights += currentLayer->m_neurons[w]->m_weights[n];
				}
				for (int w = 0; w < currentLayer->m_nb_neurons; w++)
				{
					currentLayer->m_neurons[w]->m_value = currentLayer->m_neurons[w]->m_weights[n] / sum_weights * preactivation_value;
				}
			}
		}
	}
}

double NeuralNetwork::TransferFunction(double x, FunctionType function)
{
	double output_value = 0.f;

	//LINEAR
	if (function == LINEAR)
	{
		output_value = x;
	}

	//THRESHOLD
	if (function == THRESHOLD)
	{
		if (x > 0.5f)
		{
			output_value = 1.f;
		}
		else
		{
			x = 0.f;
		}
	}

	//SIGMOID
	if (function == SIGMOID)
	{
		output_value = 1.f / (1 + exp(-x));
	}

	//TANH
	if (function == TANH)
	{
		output_value = tanh(x);
	}

	//TANSIG ~ equivalent of TANH but faster to compute
	if (function == TANSIG)
	{
		output_value = (2.f / (1 + exp(-2.f * x))) - 1;
	}

	//RELU
	if (function == RELU)
	{
		output_value = MaxBetweenValues(0, x);
	}

	//LEAKY RELU
	if (function == LEAKY_RELU)
	{
		output_value = MaxBetweenValues(0.1f * x, x);
	}

	return output_value;
}

double NeuralNetwork::TransferFunctionDerivative(double x, FunctionType function)
{
	double output_value = 0.f;

	//LINEAR
	if (function == LINEAR)
	{
		output_value = 1.f;//todo
	}

	//THRESHOLD
	if (function == THRESHOLD)
	{
		return x;//todo
	}

	//SIGMOID
	if (function == SIGMOID)
	{
		output_value = TransferFunction(x, SIGMOID) * (1 - TransferFunction(x, SIGMOID));
	}

	//TANH
	if (function == TANH)
	{		
		output_value = 1.f - tanh(x)*tanh(x);
	}

	//TANSIG ~ equivalent of TANH but faster to compute
	if (function == TANSIG)
	{
		output_value = 1.f - x*x;
	}

	//RELU
	if (function == RELU)
	{
		if (x > 0)
			output_value = 1;
		else
			output_value = 0;
	}

	//LEAKY RELU
	if (function == LEAKY_RELU)
	{
		if (x > 0)
			output_value = 1;
		else
			output_value = 0.1f;
	}

	return output_value;
}

double NeuralNetwork::TransferFunctionInverse(double x, FunctionType function)
{
	double output_value = 0.f;

	//LINEAR
	if (function == LINEAR)
	{
		output_value = x;
	}

	//THRESHOLD
	if (function == THRESHOLD)
	{
		output_value = x;//todo
	}

	//SIGMOID
	if (function == SIGMOID)
	{
		output_value = x;//todo
	}

	//TANH
	if (function == TANH)
	{
		output_value = atanh(x);
	}

	//TANSIG ~ equivalent of TANH but faster to compute
	if (function == TANSIG)
	{
		output_value = x;//todo
	}

	//RELU
	if (function == RELU)
	{
		output_value = x;//todo
	}

	//LEAKY RELU
	if (function == LEAKY_RELU)
	{
		output_value = x;//todo
	}

	return output_value;
}

//DATASET

void NeuralNetwork::CreateDataset()
{
	for (int d = 0; d < DATASET_SIZE; d++)
	{
		int r = RandomizeIntBetweenValues(1, 26);
		
		double feature = (double)r;
		vector<double> v;
		v.push_back(feature);

		if (r == 18 || r == 15 || r == 13 || r == 1 || r == 9 || r == 14)//R.O.M.A.I.N
			m_dataset.push_back(Data(v, IS_ROMAIN));
		else
			m_dataset.push_back(Data(v, IS_NOT_ROMAIN));

		if (d == DATASET_SIZE - 1)
			printf("Dataset created: %d data items.\n", d + 1);
	}
}

bool NeuralNetwork::SaveDatasetIntoFile()
{
	remove(DATASET_FILE);

	ofstream data(DATASET_FILE, ios::in | ios::trunc);

	if (data)
	{
		int nb_features = -1;
		int datasetSize = m_dataset.size();
		for (int d = 0; d < datasetSize; d++)
		{
			if (nb_features < 0)
			{
				nb_features = m_dataset[d].m_features.size();
			}

			data << d << " ";
			for (int i = 0; i < nb_features; i++)
			{
				data << m_dataset[d].m_features[i] << " ";
			}
			data << m_dataset[d].m_label;
			data << endl;
		}
		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for dataset. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::LoadDatasetFromFile()
{
	std::ifstream data(DATASET_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;

		m_dataset.clear();
		int d = 0;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			vector<double> features;
			for (int i = 0; i < NB_FEATURES; i++)
			{
				double feature;
				features.push_back(feature);
			}
			int label;

			ss >> d;
			for (int i = 0; i < NB_FEATURES; i++)
			{
				ss >> features[i];
			}
			ss >> label;

			m_dataset.push_back(Data(features, (Label)label));
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No DATASET SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void NeuralNetwork::BalanceDataset()
{
	vector<Data> dataset_label_A;
	vector<Data> dataset_label_B;

	//sorting examples into 2 categories
	int datasetSize = m_dataset.size();
	for (int d = 0; d < datasetSize; d++)
	{
		if (m_dataset[d].m_label == IS_ROMAIN)
		{
			dataset_label_A.push_back(m_dataset[d]);
		}
		else
		{
			dataset_label_B.push_back(m_dataset[d]);
		}
	}

	m_dataset.clear();

	//putting them back in the dataset alternatively, and cutting everything beyond examples parity
	int dataset_label_A_size = dataset_label_A.size();
	int dataset_label_B_size = dataset_label_B.size();

	for (int d = 0; d < datasetSize; d++)
	{
		if (d >= dataset_label_A_size || d >= dataset_label_B_size)
		{
			break;
		}

		if (d < dataset_label_A_size)
		{
			m_dataset.push_back(dataset_label_A[d]);
		}

		if (d < dataset_label_B_size)
		{
			m_dataset.push_back(dataset_label_B[d]);
		}
	}
}

bool NeuralNetwork::SaveStartingWeightsIntoFile()
{
	ofstream data(RANDOM_WEIGHTS_FILE, ios::in | ios::trunc);

	if (data)
	{
		int weightsSize = m_weightsStart.size();
		for (int i = 0; i < weightsSize; i++)
		{
			data << m_weightsStart[i] << " ";
			data << endl;
		}
		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for random weights. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::LoadWeightsFromFile(string filename)
{
	std::ifstream data(filename, ios::in);

	vector<double> weights;
	if (data) // si ouverture du fichier réussie
	{
		std::string line;

		m_weightsStart.clear();
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			double weight;
			ss >> weight;
			m_weightsStart.push_back(weight);
		}

		RestoreWeights();

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No RANDOM WEIGHTS FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::LoadHyperParametersFromFile(string filename)
{
	std::ifstream data(filename, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;

		int index;
		int activation_function = 0;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			ss >> index >> activation_function >> m_momentum >> m_learning_rate >> m_overall_attempts >> m_loops >> m_success_rate >> m_average_error;
			//m_layers.clear();
			//todo: load hidden layers
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No BEST PERF FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::SavePerfIntoFile()
{
	ofstream data(PERF_RECORDS_FILE, ios::in | ios::trunc);

	if (data)
	{
		//Save all perfs
		data << "index " << "activation_function " << "momentum " << "learning_rate " << "attempts " << "loops " << "success_rate " << "RMSE " << "topology " << endl;
		int perfSize = m_perf_records.size();
		for (int i = 0; i < perfSize; i++)
		{
			data << m_perf_records[i].m_index << " ";
			data << m_perf_records[i].m_function << " ";
			data << m_perf_records[i].m_momentum << " ";
			data << m_perf_records[i].m_learning_rate << " ";
			data << m_perf_records[i].m_overall_attempts << " ";
			data << m_perf_records[i].m_loops << " ";
			data << m_perf_records[i].m_success_rate << " ";
			data << m_perf_records[i].m_average_error << " ";

			for (int j = 0; j < m_perf_records[i].m_hidden_layers.size(); j++)
			{
				data << m_perf_records[i].m_hidden_layers[j] << " ";
			}

			data << endl;
		}

		//Close
		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for perf records. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::SaveBestPerfIntoFile()
{
	ofstream data(PERF_BEST_FILE, ios::in | ios::trunc);

	if (data)
	{
		data << "index " << "activation_function " << "momentum " << "learning_rate " << "attempts " << "loops " << "success_rate " << "RMSE " << endl;
		data << m_best_perf.m_index << " ";
		data << m_best_perf.m_function << " ";
		data << m_best_perf.m_momentum << " ";
		data << m_best_perf.m_learning_rate << " ";
		data << m_best_perf.m_overall_attempts << " ";
		data << m_best_perf.m_loops << " ";
		data << m_best_perf.m_success_rate << " ";
		data << m_best_perf.m_average_error << " ";

		//todo : record topology

		data << endl;
		
		//Close
		data.close();  // on ferme le fichier

		//Output weights
		ofstream data2(PERF_BEST_WEIGHTS_FILE, ios::in | ios::trunc);

		if (data2)
		{
			int weightsSize = m_best_perf.m_weights.size();
			for (int i = 0; i < weightsSize; i++)
			{
				data2 << m_best_perf.m_weights[i] << " ";
				data2 << endl;
			}

			data2 << endl;

			//Close
			data2.close();  // on ferme le fichier

			return true;
		}
		else  // si l'ouverture a échoué
		{
			cerr << "DEBUG: No save file found for best perf weights. A new file is going to be created.\n" << endl;
			return false;
		}
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for best perf. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::IsBetterPerfThanSaveFile(Performance &perf)
{
	std::ifstream data(PERF_BEST_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;

		int index;
		int activation_function = 0;
		double momentum;
		double learning_rate;
		int overall_attempts;
		int loops;
		double success_rate;
		double average_error;

		bool better_perf = false;
		int i = 0;
		while (std::getline(data, line))
		{
			if (i == 0)
			{
				i++;
				continue;//skip first line
			}
			std::istringstream ss(line);

			ss >> index >> activation_function >> momentum >> learning_rate >> overall_attempts >> loops >> success_rate >> average_error;

			if (perf.m_overall_attempts <= overall_attempts)
			{
				if (perf.m_success_rate <= success_rate)
				{
					if (!(perf.m_overall_attempts == overall_attempts && perf.m_success_rate == success_rate))
					{
						better_perf = true;
					}
				}
			}
		}

		data.close();  // on ferme le fichier
		return better_perf;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No BEST PERF FILE found. A new file is going to be created.\n" << endl;
		return true;
	}
}

#define LAYER_MAX_SIZE_X			(WINDOW_RESOLUTION_X - 100)
#define LAYER_MAX_SIZE_Y			(WINDOW_RESOLUTION_Y - 200)
#define NEURON_RADIUS				10
#define WEIGHTS_LINE_THICKNESS		1
#define NETWORK_DISPLAY_OFFSET_X	100
#define NETWORK_DISPLAY_OFFSET_Y	100


void NeuralNetwork::AdjustNeuronDisplayPositions()
{
	int max_nb_neurons = 0;
	for (Layer* layer : m_layers)
	{
		if (layer->m_nb_neurons > max_nb_neurons)
			max_nb_neurons = layer->m_nb_neurons;
	}

	float step_x = LAYER_MAX_SIZE_X / (m_nb_layers == 0 ? 1 : m_nb_layers);
	float step_y = LAYER_MAX_SIZE_Y / (max_nb_neurons - 1);
	float current_step_x = NETWORK_DISPLAY_OFFSET_X;
	float current_step_y = 0;

	string str;
	for (int i = 0; i < m_nb_layers; i++)
	{
		Layer* currentLayer = m_layers[i];

		int layer_nb_neurons = currentLayer->m_type != OutpuLayer && m_use_bias == true ? currentLayer->m_nb_neurons - 1 : currentLayer->m_nb_neurons;
		int max_nb_neurons_ = m_use_bias == true ? max_nb_neurons - 1 : max_nb_neurons;

		if ((max_nb_neurons_ % 2 == 0 && layer_nb_neurons % 2 == 0) || (max_nb_neurons_ % 2 != 0 && layer_nb_neurons % 2 != 0))
			current_step_y = NETWORK_DISPLAY_OFFSET_Y + (max_nb_neurons_ - layer_nb_neurons) / 2 * step_y;
		else
			current_step_y = NETWORK_DISPLAY_OFFSET_Y + step_y * 0.5 + (max_nb_neurons_ - 1 - layer_nb_neurons) / 2 * step_y;

		for (Neuron* neuron : currentLayer->m_neurons)
		{
			neuron->m_circle.setRadius(NEURON_RADIUS);
			neuron->m_circle.setPosition(current_step_x, current_step_y);
			
			//input value
			if (neuron->m_is_bias == false)
			{
				neuron->m_input_text.setCharacterSize(16);
				neuron->m_input_text.setStyle(sf::Text::Style::Regular);
				neuron->m_input_text.setColor(sf::Color::Green);
				neuron->m_input_text.setOrigin(sf::Vector2f(0, 0));
				neuron->m_input_text.setFont(*GlobalResources->m_font);
				str = to_string(neuron->m_input_value);
				neuron->m_input_text.setString(str.substr(0, 5));
				neuron->m_input_text.setPosition(sf::Vector2f(neuron->m_circle.getPosition().x + neuron->m_circle.getRadius() * 0.5 - neuron->m_input_text.getGlobalBounds().width * 0.5, neuron->m_circle.getPosition().y - neuron->m_input_text.getGlobalBounds().height - 10));
			}
			
			//ouput value
			neuron->m_output_text.setCharacterSize(16);
			neuron->m_output_text.setStyle(sf::Text::Style::Regular);
			neuron->m_output_text.setColor(sf::Color::Red);
			neuron->m_output_text.setOrigin(sf::Vector2f(0, 0));
			neuron->m_output_text.setFont(*GlobalResources->m_font);
			str = to_string(neuron->m_value);
			neuron->m_output_text.setString(str.substr(0, 5));
			neuron->m_output_text.setPosition(sf::Vector2f(neuron->m_circle.getPosition().x + neuron->m_circle.getRadius() * 0.5 - neuron->m_output_text.getGlobalBounds().width * 0.5, neuron->m_circle.getPosition().y + neuron->m_circle.getRadius() * 2 + neuron->m_output_text.getGlobalBounds().height));

			current_step_y += step_y;
		}

		current_step_x += step_x;
		current_step_y = 0;
	}

	//weights
	for (int i = 0; i < m_nb_layers; i++)
	{
		Layer* currentLayer = m_layers[i];
		for (Neuron* neuron : currentLayer->m_neurons)
		{
			neuron->m_weight_texts.clear();
			neuron->m_lines.clear();

			int n = 0;
			if (currentLayer->m_type != OutpuLayer)
			{
				Layer* nextLayer = m_layers[i + 1];
				for (Neuron* next_neuron : nextLayer->m_neurons)
				{
					if (next_neuron->m_is_bias == false && currentLayer->m_type != OutpuLayer)
					{
						float x = next_neuron->m_circle.getPosition().x - (neuron->m_circle.getPosition().x + NEURON_RADIUS * 2);
						float y = next_neuron->m_circle.getPosition().y - neuron->m_circle.getPosition().y;

						//weight lines
						sf::RectangleShape line;
						line.setOrigin(sf::Vector2f(0, 0));
						line.setFillColor(sf::Color(0, 0, 0, 100));
						line.setOutlineThickness(0);

						line.setPosition(sf::Vector2f(neuron->m_circle.getPosition().x + NEURON_RADIUS * 2, neuron->m_circle.getPosition().y + NEURON_RADIUS));
						line.setSize(sf::Vector2f(sqrt(x*x + y*y), WEIGHTS_LINE_THICKNESS));

						float angle = atan2(y, x);
						angle *= 180.0 / M_PI;
						if (angle < 0)
							angle += 360;

						line.setRotation(angle);

						neuron->m_lines.push_back(line);

						//weight texts
						sf::Text text;
						text.setCharacterSize(16);
						text.setStyle(sf::Text::Style::Regular);
						text.setColor(sf::Color::Black);
						text.setOrigin(sf::Vector2f(0, 0));
						text.setFont(*GlobalResources->m_font);
						str = to_string(neuron->m_weights[n]);
						text.setString(str.substr(0, 5));
						text.setPosition(sf::Vector2f(next_neuron->m_circle.getPosition().x, next_neuron->m_circle.getPosition().y));

						neuron->m_weight_texts.push_back(text);
					}

					n++;
				}
			}
		}
	}

	//general info
	m_general_info_text.setCharacterSize(20);
	m_general_info_text.setStyle(sf::Text::Style::Regular);
	m_general_info_text.setColor(sf::Color::White);
	m_general_info_text.setOrigin(sf::Vector2f(0, 0));
	m_general_info_text.setFont(*GlobalResources->m_font);
	m_general_info_text.setPosition(sf::Vector2f(10, 10));

}