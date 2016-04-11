#include "Globals.h"

#ifdef __APPLE__
    #include "ResourcePath.hpp"
#endif

sf::RenderWindow *window;

//to do OPTIM
//std::unique_ptr<sf::RenderWindow> window;

std::string makePath(const std::string& srcPath)
{
#ifdef __APPLE__
    return resourcePath() + srcPath;
#else
    return srcPath;
#endif
}

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (min_max_values.y - min_max_values.x);
	random_rate += min_max_values.x;
	int i = (int) ((random_rate*value) + 0.5);

	return i;
}

float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (min_max_ratios.y - min_max_ratios.x);
	random_rate += min_max_ratios.x;
	float f = (float) (random_rate*value);

	return f;
}

float ProrataBetweenThreshold(float m_value, sf::Vector2f min_max_threshold)
{
	float max = min_max_threshold.y - min_max_threshold.x;
	float value = m_value - min_max_threshold.x;
	float prorata = value / max;

	return prorata;
}

int RandomizeIntBetweenValues(int min_value, int max_value)
{
	int random_value = rand() % (max_value - min_value +1);
	random_value += min_value;

	return random_value;
}

int RandomizeIntBetweenFloats(sf::Vector2f min_max_values)
{
	int min = floor (min_max_values.x + 0.5);
	int max = floor (min_max_values.y + 0.5);
	int random_value = rand() % (max - min +1);
	random_value += min;

	return random_value;
}

float RandomizeFloatBetweenValues(sf::Vector2f min_max_values)
{
	float random_value = (float) ((double) rand() / RAND_MAX);
	random_value *= (min_max_values.y - min_max_values.x);
	random_value += min_max_values.x;

	return random_value;
}

float MaxBetweenValues(sf::Vector2f values)
{
	float max = values.x;
	if (values.y > max)
	{
		max = values.y;
	}
	return max;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) 
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}
