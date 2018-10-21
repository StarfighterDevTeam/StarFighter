#include "StarFighter.h"

int main()
{
	NeuralNetwork NeuralNetwork;

	//NeuralNetwork.CreateDataset();
	NeuralNetwork.LoadDatasetFromFile();
	NeuralNetwork.MixDataSet();
	//NeuralNetwork.SaveDatasetIntoFile();

	NeuralNetwork.Run();
	
	printf("Exit program?\n");
	cin.get();
	return 0;
}


//NEURAL NETWORK
NeuralNetwork::NeuralNetwork()
{
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

	//Input layer
	AddLayer(NB_FEATURES, InputLayer);

	//Hidden layers
	AddLayer(3, HiddenLayer);

	//Output layer
	AddLayer(NB_LABELS, OutpuLayer);
}

void NeuralNetwork::RestoreWeights()
{
	int index = 0;

	for (int i = 0; i < m_nb_layers; i++)
	{
		for (int n = 0; n < m_layers[i].m_nb_neurons; n++)
		{
			int weightsSize = m_layers[i].m_neurons[n].m_weights.size();
			for (int w = 0; w < weightsSize; w++)
			{
				m_layers[i].m_neurons[n].m_weights[w] = m_weightsStart[index];
				m_layers[i].m_neurons[n].m_deltaWeights[w] = 0;
				index++;
			}
		}
	}
}

void NeuralNetwork::Run()
{
	RestoreWeights();
	m_loops = 0;

	while (m_loops < 150)
	{
		m_loops++;

		//Train on labelled data
		//Supervised training data
		for (int d = 0; d < m_training_dataset.size(); d++)
		{
			m_attempts = 0;
			m_error = NN_ERROR_MARGIN;

			if (PRINT_TR){ printf("\nInput data %d.\n", d); }
			InitInputLayer(m_training_dataset[d]);

			while (m_error >= NN_ERROR_MARGIN && m_attempts < NN_MAX_ATTEMPTS)
			{
				m_attempts++;
				m_overall_attempts++;
				if (PRINT_TR){ printf("\nAttempts: %d (overall attempts: %d).\n", m_attempts, m_overall_attempts); }
				FeedForward();

				ErrorCalculation(m_training_dataset[d]);

				if (m_error < NN_ERROR_MARGIN)
				{
					if (PRINT_TR){ printf("\nSuccess.\n"); }
					m_success++;

					//cin.get();
					break;
				}
				else
				{
					if (PRINT_TR){ printf("\nFail.\n"); }
				}

				BackPropagationGradient(m_training_dataset[d]);

				WeightsUpdate();
			}
		}
	}
		
	//Test and display
	m_success = 0;
	for (int d = 0; d < m_datasetSize; d++)
	{
		InitInputLayer(m_dataset[d]);
		FeedForward();
		ErrorCalculation(m_dataset[d]);

		printf("%d, %d, %d ", (int)m_dataset[d].m_features[0], (int)m_dataset[d].m_features[1], (int)m_dataset[d].m_features[2]);
		Layer &outputLayer = m_layers.back();
		if (outputLayer.m_neurons[0].m_value > 0.f)
		{
			printf(" ----> jaune");

			if (m_dataset[d].m_label != IS_YELLOW)
			{
				printf(" (FAUX)");
			}
			else
			{
				m_success++;
			}
		}
		else
		{
			if (m_dataset[d].m_label == IS_YELLOW)
			{
				printf("         (FAUX)");
			}
			else
			{
				m_success++;
			}
		}
		printf("\n");
	}

	printf("Success rate: %d / %d (%.2f%%).\n", m_success, m_datasetSize, 100.f * m_success / m_datasetSize);
}



void NeuralNetwork::AddLayer(int nb_neuron, LayerType type)
{
	//Connnect previous layer's neurons
	if (!m_layers.empty())
	{
		Layer &previousLayer = m_layers.back();
		int previousLayerSize = previousLayer.m_neurons.size();
		for (int i = 0; i < previousLayerSize; i++)
		{
			for (int j = 0; j < nb_neuron; j++)
			{
				double weight;
				//Load values from file if they exist
				if (m_weightLoadIndex < m_weightsStart.size())
				{
					weight = m_weightsStart[m_weightLoadIndex];
				}
				else
				{
					weight = RandomizeFloatBetweenValues(-1.f, 1.f);
					m_weightsStart.push_back(weight);
					if (j == nb_neuron - 1)
					{
						SaveStartingWeightsIntoFile();
					}
				}

				m_weightLoadIndex++;
				previousLayer.m_neurons[i].m_weights.push_back(weight);
				previousLayer.m_neurons[i].m_deltaWeights.push_back(0.f);
			}
		}
	}

	//Create the new layer
	m_layers.push_back(Layer(nb_neuron, type));
	m_nb_layers++;
}

