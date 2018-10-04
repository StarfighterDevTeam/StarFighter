#include "Data.h"


Data::Data(vector<double> features, Label label)
{
	m_features = features;
	m_label = label;
}

Data::Data(Label label, double error_margin)
{
	int red, blue, green = 0;

	if (label == IS_GREEN)
	{
		red = RandomizeIntBetweenValues(0, (int)(error_margin * 255));
		green = RandomizeIntBetweenValues((int)(255 * (1 - error_margin)), 255);
		blue = RandomizeIntBetweenValues(0, (int)(error_margin * 255));
	}
	else if (label == NOT_GREEN)
	{
		red = RandomizeIntBetweenValues(0, 255);
		green = RandomizeIntBetweenValues(0, (int)((1 - error_margin) * 255));
		blue = RandomizeIntBetweenValues(0, 255);
	}
	else
	{
		int r = RandomizeIntBetweenValues(0, 1);
		if (r % 2 == 0)
		{
			//green examples
			red = RandomizeIntBetweenValues(0, (int)(error_margin * 255));
			green = RandomizeIntBetweenValues((int)(255 * (1 - error_margin)), 255);
			blue = RandomizeIntBetweenValues(0, (int)(error_margin * 255));
		}
		else
		{
			//not green examples
			red = RandomizeIntBetweenValues(0, 255);
			green = RandomizeIntBetweenValues(0, (int)((1 - error_margin) * 255));
			blue = RandomizeIntBetweenValues(0, 255);
		}
	}

	//Normalize data between -1 and 1
	double r = Data::RGBIntoInput(red);// (1.f * red / 255 * 2) - 1;
	double g = Data::RGBIntoInput(green);// (1.f * green / 255 * 2) - 1;
	double b = Data::RGBIntoInput(blue);// (1.f * blue / 255 * 2) - 1;

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


int Data::InputIntoRGB(double input_value)
{
	return 255 * (1.f * (input_value + 1) / 2);
}

double Data::RGBIntoInput(int rgb_value)
{
	return (1.f * rgb_value / 255 * 2) - 1;
}