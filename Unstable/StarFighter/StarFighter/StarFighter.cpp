#include "StarFighter.h"

int main()
{
	NeuralNetwork NeuralNetwork;

	//Input labelled data
	NeuralNetwork.CreateDataset();

	//Train on labelled data
	//NeuralNetwork.Train();

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

Data::Data(vector<int> features, Label label)
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

	m_features.push_back(red);
	m_features.push_back(green);
	m_features.push_back(blue);
	m_features.push_back(label);

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
	for (int i = 0; i < nb_neuron + 1; i++)//+1 for the bias neuron
	{
		m_neurons.push_back(Neuron());
	}

	m_nb_neurons = nb_neuron + 1;//+1 for the bias neuron
	m_type = type;
}


//NEURAL NETWORK
NeuralNetwork::NeuralNetwork()
{
	m_nb_layers = 0;

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
			}
		}
	}

	//Create the new layer
	m_layers.push_back(Layer(nb_neuron, type));
	m_nb_layers++;
}

void NeuralNetwork::CreateDataset()
{
	for (int i = 0; i < DATASET_SIZE; i++)
	{
		if (i < DATASET_SUPERVISED_LOT + DATASET_TESTING_LOT)
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
		}
		//else
		//{
		//	m_dataset.push_back(Data(UNLABELLED));
		//}

		if (i == DATASET_SIZE - 1)
		{
			printf("Dataset created: %d data items.\n", i + 1);
		}
	}
}

void NeuralNetwork::Training()
{
	//to rewrite
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

double NeuralNetwork::ActivationFunction(double x)
{
	double output_value = 0.f;

	//LINEAR
	if (m_function == LINEAR)
	{
		output_value = x;
	}

	//THRESHOLD
	if (m_function == THRESHOLD)
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
	if (m_function == SIGMOID)
	{
		output_value = 1.f / (1 + pow((1 / 2, 71828), x));
	}

	//TANH
	if (m_function == TANH)
	{
		output_value = tanh(x);
	}

	return output_value;
}