void NeuralNetwork::Training()
{
	if (PRINT_TR){ printf("\n*** Training. ***\n"); }

	
}

void NeuralNetwork::Testing()
{
	if (PRINT_TE){ printf("\n*** Testing. ***\n"); }

	m_average_error = 0.f;
	m_success = 0;
	m_attempts = 1;

	int attempts = 0;

	//Supervised training data
	for (int d = 0; d < m_datasetSize; d++)
	{
		attempts++;
		if (PRINT_TE){ printf("\nInput data %d (%d, %d, %d).\n", d, (int)m_dataset[d].m_features[0], (int)m_dataset[d].m_features[1], (int)m_dataset[d].m_features[2]); }

		InitInputLayer(m_dataset[d]);
		
		FeedForward();

		ErrorCalculation(m_dataset[d]);

		if (m_error < 1.0f)
		{
			if (PRINT_TE){ printf("Success.\n"); }
			m_success++;
		}
		else
		{
			if (PRINT_TE){ printf("Fail.\n"); }
		}
	}

	m_average_error = m_average_error / attempts;
	m_success_rate = 100.f * m_success / m_datasetSize;
	if (PRINT_LO){ printf("\nTesting data success: %d/%d (%.2f%%).\n", m_success, m_datasetSize, m_success_rate); }
	if (PRINT_LO){ printf("AVERAGE RMSE: %f (target: %f).\n", m_average_error, NN_ERROR_MARGIN); }
}

Label NeuralNetwork::TestSample(Data &data)
{
	InitInputLayer(data);
	FeedForward();
	
	double target_value = 1.f;

	m_error = 0.f;
	Layer &outputLayer = m_layers.back();

	for (int n = 0; n < outputLayer.m_nb_neurons; n++)
	{
		double delta = target_value - outputLayer.m_neurons[n].m_value;
		m_error += delta * delta;
	}
	m_error *= 1.f / outputLayer.m_nb_neurons;
	m_error = sqrt(m_error);//Root Means Square Error

	Label label;
	if (m_error < 0.5f)
	{
		label = IS_YELLOW;
	}
	else
	{
		label = NOT_GREEN;
	}
	return label;

}

void NeuralNetwork::InitInputLayer(const Data &data)
{
	Layer &inputLayer = m_layers.front();

	for (int n = 0; n < inputLayer.m_nb_neurons; n++)
	{
		if (n < inputLayer.m_nb_neurons - 1)
		{
			inputLayer.m_neurons[n].m_value = Data::RGBIntoInput(data.m_features[n]);
			inputLayer.m_neurons[n].m_input_value = inputLayer.m_neurons[n].m_value;
		}
		else
		{
			inputLayer.m_neurons[n].m_value = 1.f;//bias neuron
			inputLayer.m_neurons[n].m_input_value = inputLayer.m_neurons[n].m_value;
		}
	}
}

