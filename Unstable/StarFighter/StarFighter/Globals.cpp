#include "Globals.h"

int RandomizeIntBetweenRatios(int value, double min_value, double max_value)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (max_value - min_value);
	random_rate += min_value;
	int i = (int) ((random_rate*value) + 0.5);

	return i;
}

double RandomizeFloatBetweenRatios(double value, double min_value, double max_value)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (max_value - min_value);
	random_rate += min_value;
	double f = (double) (random_rate*value);

	return f;
}

double ProrataBetweenThreshold(double m_value, double min_threshold, double max_threshold)
{
	double max = max_threshold - min_threshold;
	double value = m_value - min_threshold;
	double prorata = value / max;

	return prorata;
}

int RandomizeIntBetweenValues(int min_value, int max_value)
{
	int random_value = rand() % (max_value - min_value +1);
	random_value += min_value;

	return random_value;
}

int RandomizeIntBetweenFloats(double min_value, double max_value)
{
	int min = floor(min_value + 0.5);
	int max = floor(max_value + 0.5);
	int random_value = rand() % (max - min +1);
	random_value += min;

	return random_value;
}

double RandomizeFloatBetweenValues(double min_value, double max_value)
{
	double random_value = (double) ((double) rand() / RAND_MAX);
	random_value *= (max_value - min_value);
	random_value += min_value;

	return random_value;
}

int RandomizeSign()
{
	//50% chance of being 1, 50% chance of being -1
	int sign = 1;

	int random_value = RandomizeIntBetweenValues(1, 2);
	if (random_value > 1)
	{
		sign = -1;
	}

	return sign;
}

double MaxBetweenValues(double x, double y)
{
	double max = x;
	if (y > max)
	{
		max = y;
	}
	return max;
}

double MinBetweenValues(double x, double y)
{
	double min = x;
	if (y < min)
	{
		min = y;
	}
	return min;
}

double Lerp(double value, double input_min, double input_max, double output_min, double output_max)
{
	if (input_min == input_max)
		return output_max;

	if (value < input_min)
		return output_min;

	if (value > input_max)
		return output_max;

	double ratio = (value - input_min) / (input_max - input_min);

	double lerp = output_min + ratio * (output_max - output_min);

	return lerp;
}

double Bound(double value, double output_min, double output_max)
{
	if (value < output_min)
		return output_min;

	if (value > output_max)
		return output_max;

	return value;
}

double CosInterpolation(double value, double input_min, double input_max, double output_min, double output_max)
{
	if (input_min == input_max)
		return output_max;

	if (value < input_min)
		return output_min;

	if (value > input_max)
		return output_max;

	double ratio = (1 - cos(value * M_PI)) / 2;
	//for stiffer curves, use "ratio = (1 - cos(value * value * M_PI)) / 2;"

	double ouput = output_min + ratio * (output_max - output_min);

	return ouput;
}