#include "StarFighter.h"

int main()
{
	NeuralNetwork NeuralNetwork;

	//NeuralNetwork.CreateDataset();
	NeuralNetwork.LoadDatasetFromFile();
	//NeuralNetwork.SaveDatasetIntoFile();

	int mode = 0;
	while (mode >= 0)
	{
		printf("\nChoose run mode for the Neural Network:\n\n");
		printf("0 = perform from scratch with given parameters\n");
		printf("1 = loop to find the best hyper parameters\n");
		printf("2 = load best-known weights and hyperparameters and iterate to improve weights\n");
		printf("3 = load best known parameters and weights and test the model\n\n");

		cin >> mode;
		NeuralNetwork.Run((NeuralNetworkMode)mode);
	}
	
	
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
	m_function = LEAKY_RELU;

	//Input layer
	AddLayer(NB_FEATURES, InputLayer);

	//Hidden layers
	AddLayer(1, HiddenLayer);
	//AddLayer(2, HiddenLayer);

	//Output layer
	AddLayer(NB_LABELS, OutpuLayer);
}

void NeuralNetwork::RestoreWeights(vector<double>& weights)
{
	int index = 0;

	for (int i = 0; i < m_nb_layers; i++)
	{
		for (int n = 0; n < m_layers[i].m_nb_neurons; n++)
		{
			int weightsSize = m_layers[i].m_neurons[n].m_weights.size();
			for (int w = 0; w < weightsSize; w++)
			{
				m_layers[i].m_neurons[n].m_weights[w] = weights[index];
				index++;
			}
		}
	}
}

void NeuralNetwork::CopyWeightsInto(vector<double>& weights)
{
	for (int i = 0; i < m_nb_layers; i++)
	{
		for (int n = 0; n < m_layers[i].m_nb_neurons; n++)
		{
			int weightsSize = m_layers[i].m_neurons[n].m_weights.size();
			for (int w = 0; w < weightsSize; w++)
			{
				weights.push_back(m_layers[i].m_neurons[n].m_weights[w]);
			}
		}
	}
}

