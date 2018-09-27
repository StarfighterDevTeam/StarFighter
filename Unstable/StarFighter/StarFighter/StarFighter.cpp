#include "StarFighter.h"

int main()
{
	NeuralNetwork NeuralNetwork;

	//Input labelled data
	NeuralNetwork.CreateDataset();

	//Train on labelled data
	NeuralNetwork.Training();

	//Wait
	bool dejavu = false;
	while (1)
	{
		if (dejavu = false)
		{
			printf("End\n");
			dejavu = true;
		}
	}

	return 0;
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
		red = RandomizeIntBetweenValues(0, (int)(NEURAL_NETWORK_ERROR_MARGIN * 255));
		green = RandomizeIntBetweenValues((int)(255 * (1 - NEURAL_NETWORK_ERROR_MARGIN)), 255);
		blue = RandomizeIntBetweenValues(0, (int)(NEURAL_NETWORK_ERROR_MARGIN * 255));
	}
	else if (label == NOT_GREEN)
	{
		red = RandomizeIntBetweenValues(0, 255);
		green = RandomizeIntBetweenValues(0, (int)((1 - NEURAL_NETWORK_ERROR_MARGIN) * 255));
		blue = RandomizeIntBetweenValues(0, 255);
	}
	else
	{
		int r = RandomizeIntBetweenValues(0, 1);
		if (r % 2 == 0)
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


float Neuron::RandomizeWeight()
{
	float random_weight = RandomizeFloatBetweenValues(0.f, 1.f);
	return random_weight;
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


//NEURAL NETWORK
NeuralNetwork::NeuralNetwork()
{
	m_nb_layers = 0;
	m_attempts = 0;
	m_error = 0.f;

	m_learning_rate = NEURAL_NETWORK_LEARNING_RATE;
	m_momentum = NEURAL_NETWORK_MOMENTUM;
	m_function = TANH;

	//Input layer
	AddLayer(NB_FEATURES, InputLayer);

	//Hidden layers
	AddLayer(3, HiddenLayer);
	AddLayer(2, HiddenLayer);

	//Output layer
	AddLayer(NB_LABELS, OutpuLayer);
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
				double weight = Neuron::RandomizeWeight();
				previousLayer.m_neurons[i].m_weights.push_back(weight);
				previousLayer.m_neurons[i].m_deltaWeights.push_back(0.f);
			}
		}
	}

	//Create the new layer
	m_layers.push_back(Layer(nb_neuron, type));
	m_nb_layers++;
}

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
		
		//{
		//	m_dataset.push_back(Data(UNLABELLED));
		//}

		if (d == DATASET_SIZE - 1)
		{
			printf("Dataset created: %d data items.\n", d + 1);
		}
	}
}

void NeuralNetwork::Training()
{
	//Supervised training data
	for (int d = 0; d < DATASET_SUPERVISED_LOT; d++)
	{
		m_attempts = 0;
		InitInputLayer(m_dataset[d]);

		while (m_error > NEURAL_NETWORK_ERROR_MARGIN || m_attempts < 1000)
		{
			m_attempts++;
			printf("\nAttempt n°: %d.\n", m_attempts);
			FeedForward();

			ErrorCalculation(m_dataset[d]);

			GradientBackPropagation();

			WeightsUpdate();
		}
	}

	this->DoNothing();
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
	printf("Feed forward.\n");
	Layer &inputLayer = m_layers.front();
	for (int n = 0; n < inputLayer.m_nb_neurons; n++)
	{
		printf("Layer: %d, neuron: %d, value: %f\n", 0, n, inputLayer.m_neurons[n].m_value);
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
				printf("Layer: %d, neuron: %d, input: %f, value: %f\n", i + 1, n, nextLayer.m_neurons[n].m_input_value, nextLayer.m_neurons[n].m_value);
			}
			else
			{
				m_layers[0].m_neurons[n].m_value = 1.f;//bias neuron
			}
		}
	}
}