void NeuralNetwork::FeedForward()
{
	//For display purposes
	if (PRINT_FF){ printf("Feed forward.\n"); }
	Layer &inputLayer = m_layers.front();
	for (int n = 0; n < inputLayer.m_nb_neurons; n++)
	{
		if (PRINT_FF){ printf("Layer: %d, neuron: %d, input value: %f\n", 0, n, inputLayer.m_neurons[n].m_value); }
	}

	//Feed forward
	for (int i = 0; i < m_nb_layers - 1; i++)//output layer doesn't feed forward
	{
		Layer &currentLayer = m_layers[i];
		Layer &nextLayer = m_layers[i + 1];

		for (int n = 0; n < nextLayer.m_nb_neurons; n++)
		{
			if (n < nextLayer.m_nb_neurons - 1 || nextLayer.m_type == OutpuLayer)//Output layer doesn't have a bias neuron
			{
				if (PRINT_FF){ printf("\nSum calculation:\n"); }
				double sum = 0;
				for (int w = 0; w < currentLayer.m_nb_neurons; w++)
				{
					sum += currentLayer.m_neurons[w].m_value * currentLayer.m_neurons[w].m_weights[n];
					if (PRINT_FF){ printf("   value * weight : %f * %f = %f\n", currentLayer.m_neurons[w].m_value, currentLayer.m_neurons[w].m_weights[n], sum); }
				}
				nextLayer.m_neurons[n].m_input_value = sum;
				nextLayer.m_neurons[n].m_value = TransferFunction(sum, m_function);
				if (PRINT_FF){ printf("Layer: %d, neuron: %d, input value: %f -> output value: %f\n", i + 1, n, nextLayer.m_neurons[n].m_input_value, nextLayer.m_neurons[n].m_value); }
			}
			else
			{
				nextLayer.m_neurons[n].m_value = 1.f;//bias neuron
				nextLayer.m_neurons[n].m_input_value = nextLayer.m_neurons[n].m_value;
			}
		}
	}
}

double NeuralNetwork::GetTargetValue(const Label label)
{
	double target_value;
	if (label == IS_YELLOW)
	{
		target_value = 1.f;
	}
	else
	{
		target_value = -1.f;
	}
	return target_value;
}

void NeuralNetwork::ErrorCalculation(const Data &data)
{
	//Error
	if (PRINT_EC){ printf("Error calculation.\n"); }
	double previous_error = m_attempts == 1 ? 0.f : m_error;
	m_error = 0.f;
	Layer &outputLayer = m_layers.back();

	for (int n = 0; n < outputLayer.m_nb_neurons; n++)
	{
		double delta = GetTargetValue(data.m_label) - outputLayer.m_neurons[n].m_value;
		m_error += delta * delta;
		if (PRINT_EC){ printf("Neuron: %d, output: %f, target: %f (delta: %f).\n", n, outputLayer.m_neurons[n].m_value, GetTargetValue(data.m_label), delta); }
	}
	m_error *= 1.f / outputLayer.m_nb_neurons;
	m_error = sqrt(m_error);//Root Means Square Error

	if (m_attempts <= 1)
	{
		if (PRINT_EC){ printf("RMSE: %f (target: %f)\n", m_error, NN_ERROR_MARGIN); }
	}
	else
	{
		if (PRINT_EC){ printf("RMSE: %f (previous RMSE: %f). Progression: %f (%f%%).\n", m_error, previous_error, previous_error - m_error, 100.f*(previous_error - m_error)) / previous_error; }
		if (100.f*(previous_error - m_error) / previous_error < 0)
		{
			printf("<!> Regression in the error. Previous weight update must be wrong.\n");
		}
	}

	m_average_error += m_error;
}

void NeuralNetwork::BackPropagationGradient(const Data &data)
{
	if (PRINT_BP){ printf("Back propagation.\n"); }
	Layer &outputLayer = m_layers.back();

	//Calcuation of the gradient in the output layer
	for (int n = 0; n < outputLayer.m_nb_neurons; n++)
	{
		double delta = GetTargetValue(data.m_label) - outputLayer.m_neurons[n].m_value;
		double derivative = TransferFunctionDerivative(outputLayer.m_neurons[n].m_value, m_function);
		outputLayer.m_neurons[n].m_gradient = delta * derivative;

		if (PRINT_BP){ printf("Layer: %d, neuron: %d, gradient: %f (delta: %f, derivative: %f)\n", m_nb_layers - 1, n, outputLayer.m_neurons[n].m_gradient, delta, derivative); }
	}

	//Propagation of the gradient
	for (int i = m_nb_layers - 2; i >= 0; i--)
	{
		Layer &currentLayer = m_layers[i];
		Layer &nextLayer = m_layers[i + 1];

		double sum = 0;
		for (int n = 0; n < currentLayer.m_nb_neurons; n++)
		{
				double delta = 0.f;
				for (int w = 0; w < nextLayer.m_nb_neurons; w++)//sum of the derivatives of the weighted errors
				{
					if (w < nextLayer.m_nb_neurons - 1 || nextLayer.m_type == OutpuLayer)
					{
						delta += currentLayer.m_neurons[n].m_weights[w] * nextLayer.m_neurons[w].m_gradient;
					}
				}

				double derivative = TransferFunctionDerivative(currentLayer.m_neurons[n].m_value, m_function);
				currentLayer.m_neurons[n].m_gradient = delta * derivative;
				if (PRINT_BP){ printf("Layer: %d, neuron: %d, gradient: %f (delta: %f, derivative: %f)\n", i, n, currentLayer.m_neurons[n].m_gradient, delta, derivative); }
		}
	}
}