void NeuralNetwork::Run(NeuralNetworkMode mode)
{
	// ******* Mode 0 *******
	if (mode == PerfFromScratch)
	{
		m_function = NN_ACTIVATION_FUNCTION;
		m_learning_rate = NN_LEARNING_RATE;
		m_momentum = NN_MOMENTUM;

		m_success_rate = 0.f;
		m_average_error = 0.f;
		m_loops = 0;
		m_overall_attempts = 0;

		while (m_success_rate < 100.f && m_overall_attempts < NN_MAX_OVERALL_ATTEMPTS)
		{
			m_loops++;

			//Train on labelled data
			Training();

			//Test model on known data
			Testing();
		}

		RecordPerf();

		//while (this->DoNothing()){}// <<< put breakpoint here to read values
	}

	// ******* Mode 1 *******
	if (mode == LearnHyperparameters)
	{
		m_function = NN_ACTIVATION_FUNCTION;
		while (m_function < NB_FUNCTIONS)
		{
			m_momentum = NN_MOMENTUM;
			while (m_momentum < 0.6f)
			{
				m_learning_rate = NN_LEARNING_RATE;

				while (m_learning_rate < 1.1f)
				{
					if (!m_perf_records.empty())
					{
						RestoreWeights(m_weightsStart);
					}

					m_success_rate = 0.f;
					m_average_error = 0.f;
					m_loops = 0;
					m_overall_attempts = 0;

					while (m_success_rate < 100.f && m_overall_attempts < NN_MAX_OVERALL_ATTEMPTS)
					{
						m_loops++;

						//Train on labelled data
						Training();

						//Test model on known data
						Testing();
					}

					RecordPerf();

					m_learning_rate += 0.1;
				}

				m_momentum += 0.1;
			}

			m_function = (FunctionType)((int)(m_function) + 1);
		}

		//while (this->DoNothing()){}// <<< put breakpoint here to read values
	}

	// ******* Mode 2 *******
	if (mode == ImproveWeights)
	{
		LoadHyperParametersFromFile(PERF_BEST_FILE);
		LoadWeightsFromFile(PERF_BEST_WEIGHTS_FILE);

		m_average_error = 0.f;
		m_overall_attempts = 0;

		//Test model on known data
		Testing();

		RecordPerf();

		//while (this->DoNothing()){}// <<< put breakpoint here to read values
	}

	// ******* Mode 3 *******
	if (mode == Prod)
	{
		LoadHyperParametersFromFile(PERF_BEST_FILE);
		LoadWeightsFromFile(PERF_BEST_WEIGHTS_FILE);

		m_average_error = 0.f;
		m_overall_attempts = 0;

		//Test model on known data
		Testing();

		//while (this->DoNothing()){}// <<< put breakpoint here to read values
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
	for (int i = 0; i < m_nb_layers; i++)
	{
		if (i > 0 && i < m_nb_layers - 1)
		{
			perf.m_hidden_layers.push_back(m_layers[i].m_nb_neurons - 1);//-1 for the bias neuron
		}
	}
	CopyWeightsInto(perf.m_weights);

	//Save perf
	m_perf_records.push_back(perf);
	SavePerfIntoFile();
	if (UpdateBestPerf())
	{
		printf("New best perf: loops: %d, attempts: %d, success rate: %f, RMSE: %f (target: %f) [perf index: %d]\n", m_loops, m_overall_attempts, m_success_rate, m_average_error, NN_ERROR_MARGIN, m_perf_records.size() - 1);
		SaveBestPerfIntoFile();
		return true;
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
		if (min_attempts < 0 || m_perf_records[i].m_overall_attempts < min_attempts)
		{
			if (m_success_rate < 0 || m_perf_records[i].m_success_rate > max_success_rate)
			{
				min_attempts = m_perf_records[i].m_overall_attempts;
				max_success_rate = m_perf_records[i].m_success_rate;
				best_perf = i;
			}
		}
	}

	m_best_perf = m_perf_records[best_perf];

	return best_perf == perfSize - 1;//return true if the latest perf is the best one
}

Data::Data(vector<double> features, Label label)
{
	m_features = features;
	m_label = label;
}

Data::Data(Label label)
{
	int red, blue, green = 0;

	if (label == IS_GREEN)
	{
		red = RandomizeIntBetweenValues(0, (int)(NN_ERROR_MARGIN * 255));
		green = RandomizeIntBetweenValues((int)(255 * (1 - NN_ERROR_MARGIN)), 255);
		blue = RandomizeIntBetweenValues(0, (int)(NN_ERROR_MARGIN * 255));
	}
	else if (label == NOT_GREEN)
	{
		red = RandomizeIntBetweenValues(0, 255);
		green = RandomizeIntBetweenValues(0, (int)((1 - NN_ERROR_MARGIN) * 255));
		blue = RandomizeIntBetweenValues(0, 255);
	}
	else
	{
		int r = RandomizeIntBetweenValues(0, 1);
		if (r % 2 == 0)
		{
			//green examples
			red = RandomizeIntBetweenValues(0, (int)(NN_ERROR_MARGIN * 255));
			green = RandomizeIntBetweenValues((int)(255 * (1 - NN_ERROR_MARGIN)), 255);
			blue = RandomizeIntBetweenValues(0, (int)(NN_ERROR_MARGIN * 255));
		}
		else
		{
			//not green examples
			red = RandomizeIntBetweenValues(0, 255);
			green = RandomizeIntBetweenValues(0, (int)((1 - NN_ERROR_MARGIN) * 255));
			blue = RandomizeIntBetweenValues(0, 255);
		}
	}

	//Normalize data between -1 and 1
	double r = (1.f * red / 255 * 2) - 1;
	double g = (1.f * green / 255 * 2) - 1;
	double b = (1.f * blue / 255 * 2) - 1;

	m_features.push_back(r);
	m_features.push_back(g);
	m_features.push_back(b);

	m_label = label;

	//if (label != UNLABELLED)
	//{
	//	printf("Data created: %d, %d, %d, labelled as %s.\n", red, green, blue, NeuralNetwork::GetLabelString(label).c_str());
	//}
	//else
	//{
	//	printf("Data created: %d, %d, %d, unlabelled.\n", red, green, blue);
	//}
}

Layer::Layer(int nb_neuron, LayerType type)
{
	for (int i = 0; i < nb_neuron; i++)
	{
		m_neurons.push_back(Neuron());
	}
	m_nb_neurons = nb_neuron;

	if (type != OutpuLayer)//bias neuron
	{
		m_neurons.push_back(Neuron());
		m_neurons.back().m_value = 1.f;
		m_nb_neurons++;
	}

	m_type = type;
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
						SaveWeightsIntoFile();
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

	m_success = 0;
	int training_attempts = 0;
	//Supervised training data
	while (m_success_rate < 100.f && training_attempts < NN_MAX_ATTEMPTS)
	{
		training_attempts++;

		for (int d = 0; d < DATASET_SUPERVISED_LOT; d++)
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
					if (PRINT_TR){ printf("Success.\n"); }
					m_success++;
					break;
				}
				else
				{
					if (PRINT_TR){ printf("Fail.\n"); }
				}

				BackPropagationGradient(m_dataset[d]);

				WeightsUpdate();
			}
		}

		m_success_rate = 100.f * m_success / DATASET_SUPERVISED_LOT;
	}
}