void NeuralNetwork::ErrorCalculation(const Data &data)
{
	//Target value
	double target_value;
	if (data.m_label == IS_GREEN)
	{
		target_value = 1.f;
	}
	else
	{
		target_value = 0.f;
	}

	//Error
	printf("Error calculation.\n");
	m_error = 0.f;
	Layer &outputLayer = m_layers.back();

	for (int n = 0; n < outputLayer.m_nb_neurons; n++)
	{
		double delta = target_value - outputLayer.m_neurons[n].m_value;
		outputLayer.m_neurons[n].m_gradient = delta * TransferFunctionDerivative(outputLayer.m_neurons[n].m_value, m_function);
		m_error += delta * delta;
		printf("Neuron: %d, output: %f, target: %f (delta: %f).\n", n, outputLayer.m_neurons[n].m_value, target_value, delta);
	}
	m_error *= 1.f / outputLayer.m_nb_neurons;
	m_error = sqrt(m_error);//Root Means Square Error

	printf("RMSE: %f\n", m_error);
}

void NeuralNetwork::GradientBackPropagation()
{
	printf("Back propagation.\n");

	Layer &outputLayer = m_layers.back();

	//For display purposes
	for (int n = 0; n < outputLayer.m_nb_neurons; n++)
	{
		//Calculation already made in NeuralNetwork::ErrorCalculation(const Data &data)
		printf("Layer: %d, neuron: %d, gradient: %f\n", m_nb_layers - 1, n, outputLayer.m_neurons[n].m_gradient);
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

				currentLayer.m_neurons[n].m_gradient = delta * TransferFunctionDerivative(currentLayer.m_neurons[n].m_input_value, m_function);
				printf("Layer: %d, neuron: %d, gradient: %f\n", i, n, currentLayer.m_neurons[n].m_gradient);
		}
	}
}

void NeuralNetwork::WeightsUpdate()
{
	//Weights update
	printf("Weights update.\n");
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
					printf("Layer: %d, neuron: %d, old weight: %f, new weight: %f (delta: %f).\n", i, n, currentNeuron.m_weights[w] - newDeltaWeight, currentNeuron.m_weights[w], newDeltaWeight);
				}
			}
		}
	}
}