void NeuralNetwork::WeightsUpdate()
{
	//Weights update
	if (PRINT_WU){ printf("Weights update.\n"); }
	for (int i = m_nb_layers - 2; i >= 0; i--)
	{
		Layer &currentLayer = m_layers[i];
		Layer &nextLayer = m_layers[i + 1];

		for (int n = 0; n < currentLayer.m_nb_neurons; n++)
		{
			//update each weight towards the next layer's neurons
			for (int w = 0; w < nextLayer.m_nb_neurons; w++)
			{
				if (w < nextLayer.m_nb_neurons - 1 || nextLayer.m_type == OutpuLayer)
				{
					Neuron &currentNeuron = currentLayer.m_neurons[n];
					Neuron &connectedNeuron = nextLayer.m_neurons[w];

					double newDeltaWeight = currentNeuron.m_value * connectedNeuron.m_gradient * m_learning_rate;
					//momentum applies from the second try on
					if (m_attempts > 1)
					{
						newDeltaWeight += currentNeuron.m_deltaWeights[w] * m_momentum;
					}

					currentNeuron.m_deltaWeights[w] = newDeltaWeight;
					currentNeuron.m_weights[w] += newDeltaWeight;

					if (PRINT_WU){ printf("Layer: %d, neuron: %d, old weight: %f, new weight: %f (delta: %f).\n", i, n, currentNeuron.m_weights[w] - newDeltaWeight, currentNeuron.m_weights[w], newDeltaWeight); }
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
		int r = RandomizeIntBetweenValues(0, 1);
		if (r % 2 == 0)
		{
			m_dataset.push_back(Data(IS_YELLOW, NN_ERROR_MARGIN));
		}
		else
		{
			m_dataset.push_back(Data(NOT_GREEN, NN_ERROR_MARGIN));
		}

		if (d == DATASET_SIZE - 1)
		{
			printf("Dataset created: %d data items.\n", d + 1);
		}
	}

	m_datasetSize = DATASET_SIZE;
}

void NeuralNetwork::MixDataSet()
{
	vector<Data> yellow_data;
	vector<Data> not_yellow_data;

	for (int d = 0; d < m_datasetSize; d++)
	{
		if (m_dataset[d].m_label == IS_YELLOW)
		{
			yellow_data.push_back(m_dataset[d]);
		}
		else
		{
			not_yellow_data.push_back(m_dataset[d]);
		}
	}

	for (int d = 0; d < m_datasetSize; d++)
	{
		m_training_dataset.push_back(yellow_data[d % yellow_data.size()]);
		
		m_training_dataset.push_back(not_yellow_data[d % not_yellow_data.size()]);
	
		if (d >= yellow_data.size() - 1 && d >= not_yellow_data.size() - 1)
		{
			break;
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
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No save file found for random weights. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::LoadWeightsFromFile(string filename)
{
	std::ifstream data(filename, ios::in);

	vector<double> weights;
	if (data) // si ouverture du fichier r�ussie
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
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No RANDOM WEIGHTS FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::LoadDatasetFromFile()
{
	std::ifstream data(DATASET_FILE, ios::in);

	if (data) // si ouverture du fichier r�ussie
	{
		std::string line;

		m_dataset.clear();
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			vector<double> features;
			for (int i = 0; i < NB_FEATURES; i++)
			{
				double feature;
				features.push_back(feature);
			}
			int label_flag;

			for (int i = 0; i < NB_FEATURES; i++)
			{
				ss >> features[i];
			}
			ss >> label_flag;
			Label label = label_flag == 1 ? IS_YELLOW : NB_LABELS;

			m_dataset.push_back(Data(features, label));
		}

		m_datasetSize = m_dataset.size();

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No DATASET SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}