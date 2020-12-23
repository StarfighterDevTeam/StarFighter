#include "Globals.h"

#ifdef __APPLE__
    #include "ResourcePath.hpp"
	#include "sys/stat.h"
#endif

sf::RenderWindow *window;

//to do OPTIM
//std::unique_ptr<sf::RenderWindow> window;

std::string makePath(const std::string& srcPath)
{
#ifdef __APPLE__
    return resourcePath() + "Assets/" + srcPath;
#else
	return "Assets/" + srcPath;
#endif
}

void createSavesDirectory()
{
#ifdef __APPLE__
	char strPath[1024];
	snprintf(strPath, sizeof(strPath), "%s/Library/Application Support/StarFighter", getenv("HOME"));
	mkdir(strPath, 0755);
	snprintf(strPath, sizeof(strPath), "%s/Library/Application Support/StarFighter/Saves", getenv("HOME"));
	mkdir(strPath, 0755);
#else
	//::CreateDirectory(path, NULL);	// TODO: to be implemented on Windows
#endif
}

const char* getSavesPath()
{
#ifdef __APPLE__
	static char strPath[1024];
	snprintf(strPath, sizeof(strPath), "%s/Library/Application Support/StarFighter/Saves/", getenv("HOME"));
	return strPath;
#else
	return "";	// TODO: should use %APPDATA% / "My Games/Saves" on Windows
#endif
}

int RandomizeIntBetweenRatios(int value, sf::Vector2f min_max_values)
{
	double random_rate = 1.f * rand() / RAND_MAX;
	random_rate *= (min_max_values.y - min_max_values.x);
	random_rate += min_max_values.x;
	int i = (int) ((random_rate*value) + 0.5);

	return i;
}

float RandomizeFloatBetweenRatios(float value, sf::Vector2f min_max_ratios)
{
	double random_rate = 1.f * rand() / RAND_MAX;
	random_rate *= (min_max_ratios.y - min_max_ratios.x);
	random_rate += min_max_ratios.x;
	float f = (float) (random_rate*value);

	return f;
}

float ProrataBetweenThreshold(float value, sf::Vector2f min_max_threshold)
{
	float max = min_max_threshold.y - min_max_threshold.x;

	if (max == 0)
		return 1;

	float scaled_value = value - min_max_threshold.x;
	float prorata = scaled_value / max;
	if (prorata > 1)
		prorata = 1;

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

float RandomizeFloatBetweenValues(float value_min, float value_max)
{
	float random_value = 1.f * rand() / RAND_MAX;
	random_value *= (value_max - value_min);
	random_value += value_min;

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

std::string StringCut(std::string str, const size_t length)
{
	size_t current_length = length;
	size_t current_cut = 0;
	size_t str_size = str.size();
	while (str_size - current_cut > length)
	{
		bool found_cut = false;
		for (size_t i = current_cut + length; i > 0; i--)
		{
			if (str[i] == ' ')
			{
				str[i] = '\n';
				current_cut = i;
				current_length = str_size - current_cut;
				found_cut = true;
				break;
			}
		}

		if (found_cut == false)
			break;
	}

	return str;
}

std::string SFReplaceTexts(std::string str, const size_t length)
{
	return StringCut(ReplaceAll(ReplaceAll(str, "_", " "), "[COMA]", ","), length);
}

void BoundToValues(float& input, float min, float max)
{
	if (min > max)
	{
		float _min = min;
		min = max;
		max = _min;
	}

	if (input < min)
		input = min;

	if (input > max)
		input = max;
}