/*

void NeuralNetwork::Train()
{
	double learning_rate = NEURAL_NETWORK_LEARNING_RATE;

	for (int d = 0; d < DATASET_SIZE; d++)
	{
		Label input_label;
		if (d < DATASET_SUPERVISED_LOT)
		{
			input_label = m_labelled_dataset[d].m_label;
		}
		else if (d < DATASET_SUPERVISED_LOT + DATASET_TESTING_LOT)
		{
			input_label = m_labelled_dataset[d].m_label;
			printf("\nTesting model on labelled data: %d/%d.\n", d - DATASET_SUPERVISED_LOT, DATASET_TESTING_LOT - 1);
		}
		else
		{
			break;//todo : test unlabelled examples
		}

		double error = -1;
		double previous_error = -1;
		double learning_rate = NEURAL_NETWORK_LEARNING_RATE;
		int attempt = 0;

		while (abs(error) > NEURAL_NETWORK_ERROR_MARGIN)
		{
			attempt++;
			previous_error = error;
			printf("\nAttempt %d\n", attempt);

			//Convert data into Input features between 0 and 1
			for (int f = 0; f < NB_FEATURES; f++)
			{
				//clamping values on entropy and feeding them into the Input layer
				if (d < DATASET_SUPERVISED_LOT)
				{
					m_input_layer[f]->m_value = Lerp(m_labelled_dataset[d].m_features[f], 0.f, 255.f, 0.f, 1.f);
				}
				else
				{
					m_input_layer[f]->m_value = Lerp(m_unlabelled_dataset[d - DATASET_SUPERVISED_LOT].m_features[f], 0.f, 255.f, 0.f, 1.f);
				}
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
				printf("Feed-forward|Hidden %d->Hidden %d:\n", i, i + 1);
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					printf("Neuron %d: ", j);
					m_hidden_layers[i][j]->m_value = 0;
					for (int k = 0; k < NEURAL_NETWORK_HEIGHT; k++)
					{
						m_hidden_layers[i][j]->m_value += m_hidden_layers[i - 1][k]->m_value * m_hidden_layers[i - 1][k]->m_weight[j];
						printf("%.3f * %.3f", m_hidden_layers[i - 1][k]->m_value, m_hidden_layers[i - 1][k]->m_weight[j]);
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
			printf("Feed-forward|Hidden %d->Output:\n", NEURAL_NETWORK_DEPTH - 1);
			m_output_layer[0]->m_value = 0;
			for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
			{
				m_output_layer[0]->m_value += m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_value * m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0];
				printf("%.3f * %.3f", m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_value, m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0]);
				if (j < NEURAL_NETWORK_HEIGHT - 1)
					printf(" + ");
			}
			printf(" = %.3f", m_output_layer[0]->m_value);

			//m_output_layer[0]->m_value *= (1.f / NEURAL_NETWORK_HEIGHT);
			m_output_layer[0]->m_value = ActivationFunction(m_output_layer[0]->m_value);
			printf(" |->activation: %.3f\n", m_output_layer[0]->m_value);

			//Label recognition
			if (d < DATASET_SUPERVISED_LOT)
			{
				if (input_label == IS_GREEN)
				{
					error = 1 - m_output_layer[0]->m_value;
				}
				else if (input_label == NOT_GREEN)
				{
					error = -m_output_layer[0]->m_value;
				}
			}
			else if (d < DATASET_SUPERVISED_LOT + DATASET_TESTING_LOT)
			{
				//Control of predicted labels
				if (input_label == IS_GREEN)
				{
					if (m_output_layer[0]->m_value > 1 - NEURAL_NETWORK_ERROR_MARGIN && m_output_layer[0]->m_value < 1 + NEURAL_NETWORK_ERROR_MARGIN)
					{
						printf("\nTest data %d labelled as %s = SUCCESS.\n\n", d, GetLabelString(m_labelled_dataset[d].m_label).c_str());
						break;
					}
					else
					{
						printf("\nTest data %d wrongfully labelled as %s = FAIL. Rebooting supervised data evaluation.\n\n", d, "NOT_GREEN");
						//error = - m_output_layer[0]->m_value;
						d = 0;
						break;
					}
				}
				else if (input_label == NOT_GREEN)
				{
					if (abs(m_output_layer[0]->m_value) < NEURAL_NETWORK_ERROR_MARGIN)
					{
						printf("\nTest data %d labelled as %s = SUCCESS.\n\n", d, GetLabelString(m_labelled_dataset[d].m_label).c_str());
						break;
					}
					else
					{
						printf("\nTest data %d wrongfully labelled as %s = FAIL. Rebooting supervised data evaluation.\n\n", d, "IS_GREEN");
						d = 0;
						break;
						//error = 1 - m_output_layer[0]->m_value;
					}
				}
			}

			//Success?
			if (abs(error) < NEURAL_NETWORK_ERROR_MARGIN)
			{
				printf("\nSupervised data %d successfully recognized as %s (%d attempt(s)).\n\n", d, GetLabelString(input_label).c_str(), attempt);
				break;
			}
			//Error backpropagation
			else
			{
				m_output_layer[0]->m_error = error;

				//Learning rate moderation
				if (abs(error) < abs(previous_error))
				{
					learning_rate *= (1 + NEURAL_NETWORK_MOMENTUM);
					printf("\nERROR decreased: %f (Previous error: %f). Learning rate increased: %f\n\n", error, previous_error, learning_rate);
				}
				else
				{
					RestorePreviousWeight();
					if (learning_rate > NEURAL_NETWORK_LEARNING_RATE)
					{
						learning_rate = NEURAL_NETWORK_LEARNING_RATE;
					}
					else
					{
						learning_rate *= (1 - NEURAL_NETWORK_MOMENTUM);
					}
					printf("\nERROR increased <!>: %f (Previous error: %f). Restoring old weights. Learning rate decreased: %f\n\n", error, previous_error, learning_rate);
					continue;
				}

				//Hidden layers -> output layer
				printf("Backpropagation|Output->Hidden %d:\n", NEURAL_NETWORK_DEPTH - 1);
				double total_weight = 0;
				double previous_weight[NEURAL_NETWORK_HEIGHT];
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					previous_weight[j] = m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0];
					total_weight += previous_weight[j];
				}
				for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
				{
					double synaptic_weight = total_weight == 0 ? 0 : previous_weight[j] / total_weight;

					m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_error = m_output_layer[0]->m_error * synaptic_weight * learning_rate;

					double new_weight = previous_weight[j] * (1 + m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_error);
					new_weight = Bound(new_weight, 0.f, 1.f);
					m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_previous_weight[0] = m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0];
					m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[0] = new_weight;
					printf("weight: %d, neuron output, old_weight: %.3f | synaptic: %.1f | parent_error: %.3f | error: %.3f | new_weight: %.3f\n", j, previous_weight[j], synaptic_weight, m_output_layer[0]->m_error, m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_error, new_weight);
				}

				//Hidden layers
				for (int i = NEURAL_NETWORK_DEPTH - 1; i > 0; i--)
				{
					printf("Backpropagation|Hidden %d->Hidden %d:\n", i, i - 1);
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
							double synaptic_weight = total_weight == 0 ? 0 : previous_weight[k] / total_weight;

							m_hidden_layers[i - 1][k]->m_error = m_hidden_layers[i][j]->m_error * synaptic_weight * learning_rate;

							double new_weight = previous_weight[k] * (1 + m_hidden_layers[i - 1][j]->m_error);
							new_weight = Bound(new_weight, 0.f, 1.f);
							m_hidden_layers[i - 1][k]->m_previous_weight[j] = m_hidden_layers[i - 1][k]->m_weight[j];
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
						double synaptic_weight = total_weight == 0 ? 0 : previous_weight[f] / total_weight;

						m_input_layer[f]->m_error = m_hidden_layers[0][j]->m_error * synaptic_weight * learning_rate;

						double new_weight = previous_weight[f] * (1 + m_input_layer[f]->m_error);
						new_weight = Bound(new_weight, 0.f, 1.f);
						m_input_layer[f]->m_previous_weight[j] = m_input_layer[f]->m_weight[j];
						m_input_layer[f]->m_weight[j] = new_weight;
						printf("weight %d, neuron: %d, old_weight:%.3f | synaptic: %.1f | parent_error: %.3f | error: %.3f | new_weight: %.3f\n", f, j, previous_weight[j], synaptic_weight, m_hidden_layers[0][j]->m_error, m_input_layer[f]->m_error, new_weight);
					}
				}
			}
		}
	}

	printf("End.\n");
}

void NeuralNetwork::RestorePreviousWeight()
{
	for (int f = 0; f < NB_FEATURES + 1; f++)
	{
		for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
		{
			m_input_layer[f]->m_weight[j] = m_input_layer[f]->m_previous_weight[j];
		}
	}

	for (int i = 0; i < NEURAL_NETWORK_DEPTH - 1; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
		{
			for (int k = 0; k < NEURAL_NETWORK_HEIGHT; k++)
			{
				m_hidden_layers[i][j]->m_weight[k] = m_hidden_layers[i][j]->m_previous_weight[k];
			}

		}
	}

	for (int i = 0; i < NB_LABELS; i++)
	{
		for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
		{
			m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_weight[i] = m_hidden_layers[NEURAL_NETWORK_DEPTH - 1][j]->m_previous_weight[i];
		}
	}
}

*/

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
		output_value = 1.f / (1 + pow((1 / 2, 71828), x));
	}

	//TANH
	if (function == TANH)
	{
		output_value = tanh(x);
	}

	return output_value;
}

double NeuralNetwork::TransferFunctionDerivative(double x, FunctionType function)
{
	double output_value = 0.f;

	//LINEAR
	if (function == LINEAR)
	{
		return x;//todo
	}

	//THRESHOLD
	if (function == THRESHOLD)
	{
		return x;//todo
	}

	//SIGMOID
	if (function == SIGMOID)
	{
		return x;//todo
	}

	//TANH
	if (function == TANH)
	{		
		output_value = 1.f - tanh(x)*tanh(x);
	}

	return output_value;
}