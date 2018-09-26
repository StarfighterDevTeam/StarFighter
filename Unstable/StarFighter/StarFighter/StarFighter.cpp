#include "StarFighter.h"

int main()
{
	NeuralNetwork NeuralNetwork;

	//Input labelled data
	NeuralNetwork.FillWithData();

	//Train on labelled data
	printf("\nTraining neural network\n");
	NeuralNetwork.Train();

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


Neuron::Neuron()
{
	float total_weight = 0;
	for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
	{
		m_weight[j] = RandomizeFloatBetweenValues(0.f, 1.f);
		total_weight += m_weight[j];
	}
	for (int j = 0; j < NEURAL_NETWORK_HEIGHT; j++)
	{
		m_weight[j] = total_weight == 0 ? 0 : m_weight[j] / total_weight;

		m_previous_weight[j] = m_weight[j];
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

void NeuralNetwork::FillWithData()
{
	for (int i = 0; i < DATASET_SIZE; i++)
	{
		int red, green, blue = 0;
		Label label;

		//labelled examples
		if (i < DATASET_SUPERVISED_LOT + DATASET_TESTING_LOT)
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

void NeuralNetwork::Train()
{
	float learning_rate = NEURAL_NETWORK_LEARNING_RATE;

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

		float error = -1;
		float previous_error = -1;
		float learning_rate = NEURAL_NETWORK_LEARNING_RATE;
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
							float synaptic_weight = total_weight == 0 ? 0 : previous_weight[k] / total_weight;

							m_hidden_layers[i - 1][k]->m_error = m_hidden_layers[i][j]->m_error * synaptic_weight * learning_rate;

							float new_weight = previous_weight[k] * (1 + m_hidden_layers[i - 1][j]->m_error);
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
						float synaptic_weight = total_weight == 0 ? 0 : previous_weight[f] / total_weight;

						m_input_layer[f]->m_error = m_hidden_layers[0][j]->m_error * synaptic_weight * learning_rate;

						float new_weight = previous_weight[f] * (1 + m_input_layer[f]->m_error);
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

float NeuralNetwork::ActivationFunction(float x)
{
	float output_value = 0.f;
	ActivationFunctions function = SIGMOID;

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

	//SIGMOID
	if (function == TAHN)
	{
		output_value = tanh(x);
	}

	return output_value;
}