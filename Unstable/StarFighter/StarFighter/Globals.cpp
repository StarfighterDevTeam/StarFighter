#include "Globals.h"

int RandomizeIntBetweenRatios(int value, float min_value, float max_value)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (max_value - min_value);
	random_rate += min_value;
	int i = (int) ((random_rate*value) + 0.5);

	return i;
}

float RandomizeFloatBetweenRatios(float value, float min_value, float max_value)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (max_value - min_value);
	random_rate += min_value;
	float f = (float) (random_rate*value);

	return f;
}

float ProrataBetweenThreshold(float m_value, float min_threshold, float max_threshold)
{
	float max = max_threshold - min_threshold;
	float value = m_value - min_threshold;
	float prorata = value / max;

	return prorata;
}

int RandomizeIntBetweenValues(int min_value, int max_value)
{
	int random_value = rand() % (max_value - min_value +1);
	random_value += min_value;

	return random_value;
}

int RandomizeIntBetweenFloats(float min_value, float max_value)
{
	int min = floor(min_value + 0.5);
	int max = floor(max_value + 0.5);
	int random_value = rand() % (max - min +1);
	random_value += min;

	return random_value;
}

float RandomizeFloatBetweenValues(float min_value, float max_value)
{
	float random_value = (float) ((double) rand() / RAND_MAX);
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

float MaxBetweenValues(float x, float y)
{
	float max = x;
	if (y > max)
	{
		max = y;
	}
	return max;
}

float MinBetweenValues(float x, float y)
{
	float min = x;
	if (y < min)
	{
		min = y;
	}
	return min;
}

float Lerp(float value, float input_min, float input_max, float output_min, float output_max)
{
	if (input_min == input_max)
		return output_max;

	if (value < input_min)
		return output_min;

	if (value > input_max)
		return output_max;

	float ratio = (value - input_min) / (input_max - input_min);

	float lerp = output_min + ratio * (output_max - output_min);

	return lerp;
}

float Bound(float value, float output_min, float output_max)
{
	if (value < output_min)
		return output_min;

	if (value > output_max)
		return output_max;

	return value;
}

float CosInterpolation(float value, float input_min, float input_max, float output_min, float output_max)
{
	if (input_min == input_max)
		return output_max;

	if (value < input_min)
		return output_min;

	if (value > input_max)
		return output_max;

	float ratio = (1 - cos(value * M_PI)) / 2;
	//for stiffer curves, use "ratio = (1 - cos(value * value * M_PI)) / 2;"

	float ouput = output_min + ratio * (output_max - output_min);

	return ouput;
}