void NeuralNetwork::Testing()
{
	if (PRINT_TE){ printf("\n*** Testing. ***\n"); }

	m_average_error = 0.f;
	m_success = 0;
	m_attempts = 0;

	//Supervised training data
	for (int d = DATASET_SUPERVISED_LOT; d < DATASET_SIZE; d++)
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
	if (PRINT_TE){ printf("\nTesting data success: %d/%d (%.2f%%).\n", m_success, DATASET_TESTING_LOT, m_success_rate); }
	if (PRINT_TE){ printf("AVERAGE RMSE: %f (target: %f).\n", m_average_error, NN_ERROR_MARGIN); }
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
		label = IS_GREEN;
	}
	else
	{
		label = NOT_GREEN;
	}
	return label;

}

void NeuralNetwork::Creating()
{
	printf("\n*** Creating. ***\n");
	printf("Enter 0 for green, 1 for NOT-green.\n");
	int user_input;
	cin >> user_input;
	if (user_input == IS_GREEN)
	{

	}

}

void NeuralNetwork::InitInputLayer(const Data &data)
{
	Layer &inputLayer = m_layers.front();

	for (int n = 0; n < inputLayer.m_nb_neurons; n++)
	{
		if (n < inputLayer.m_nb_neurons - 1)
		{
			inputLayer.m_neurons[n].m_value = data.m_features[n];
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
		if (PRINT_FF){ printf("Layer: %d, neuron: %d, value: %f\n", 0, n, inputLayer.m_neurons[n].m_value); }
	}
	
	//Feed forward
	for (int i = 0; i < m_nb_layers - 1; i++)//output layer doesn't feed forward
	{
		Layer &currentLayer = m_layers[i];
		Layer &nextLayer = m_layers[i +1];

		for (int n = 0; n < nextLayer.m_nb_neurons; n++)
		{
			if (n < nextLayer.m_nb_neurons - 1 || nextLayer.m_type == OutpuLayer)//Output layer doesn't have a bias neuron
			{
				double sum = 0;
				for (int w = 0; w < currentLayer.m_nb_neurons; w++)
				{
					sum += currentLayer.m_neurons[w].m_value * currentLayer.m_neurons[w].m_weights[n];
					//printf("value * weight : %f * %f = %f\n", currentLayer.m_neurons[w].m_value, currentLayer.m_neurons[w].m_weights[n], sum);
				}
				nextLayer.m_neurons[n].m_input_value = sum;
				nextLayer.m_neurons[n].m_value = TransferFunction(sum, m_function);
				if (PRINT_FF){ printf("Layer: %d, neuron: %d, input: %f, value: %f\n", i + 1, n, nextLayer.m_neurons[n].m_input_value, nextLayer.m_neurons[n].m_value); }
			}
			else
			{
				nextLayer.m_neurons[n].m_value = 1.f;//bias neuron
				nextLayer.m_neurons[n].m_input_value = nextLayer.m_neurons[n].m_value;
			}
		}
	}
}

double NeuralNetwork::GetTargetValue(const Data &data)
{
	double target_value;
	if (data.m_label == IS_GREEN)
	{
		target_value = 1.f;
	}
	else
	{
		target_value = 0.f;
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
		double delta = GetTargetValue(data) - outputLayer.m_neurons[n].m_value;
		m_error += delta * delta;
		if (PRINT_EC){ printf("Neuron: %d, output: %f, target: %f (delta: %f).\n", n, outputLayer.m_neurons[n].m_value, GetTargetValue(data), delta); }
	}
	m_error *= 1.f / outputLayer.m_nb_neurons;
	m_error = sqrt(m_error);//Root Means Square Error
	
	if (m_attempts <= 1)
	{
		if (PRINT_EC){ printf("RMSE: %f (target: %f)\n", m_error, NN_ERROR_MARGIN);}
	}
	else
	{
		if (PRINT_EC){ printf("RMSE: %f (previous RMSE: %f). Progression: %f (%f%%).\n", m_error, previous_error, previous_error - m_error, (previous_error - m_error) / previous_error); }
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
		double delta = GetTargetValue(data) - outputLayer.m_neurons[n].m_value;
		outputLayer.m_neurons[n].m_gradient = delta * TransferFunctionDerivative(outputLayer.m_neurons[n].m_value, m_function);

		if (PRINT_BP){ printf("Layer: %d, neuron: %d, gradient: %f\n", m_nb_layers - 1, n, outputLayer.m_neurons[n].m_gradient); }
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

				currentLayer.m_neurons[n].m_gradient = delta * TransferFunctionDerivative(currentLayer.m_neurons[n].m_value, m_function);
				if (PRINT_BP){ printf("Layer: %d, neuron: %d, gradient: %f\n", i, n, currentLayer.m_neurons[n].m_gradient); }
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
					newDeltaWeight += currentNeuron.m_deltaWeights[w] * m_momentum;

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

//DATASET

void NeuralNetwork::CreateDataset()
{
	for (int d = 0; d < DATASET_SIZE; d++)
	{

		int r = RandomizeIntBetweenValues(0, 1);
		if (r % 2 == 0)
		{
			m_dataset.push_back(Data(IS_GREEN));
		}
		else
		{
			m_dataset.push_back(Data(NOT_GREEN));
		}

		if (d == DATASET_SIZE - 1)
		{
			printf("Dataset created: %d data items.\n", d + 1);
		}
	}
}

bool NeuralNetwork::SaveDatasetIntoFile()
{
	ofstream data(DATASET_FILE, ios::in | ios::trunc);

	if (data)
	{
		int nb_features = -1;
		for (int d = 0; d < DATASET_SIZE; d++)
		{
			Data &current_data = m_dataset[d];
			if (nb_features < 0)
			{
				nb_features = current_data.m_features.size();
			}

			data << d << " ";
			for (int i = 0; i < nb_features; i++)
			{
				data << current_data.m_features[i] << " ";
			}
			data << current_data.m_label;
			data << endl;
		}
		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No save file found for dataset. A new file is going to be created.\n" << endl;
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

			if (d >= DATASET_SIZE - 1)
			{
				break;
			}
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No DATASET SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::SaveWeightsIntoFile()
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

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No RANDOM WEIGHTS FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool NeuralNetwork::LoadHyperParametersFromFile(string filename)
{
	std::ifstream data(filename, ios::in);

	if (data) // si ouverture du fichier r�ussie
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
	else  // si l'ouverture a �chou�
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
	else  // si l'ouverture a �chou�
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
		data << "index " << "activation_function " << "momentum " << "learning_rate " << "attempts " << "loops " << "success_rate " << "RMSE " << "topology " << endl;
		data << m_best_perf.m_index << " ";
		data << m_best_perf.m_function << " ";
		data << m_best_perf.m_momentum << " ";
		data << m_best_perf.m_learning_rate << " ";
		data << m_best_perf.m_overall_attempts << " ";
		data << m_best_perf.m_loops << " ";
		data << m_best_perf.m_success_rate << " ";
		data << m_best_perf.m_success_rate << " ";

		for (int j = 0; j < m_best_perf.m_hidden_layers.size(); j++)
		{
			data << m_best_perf.m_hidden_layers[j] << " ";
		}

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
		else  // si l'ouverture a �chou�
		{
			cerr << "DEBUG: No save file found for best perf weights. A new file is going to be created.\n" << endl;
			return false;
		}
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No save file found for best perf. A new file is going to be created.\n" << endl;
		return false;
	}
}